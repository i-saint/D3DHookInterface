#include "DirectX11HookInterface.h"
#include <vector>
#include <map>


template<class T>
inline void** get_vtable(T _this)
{
    return ((void***)_this)[0];
}

template<class T>
inline void set_vtable(T _this, void **vtable)
{
    ((void***)_this)[0] = vtable;
}


// 多重 hook を実現するための vtable stack
class VTableStack
{
private:
    std::vector<void**> m_vtables;
    int m_depth;

public:
    VTableStack() : m_depth(-1) {}

    size_t getStackSize() const { return m_vtables.size(); }

    void pushVTable(void **v)
    {
        m_vtables.push_back(v);
        m_depth = int(m_vtables.size())-1;
    }

    void popVTable()
    {
        m_vtables.pop_back();
        m_depth = int(m_vtables.size())-1;
    }

    int getDepth() const { return m_depth; }
    void** up()     { return --m_depth >= 0 ? m_vtables[m_depth] : NULL; }
    void** down()   { return ++m_depth >= 0 ? m_vtables[m_depth] : NULL; }
};

namespace {
    std::map<void*, VTableStack> g_vtables;

    void _SetD3D11HookInterface(void *pTarget, void *pHook)
    {
        VTableStack &vs = g_vtables[pTarget];
        if(vs.getStackSize()==0) {
            vs.pushVTable(get_vtable(pTarget));
        }
        vs.pushVTable(get_vtable(pHook));
        set_vtable(pTarget, get_vtable(pHook));
    }
} // namespace 

void _SetDXGISwapChainHookInterface(IDXGISwapChain *pTarget, IDXGISwapChain *pHook)                 { _SetD3D11HookInterface(pTarget, pHook); }
void _SetD3D11DeviceHookInterface(ID3D11Device *pTarget, ID3D11Device *pHook)                       { _SetD3D11HookInterface(pTarget, pHook); }
void _SetD3D11DeviceContextHookInterface(ID3D11DeviceContext *pTarget, ID3D11DeviceContext *pHook)  { _SetD3D11HookInterface(pTarget, pHook); }

void _SetD3D11BufferHookInterface(ID3D11Buffer *pTarget, ID3D11Buffer *pHook)           { _SetD3D11HookInterface(pTarget, pHook); }
void _SetD3D11Texture1DHookInterface(ID3D11Texture1D *pTarget, ID3D11Texture1D *pHook)  { _SetD3D11HookInterface(pTarget, pHook); }
void _SetD3D11Texture2DHookInterface(ID3D11Texture2D *pTarget, ID3D11Texture2D *pHook)  { _SetD3D11HookInterface(pTarget, pHook); }
void _SetD3D11Texture3DHookInterface(ID3D11Texture3D *pTarget, ID3D11Texture3D *pHook)  { _SetD3D11HookInterface(pTarget, pHook); }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      template implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
HRESULT STDMETHODCALLTYPE TUnknownHookInterface<T>::QueryInterface( REFIID riid, __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = QueryInterface(riid, ppvObject);
    set_vtable(this, vs.down());
    return r;
}

template<class T>
ULONG STDMETHODCALLTYPE TUnknownHookInterface<T>::AddRef( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    ULONG r = AddRef();
    set_vtable(this, vs.down());
    return r;
}

template<class T>
ULONG STDMETHODCALLTYPE TUnknownHookInterface<T>::Release( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    ULONG r = Release();
    if(r==0) {
        if(vs.getDepth()==0) { g_vtables.erase(this); }
    }
    else {
        set_vtable(this, vs.down());
    }
    return r;
}


template<class T>
HRESULT STDMETHODCALLTYPE TDXGIObjectHookInterface<T>::SetPrivateData( REFGUID Name, UINT DataSize, const void *pData )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = SetPrivateData(Name, DataSize, pData);
    set_vtable(this, vs.down());
    return r;
}

