#include "../D3D11HookInterface.h"
#include "D3D11LeakChecker.h"
#include "Callstack.h"
#include <algorithm>
#include <map>
#include <vector>


struct CallStack
{
    void *stack[D3D11LEAKCHECKER_MAX_CALLSTACK_SIZE];
    size_t size;
    size_t frame;

    CallStack() : size(0), frame(0)
    {
        std::fill_n(stack, _countof(stack), (void*)NULL);
    }

    void getCurrentCallstack();
    std::string genKeyString() const;
};

struct ReferenceInfo
{
    CallStack stack;
    size_t count;

    ReferenceInfo() : count(0)
    {}
};

struct TraceInfo
{
    void *address;
    void **vtable;
    size_t ref_count;
    CallStack trace_create;
#ifdef D3D11LEAKCHECKER_ENABLE_ADDREF_TRACE
    typedef std::map<std::string, ReferenceInfo> ReferenceTable;
    ReferenceTable trace_addref;
    ReferenceTable trace_release;
#endif // D3D11LEAKCHECKER_ENABLE_ADDREF_TRACE

    TraceInfo() : address(NULL), vtable(NULL), ref_count(1)
    {
    }

    void handleAddRef(ULONG rc);
    void handleRelease(ULONG rc);
    void printLeakInfo();
};

typedef std::map<IUnknown*, TraceInfo> TraceTable;


namespace {

TraceTable g_trace_info;
size_t g_frame = 0;
bool g_opt_hook_add = false;
bool g_opt_initialize_symbols = false;

} // namespace


void CallStack::getCurrentCallstack()
{
    frame = g_frame;
    size = GetCallstack(stack, _countof(stack), 0);
}

std::string CallStack::genKeyString() const
{
    return std::string((char*)stack, sizeof(void*)*size);
}

void TraceInfo::handleAddRef(ULONG rc)
{
#ifdef D3D11LEAKCHECKER_ENABLE_ADDREF_TRACE
    ref_count = rc;
    CallStack stack;
    stack.getCurrentCallstack();
    std::string key = stack.genKeyString();
    ReferenceInfo &ri = trace_addref[key];
    if(ri.count++ == 0) {
        ri.stack = stack;
    }
#endif // D3D11LEAKCHECKER_ENABLE_ADDREF_TRACE
}

void TraceInfo::handleRelease(ULONG rc)
{
#ifdef D3D11LEAKCHECKER_ENABLE_ADDREF_TRACE
    ref_count = rc;
    CallStack stack;
    stack.getCurrentCallstack();
    std::string key = stack.genKeyString();
    ReferenceInfo &ri = trace_release[key];
    if(ri.count++ == 0) {
        ri.stack = stack;
    }
#endif // D3D11LEAKCHECKER_ENABLE_ADDREF_TRACE
}

void TraceInfo::printLeakInfo()
{
    // コールスタックの必要な部分だけ抽出
    // コンパイルオプションで変動するのでこの指定の仕方はよくないかも…
    int c_head=0, c_tail=0;
#ifdef _WIN64
#ifdef _DEBUG
    c_head = 2; c_tail = 4;
#else
    c_head = 0; c_tail = 3;
#endif
#else
#ifdef _DEBUG
    c_head = 2; c_tail = 5;
#else
    c_tail = 0; c_tail = 4;
#endif
#endif

    std::string str;
    char buf[128];
    sprintf_s(buf, "0x%p created here [frame %d]", address, trace_create.frame);
    str += buf;
#ifdef D3D11LEAKCHECKER_ENABLE_ADDREF_TRACE
    sprintf_s(buf, "[ref %d]", ref_count);
    str += buf;
#endif // D3D11LEAKCHECKER_ENABLE_ADDREF_TRACE
    str += "\n";
    str += CallstackToString(trace_create.stack, static_cast<int>(trace_create.size), c_head, c_tail, "    ");

#ifdef D3D11LEAKCHECKER_ENABLE_ADDREF_TRACE
    for(ReferenceTable::iterator i=trace_addref.begin(); i!=trace_addref.end(); ++i) {
        ReferenceInfo &ri = i->second;
        sprintf_s(buf, "  AddRef() %d times\n", ri.count);
        str += buf;
        str += CallstackToString(ri.stack.stack, static_cast<int>(ri.stack.size), c_head+1, c_tail, "    ");
    }
    for(ReferenceTable::iterator i=trace_release.begin(); i!=trace_release.end(); ++i) {
        ReferenceInfo &ri = i->second;
        sprintf_s(buf, "  Release() %d times\n", ri.count);
        str += buf;
        str += CallstackToString(ri.stack.stack, static_cast<int>(ri.stack.size), c_head+1, c_tail, "    ");
    }
#endif // D3D11LEAKCHECKER_ENABLE_ADDREF_TRACE
    str += "\n";
    OutputDebugStringA(str.c_str());
}


