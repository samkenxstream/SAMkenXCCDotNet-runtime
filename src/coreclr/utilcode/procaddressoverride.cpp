#include "stdafx.h"
#include "procaddressoverride.h"

static GetProcAddressDirect_Callback g_ProcAddressCallback = nullptr;

void SetProcAddressDirectOverrideCallback(GetProcAddressDirect_Callback callback)
{
    g_ProcAddressCallback = callback;
}

GetProcAddressDirect_Callback GetProcAddressDirectOverrideCallback()
{
    return g_ProcAddressCallback;
}