template<class T>
HRESULT STDMETHODCALLTYPE TDXGIObjectHookInterface<T>::SetPrivateDataInterface( REFGUID Name, const IUnknown *pUnknown )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = SetPrivateDataInterface(Name, pUnknown);
    set_vtable(this, vs.down());
    return r;
}

template<class T>
HRESULT STDMETHODCALLTYPE TDXGIObjectHookInterface<T>::GetPrivateData( REFGUID Name, UINT *pDataSize, void *pData )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetPrivateData(Name, pDataSize, pData);
    set_vtable(this, vs.down());
    return r;
}

template<class T>
HRESULT STDMETHODCALLTYPE TDXGIObjectHookInterface<T>::GetParent( REFIID riid, void **ppParent )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetParent(riid, ppParent);
    set_vtable(this, vs.down());
    return r;
}


template<class T>
HRESULT STDMETHODCALLTYPE TDXGIDeviceSubObjectHookInterface<T>::GetDevice( REFIID riid, void **ppDevice )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetDevice(riid, ppDevice);
    set_vtable(this, vs.down());
    return r;
}


template<class T>
void STDMETHODCALLTYPE TD3D11DeviceChildHookInterface<T>::GetDevice( ID3D11Device **ppDevice )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetDevice(ppDevice);
    set_vtable(this, vs.down());
}

template<class T>
HRESULT STDMETHODCALLTYPE TD3D11DeviceChildHookInterface<T>::GetPrivateData( REFGUID guid, UINT *pDataSize, void *pData )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetPrivateData(guid, pDataSize, pData);
    set_vtable(this, vs.down());
    return r;
}

template<class T>
HRESULT STDMETHODCALLTYPE TD3D11DeviceChildHookInterface<T>::SetPrivateData( REFGUID guid, UINT DataSize, const void *pData )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = SetPrivateData(guid, DataSize, pData);
    set_vtable(this, vs.down());
    return r;
}

template<class T>
HRESULT STDMETHODCALLTYPE TD3D11DeviceChildHookInterface<T>::SetPrivateDataInterface( REFGUID guid, const IUnknown *pData )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = SetPrivateDataInterface(guid, pData);
    set_vtable(this, vs.down());
    return r;
}


template<class T>
void STDMETHODCALLTYPE TD3D11ResourceHookInterface<T>::GetType( D3D11_RESOURCE_DIMENSION *pResourceDimension )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetType(pResourceDimension);
    set_vtable(this, vs.down());
}

template<class T>
void STDMETHODCALLTYPE TD3D11ResourceHookInterface<T>::SetEvictionPriority( UINT EvictionPriority )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    SetEvictionPriority(EvictionPriority);
    set_vtable(this, vs.down());
}

