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

#endif
