
#ifndef __PROCESS_INFO_H__
#define __PROCESS_INFO_H__
#include <windows.h>
#include "debug.h"
#include <stdint.h>
#include <string>


class ProcessInfo
{
  private:
    std::string mExecutableName;
    HWND mhWindow;
    HANDLE mhProcessHandle;
    DWORD mdwProcessId;
    HANDLE mhDllHandle;
    void *mpDllPointer;

  public:
    ProcessInfo();
    ProcessInfo(std::string executableName);
    ~ProcessInfo();

    std::string getExecutableName();
    int setExecutableName(std::string executableName);

    int getIdFromWindow(std::string sWindowTitle);
    int getIdFromExe();

    int openProcessHandle();
    int closeProcessHandle();

    int injectDll(std::string pszDllPath);
    HANDLE getDllHandle();
    //int proc_info_list_remote_functions(proc_info_t *pProcInfo, HMODULE hModule);
    void* getRemoteModuleAddr(std::string sFunctionName);
    void* getRemoteFunctionAddr(std::string sFunctionName);
    int createRemoteFunctionThread(std::string sFunctionName);
    bool check_if_process_is_x64();

    int readMemoryBlock(const void* pRemoteReadAddr, char* pReadBuffer, SIZE_T nBytesRequested) const;
    int writeMemoryBlock(const void* pRemoteWriteAddr, const char* pWriteBuffer, SIZE_T nBytes) const;

    template <class T>
    int readMemory(const void* pRemoteReadAddr, T* pReadBuffer) const
    {
      SIZE_T nBytesRead;

      // check valid params
      if (mhProcessHandle == 0 || pRemoteReadAddr == 0 || pReadBuffer == 0) 
      {
        debug_error("Receive invalid param for readMemory");
        return -1;
      }

      if ((ReadProcessMemory(mhProcessHandle, (LPVOID)pRemoteReadAddr, pReadBuffer, sizeof(T), &nBytesRead) == 0)
          || (nBytesRead != sizeof(T)))
      {
        debug_error("Fail to read process memory");
        return -1;
      }

      return 0;
    };

    template <class T>
    int writeMemory(const void* pRemoteWriteAddr, const T* pWriteBuffer) const
    {
      SIZE_T nBytesWritten;

      // check valid params
      if (mhProcessHandle == 0 || pRemoteWriteAddr == 0 || pWriteBuffer == 0) 
      {
        debug_error("Receive invalid param for readMemory");
        return -1;
      }

      if ((WriteProcessMemory(mhProcessHandle, (LPVOID)pRemoteWriteAddr, pWriteBuffer, sizeof(T), &nBytesWritten) == 0)
          || (nBytesWritten != sizeof(T)))
      {
        debug_error("Fail to write process memory");
        return -1;
      }

      return 0;
    };

};

// typedef struct _IMAGE_DOS_HEADER
// {
//      WORD e_magic;
//      WORD e_cblp;
//      WORD e_cp;
//      WORD e_crlc;
//      WORD e_cparhdr;
//      WORD e_minalloc;
//      WORD e_maxalloc;
//      WORD e_ss;
//      WORD e_sp;
//      WORD e_csum;
//      WORD e_ip;
//      WORD e_cs;
//      WORD e_lfarlc;
//      WORD e_ovno;
//      WORD e_res[4];
//      WORD e_oemid;
//      WORD e_oeminfo;
//      WORD e_res2[10];
//      LONG e_lfanew; -> relative offset of 
// } IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

// typedef struct _IMAGE_NT_HEADERS64 {
//     DWORD Signature;
//     IMAGE_FILE_HEADER FileHeader;
//     IMAGE_OPTIONAL_HEADER64 OptionalHeader;
// } IMAGE_NT_HEADERS64, *PIMAGE_NT_HEADERS64;

// typedef struct _IMAGE_FILE_HEADER {
//     WORD    Machine;   // 0x8864 for AMD64 | 0x14C for i386
//     WORD    NumberOfSections;
//     DWORD   TimeDateStamp;
//     DWORD   PointerToSymbolTable;
//     DWORD   NumberOfSymbols;
//     WORD    SizeOfOptionalHeader;
//     WORD    Characteristics;
// } IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

// typedef struct _IMAGE_OPTIONAL_HEADER64 {
//   WORD                 Magic; // IMAGE_NT_OPTIONAL_HDR64_MAGIC == 0x020B
//   BYTE                 MajorLinkerVersion;
//   BYTE                 MinorLinkerVersion;
//   DWORD                SizeOfCode;
//   DWORD                SizeOfInitializedData;
//   DWORD                SizeOfUninitializedData;
//   DWORD                AddressOfEntryPoint;
//   DWORD                BaseOfCode;
//   ULONGLONG            ImageBase;
//   DWORD                SectionAlignment;
//   DWORD                FileAlignment;
//   WORD                 MajorOperatingSystemVersion;
//   WORD                 MinorOperatingSystemVersion;
//   WORD                 MajorImageVersion;
//   WORD                 MinorImageVersion;
//   WORD                 MajorSubsystemVersion;
//   WORD                 MinorSubsystemVersion;
//   DWORD                Win32VersionValue;
//   DWORD                SizeOfImage;
//   DWORD                SizeOfHeaders;
//   DWORD                CheckSum;
//   WORD                 Subsystem;
//   WORD                 DllCharacteristics;
//   ULONGLONG            SizeOfStackReserve;
//   ULONGLONG            SizeOfStackCommit;
//   ULONGLONG            SizeOfHeapReserve;
//   ULONGLONG            SizeOfHeapCommit;
//   DWORD                LoaderFlags;
//   DWORD                NumberOfRvaAndSizes;
//   IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
// } IMAGE_OPTIONAL_HEADER64, *PIMAGE_OPTIONAL_HEADER64;

  //  IMAGE_NT_HEADERS64
  //    DWORD Signature;
  //    IMAGE_FILE_HEADER FileHeader;
  //    IMAGE_OPTIONAL_HEADER64 OptionalHeader;

