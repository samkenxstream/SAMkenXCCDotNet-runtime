#include "common.h"
#ifdef PLATFORM_UNIX
#include "pal.h"
#else
#include "palclr_win.h"
#endif
#include "pathremapping.h"
#include "debugmacros.h"
#include "procaddressoverride.h"

const char*
mono_unity_remap_path (const char* path);

FARPROC
mono_PAL_GetProcAddressDirect_Callback(
        IN NATIVE_LIBRARY_HANDLE dl_handle,
        IN LPCSTR lpProcName,
        void* originalFunctionPtr);

RemapPathFunction g_RemapPathFunc = NULL;

static inline size_t
call_remapper(const char* path, char** buf)
{
	size_t len;

	if (!g_RemapPathFunc)
		return 0;

	*buf = NULL;
	len = g_RemapPathFunc(path, *buf, 0);

	if (len == 0)
		return 0;

	*buf = (char*)malloc (len);
	g_RemapPathFunc(path, *buf, len);

	return len;
}

const char* mono_unity_remap_path (const char* path)
{
	char* path_remap = NULL;
	call_remapper (path, &path_remap);

	return path_remap;
}

void register_path_remapper (RemapPathFunction func)
{
    g_RemapPathFunc = func;
}

struct ProcOverride;

ProcOverride* g_ProcOverridesList = NULL;

struct ProcOverride
{
    const char* name;
    void* func;
    void** originalFunctionPtr;
    ProcOverride* next;

    ProcOverride(const char* _name, void* _func, void** _originalFunctionPtr)
        : name(_name), func(_func), originalFunctionPtr(_originalFunctionPtr)
    {
        next = g_ProcOverridesList;
        g_ProcOverridesList = this;
    }
};

FARPROC
mono_PAL_GetProcAddressDirect_Callback(
        IN NATIVE_LIBRARY_HANDLE dl_handle,
        IN LPCSTR lpProcName,
        void* originalFunctionPtr)
{
    ProcOverride *p = g_ProcOverridesList;
    while(p)
    {
        if (strcmp(p->name, lpProcName) == 0)
        {
            if (p->originalFunctionPtr != NULL && *p->originalFunctionPtr == NULL)
                *p->originalFunctionPtr = originalFunctionPtr;
            return (FARPROC)p->func;
        }
        p = p->next;
    }

    return NULL;
}

void SetupMonoProcOverrides()
{
    SetProcAddressDirectOverrideCallback(mono_PAL_GetProcAddressDirect_Callback);
}

#define OVERRIDE_FUNCTION(retType,name,args)\
retType (*name##_OriginalFunc) args = NULL;\
retType name##_OverrideFunc args;\
ProcOverride g_##name##_Override(#name, (void*)name##_OverrideFunc, (void**)&name##_OriginalFunc);\
retType name##_OverrideFunc args\

#define OVERRIDE_FUNCTION_PATH_REMAP_ARGS(retType,name,args,args_call)\
OVERRIDE_FUNCTION(retType,name,args)\
{\
	const char *path_remapped = NULL;\
	if ((path_remapped = mono_unity_remap_path (path)))\
		path = path_remapped;\
    auto retval = name##_OriginalFunc args_call;\
	free ((void*)path_remapped);\
    return retval;\
}\


#define OVERRIDE_FUNCTION_PATH_REMAP_1(retType,name,arg1Type,arg1) OVERRIDE_FUNCTION_PATH_REMAP_ARGS(retType, name, (arg1Type arg1), (arg1))
#define OVERRIDE_FUNCTION_PATH_REMAP_2(retType,name,arg1Type,arg1,arg2Type,arg2) OVERRIDE_FUNCTION_PATH_REMAP_ARGS(retType, name, (arg1Type arg1, arg2Type arg2), (arg1, arg2))
#define OVERRIDE_FUNCTION_PATH_REMAP_3(retType,name,arg1Type,arg1,arg2Type,arg2,arg3Type,arg3) OVERRIDE_FUNCTION_PATH_REMAP_ARGS(retType, name, (arg1Type arg1, arg2Type arg2, arg3Type arg3), (arg1, arg2, arg3))

#define CONCATENATE(arg1, arg2)   CONCATENATE1(arg1, arg2)
#define CONCATENATE1(arg1, arg2)  CONCATENATE2(arg1, arg2)
#define CONCATENATE2(arg1, arg2)  arg1##arg2


#if PLATFORM_UNIX
#define GET_ARG_COUNT(...) OVERRIDE_FUNCTION_PATH_REMAP_NARG_(__VA_ARGS__, OVERRIDE_FUNCTION_PATH_REMAP_RSEQ_N())
#define OVERRIDE_FUNCTION_PATH_REMAP_NARG_(...) OVERRIDE_FUNCTION_PATH_REMAP_ARG_N(__VA_ARGS__) 
#define OVERRIDE_FUNCTION_PATH_REMAP_ARG_N(_1, _1name, _2, _2name, _3, _3name, _4, _4name, _5, _5name, _6, _6name, _7, _7name, _8, _8name, N, ...) N 
#define OVERRIDE_FUNCTION_PATH_REMAP_RSEQ_N() 8, error, 7, error, 6, error, 5, error, 4, error, 3, error, 2, error, 1, error, 0
#else
#define GET_ARG_COUNT(...)  INTERNAL_EXPAND_ARGS_PRIVATE(INTERNAL_ARGS_AUGMENTER(__VA_ARGS__))
#define INTERNAL_ARGS_AUGMENTER(...) unused, __VA_ARGS__
#define INTERNAL_EXPAND(x) x
#define INTERNAL_EXPAND_ARGS_PRIVATE(...) INTERNAL_EXPAND(INTERNAL_GET_ARG_COUNT_PRIVATE(__VA_ARGS__, error8,8, error7, 7, error6, 6, error5, 5, error4, 4, error3, 3, error2, 2, error1, 1, error0, 0))
#define INTERNAL_GET_ARG_COUNT_PRIVATE(_0, _0_name, _1, _1name, _2, _2name, _3, _3name, _4, _4name, _5, _5name, _6, _6name, _7, _7name, _8, _8name, count, ...) count
#endif

#define OVERRIDE_FUNCTION_PATH_REMAP_(N, retType, name, ...) CONCATENATE(OVERRIDE_FUNCTION_PATH_REMAP_, N)(retType, name, __VA_ARGS__)
#define OVERRIDE_FUNCTION_PATH_REMAP(retType, name, ...) OVERRIDE_FUNCTION_PATH_REMAP_(GET_ARG_COUNT(__VA_ARGS__), retType, name, __VA_ARGS__)

OVERRIDE_FUNCTION_PATH_REMAP(int32_t, SystemNative_Stat, const char*, path, void*, output)
OVERRIDE_FUNCTION_PATH_REMAP(intptr_t, SystemNative_Open, const char*, path, int32_t, flags, int32_t, mode)
