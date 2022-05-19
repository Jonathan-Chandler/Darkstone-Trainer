#include "debug.h"
#include "process_info.h"
#include <errno.h>
#include <psapi.h>        //MODULEINFO
#include <tlhelp32.h>     // PROCESSENTRY32
// #include <stdint.h>
#include <inttypes.h>
#include <stdio.h>

ProcessInfo::ProcessInfo()
  : mExecutableName("")
  , mhWindow(nullptr)
  , mhProcessHandle(nullptr)
  , mdwProcessId(0)
  , mhDllHandle(0)
  , mpDllPointer(0)
{
}

ProcessInfo::ProcessInfo(std::string executableName)
  : mExecutableName(executableName)
  , mhWindow(nullptr)
  , mhProcessHandle(nullptr)
  , mdwProcessId(0)
  , mhDllHandle(0)
  , mpDllPointer(0)
{
  int retval;

  // get mdwProcessId from executable name
  if ((retval = getIdFromExe()) < 0)
    debug_error("Fail to open process handle - %d", retval);
}

ProcessInfo::~ProcessInfo()
{
  int retval;

  if (mhProcessHandle != nullptr)
  {
    if ((retval = closeProcessHandle()) < 0)
    {
      debug_error("Fail to close process handle - %d", retval);
    }
  }
}

std::string ProcessInfo::getExecutableName()
{
  return mExecutableName;
}

int ProcessInfo::setExecutableName(std::string executableName)
{
  if (executableName.empty())
  {
    debug_error("Invalid executable name");
    return -EINVAL;
  }
  mExecutableName = executableName;
  return 0;
}

int ProcessInfo::getIdFromWindow(std::string sWindowTitle)
{
  if ((mhWindow = FindWindow(0, sWindowTitle.c_str())) == 0)
  {
    //debug_error("Fail to find window");
    return -1;
  }

  GetWindowThreadProcessId(mhWindow, &mdwProcessId);
  if (mdwProcessId == 0)
  {
    debug_error("Get invalid process ID");
    return -1;
  }

  return 0;
}

int ProcessInfo::getIdFromExe()
{
  HANDLE hProcessSnap;
  PROCESSENTRY32 pe32;
  DWORD dwProcessId;
  const char *pszExeName = mExecutableName.c_str();

  if (mExecutableName == "")
  {
    debug_error("Invalid executable name");
    return -EINVAL;
  }

  // get all 32 bit processes
  hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (INVALID_HANDLE_VALUE == hProcessSnap) 
  {
    debug_error("Returned invalid process snapshot handle");
    return -ENXIO;
  }

  // set struct size
  pe32.dwSize = sizeof(PROCESSENTRY32);

  // get first process
  if (!Process32First(hProcessSnap, &pe32))
  {
    CloseHandle(hProcessSnap);
    debug_error("Failed to read Process32First information");
    return -ENXIO;
  }

  // match executable name to process list
  do
  {
    if (0 == strcmp(pszExeName, pe32.szExeFile))
    {
      dwProcessId = pe32.th32ProcessID;
      break;
    }
  } while (Process32Next(hProcessSnap, &pe32));
  CloseHandle(hProcessSnap);

  // invalid / wrong process ID
  if (dwProcessId == 0 || 0 != strcmp(pszExeName, pe32.szExeFile))
  {
    debug_error("Process \"%s\" was not found", pszExeName);
    return -ENXIO;
  }

  debug_verbose("Found process ID %u", (uint32_t)dwProcessId);
  mdwProcessId = dwProcessId;

  return 0;
}

int ProcessInfo::openProcessHandle()
{
  if (mdwProcessId == 0)
  {
    debug_error("Null process ID");
    return -EINVAL;
  }

  mhProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, mdwProcessId);

  if (mhProcessHandle == 0)
  {
    debug_error("Failed to get handle for process");
    return -ENXIO;
  }

  debug_verbose("Open process handle: %p", mhProcessHandle);
  return 0;
}

int ProcessInfo::closeProcessHandle()
{
  if (mhProcessHandle != nullptr)
  {
    debug_verbose("Closing process handle: %p", mhProcessHandle);
    CloseHandle(mhProcessHandle);
    mhProcessHandle = nullptr;
  }
  else
  {
    debug_error("Tried to close null process handle");
    return -ENXIO;
  }

  return 0;
}