template<class T>
UINT STDMETHODCALLTYPE TD3D11ResourceHookInterface<T>::GetEvictionPriority( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    UINT r = GetEvictionPriority();
    set_vtable(this, vs.down());
    return r;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      DXGISwapChainHookInterface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template TUnknownHookInterface<IDXGISwapChain>;
template TDXGIObjectHookInterface<IDXGISwapChain>;
template TDXGIDeviceSubObjectHookInterface<IDXGISwapChain>;

HRESULT STDMETHODCALLTYPE DXGISwapChainHookInterface::Present( UINT SyncInterval, UINT Flags )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = Present(SyncInterval, Flags);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE DXGISwapChainHookInterface::GetBuffer( UINT Buffer, REFIID riid, void **ppSurface )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetBuffer(Buffer, riid, ppSurface);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE DXGISwapChainHookInterface::SetFullscreenState( BOOL Fullscreen, IDXGIOutput *pTarget )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = SetFullscreenState(Fullscreen, pTarget);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE DXGISwapChainHookInterface::GetFullscreenState( BOOL *pFullscreen, IDXGIOutput **ppTarget )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetFullscreenState(pFullscreen, ppTarget);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE DXGISwapChainHookInterface::GetDesc( DXGI_SWAP_CHAIN_DESC *pDesc )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetDesc(pDesc);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE DXGISwapChainHookInterface::ResizeBuffers( UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = ResizeBuffers(BufferCount, Width, Height, NewFormat, SwapChainFlags);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE DXGISwapChainHookInterface::ResizeTarget( const DXGI_MODE_DESC *pNewTargetParameters )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = ResizeTarget(pNewTargetParameters);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE DXGISwapChainHookInterface::GetContainingOutput( IDXGIOutput **ppOutput )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetContainingOutput(ppOutput);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE DXGISwapChainHookInterface::GetFrameStatistics( DXGI_FRAME_STATISTICS *pStats )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetFrameStatistics(pStats);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE DXGISwapChainHookInterface::GetLastPresentCount( UINT *pLastPresentCount )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetLastPresentCount(pLastPresentCount);
    set_vtable(this, vs.down());
    return r;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      D3D11DeviceHookInterface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template TUnknownHookInterface<ID3D11Device>;

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreateBuffer( const D3D11_BUFFER_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Buffer **ppBuffer )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateBuffer(pDesc, pInitialData, ppBuffer);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreateTexture1D( const D3D11_TEXTURE1D_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Texture1D **ppTexture1D )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateTexture1D(pDesc, pInitialData, ppTexture1D);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreateTexture2D( const D3D11_TEXTURE2D_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Texture2D **ppTexture2D )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateTexture2D(pDesc, pInitialData, ppTexture2D);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreateTexture3D( const D3D11_TEXTURE3D_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Texture3D **ppTexture3D )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateTexture3D(pDesc, pInitialData, ppTexture3D);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreateShaderResourceView( ID3D11Resource *pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc, ID3D11ShaderResourceView **ppSRView )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateShaderResourceView(pResource, pDesc, ppSRView);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreateUnorderedAccessView( ID3D11Resource *pResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC *pDesc, ID3D11UnorderedAccessView **ppUAView )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateUnorderedAccessView(pResource, pDesc, ppUAView);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreateRenderTargetView( ID3D11Resource *pResource, const D3D11_RENDER_TARGET_VIEW_DESC *pDesc, ID3D11RenderTargetView **ppRTView )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateRenderTargetView(pResource, pDesc, ppRTView);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreateDepthStencilView( ID3D11Resource *pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc, ID3D11DepthStencilView **ppDepthStencilView )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateDepthStencilView(pResource, pDesc, ppDepthStencilView);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreateInputLayout( const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs, UINT NumElements, const void *pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, ID3D11InputLayout **ppInputLayout )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateInputLayout(pInputElementDescs, NumElements, pShaderBytecodeWithInputSignature, BytecodeLength, ppInputLayout);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreateVertexShader( const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11VertexShader **ppVertexShader )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateVertexShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppVertexShader);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreateGeometryShader( const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11GeometryShader **ppGeometryShader )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateGeometryShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppGeometryShader);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreateGeometryShaderWithStreamOutput( const void *pShaderBytecode, SIZE_T BytecodeLength, const D3D11_SO_DECLARATION_ENTRY *pSODeclaration, UINT NumEntries, const UINT *pBufferStrides, UINT NumStrides, UINT RasterizedStream, ID3D11ClassLinkage *pClassLinkage, ID3D11GeometryShader **ppGeometryShader )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateGeometryShaderWithStreamOutput(pShaderBytecode, BytecodeLength, pSODeclaration, NumEntries, pBufferStrides, NumStrides, RasterizedStream, pClassLinkage, ppGeometryShader);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreatePixelShader( const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11PixelShader **ppPixelShader )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreatePixelShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppPixelShader);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreateHullShader( const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11HullShader **ppHullShader )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateHullShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppHullShader);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreateDomainShader( const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11DomainShader **ppDomainShader )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateDomainShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppDomainShader);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreateComputeShader( const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11ComputeShader **ppComputeShader )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateComputeShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppComputeShader);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreateClassLinkage( ID3D11ClassLinkage **ppLinkage )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateClassLinkage(ppLinkage);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreateBlendState( const D3D11_BLEND_DESC *pBlendStateDesc, ID3D11BlendState **ppBlendState )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateBlendState(pBlendStateDesc, ppBlendState);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreateDepthStencilState( const D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc, ID3D11DepthStencilState **ppDepthStencilState )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateDepthStencilState(pDepthStencilDesc, ppDepthStencilState);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreateRasterizerState( const D3D11_RASTERIZER_DESC *pRasterizerDesc, ID3D11RasterizerState **ppRasterizerState )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateRasterizerState(pRasterizerDesc, ppRasterizerState);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreateSamplerState( const D3D11_SAMPLER_DESC *pSamplerDesc, ID3D11SamplerState **ppSamplerState )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateSamplerState(pSamplerDesc, ppSamplerState);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreateQuery( const D3D11_QUERY_DESC *pQueryDesc, ID3D11Query **ppQuery )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateQuery(pQueryDesc, ppQuery);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreatePredicate( const D3D11_QUERY_DESC *pPredicateDesc, ID3D11Predicate **ppPredicate )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreatePredicate(pPredicateDesc, ppPredicate);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreateCounter( const D3D11_COUNTER_DESC *pCounterDesc, ID3D11Counter **ppCounter )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateCounter(pCounterDesc, ppCounter);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CreateDeferredContext( UINT ContextFlags, ID3D11DeviceContext **ppDeferredContext )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateDeferredContext(ContextFlags, ppDeferredContext);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::OpenSharedResource( HANDLE hResource, REFIID ReturnedInterface, void **ppResource )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = OpenSharedResource(hResource, ReturnedInterface, ppResource);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CheckFormatSupport( DXGI_FORMAT Format, UINT *pFormatSupport )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CheckFormatSupport(Format, pFormatSupport);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CheckMultisampleQualityLevels( DXGI_FORMAT Format, UINT SampleCount, UINT *pNumQualityLevels )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CheckMultisampleQualityLevels(Format, SampleCount, pNumQualityLevels);
    set_vtable(this, vs.down());
    return r;
}