//  HMODULE *hModule = 0;
//  DWORD nBytesNeeded = 0;
//  // DOS MZ Header
//  //   mMagic 0x5A4D
//  // 64bit PeHeader
//  //   mMagic == 0x020B
//  //   mMachine == 0x8664
//  //
//  // 64bit optional header
//  //   mMagic == 0x020B
//  //   mMagic == 0x020B
//  //
//  // Data Directories
//  // PE/COFF Image contains a pointer at offset 0x3C which points to the PE header
//  // The COFF File Header immediately follows this PE signature
//  //   
//  uint16_t mMagic; // 0x010b - PE32, 0x020b - PE32+ (64 bit)
//
//  // should not return 0 unless fail to enum any handles
//  if (EnumProcessModules(mhProcessHandle, hModule, 0, &nBytesNeeded) != 0)
//  {
//    debug_error("Receive 0 byte process module");
//    return 0;
//  }
//
//  hModule = (HMODULE*) malloc(nBytesNeeded);
//  if (hModule == 0)
//  {
//    debug_error("Fail to allocate %u bytes", nBytesNeeded);
//    return 0;
//  }
//
//  // should not fail
//  if (EnumProcessModules(mhProcessHandle, hModule, 0, &nBytesNeeded) == 0)
//  {
//    debug_error("Fail to enumerate process modules");
//    free(hModule);
//    return 0;
//  }
//
//  if (GetModuleBaseName(mhProcessHandle, h
//
//  hModule = (HMODULE*) malloc(nBytesNeeded);
//  remote_dll_base	= (UINT_PTR)RemoteModuleInfo.lpBaseOfDll;
//  debug_verbose("Found remote DLL base at 0x%p", (void*)remote_dll_base);
//

  //   typedef struct _IMAGE_NT_HEADERS64 {
  //   DWORD                   Signature;
  //   IMAGE_FILE_HEADER       FileHeader;
  //   IMAGE_OPTIONAL_HEADER64 OptionalHeader;
  // } IMAGE_NT_HEADERS64, *PIMAGE_NT_HEADERS64;

  // typedef struct _IMAGE_FILE_HEADER {
  //  WORD  Machine; // IMAGE_FILE_MACHINE_AMD64=0x8664 | IMAGE_FILE_MACHINE_I386=0x014C
  //  WORD  NumberOfSections;
  //  DWORD TimeDateStamp;
  //  DWORD PointerToSymbolTable;
  //  DWORD NumberOfSymbols;
  //  WORD  SizeOfOptionalHeader;
  //  WORD  Characteristics;
  //} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

  // typedef struct _IMAGE_OPTIONAL_HEADER64 {
  //   WORD                 Magic;
  //   BYTE                 MajorLinkerVersion;
  //   BYTE                 MinorLinkerVersion;
  //   DWORD                SizeOfCode;
  //   DWORD                SizeOfInitializedData;
  //   DWORD                SizeOfUninitializedData;
  //   DWORD                AddressOfEntryPoint;
  //   DWORD                BaseOfCode;
  //   ULONGLONG            ImageBase;
  //   DWORD                SectionAlignment;
  //   DWORD                FileAlignment;
  //   WORD                 MajorOperatingSystemVersion;
  //   WORD                 MinorOperatingSystemVersion;
  //   WORD                 MajorImageVersion;
  //   WORD                 MinorImageVersion;
  //   WORD                 MajorSubsystemVersion;
  //   WORD                 MinorSubsystemVersion;
  //   DWORD                Win32VersionValue;
  //   DWORD                SizeOfImage;
  //   DWORD                SizeOfHeaders;
  //   DWORD                CheckSum;
  //   WORD                 Subsystem;
  //   WORD                 DllCharacteristics;
  //   ULONGLONG            SizeOfStackReserve;
  //   ULONGLONG            SizeOfStackCommit;
  //   ULONGLONG            SizeOfHeapReserve;
  //   ULONGLONG            SizeOfHeapCommit;
  //   DWORD                LoaderFlags;
  //   DWORD                NumberOfRvaAndSizes;
  //   IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
  // } IMAGE_OPTIONAL_HEADER64, *PIMAGE_OPTIONAL_HEADER64;

  //   typedef struct _IMAGE_DATA_DIRECTORY {
  //   DWORD VirtualAddress;
  //   DWORD Size;
  // } IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

  // public unsafe struct IMAGE_EXPORT_DIRECTORY
  // {
  //     uint32_t Characteristics;
  //     uint32_t TimeDateStamp;
  //     uint16_t MajorVersion;
  //     uint16_t MinorVersion;
  //     uint32_t Name;
  //     uint32_t Base;
  //     uint32_t NumberOfFunctions;
  //     uint32_t NumberOfNames;
  //     uint32_t AddressOfFunctions; // RVA from base of image
  //     uint32_t AddressOfNames; // RVA from base of image
  //     uint32_t AddressOfNameOrdinals; // RVA from base of image
  // }

  //// IMAGE_EXPORT_DIRECTORY dllImageExportDirectory = {0};
  //// DWORD *pExportFunctions = nullptr;
  //// DWORD *pExportNames = nullptr;
  //// DWORD *pExportNameOrdinals = nullptr;


#endif