template<class T>
inline void** get_vtable(T _this)
{
    return ((void***)_this)[0];
}

template<class T> class TLeakChecker;
class D3D11DeviceLeakChecked;
class DXGISwapChainLeakChecked;

template<class T> struct GetLeakCheckedTypeD3D11 { typedef TLeakChecker<typename D3D11GetHookType<T>::result_type> result_type; };
template<> struct GetLeakCheckedTypeD3D11<ID3D11Device> { typedef D3D11DeviceLeakChecked result_type; };
template<> struct GetLeakCheckedTypeD3D11<IDXGISwapChain> { typedef DXGISwapChainLeakChecked result_type; };

template<class T>
void AddD3D11Object(T *v)
{
    typedef typename GetLeakCheckedTypeD3D11<T>::result_type HookType;
    if(g_opt_hook_add) {
        D3D11AddHook<HookType>(v);
    }
    else {
        D3D11SetHook<HookType>(v);
    }

    HookType hooker;
    TraceInfo &ti = g_trace_info[v];
    ti = TraceInfo(); // 古い内部オブジェクトの情報が残ってる可能性があるので掃除
    ti.address = v;
    ti.vtable = get_vtable(&hooker);
    ti.trace_create.frame = g_frame;
    ti.trace_create.size = GetCallstack(ti.trace_create.stack, D3D11LEAKCHECKER_MAX_CALLSTACK_SIZE, 0);
}


template<class T>
class TLeakChecker : public T
{
typedef T super;
public:
    virtual ULONG STDMETHODCALLTYPE AddRef(void)
    {
        ULONG r = super::AddRef();
        TraceTable::iterator i = g_trace_info.find(this);
        if(i!=g_trace_info.end()) {
            i->second.handleAddRef(r);
        }
        return r;
    }

    virtual ULONG STDMETHODCALLTYPE Release(void)
    {
        ULONG r = super::Release();
        TraceTable::iterator i = g_trace_info.find(this);
        if(i!=g_trace_info.end()) {
            if(r==0) { g_trace_info.erase(i); }
            else { i->second.handleRelease(r); }
        }

        return r;
    }
};