void STDMETHODCALLTYPE D3D11DeviceHookInterface::CheckCounterInfo( D3D11_COUNTER_INFO *pCounterInfo )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CheckCounterInfo(pCounterInfo);
    set_vtable(this, vs.down());
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CheckCounter( const D3D11_COUNTER_DESC *pDesc, D3D11_COUNTER_TYPE *pType, UINT *pActiveCounters, LPSTR szName, UINT *pNameLength, LPSTR szUnits, UINT *pUnitsLength, LPSTR szDescription, UINT *pDescriptionLength )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CheckCounter(pDesc, pType, pActiveCounters, szName, pNameLength, szUnits, pUnitsLength, szDescription, pDescriptionLength);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::CheckFeatureSupport( D3D11_FEATURE Feature, void *pFeatureSupportData, UINT FeatureSupportDataSize )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CheckFeatureSupport(Feature, pFeatureSupportData, FeatureSupportDataSize);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::GetPrivateData( REFGUID guid, UINT *pDataSize, void *pData )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetPrivateData(guid, pDataSize, pData);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::SetPrivateData( REFGUID guid, UINT DataSize, const void *pData )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = SetPrivateData(guid, DataSize, pData);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::SetPrivateDataInterface( REFGUID guid, const IUnknown *pData )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = SetPrivateDataInterface(guid, pData);
    set_vtable(this, vs.down());
    return r;
}

D3D_FEATURE_LEVEL STDMETHODCALLTYPE D3D11DeviceHookInterface::GetFeatureLevel( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    D3D_FEATURE_LEVEL r = GetFeatureLevel();
    set_vtable(this, vs.down());
    return r;
}

