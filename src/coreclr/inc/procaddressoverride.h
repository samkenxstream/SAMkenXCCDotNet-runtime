#ifndef PROCADDRESSOVERRIDE_H_
#define PROCADDRESSOVERRIDE_H_

#ifdef PLATFORM_UNIX
typedef 
FARPROC
(*GetProcAddressDirect_Callback)(
        IN NATIVE_LIBRARY_HANDLE dl_handle,
        IN LPCSTR lpProcName,
        void* originalFunctionPtr);
#else
typedef 
FARPROC
(*GetProcAddressDirect_Callback)(
        HMODULE dl_handle,
        const char* lpProcName,
        void* originalFunctionPtr);
#endif // PLATFORM_UNIX

void SetProcAddressDirectOverrideCallback(GetProcAddressDirect_Callback callback);
GetProcAddressDirect_Callback GetProcAddressDirectOverrideCallback();

#endif // PROCADDRESSOVERRIDE_H_