class D3D11DeviceLeakChecked : public TLeakChecker<D3D11DeviceHook>
{
typedef D3D11DeviceHook super;
public:
    virtual HRESULT STDMETHODCALLTYPE CreateBuffer( 
        const D3D11_BUFFER_DESC *pDesc,
        const D3D11_SUBRESOURCE_DATA *pInitialData,
        ID3D11Buffer **ppBuffer)
    {
        HRESULT r = super::CreateBuffer(pDesc, pInitialData, ppBuffer);
        if(r==S_OK) { AddD3D11Object(*ppBuffer); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateTexture1D( 
        const D3D11_TEXTURE1D_DESC *pDesc,
        const D3D11_SUBRESOURCE_DATA *pInitialData,
        ID3D11Texture1D **ppTexture1D)
    {
        HRESULT r = super::CreateTexture1D(pDesc, pInitialData, ppTexture1D);
        if(r==S_OK) { AddD3D11Object(*ppTexture1D); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateTexture2D( 
        const D3D11_TEXTURE2D_DESC *pDesc,
        const D3D11_SUBRESOURCE_DATA *pInitialData,
        ID3D11Texture2D **ppTexture2D)
    {
        HRESULT r = super::CreateTexture2D(pDesc, pInitialData, ppTexture2D);
        if(r==S_OK) { AddD3D11Object(*ppTexture2D); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateTexture3D( 
        const D3D11_TEXTURE3D_DESC *pDesc,
        const D3D11_SUBRESOURCE_DATA *pInitialData,
        ID3D11Texture3D **ppTexture3D)
    {
        HRESULT r = super::CreateTexture3D(pDesc, pInitialData, ppTexture3D);
        if(r==S_OK) { AddD3D11Object(*ppTexture3D); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateShaderResourceView( 
        ID3D11Resource *pResource,
        const D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc,
        ID3D11ShaderResourceView **ppSRView)
    {
        HRESULT r = super::CreateShaderResourceView(pResource, pDesc, ppSRView);
        if(r==S_OK) { AddD3D11Object(*ppSRView); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateUnorderedAccessView( 
        ID3D11Resource *pResource,
        const D3D11_UNORDERED_ACCESS_VIEW_DESC *pDesc,
        ID3D11UnorderedAccessView **ppUAView)
    {
        HRESULT r = super::CreateUnorderedAccessView(pResource, pDesc, ppUAView);
        if(r==S_OK) { AddD3D11Object(*ppUAView); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateRenderTargetView( 
        ID3D11Resource *pResource,
        const D3D11_RENDER_TARGET_VIEW_DESC *pDesc,
        ID3D11RenderTargetView **ppRTView)
    {
        HRESULT r = super::CreateRenderTargetView(pResource, pDesc, ppRTView);
        if(r==S_OK) { AddD3D11Object(*ppRTView); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilView( 
        ID3D11Resource *pResource,
        const D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc,
        ID3D11DepthStencilView **ppDepthStencilView)
    {
        HRESULT r = super::CreateDepthStencilView(pResource, pDesc, ppDepthStencilView);
        if(r==S_OK) { AddD3D11Object(*ppDepthStencilView); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateInputLayout( 
        const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,
        UINT NumElements,
        const void *pShaderBytecodeWithInputSignature,
        SIZE_T BytecodeLength,
        ID3D11InputLayout **ppInputLayout)
    {
        HRESULT r = super::CreateInputLayout(pInputElementDescs, NumElements, pShaderBytecodeWithInputSignature, BytecodeLength, ppInputLayout);
        if(r==S_OK) { AddD3D11Object(*ppInputLayout); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateVertexShader( 
        const void *pShaderBytecode,
        SIZE_T BytecodeLength,
        ID3D11ClassLinkage *pClassLinkage,
        ID3D11VertexShader **ppVertexShader)
    {
        HRESULT r = super::CreateVertexShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppVertexShader);
        if(r==S_OK) { AddD3D11Object(*ppVertexShader); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateGeometryShader( 
        const void *pShaderBytecode,
        SIZE_T BytecodeLength,
        ID3D11ClassLinkage *pClassLinkage,
        ID3D11GeometryShader **ppGeometryShader)
    {
        HRESULT r = super::CreateGeometryShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppGeometryShader);
        if(r==S_OK) { AddD3D11Object(*ppGeometryShader); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateGeometryShaderWithStreamOutput( 
        const void *pShaderBytecode,
        SIZE_T BytecodeLength,
        const D3D11_SO_DECLARATION_ENTRY *pSODeclaration,
        UINT NumEntries,
        const UINT *pBufferStrides,
        UINT NumStrides,
        UINT RasterizedStream,
        ID3D11ClassLinkage *pClassLinkage,
        ID3D11GeometryShader **ppGeometryShader)
    {
        HRESULT r = super::CreateGeometryShaderWithStreamOutput(pShaderBytecode, BytecodeLength, pSODeclaration, NumEntries, pBufferStrides, NumStrides, RasterizedStream, pClassLinkage, ppGeometryShader);
        if(r==S_OK) { AddD3D11Object(*ppGeometryShader); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreatePixelShader( 
        const void *pShaderBytecode,
        SIZE_T BytecodeLength,
        ID3D11ClassLinkage *pClassLinkage,
        ID3D11PixelShader **ppPixelShader)
    {
        HRESULT r = super::CreatePixelShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppPixelShader);
        if(r==S_OK) { AddD3D11Object(*ppPixelShader); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateHullShader( 
        const void *pShaderBytecode,
        SIZE_T BytecodeLength,
        ID3D11ClassLinkage *pClassLinkage,
        ID3D11HullShader **ppHullShader)
    {
        HRESULT r = super::CreateHullShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppHullShader);
        if(r==S_OK) { AddD3D11Object(*ppHullShader); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateDomainShader( 
        const void *pShaderBytecode,
        SIZE_T BytecodeLength,
        ID3D11ClassLinkage *pClassLinkage,
        ID3D11DomainShader **ppDomainShader)
    {
        HRESULT r = super::CreateDomainShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppDomainShader);
        if(r==S_OK) { AddD3D11Object(*ppDomainShader); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateComputeShader( 
        const void *pShaderBytecode,
        SIZE_T BytecodeLength,
        ID3D11ClassLinkage *pClassLinkage,
        ID3D11ComputeShader **ppComputeShader)
    {
        HRESULT r = super::CreateComputeShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppComputeShader);
        if(r==S_OK) { AddD3D11Object(*ppComputeShader); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateClassLinkage( 
        ID3D11ClassLinkage **ppLinkage)
    {
        HRESULT r = super::CreateClassLinkage(ppLinkage);
        if(r==S_OK) { AddD3D11Object(*ppLinkage); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateBlendState( 
        const D3D11_BLEND_DESC *pBlendStateDesc,
        ID3D11BlendState **ppBlendState)
    {
        HRESULT r = super::CreateBlendState(pBlendStateDesc, ppBlendState);
        if(r==S_OK) { AddD3D11Object(*ppBlendState); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilState( 
        const D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc,
        ID3D11DepthStencilState **ppDepthStencilState)
    {
        HRESULT r = super::CreateDepthStencilState(pDepthStencilDesc, ppDepthStencilState);
        if(r==S_OK) { AddD3D11Object(*ppDepthStencilState); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateRasterizerState( 
        const D3D11_RASTERIZER_DESC *pRasterizerDesc,
        ID3D11RasterizerState **ppRasterizerState)
    {
        HRESULT r = super::CreateRasterizerState(pRasterizerDesc, ppRasterizerState);
        if(r==S_OK) { AddD3D11Object(*ppRasterizerState); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateSamplerState( 
        const D3D11_SAMPLER_DESC *pSamplerDesc,
        ID3D11SamplerState **ppSamplerState)
    {
        HRESULT r = super::CreateSamplerState(pSamplerDesc, ppSamplerState);
        if(r==S_OK) { AddD3D11Object(*ppSamplerState); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateQuery( 
        const D3D11_QUERY_DESC *pQueryDesc,
        ID3D11Query **ppQuery)
    {
        HRESULT r = super::CreateQuery(pQueryDesc, ppQuery);
        if(r==S_OK) { AddD3D11Object(*ppQuery); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreatePredicate( 
        const D3D11_QUERY_DESC *pPredicateDesc,
        ID3D11Predicate **ppPredicate)
    {
        HRESULT r = super::CreatePredicate(pPredicateDesc, ppPredicate);
        if(r==S_OK) { AddD3D11Object(*ppPredicate); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateCounter( 
        const D3D11_COUNTER_DESC *pCounterDesc,
        ID3D11Counter **ppCounter)
    {
        HRESULT r = super::CreateCounter(pCounterDesc, ppCounter);
        if(r==S_OK) { AddD3D11Object(*ppCounter); }
        return r;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateDeferredContext( 
        UINT ContextFlags,
        ID3D11DeviceContext **ppDeferredContext)
    {
        HRESULT r = super::CreateDeferredContext(ContextFlags, ppDeferredContext);
        if(r==S_OK) { AddD3D11Object(*ppDeferredContext); }
        return r;
    }
};

class DXGISwapChainLeakChecked : public TLeakChecker<DXGISwapChainHook>
{
typedef DXGISwapChainHook super;
public:
    virtual HRESULT STDMETHODCALLTYPE Present( 
        UINT SyncInterval,
        UINT Flags)
    {
        ++g_frame;
        return super::Present(SyncInterval, Flags);
    }
};


void D3D11LeakCheckerInitialize(IDXGISwapChain *pSwapChain, ID3D11Device *pDevice, int opt)
{
    if((opt & D3D11LC_HOOK_ADD)!=0) { g_opt_hook_add=true; }
    if((opt & D3D11LC_INIT_SYMBOLS)!=0) { g_opt_initialize_symbols=true; }

    if(g_opt_initialize_symbols) { InitializeSymbol(); }
    AddD3D11Object(pSwapChain);
    AddD3D11Object(pDevice);
}

void D3D11LeakCheckerFinalize()
{
    for(TraceTable::iterator i=g_trace_info.begin(); i!=g_trace_info.end(); ++i) {
        D3D11RemoveHook(i->first, i->second.vtable);
    }

    if(g_opt_initialize_symbols) { FinalizeSymbol(); }
}

void D3D11LeakCheckerPrintLeakInfo()
{
    if(g_trace_info.empty()) {
        OutputDebugStringA("D3D11LeakCheckerPrintLeakInfo(): no leak detected.\n");
    }
    else {
        OutputDebugStringA("D3D11LeakCheckerPrintLeakInfo(): leak detected.\n");
        for(TraceTable::iterator i=g_trace_info.begin(); i!=g_trace_info.end(); ++i) {
            i->second.printLeakInfo();
        }
    }
}