UINT STDMETHODCALLTYPE D3D11DeviceHookInterface::GetCreationFlags( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    UINT r = GetCreationFlags();
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::GetDeviceRemovedReason( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetDeviceRemovedReason();
    set_vtable(this, vs.down());
    return r;
}

void STDMETHODCALLTYPE D3D11DeviceHookInterface::GetImmediateContext( ID3D11DeviceContext **ppImmediateContext )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetImmediateContext(ppImmediateContext);
    set_vtable(this, vs.down());
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHookInterface::SetExceptionMode( UINT RaiseFlags )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = SetExceptionMode(RaiseFlags);
    set_vtable(this, vs.down());
    return r;
}

UINT STDMETHODCALLTYPE D3D11DeviceHookInterface::GetExceptionMode( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    UINT r = GetExceptionMode();
    set_vtable(this, vs.down());
    return r;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      D3D11DeviceContextHookInterface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template TUnknownHookInterface<ID3D11DeviceContext>;
template TD3D11DeviceChildHookInterface<ID3D11DeviceContext>;

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::VSSetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    VSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::PSSetShaderResources( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    PSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::PSSetShader( ID3D11PixelShader *pPixelShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    PSSetShader(pPixelShader, ppClassInstances, NumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::PSSetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    PSSetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::VSSetShader( ID3D11VertexShader *pVertexShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    VSSetShader(pVertexShader, ppClassInstances, NumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::DrawIndexed( UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::Draw( UINT VertexCount, UINT StartVertexLocation )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    Draw(VertexCount, StartVertexLocation);
    set_vtable(this, vs.down());
}

HRESULT STDMETHODCALLTYPE D3D11DeviceContextHookInterface::Map( ID3D11Resource *pResource, UINT Subresource, D3D11_MAP MapType, UINT MapFlags, D3D11_MAPPED_SUBRESOURCE *pMappedResource )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = Map(pResource, Subresource, MapType, MapFlags, pMappedResource);
    set_vtable(this, vs.down());
    return r;
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::Unmap( ID3D11Resource *pResource, UINT Subresource )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    Unmap(pResource, Subresource);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::PSSetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    PSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::IASetInputLayout( ID3D11InputLayout *pInputLayout )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    IASetInputLayout(pInputLayout);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::IASetVertexBuffers(  UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppVertexBuffers, const UINT *pStrides, const UINT *pOffsets )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    IASetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::IASetIndexBuffer(  ID3D11Buffer *pIndexBuffer, DXGI_FORMAT Format, UINT Offset )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    IASetIndexBuffer(pIndexBuffer, Format, Offset);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::DrawIndexedInstanced( UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::DrawInstanced( UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::GSSetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::GSSetShader( ID3D11GeometryShader *pShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GSSetShader(pShader, ppClassInstances, NumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY Topology )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    IASetPrimitiveTopology(Topology);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::VSSetShaderResources( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    VSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::VSSetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    VSSetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::Begin( ID3D11Asynchronous *pAsync )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    Begin(pAsync);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::End( ID3D11Asynchronous *pAsync )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    End(pAsync);
    set_vtable(this, vs.down());
}

HRESULT STDMETHODCALLTYPE D3D11DeviceContextHookInterface::GetData( ID3D11Asynchronous *pAsync, void *pData, UINT DataSize, UINT GetDataFlags )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetData(pAsync, pData, DataSize, GetDataFlags);
    set_vtable(this, vs.down());
    return r;
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::SetPredication( ID3D11Predicate *pPredicate, BOOL PredicateValue )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    SetPredication(pPredicate, PredicateValue);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::GSSetShaderResources( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::GSSetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GSSetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::OMSetRenderTargets( UINT NumViews, ID3D11RenderTargetView *const *ppRenderTargetViews, ID3D11DepthStencilView *pDepthStencilView)
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilView);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::OMSetRenderTargetsAndUnorderedAccessViews(  UINT NumRTVs, ID3D11RenderTargetView *const *ppRenderTargetViews, ID3D11DepthStencilView *pDepthStencilView, UINT UAVStartSlot, UINT NumUAVs, ID3D11UnorderedAccessView *const *ppUnorderedAccessViews, const UINT *pUAVInitialCounts)
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    OMSetRenderTargetsAndUnorderedAccessViews(NumRTVs, ppRenderTargetViews, pDepthStencilView, UAVStartSlot, NumUAVs, ppUnorderedAccessViews, pUAVInitialCounts);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::OMSetBlendState( ID3D11BlendState *pBlendState, const FLOAT BlendFactor[ 4 ], UINT SampleMask )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    OMSetBlendState(pBlendState, BlendFactor, SampleMask);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::OMSetDepthStencilState( ID3D11DepthStencilState *pDepthStencilState, UINT StencilRef )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    OMSetDepthStencilState(pDepthStencilState, StencilRef);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::SOSetTargets( UINT NumBuffers, ID3D11Buffer *const *ppSOTargets, const UINT *pOffsets )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    SOSetTargets(NumBuffers, ppSOTargets, pOffsets);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::DrawAuto( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DrawAuto();
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::DrawIndexedInstancedIndirect( ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DrawIndexedInstancedIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::DrawInstancedIndirect( ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DrawInstancedIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::Dispatch( UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::DispatchIndirect( ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DispatchIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::RSSetState( ID3D11RasterizerState *pRasterizerState )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    RSSetState(pRasterizerState);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::RSSetViewports( UINT NumViewports, const D3D11_VIEWPORT *pViewports )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    RSSetViewports(NumViewports, pViewports);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::RSSetScissorRects( UINT NumRects, const D3D11_RECT *pRects )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    RSSetScissorRects(NumRects, pRects);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::CopySubresourceRegion( ID3D11Resource *pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ, ID3D11Resource *pSrcResource, UINT SrcSubresource, const D3D11_BOX *pSrcBox )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CopySubresourceRegion(pDstResource, DstSubresource, DstX, DstY, DstZ, pSrcResource, SrcSubresource, pSrcBox);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::CopyResource( ID3D11Resource *pDstResource, ID3D11Resource *pSrcResource )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CopyResource(pDstResource, pSrcResource);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::UpdateSubresource( ID3D11Resource *pDstResource, UINT DstSubresource, const D3D11_BOX *pDstBox, const void *pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    UpdateSubresource(pDstResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::CopyStructureCount( ID3D11Buffer *pDstBuffer, UINT DstAlignedByteOffset, ID3D11UnorderedAccessView *pSrcView )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CopyStructureCount(pDstBuffer, DstAlignedByteOffset, pSrcView);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::ClearRenderTargetView( ID3D11RenderTargetView *pRenderTargetView, const FLOAT ColorRGBA[ 4 ] )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    ClearRenderTargetView(pRenderTargetView, ColorRGBA);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::ClearUnorderedAccessViewUint( ID3D11UnorderedAccessView *pUnorderedAccessView, const UINT Values[ 4 ] )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    ClearUnorderedAccessViewUint(pUnorderedAccessView, Values);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::ClearUnorderedAccessViewFloat( ID3D11UnorderedAccessView *pUnorderedAccessView, const FLOAT Values[ 4 ] )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    ClearUnorderedAccessViewFloat(pUnorderedAccessView, Values);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::ClearDepthStencilView( ID3D11DepthStencilView *pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    ClearDepthStencilView(pDepthStencilView, ClearFlags, Depth, Stencil);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::GenerateMips( ID3D11ShaderResourceView *pShaderResourceView )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GenerateMips(pShaderResourceView);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::SetResourceMinLOD( ID3D11Resource *pResource, FLOAT MinLOD )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    SetResourceMinLOD(pResource, MinLOD);
    set_vtable(this, vs.down());
}

FLOAT STDMETHODCALLTYPE D3D11DeviceContextHookInterface::GetResourceMinLOD( ID3D11Resource *pResource )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    FLOAT r = GetResourceMinLOD(pResource);
    set_vtable(this, vs.down());
    return r;
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::ResolveSubresource( ID3D11Resource *pDstResource, UINT DstSubresource, ID3D11Resource *pSrcResource, UINT SrcSubresource, DXGI_FORMAT Format )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    ResolveSubresource(pDstResource, DstSubresource, pSrcResource, SrcSubresource, Format);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::ExecuteCommandList( ID3D11CommandList *pCommandList, BOOL RestoreContextState )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    ExecuteCommandList(pCommandList, RestoreContextState);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::HSSetShaderResources( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::HSSetShader( ID3D11HullShader *pHullShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HSSetShader(pHullShader, ppClassInstances, NumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::HSSetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HSSetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::HSSetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::DSSetShaderResources( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::DSSetShader( ID3D11DomainShader *pDomainShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DSSetShader(pDomainShader, ppClassInstances, NumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::DSSetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DSSetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::DSSetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::CSSetShaderResources( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::CSSetUnorderedAccessViews( UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView *const *ppUnorderedAccessViews, const UINT *pUAVInitialCounts )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CSSetUnorderedAccessViews(StartSlot, NumUAVs, ppUnorderedAccessViews, pUAVInitialCounts);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::CSSetShader( ID3D11ComputeShader *pComputeShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CSSetShader(pComputeShader, ppClassInstances, NumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::CSSetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CSSetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::CSSetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::VSGetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    VSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::PSGetShaderResources( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView **ppShaderResourceViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    PSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::PSGetShader( ID3D11PixelShader **ppPixelShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    PSGetShader(ppPixelShader, ppClassInstances, pNumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::PSGetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    PSGetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::VSGetShader( ID3D11VertexShader **ppVertexShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    VSGetShader(ppVertexShader, ppClassInstances, pNumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::PSGetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    PSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::IAGetInputLayout( ID3D11InputLayout **ppInputLayout )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    IAGetInputLayout(ppInputLayout);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::IAGetVertexBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppVertexBuffers, UINT *pStrides, UINT *pOffsets )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    IAGetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::IAGetIndexBuffer( ID3D11Buffer **pIndexBuffer, DXGI_FORMAT *Format, UINT *Offset )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    IAGetIndexBuffer(pIndexBuffer, Format, Offset);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::GSGetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::GSGetShader( ID3D11GeometryShader **ppGeometryShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GSGetShader(ppGeometryShader, ppClassInstances, pNumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::IAGetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY *pTopology )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    IAGetPrimitiveTopology(pTopology);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::VSGetShaderResources( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView **ppShaderResourceViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    VSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::VSGetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    VSGetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::GetPredication( ID3D11Predicate **ppPredicate, BOOL *pPredicateValue )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetPredication(ppPredicate, pPredicateValue);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::GSGetShaderResources( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView **ppShaderResourceViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::GSGetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GSGetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::OMGetRenderTargets( UINT NumViews, ID3D11RenderTargetView **ppRenderTargetViews, ID3D11DepthStencilView **ppDepthStencilView )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    OMGetRenderTargets(NumViews, ppRenderTargetViews, ppDepthStencilView);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::OMGetRenderTargetsAndUnorderedAccessViews( UINT NumRTVs, ID3D11RenderTargetView **ppRenderTargetViews, ID3D11DepthStencilView **ppDepthStencilView, UINT UAVStartSlot, UINT NumUAVs, ID3D11UnorderedAccessView **ppUnorderedAccessViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    OMGetRenderTargetsAndUnorderedAccessViews(NumRTVs, ppRenderTargetViews, ppDepthStencilView, UAVStartSlot, NumUAVs, ppUnorderedAccessViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::OMGetBlendState( ID3D11BlendState **ppBlendState, FLOAT BlendFactor[ 4 ], UINT *pSampleMask )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    OMGetBlendState(ppBlendState, BlendFactor, pSampleMask);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::OMGetDepthStencilState( ID3D11DepthStencilState **ppDepthStencilState, UINT *pStencilRef )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    OMGetDepthStencilState(ppDepthStencilState, pStencilRef);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::SOGetTargets( UINT NumBuffers, ID3D11Buffer **ppSOTargets )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    SOGetTargets(NumBuffers, ppSOTargets);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::RSGetState( ID3D11RasterizerState **ppRasterizerState )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    RSGetState(ppRasterizerState);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::RSGetViewports( 
    UINT *pNumViewports,
    D3D11_VIEWPORT *pViewports)
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    RSGetViewports(pNumViewports, pViewports);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::RSGetScissorRects( UINT *pNumRects, D3D11_RECT *pRects )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    RSGetScissorRects(pNumRects, pRects);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::HSGetShaderResources( 
    UINT StartSlot,
    UINT NumViews,
    ID3D11ShaderResourceView **ppShaderResourceViews)
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::HSGetShader( ID3D11HullShader **ppHullShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HSGetShader(ppHullShader, ppClassInstances, pNumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::HSGetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HSGetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::HSGetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::DSGetShaderResources( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView **ppShaderResourceViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::DSGetShader( ID3D11DomainShader **ppDomainShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DSGetShader(ppDomainShader, ppClassInstances, pNumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::DSGetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DSGetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::DSGetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::CSGetShaderResources( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView **ppShaderResourceViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::CSGetUnorderedAccessViews( UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView **ppUnorderedAccessViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CSGetUnorderedAccessViews(StartSlot, NumUAVs, ppUnorderedAccessViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::CSGetShader( ID3D11ComputeShader **ppComputeShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CSGetShader(ppComputeShader, ppClassInstances, pNumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::CSGetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CSGetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::CSGetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::ClearState( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    ClearState();
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHookInterface::Flush( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    Flush();
    set_vtable(this, vs.down());
}

D3D11_DEVICE_CONTEXT_TYPE STDMETHODCALLTYPE D3D11DeviceContextHookInterface::GetType( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    D3D11_DEVICE_CONTEXT_TYPE r = GetType();
    set_vtable(this, vs.down());
    return r;
}

UINT STDMETHODCALLTYPE D3D11DeviceContextHookInterface::GetContextFlags( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    UINT r = GetContextFlags();
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceContextHookInterface::FinishCommandList( BOOL RestoreDeferredContextState, ID3D11CommandList **ppCommandList )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = FinishCommandList(RestoreDeferredContextState, ppCommandList);
    set_vtable(this, vs.down());
    return r;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      D3D11Texture1DHookInterface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template TUnknownHookInterface<ID3D11Buffer>;
template TD3D11DeviceChildHookInterface<ID3D11Buffer>;
template TD3D11ResourceHookInterface<ID3D11Buffer>;

void STDMETHODCALLTYPE D3D11BufferHookInterface::GetDesc( D3D11_BUFFER_DESC *pDesc )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetDesc(pDesc);
    set_vtable(this, vs.down());
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      D3D11Texture1DHookInterface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template TUnknownHookInterface<ID3D11Texture1D>;
template TD3D11DeviceChildHookInterface<ID3D11Texture1D>;
template TD3D11ResourceHookInterface<ID3D11Texture1D>;

void STDMETHODCALLTYPE D3D11Texture1DHookInterface::GetDesc( D3D11_TEXTURE1D_DESC *pDesc )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetDesc(pDesc);
    set_vtable(this, vs.down());
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      D3D11Texture2DHookInterface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template TUnknownHookInterface<ID3D11Texture2D>;
template TD3D11DeviceChildHookInterface<ID3D11Texture2D>;
template TD3D11ResourceHookInterface<ID3D11Texture2D>;

void STDMETHODCALLTYPE D3D11Texture2DHookInterface::GetDesc( D3D11_TEXTURE2D_DESC *pDesc )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetDesc(pDesc);
    set_vtable(this, vs.down());
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      D3D11Texture3DHookInterface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template TUnknownHookInterface<ID3D11Texture3D>;
template TD3D11DeviceChildHookInterface<ID3D11Texture3D>;
template TD3D11ResourceHookInterface<ID3D11Texture3D>;

void STDMETHODCALLTYPE D3D11Texture3DHookInterface::GetDesc( D3D11_TEXTURE3D_DESC *pDesc )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetDesc(pDesc);
    set_vtable(this, vs.down());
}