int ProcessInfo::injectDll(std::string sDllPath)
{
  HANDLE hDll;
  LPVOID lpAllocatedDllPath;
  HANDLE ret;
  int n;
  LPVOID lpLoadLibAddr;

  if (sDllPath.empty())
  {
    debug_error("Null process information pointer");
    return -EINVAL;
  }

  if (mhProcessHandle == 0)
  {
    debug_error("Invalid process handle");
    return -EINVAL;
  }

  // get addr of load library
  lpLoadLibAddr = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
  if(lpLoadLibAddr == NULL) 
  {
    debug_error("LoadLibraryExA function was not found inside kernel32.dll library");
    return -ENXIO;
  }
  else
    debug_verbose("Found address of kernel32 LoadLibraryA: 0x%p", lpLoadLibAddr);

  // allocate foreign memory for dll path
  lpAllocatedDllPath = (LPVOID)VirtualAllocEx(mhProcessHandle, NULL, sDllPath.length(), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
  if(lpAllocatedDllPath == NULL) 
  {
    debug_error("Could not allocate foreign process memory");
    return -ENXIO;
  }

  // write path to DLL into foreign memory
  n = WriteProcessMemory(mhProcessHandle, lpAllocatedDllPath, sDllPath.c_str(), sDllPath.length(), NULL);
  if(n == 0) 
  {
    debug_error("No bytes written to address space");
    return -ENXIO;
  }
  //debug_verbose("Wrote path %s to remote address 0x%p", sDllPath.c_str(), lpAllocatedDllPath);

  // create LoadLibraryA thread to load dll at allocated path
  ret = CreateRemoteThread(mhProcessHandle, NULL, 0, (LPTHREAD_START_ROUTINE)lpLoadLibAddr, lpAllocatedDllPath, 0, NULL);
  if(ret == NULL) 
  {
    debug_error("Remote thread could not be created");
    return -ENXIO;
  }

  debug_verbose("Wait for remote thread");
  WaitForSingleObject(ret, INFINITE);

  // exit code is retval of LoadLibraryA
  GetExitCodeThread(ret, (DWORD*) &hDll);
  debug_verbose("DLL handle is 0x%p", hDll);
  mhDllHandle = hDll;

  if ((mpDllPointer = getRemoteModuleAddr(sDllPath.c_str())) == 0)
  {
    debug_error("Failed to get pointer to remote module");
    return -ENXIO;
  }
  debug_verbose("DLL address is 0x%p", mpDllPointer);

  return 0;
}

bool ProcessInfo::check_if_process_is_x64()
{
  BOOL is_wow64_process = TRUE;

  if (::IsWow64Process(mhProcessHandle, &is_wow64_process))
  {
    printf("nonzero\n");
  }

  if (is_wow64_process == TRUE)
    return true;

  return false;
}

void* ProcessInfo::getRemoteModuleAddr(std::string sModuleName)
{
  HMODULE hMods[1024];
  DWORD cbNeeded;
  unsigned int i;

  // Get a list of all the modules in this process.
  if(EnumProcessModules(mhProcessHandle, hMods, sizeof(hMods), &cbNeeded))
  {
    for ( i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
    {
      char szModuleNameBuff[MAX_PATH];

      if (!GetModuleFileNameExA(mhProcessHandle, hMods[i], szModuleNameBuff, sizeof(szModuleNameBuff)))
      {
        debug_error("Fail to enum process module at index %u", i);
      }
      else
      {
        //printf("%s (0x%p)\n", szModuleNameBuff, hMods[i]);
      }

      if (strcmp(szModuleNameBuff, sModuleName.c_str()) == 0)
      {
        //debug_verbose("Found searched module %s (0x%p)", szModuleNameBuff, hMods[i]);
        mpDllPointer = hMods[i];
        return hMods[i];
      }
    }
  }
  else
  {
    debug_error("Fail to enum process modules");
  }

  return 0;
}

void* ProcessInfo::getRemoteFunctionAddr(std::string sFunctionName)
{
  void *retAddr = nullptr;
  IMAGE_DOS_HEADER dos_header = {0};
  IMAGE_NT_HEADERS64 nt_headers = {0};        // contains signature / IMAGE_FILE_HEADER / IMAGE_OPTIONAL_HEADER64
  IMAGE_DATA_DIRECTORY dllExports = {0};
  IMAGE_EXPORT_DIRECTORY dllImageExportDirectory = {0};
  DWORD *pExportFunctions = nullptr;
  DWORD *pExportNames = nullptr;
  DWORD *pExportNameOrdinals = nullptr;

  if (sFunctionName.empty())
  {
    debug_error("Null process information pointer");
    return 0;
  }

  if (mhProcessHandle == 0)
  {
    debug_error("Invalid process handle");
    return 0;
  }

  //if (mhDllHandle == 0 || mpDllPointer == 0)
  if (mhDllHandle == 0)
  {
    debug_error("Invalid dll handle");
    return 0;
  }

  // read dos header of dll (IMAGE_DOS_SIGNATURE = 0x5A4D = "MZ")
  if (!ReadProcessMemory(mhProcessHandle, mpDllPointer, &dos_header, sizeof(dos_header), NULL))
  {
    debug_error("Fail to get read process dos header from process handle %p at address %p\n", mhProcessHandle, mpDllPointer);
    goto dealloc_exit;
  }

  // read dos header of dll (IMAGE_DOS_SIGNATURE = 0x5A4D = "MZ")
  if (dos_header.e_magic != IMAGE_DOS_SIGNATURE)
  {
    debug_error("Fail to get dll dos header - expect %" PRIx16 " but received value %" PRIx16 "", (0xFFFF & IMAGE_DOS_SIGNATURE), (0xFFFF & dos_header.e_magic));
    goto dealloc_exit;
  }

  // read IMAGE_NT_HEADERS64 starting at offset *(dll_base + dos_header.e_lfanew) (.Signature = IMAGE_NT_SIGNATURE == 0x00004550 == "PE00")
  if (!ReadProcessMemory(mhProcessHandle, (LPCVOID)((uintptr_t)mpDllPointer + dos_header.e_lfanew), &nt_headers, sizeof(nt_headers), NULL) 
      || nt_headers.Signature != IMAGE_NT_SIGNATURE)
  {
    debug_error("Fail to get IMAGE_NT_HEADERS64");
    goto dealloc_exit;
  }

  // check machine matches *(nt_headers + 4) (IMAGE_FILE_HEADER.Machine == IMAGE_NT_HEADERS64 == 0x8664)
  if (nt_headers.FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64)
  {
    debug_error("Machine did not match expected AMD64");
    goto dealloc_exit;
  }

  // check optional header magic value *(nt_headers + 0x18) (IMAGE_NT_OPTIONAL_HDR64_MAGIC == 0x020B)
  if (nt_headers.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
  {
    debug_error("Optional Header Magic did not match expected IMAGE_NT_OPTIONAL_HDR64_MAGIC");
    goto dealloc_exit;
  }

  // must have (NumberOfRvaAndSizes >= (IMAGE_DIRECTORY_ENTRY_* + 1)) otherwise table entry access is not valid
  if (nt_headers.OptionalHeader.NumberOfRvaAndSizes < (IMAGE_DIRECTORY_ENTRY_EXPORT + 1))
  {
    debug_error("Optional Header did not contain export RVA directory");
    goto dealloc_exit;
  }

  dllExports.VirtualAddress = nt_headers.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
  dllExports.Size = nt_headers.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

  // copy from dll export directory virtual address to dll export table
  if (!ReadProcessMemory(mhProcessHandle, (LPCVOID)((uintptr_t)mpDllPointer + dllExports.VirtualAddress), &dllImageExportDirectory, sizeof(dllImageExportDirectory), NULL))
  {
    debug_error("Fail to get dll image export directory");
    goto dealloc_exit;
  }

  // allocate space to copy tables from remote image export
  pExportFunctions = (DWORD*) malloc(sizeof(DWORD) * dllImageExportDirectory.NumberOfFunctions);
  pExportNames = (DWORD*) malloc(sizeof(DWORD) * dllImageExportDirectory.NumberOfNames);
  pExportNameOrdinals = (DWORD*) malloc(sizeof(DWORD) * dllImageExportDirectory.NumberOfNames);
  if (pExportFunctions == NULL || pExportNames == NULL || pExportNameOrdinals == NULL)
  {
    debug_error("Fail to allocate space for export tables");
    goto dealloc_exit;
  }

  // copy function addresses
  if (!ReadProcessMemory(mhProcessHandle, (LPCVOID)((uintptr_t)mpDllPointer + dllImageExportDirectory.AddressOfFunctions), pExportFunctions, (sizeof(DWORD) * dllImageExportDirectory.NumberOfFunctions), NULL))
  {
    debug_error("Fail to copy dll function address table");
    goto dealloc_exit;
  }

  // copy function name addresses
  if (!ReadProcessMemory(mhProcessHandle, (LPCVOID)((uintptr_t)mpDllPointer + dllImageExportDirectory.AddressOfNames), pExportNames, (sizeof(DWORD) * dllImageExportDirectory.NumberOfNames), NULL))
  {
    debug_error("Fail to copy dll function name table");
    goto dealloc_exit;
  }

  // copy function name ordinal addresses
  if (!ReadProcessMemory(mhProcessHandle, (LPCVOID)((uintptr_t)mpDllPointer + dllImageExportDirectory.AddressOfNameOrdinals), pExportNameOrdinals, (sizeof(DWORD) * dllImageExportDirectory.NumberOfNames), NULL))
  {
    debug_error("Fail to copy dll function name ordinal table");
    goto dealloc_exit;
  }

  // read functions
  for (uint32_t i = 0; i < dllImageExportDirectory.NumberOfNames; i++)
  {
    char pFunctionNameBuff[256];

    // get function name
    for (uint32_t charIndex = 0; charIndex < 255; charIndex++)
    {
      // copy single char
      if(!ReadProcessMemory(mhProcessHandle, (LPCVOID)((uintptr_t)mpDllPointer + pExportNames[i] + charIndex),
            &pFunctionNameBuff[charIndex], sizeof(char), NULL))
      {
        debug_error("Fail while reading function name %" PRIu32 " character %" PRIu32 " from dll", i, charIndex);
        goto dealloc_exit;
      }

      // check if copied entire function name
      if (charIndex > 0 && pFunctionNameBuff[charIndex] == '\0')
      {
        // compare with searched function name
        if (strcmp(pFunctionNameBuff, sFunctionName.c_str()) == 0)
        {
          retAddr = (void*)((uintptr_t)mpDllPointer + pExportFunctions[i]);
          debug_verbose("Match function[%" PRIu32 "] name: %s", i, pFunctionNameBuff);
          goto dealloc_exit;
        }

        //debug_verbose("function[%" PRIu32 "] name: %s", i, pFunctionNameBuff);
        break;
      }
    }
  }

dealloc_exit:
  if (pExportFunctions != nullptr)
    free(pExportFunctions);

  if (pExportNames != nullptr)
    free(pExportNames);

  if (pExportNameOrdinals != nullptr)
    free(pExportNameOrdinals);

  return retAddr;
}

int ProcessInfo::createRemoteFunctionThread(std::string sFunctionName)
{
  void* lpRemoteFunctionAddr;
  HANDLE ret;

  if ((lpRemoteFunctionAddr = getRemoteFunctionAddr(sFunctionName)) == 0)
  {
    debug_error("Could not get remote function addr for %s", sFunctionName.c_str());
    return -ENXIO;
  }

  // create LoadLibraryA thread to load dll at allocated path
  ret = CreateRemoteThread(mhProcessHandle, NULL, 0, (LPTHREAD_START_ROUTINE)lpRemoteFunctionAddr, 0, 0, NULL);
  if(ret == NULL) 
  {
    debug_error("Remote thread could not be created");
    return -ENXIO;
  }

  return 0;
}

int ProcessInfo::readMemoryBlock(const void* pRemoteReadAddr, char* pReadBuffer, SIZE_T nBytesRequested) const
{
  SIZE_T nBytesRead;

  // check valid params
  if (mhProcessHandle == 0 || pRemoteReadAddr == 0 || pReadBuffer == 0) 
  {
    debug_error("Receive invalid param for readMemoryBlock");
    return -1;
  }

  if ((ReadProcessMemory(mhProcessHandle, (LPVOID)pRemoteReadAddr, pReadBuffer, nBytesRequested, &nBytesRead) == 0)
      || (nBytesRead != nBytesRequested))
  {
    debug_error("Fail to read process memory");
    return -1;
  }

  return 0;
}

int ProcessInfo::writeMemoryBlock(const void* pRemoteWriteAddr, const char* pWriteBuffer, SIZE_T nBytes) const
{
  SIZE_T nBytesWritten;

  // check valid params
  if (mhProcessHandle == 0 || pRemoteWriteAddr == 0 || pWriteBuffer == 0) 
  {
    debug_error("Receive invalid param for writeMemoryBlock");
    return -1;
  }

  if ((WriteProcessMemory(mhProcessHandle, (LPVOID)pRemoteWriteAddr, pWriteBuffer, nBytes, &nBytesWritten) == 0)
      || (nBytesWritten != nBytes))
  {
    debug_error("Fail to write process memory");
    return -1;
  }

  return 0;
}

