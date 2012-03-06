#include "Callstack.h"
#include <windows.h>
#include <imagehlp.h>

#pragma comment(lib, "imagehlp.lib")


namespace {
    typedef USHORT (WINAPI *RtlCaptureStackBackTraceDef)(ULONG FramesToSkip, ULONG FramesToCapture, PVOID *BackTrace, PULONG BackTraceHash);

    RtlCaptureStackBackTraceDef RtlCaptureStackBackTraceProc = NULL;
    HMODULE g_kernel32 = NULL;
} // namespace


bool InitializeSymbol()
{
    g_kernel32 = ::LoadLibraryA("kernel32.dll");
    if(!g_kernel32) {
        return false;
    }
    RtlCaptureStackBackTraceProc = (RtlCaptureStackBackTraceDef)::GetProcAddress(g_kernel32, "RtlCaptureStackBackTrace");

    if(!::SymInitialize(::GetCurrentProcess(), NULL, TRUE)) {
        return false;
    }
    ::SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);

    return true;
}

void FinalizeSymbol()
{
    ::SymCleanup(::GetCurrentProcess());
    ::FreeLibrary(g_kernel32);
    RtlCaptureStackBackTraceProc = NULL;
    g_kernel32 = NULL;
}


int GetCallstack(void **callstack, int callstack_size, int skip_size)
{
    if(RtlCaptureStackBackTraceProc) {
        return RtlCaptureStackBackTraceProc(skip_size, callstack_size, callstack, NULL);
    }
    return 0;
}

std::string AddressToString(void *address)
{
#ifdef _WIN64
    typedef DWORD64 DWORDX;
    typedef PDWORD64 PDWORDX;
#else
    typedef DWORD DWORDX;
    typedef PDWORD PDWORDX;
#endif

    char buf[256];
    HANDLE process = ::GetCurrentProcess();
    IMAGEHLP_MODULE imageModule = { sizeof(IMAGEHLP_MODULE) };
    IMAGEHLP_LINE line ={sizeof(IMAGEHLP_LINE)};
    DWORDX dispSym = 0;
    DWORD dispLine = 0;

    char symbolBuffer[sizeof(IMAGEHLP_SYMBOL) + MAX_PATH] = {0};
    IMAGEHLP_SYMBOL * imageSymbol = (IMAGEHLP_SYMBOL*)symbolBuffer;
    imageSymbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
    imageSymbol->MaxNameLength = MAX_PATH;

    if(!::SymGetModuleInfo(process, (DWORDX)address, &imageModule)) {
        sprintf_s(buf, "0x%p\n", address);
    }
    else if(!::SymGetSymFromAddr(process, (DWORDX)address, &dispSym, imageSymbol)) {
        sprintf_s(buf, "%s 0x%p\n", imageModule.ModuleName, address);
    }
    else if(!::SymGetLineFromAddr(process, (DWORDX)address, &dispLine, &line)) {
        sprintf_s(buf, "%s!%s + 0x%x\n", imageModule.ModuleName, imageSymbol->Name, ((size_t)address-(size_t)imageSymbol->Address) );
        return buf;
    }
    else {
        sprintf_s(buf, "%s(%d): %s!%s + 0x%x\n", line.FileName, line.LineNumber,
            imageModule.ModuleName, imageSymbol->Name, ((size_t)address-(size_t)imageSymbol->Address));
    }
    return buf;
}

std::string CallstackToString(void **callstack, int callstack_size, int clamp_head, int clamp_tail, const char *indent)
{
    std::string tmp;
    int begin = std::max<int>(0, clamp_head);
    int end = std::max<int>(0, callstack_size-clamp_tail);
    for(int i=begin; i<end; ++i) {
        tmp += indent;
        tmp += AddressToString(callstack[i]);
    }
    return tmp;
}
