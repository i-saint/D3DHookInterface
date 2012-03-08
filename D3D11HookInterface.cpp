#include <vector>
#include <map>
#include <algorithm>
#include "D3D11HookInterface.h"
#include "Utilities/Module.h"


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

    void removeVTable(void *target, void **v)
    {
        m_vtables.erase(std::remove(m_vtables.begin(), m_vtables.end(), v), m_vtables.end());
        set_vtable(target, m_vtables.back());
        m_depth = int(m_vtables.size())-1;
    }

    void removeAllVTable(void *target)
    {
        set_vtable(target, m_vtables.front());
        m_vtables.resize(1);
    }

    int getDepth() const { return m_depth; }
    void** up()     { return --m_depth >= 0 ? m_vtables[m_depth] : NULL; }
    void** down()   { return ++m_depth >= 0 ? m_vtables[m_depth] : NULL; }
};

namespace {
    typedef std::map<IUnknown*, VTableStack> VTables;
    VTables g_vtables;

    void _D3D11SetHook(IUnknown *pTarget, IUnknown *pHook)
    {
        VTableStack &vs = g_vtables[pTarget];

        // vtable を stack に追加
        if(vs.getStackSize()==0) {
            vs.pushVTable(get_vtable(pTarget));
        }
        vs.pushVTable(get_vtable(pHook));

        // vtable を差し替える
        set_vtable(pTarget, get_vtable(pHook));
    }

    void _D3D11RemoveHook(IUnknown *pTarget, void **vtable)
    {
        VTables::iterator i = g_vtables.find(pTarget);
        if(i!=g_vtables.end()) {
            VTableStack &vs = i->second;
            vs.removeVTable(pTarget, vtable);
            if(vs.getStackSize()==1) {
                g_vtables.erase(pTarget);
            }
        }
    }

    void _D3D11RemoveAllHooks(IUnknown *pTarget)
    {
        VTables::iterator i = g_vtables.find(pTarget);
        if(i!=g_vtables.end()) {
            VTableStack &vs = i->second;
            vs.removeAllVTable(pTarget);
            g_vtables.erase(pTarget);
        }
    }
} // namespace 

void D3D11SetHook(IDXGISwapChain *pTarget, IDXGISwapChain *pHook)                       { _D3D11SetHook(pTarget, pHook); }

void D3D11SetHook(ID3D11Device *pTarget, ID3D11Device *pHook)                           { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11DeviceContext *pTarget, ID3D11DeviceContext *pHook)             { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11Asynchronous *pTarget, ID3D11Asynchronous *pHook)               { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11BlendState *pTarget, ID3D11BlendState *pHook)                   { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11Counter *pTarget, ID3D11Counter *pHook)                         { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11CommandList *pTarget, ID3D11CommandList *pHook)                 { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11DepthStencilState *pTarget, ID3D11DepthStencilState *pHook)     { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11InputLayout *pTarget, ID3D11InputLayout *pHook)                 { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11Predicate *pTarget, ID3D11Predicate *pHook)                     { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11Query *pTarget, ID3D11Query *pHook)                             { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11RasterizerState *pTarget, ID3D11RasterizerState *pHook)         { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11SamplerState *pTarget, ID3D11SamplerState *pHook)               { _D3D11SetHook(pTarget, pHook); }

void D3D11SetHook(ID3D11Buffer *pTarget, ID3D11Buffer *pHook)                           { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11Texture1D *pTarget, ID3D11Texture1D *pHook)                     { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11Texture2D *pTarget, ID3D11Texture2D *pHook)                     { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11Texture3D *pTarget, ID3D11Texture3D *pHook)                     { _D3D11SetHook(pTarget, pHook); }

void D3D11SetHook(ID3D11DepthStencilView *pTarget, ID3D11DepthStencilView *pHook)       { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11RenderTargetView *pTarget, ID3D11RenderTargetView *pHook)       { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11ShaderResourceView *pTarget, ID3D11ShaderResourceView *pHook)   { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11UnorderedAccessView *pTarget, ID3D11UnorderedAccessView *pHook) { _D3D11SetHook(pTarget, pHook); }

void D3D11SetHook(ID3D11ClassInstance *pTarget, ID3D11ClassInstance *pHook)             { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11ClassLinkage *pTarget, ID3D11ClassLinkage *pHook)               { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11VertexShader *pTarget, ID3D11VertexShader *pHook)               { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11PixelShader *pTarget, ID3D11PixelShader *pHook)                 { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11GeometryShader *pTarget, ID3D11GeometryShader *pHook)           { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11HullShader *pTarget, ID3D11HullShader *pHook)                   { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11DomainShader *pTarget, ID3D11DomainShader *pHook)               { _D3D11SetHook(pTarget, pHook); }
void D3D11SetHook(ID3D11ComputeShader *pTarget, ID3D11ComputeShader *pHook)             { _D3D11SetHook(pTarget, pHook); }

void D3D11RemoveHook(IUnknown *pTarget, IUnknown *pHook)                                { _D3D11RemoveHook(pTarget, get_vtable(pHook)); }
void D3D11RemoveHook(IUnknown *pTarget, void **vtable)                                  { _D3D11RemoveHook(pTarget, vtable); }
void D3D11RemoveAllHooks(IUnknown *pTarget)                                             { _D3D11RemoveAllHooks(pTarget); }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      template implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
HRESULT STDMETHODCALLTYPE TUnknownHook<T>::QueryInterface( REFIID riid, __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = QueryInterface(riid, ppvObject);
    set_vtable(this, vs.down());
    return r;
}

template<class T>
ULONG STDMETHODCALLTYPE TUnknownHook<T>::AddRef( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    ULONG r = AddRef();
    set_vtable(this, vs.down());
    return r;
}

template<class T>
ULONG STDMETHODCALLTYPE TUnknownHook<T>::Release( void )
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
HRESULT STDMETHODCALLTYPE TDXGIObjectHook<T>::SetPrivateData( REFGUID Name, UINT DataSize, const void *pData )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = SetPrivateData(Name, DataSize, pData);
    set_vtable(this, vs.down());
    return r;
}

template<class T>
HRESULT STDMETHODCALLTYPE TDXGIObjectHook<T>::SetPrivateDataInterface( REFGUID Name, const IUnknown *pUnknown )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = SetPrivateDataInterface(Name, pUnknown);
    set_vtable(this, vs.down());
    return r;
}

template<class T>
HRESULT STDMETHODCALLTYPE TDXGIObjectHook<T>::GetPrivateData( REFGUID Name, UINT *pDataSize, void *pData )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetPrivateData(Name, pDataSize, pData);
    set_vtable(this, vs.down());
    return r;
}

template<class T>
HRESULT STDMETHODCALLTYPE TDXGIObjectHook<T>::GetParent( REFIID riid, void **ppParent )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetParent(riid, ppParent);
    set_vtable(this, vs.down());
    return r;
}


template<class T>
HRESULT STDMETHODCALLTYPE TDXGIDeviceSubObjectHook<T>::GetDevice( REFIID riid, void **ppDevice )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetDevice(riid, ppDevice);
    set_vtable(this, vs.down());
    return r;
}


template<class T>
void STDMETHODCALLTYPE TD3D11DeviceChildHook<T>::GetDevice( ID3D11Device **ppDevice )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetDevice(ppDevice);
    set_vtable(this, vs.down());
}

template<class T>
HRESULT STDMETHODCALLTYPE TD3D11DeviceChildHook<T>::GetPrivateData( REFGUID guid, UINT *pDataSize, void *pData )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetPrivateData(guid, pDataSize, pData);
    set_vtable(this, vs.down());
    return r;
}

template<class T>
HRESULT STDMETHODCALLTYPE TD3D11DeviceChildHook<T>::SetPrivateData( REFGUID guid, UINT DataSize, const void *pData )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = SetPrivateData(guid, DataSize, pData);
    set_vtable(this, vs.down());
    return r;
}

template<class T>
HRESULT STDMETHODCALLTYPE TD3D11DeviceChildHook<T>::SetPrivateDataInterface( REFGUID guid, const IUnknown *pData )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = SetPrivateDataInterface(guid, pData);
    set_vtable(this, vs.down());
    return r;
}


template<class T>
void STDMETHODCALLTYPE TD3D11ResourceHook<T>::GetType( D3D11_RESOURCE_DIMENSION *pResourceDimension )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetType(pResourceDimension);
    set_vtable(this, vs.down());
}

template<class T>
void STDMETHODCALLTYPE TD3D11ResourceHook<T>::SetEvictionPriority( UINT EvictionPriority )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    SetEvictionPriority(EvictionPriority);
    set_vtable(this, vs.down());
}

template<class T>
UINT STDMETHODCALLTYPE TD3D11ResourceHook<T>::GetEvictionPriority( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    UINT r = GetEvictionPriority();
    set_vtable(this, vs.down());
    return r;
}


template<class T>
void STDMETHODCALLTYPE TD3D11ViewHook<T>::GetResource( ID3D11Resource **ppResource )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetResource(ppResource);
    set_vtable(this, vs.down());
}


template<class T>
UINT STDMETHODCALLTYPE TD3D11AsynchronousHook<T>::GetDataSize( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    UINT r = GetDataSize();
    set_vtable(this, vs.down());
    return r;
}

template<class T>
void STDMETHODCALLTYPE TD3D11QueryHook<T>::GetDesc( D3D11_QUERY_DESC *pDesc )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetDesc(pDesc);
    set_vtable(this, vs.down());
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      DXGISwapChainHookInterface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template TUnknownHook<IDXGISwapChain>;
template TDXGIObjectHook<IDXGISwapChain>;
template TDXGIDeviceSubObjectHook<IDXGISwapChain>;

HRESULT STDMETHODCALLTYPE DXGISwapChainHook::Present( UINT SyncInterval, UINT Flags )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = Present(SyncInterval, Flags);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE DXGISwapChainHook::GetBuffer( UINT Buffer, REFIID riid, void **ppSurface )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetBuffer(Buffer, riid, ppSurface);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE DXGISwapChainHook::SetFullscreenState( BOOL Fullscreen, IDXGIOutput *pTarget )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = SetFullscreenState(Fullscreen, pTarget);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE DXGISwapChainHook::GetFullscreenState( BOOL *pFullscreen, IDXGIOutput **ppTarget )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetFullscreenState(pFullscreen, ppTarget);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE DXGISwapChainHook::GetDesc( DXGI_SWAP_CHAIN_DESC *pDesc )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetDesc(pDesc);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE DXGISwapChainHook::ResizeBuffers( UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = ResizeBuffers(BufferCount, Width, Height, NewFormat, SwapChainFlags);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE DXGISwapChainHook::ResizeTarget( const DXGI_MODE_DESC *pNewTargetParameters )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = ResizeTarget(pNewTargetParameters);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE DXGISwapChainHook::GetContainingOutput( IDXGIOutput **ppOutput )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetContainingOutput(ppOutput);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE DXGISwapChainHook::GetFrameStatistics( DXGI_FRAME_STATISTICS *pStats )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetFrameStatistics(pStats);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE DXGISwapChainHook::GetLastPresentCount( UINT *pLastPresentCount )
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

template TUnknownHook<ID3D11Device>;

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreateBuffer( const D3D11_BUFFER_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Buffer **ppBuffer )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateBuffer(pDesc, pInitialData, ppBuffer);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreateTexture1D( const D3D11_TEXTURE1D_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Texture1D **ppTexture1D )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateTexture1D(pDesc, pInitialData, ppTexture1D);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreateTexture2D( const D3D11_TEXTURE2D_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Texture2D **ppTexture2D )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateTexture2D(pDesc, pInitialData, ppTexture2D);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreateTexture3D( const D3D11_TEXTURE3D_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Texture3D **ppTexture3D )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateTexture3D(pDesc, pInitialData, ppTexture3D);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreateShaderResourceView( ID3D11Resource *pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc, ID3D11ShaderResourceView **ppSRView )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateShaderResourceView(pResource, pDesc, ppSRView);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreateUnorderedAccessView( ID3D11Resource *pResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC *pDesc, ID3D11UnorderedAccessView **ppUAView )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateUnorderedAccessView(pResource, pDesc, ppUAView);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreateRenderTargetView( ID3D11Resource *pResource, const D3D11_RENDER_TARGET_VIEW_DESC *pDesc, ID3D11RenderTargetView **ppRTView )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateRenderTargetView(pResource, pDesc, ppRTView);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreateDepthStencilView( ID3D11Resource *pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc, ID3D11DepthStencilView **ppDepthStencilView )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateDepthStencilView(pResource, pDesc, ppDepthStencilView);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreateInputLayout( const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs, UINT NumElements, const void *pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, ID3D11InputLayout **ppInputLayout )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateInputLayout(pInputElementDescs, NumElements, pShaderBytecodeWithInputSignature, BytecodeLength, ppInputLayout);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreateVertexShader( const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11VertexShader **ppVertexShader )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateVertexShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppVertexShader);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreateGeometryShader( const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11GeometryShader **ppGeometryShader )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateGeometryShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppGeometryShader);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreateGeometryShaderWithStreamOutput( const void *pShaderBytecode, SIZE_T BytecodeLength, const D3D11_SO_DECLARATION_ENTRY *pSODeclaration, UINT NumEntries, const UINT *pBufferStrides, UINT NumStrides, UINT RasterizedStream, ID3D11ClassLinkage *pClassLinkage, ID3D11GeometryShader **ppGeometryShader )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateGeometryShaderWithStreamOutput(pShaderBytecode, BytecodeLength, pSODeclaration, NumEntries, pBufferStrides, NumStrides, RasterizedStream, pClassLinkage, ppGeometryShader);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreatePixelShader( const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11PixelShader **ppPixelShader )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreatePixelShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppPixelShader);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreateHullShader( const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11HullShader **ppHullShader )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateHullShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppHullShader);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreateDomainShader( const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11DomainShader **ppDomainShader )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateDomainShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppDomainShader);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreateComputeShader( const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11ComputeShader **ppComputeShader )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateComputeShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppComputeShader);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreateClassLinkage( ID3D11ClassLinkage **ppLinkage )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateClassLinkage(ppLinkage);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreateBlendState( const D3D11_BLEND_DESC *pBlendStateDesc, ID3D11BlendState **ppBlendState )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateBlendState(pBlendStateDesc, ppBlendState);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreateDepthStencilState( const D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc, ID3D11DepthStencilState **ppDepthStencilState )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateDepthStencilState(pDepthStencilDesc, ppDepthStencilState);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreateRasterizerState( const D3D11_RASTERIZER_DESC *pRasterizerDesc, ID3D11RasterizerState **ppRasterizerState )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateRasterizerState(pRasterizerDesc, ppRasterizerState);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreateSamplerState( const D3D11_SAMPLER_DESC *pSamplerDesc, ID3D11SamplerState **ppSamplerState )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateSamplerState(pSamplerDesc, ppSamplerState);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreateQuery( const D3D11_QUERY_DESC *pQueryDesc, ID3D11Query **ppQuery )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateQuery(pQueryDesc, ppQuery);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreatePredicate( const D3D11_QUERY_DESC *pPredicateDesc, ID3D11Predicate **ppPredicate )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreatePredicate(pPredicateDesc, ppPredicate);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreateCounter( const D3D11_COUNTER_DESC *pCounterDesc, ID3D11Counter **ppCounter )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateCounter(pCounterDesc, ppCounter);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CreateDeferredContext( UINT ContextFlags, ID3D11DeviceContext **ppDeferredContext )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateDeferredContext(ContextFlags, ppDeferredContext);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::OpenSharedResource( HANDLE hResource, REFIID ReturnedInterface, void **ppResource )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = OpenSharedResource(hResource, ReturnedInterface, ppResource);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CheckFormatSupport( DXGI_FORMAT Format, UINT *pFormatSupport )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CheckFormatSupport(Format, pFormatSupport);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CheckMultisampleQualityLevels( DXGI_FORMAT Format, UINT SampleCount, UINT *pNumQualityLevels )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CheckMultisampleQualityLevels(Format, SampleCount, pNumQualityLevels);
    set_vtable(this, vs.down());
    return r;
}

void STDMETHODCALLTYPE D3D11DeviceHook::CheckCounterInfo( D3D11_COUNTER_INFO *pCounterInfo )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CheckCounterInfo(pCounterInfo);
    set_vtable(this, vs.down());
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CheckCounter( const D3D11_COUNTER_DESC *pDesc, D3D11_COUNTER_TYPE *pType, UINT *pActiveCounters, LPSTR szName, UINT *pNameLength, LPSTR szUnits, UINT *pUnitsLength, LPSTR szDescription, UINT *pDescriptionLength )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CheckCounter(pDesc, pType, pActiveCounters, szName, pNameLength, szUnits, pUnitsLength, szDescription, pDescriptionLength);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::CheckFeatureSupport( D3D11_FEATURE Feature, void *pFeatureSupportData, UINT FeatureSupportDataSize )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CheckFeatureSupport(Feature, pFeatureSupportData, FeatureSupportDataSize);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::GetPrivateData( REFGUID guid, UINT *pDataSize, void *pData )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetPrivateData(guid, pDataSize, pData);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::SetPrivateData( REFGUID guid, UINT DataSize, const void *pData )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = SetPrivateData(guid, DataSize, pData);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::SetPrivateDataInterface( REFGUID guid, const IUnknown *pData )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = SetPrivateDataInterface(guid, pData);
    set_vtable(this, vs.down());
    return r;
}

D3D_FEATURE_LEVEL STDMETHODCALLTYPE D3D11DeviceHook::GetFeatureLevel( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    D3D_FEATURE_LEVEL r = GetFeatureLevel();
    set_vtable(this, vs.down());
    return r;
}

UINT STDMETHODCALLTYPE D3D11DeviceHook::GetCreationFlags( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    UINT r = GetCreationFlags();
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::GetDeviceRemovedReason( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetDeviceRemovedReason();
    set_vtable(this, vs.down());
    return r;
}

void STDMETHODCALLTYPE D3D11DeviceHook::GetImmediateContext( ID3D11DeviceContext **ppImmediateContext )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetImmediateContext(ppImmediateContext);
    set_vtable(this, vs.down());
}

HRESULT STDMETHODCALLTYPE D3D11DeviceHook::SetExceptionMode( UINT RaiseFlags )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = SetExceptionMode(RaiseFlags);
    set_vtable(this, vs.down());
    return r;
}

UINT STDMETHODCALLTYPE D3D11DeviceHook::GetExceptionMode( void )
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

template TUnknownHook<ID3D11DeviceContext>;
template TD3D11DeviceChildHook<ID3D11DeviceContext>;

void STDMETHODCALLTYPE D3D11DeviceContextHook::VSSetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    VSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::PSSetShaderResources( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    PSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::PSSetShader( ID3D11PixelShader *pPixelShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    PSSetShader(pPixelShader, ppClassInstances, NumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::PSSetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    PSSetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::VSSetShader( ID3D11VertexShader *pVertexShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    VSSetShader(pVertexShader, ppClassInstances, NumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::DrawIndexed( UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::Draw( UINT VertexCount, UINT StartVertexLocation )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    Draw(VertexCount, StartVertexLocation);
    set_vtable(this, vs.down());
}

HRESULT STDMETHODCALLTYPE D3D11DeviceContextHook::Map( ID3D11Resource *pResource, UINT Subresource, D3D11_MAP MapType, UINT MapFlags, D3D11_MAPPED_SUBRESOURCE *pMappedResource )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = Map(pResource, Subresource, MapType, MapFlags, pMappedResource);
    set_vtable(this, vs.down());
    return r;
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::Unmap( ID3D11Resource *pResource, UINT Subresource )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    Unmap(pResource, Subresource);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::PSSetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    PSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::IASetInputLayout( ID3D11InputLayout *pInputLayout )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    IASetInputLayout(pInputLayout);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::IASetVertexBuffers(  UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppVertexBuffers, const UINT *pStrides, const UINT *pOffsets )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    IASetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::IASetIndexBuffer(  ID3D11Buffer *pIndexBuffer, DXGI_FORMAT Format, UINT Offset )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    IASetIndexBuffer(pIndexBuffer, Format, Offset);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::DrawIndexedInstanced( UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::DrawInstanced( UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::GSSetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::GSSetShader( ID3D11GeometryShader *pShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GSSetShader(pShader, ppClassInstances, NumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY Topology )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    IASetPrimitiveTopology(Topology);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::VSSetShaderResources( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    VSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::VSSetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    VSSetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::Begin( ID3D11Asynchronous *pAsync )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    Begin(pAsync);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::End( ID3D11Asynchronous *pAsync )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    End(pAsync);
    set_vtable(this, vs.down());
}

HRESULT STDMETHODCALLTYPE D3D11DeviceContextHook::GetData( ID3D11Asynchronous *pAsync, void *pData, UINT DataSize, UINT GetDataFlags )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetData(pAsync, pData, DataSize, GetDataFlags);
    set_vtable(this, vs.down());
    return r;
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::SetPredication( ID3D11Predicate *pPredicate, BOOL PredicateValue )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    SetPredication(pPredicate, PredicateValue);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::GSSetShaderResources( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::GSSetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GSSetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::OMSetRenderTargets( UINT NumViews, ID3D11RenderTargetView *const *ppRenderTargetViews, ID3D11DepthStencilView *pDepthStencilView)
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilView);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::OMSetRenderTargetsAndUnorderedAccessViews(  UINT NumRTVs, ID3D11RenderTargetView *const *ppRenderTargetViews, ID3D11DepthStencilView *pDepthStencilView, UINT UAVStartSlot, UINT NumUAVs, ID3D11UnorderedAccessView *const *ppUnorderedAccessViews, const UINT *pUAVInitialCounts)
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    OMSetRenderTargetsAndUnorderedAccessViews(NumRTVs, ppRenderTargetViews, pDepthStencilView, UAVStartSlot, NumUAVs, ppUnorderedAccessViews, pUAVInitialCounts);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::OMSetBlendState( ID3D11BlendState *pBlendState, const FLOAT BlendFactor[ 4 ], UINT SampleMask )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    OMSetBlendState(pBlendState, BlendFactor, SampleMask);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::OMSetDepthStencilState( ID3D11DepthStencilState *pDepthStencilState, UINT StencilRef )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    OMSetDepthStencilState(pDepthStencilState, StencilRef);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::SOSetTargets( UINT NumBuffers, ID3D11Buffer *const *ppSOTargets, const UINT *pOffsets )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    SOSetTargets(NumBuffers, ppSOTargets, pOffsets);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::DrawAuto( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DrawAuto();
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::DrawIndexedInstancedIndirect( ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DrawIndexedInstancedIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::DrawInstancedIndirect( ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DrawInstancedIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::Dispatch( UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::DispatchIndirect( ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DispatchIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::RSSetState( ID3D11RasterizerState *pRasterizerState )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    RSSetState(pRasterizerState);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::RSSetViewports( UINT NumViewports, const D3D11_VIEWPORT *pViewports )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    RSSetViewports(NumViewports, pViewports);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::RSSetScissorRects( UINT NumRects, const D3D11_RECT *pRects )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    RSSetScissorRects(NumRects, pRects);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::CopySubresourceRegion( ID3D11Resource *pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ, ID3D11Resource *pSrcResource, UINT SrcSubresource, const D3D11_BOX *pSrcBox )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CopySubresourceRegion(pDstResource, DstSubresource, DstX, DstY, DstZ, pSrcResource, SrcSubresource, pSrcBox);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::CopyResource( ID3D11Resource *pDstResource, ID3D11Resource *pSrcResource )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CopyResource(pDstResource, pSrcResource);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::UpdateSubresource( ID3D11Resource *pDstResource, UINT DstSubresource, const D3D11_BOX *pDstBox, const void *pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    UpdateSubresource(pDstResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::CopyStructureCount( ID3D11Buffer *pDstBuffer, UINT DstAlignedByteOffset, ID3D11UnorderedAccessView *pSrcView )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CopyStructureCount(pDstBuffer, DstAlignedByteOffset, pSrcView);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::ClearRenderTargetView( ID3D11RenderTargetView *pRenderTargetView, const FLOAT ColorRGBA[ 4 ] )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    ClearRenderTargetView(pRenderTargetView, ColorRGBA);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::ClearUnorderedAccessViewUint( ID3D11UnorderedAccessView *pUnorderedAccessView, const UINT Values[ 4 ] )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    ClearUnorderedAccessViewUint(pUnorderedAccessView, Values);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::ClearUnorderedAccessViewFloat( ID3D11UnorderedAccessView *pUnorderedAccessView, const FLOAT Values[ 4 ] )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    ClearUnorderedAccessViewFloat(pUnorderedAccessView, Values);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::ClearDepthStencilView( ID3D11DepthStencilView *pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    ClearDepthStencilView(pDepthStencilView, ClearFlags, Depth, Stencil);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::GenerateMips( ID3D11ShaderResourceView *pShaderResourceView )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GenerateMips(pShaderResourceView);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::SetResourceMinLOD( ID3D11Resource *pResource, FLOAT MinLOD )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    SetResourceMinLOD(pResource, MinLOD);
    set_vtable(this, vs.down());
}

FLOAT STDMETHODCALLTYPE D3D11DeviceContextHook::GetResourceMinLOD( ID3D11Resource *pResource )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    FLOAT r = GetResourceMinLOD(pResource);
    set_vtable(this, vs.down());
    return r;
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::ResolveSubresource( ID3D11Resource *pDstResource, UINT DstSubresource, ID3D11Resource *pSrcResource, UINT SrcSubresource, DXGI_FORMAT Format )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    ResolveSubresource(pDstResource, DstSubresource, pSrcResource, SrcSubresource, Format);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::ExecuteCommandList( ID3D11CommandList *pCommandList, BOOL RestoreContextState )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    ExecuteCommandList(pCommandList, RestoreContextState);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::HSSetShaderResources( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::HSSetShader( ID3D11HullShader *pHullShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HSSetShader(pHullShader, ppClassInstances, NumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::HSSetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HSSetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::HSSetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::DSSetShaderResources( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::DSSetShader( ID3D11DomainShader *pDomainShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DSSetShader(pDomainShader, ppClassInstances, NumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::DSSetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DSSetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::DSSetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::CSSetShaderResources( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::CSSetUnorderedAccessViews( UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView *const *ppUnorderedAccessViews, const UINT *pUAVInitialCounts )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CSSetUnorderedAccessViews(StartSlot, NumUAVs, ppUnorderedAccessViews, pUAVInitialCounts);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::CSSetShader( ID3D11ComputeShader *pComputeShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CSSetShader(pComputeShader, ppClassInstances, NumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::CSSetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CSSetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::CSSetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::VSGetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    VSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::PSGetShaderResources( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView **ppShaderResourceViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    PSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::PSGetShader( ID3D11PixelShader **ppPixelShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    PSGetShader(ppPixelShader, ppClassInstances, pNumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::PSGetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    PSGetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::VSGetShader( ID3D11VertexShader **ppVertexShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    VSGetShader(ppVertexShader, ppClassInstances, pNumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::PSGetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    PSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::IAGetInputLayout( ID3D11InputLayout **ppInputLayout )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    IAGetInputLayout(ppInputLayout);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::IAGetVertexBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppVertexBuffers, UINT *pStrides, UINT *pOffsets )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    IAGetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::IAGetIndexBuffer( ID3D11Buffer **pIndexBuffer, DXGI_FORMAT *Format, UINT *Offset )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    IAGetIndexBuffer(pIndexBuffer, Format, Offset);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::GSGetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::GSGetShader( ID3D11GeometryShader **ppGeometryShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GSGetShader(ppGeometryShader, ppClassInstances, pNumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::IAGetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY *pTopology )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    IAGetPrimitiveTopology(pTopology);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::VSGetShaderResources( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView **ppShaderResourceViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    VSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::VSGetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    VSGetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::GetPredication( ID3D11Predicate **ppPredicate, BOOL *pPredicateValue )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetPredication(ppPredicate, pPredicateValue);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::GSGetShaderResources( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView **ppShaderResourceViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::GSGetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GSGetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::OMGetRenderTargets( UINT NumViews, ID3D11RenderTargetView **ppRenderTargetViews, ID3D11DepthStencilView **ppDepthStencilView )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    OMGetRenderTargets(NumViews, ppRenderTargetViews, ppDepthStencilView);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::OMGetRenderTargetsAndUnorderedAccessViews( UINT NumRTVs, ID3D11RenderTargetView **ppRenderTargetViews, ID3D11DepthStencilView **ppDepthStencilView, UINT UAVStartSlot, UINT NumUAVs, ID3D11UnorderedAccessView **ppUnorderedAccessViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    OMGetRenderTargetsAndUnorderedAccessViews(NumRTVs, ppRenderTargetViews, ppDepthStencilView, UAVStartSlot, NumUAVs, ppUnorderedAccessViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::OMGetBlendState( ID3D11BlendState **ppBlendState, FLOAT BlendFactor[ 4 ], UINT *pSampleMask )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    OMGetBlendState(ppBlendState, BlendFactor, pSampleMask);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::OMGetDepthStencilState( ID3D11DepthStencilState **ppDepthStencilState, UINT *pStencilRef )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    OMGetDepthStencilState(ppDepthStencilState, pStencilRef);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::SOGetTargets( UINT NumBuffers, ID3D11Buffer **ppSOTargets )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    SOGetTargets(NumBuffers, ppSOTargets);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::RSGetState( ID3D11RasterizerState **ppRasterizerState )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    RSGetState(ppRasterizerState);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::RSGetViewports( 
    UINT *pNumViewports,
    D3D11_VIEWPORT *pViewports)
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    RSGetViewports(pNumViewports, pViewports);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::RSGetScissorRects( UINT *pNumRects, D3D11_RECT *pRects )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    RSGetScissorRects(pNumRects, pRects);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::HSGetShaderResources( 
    UINT StartSlot,
    UINT NumViews,
    ID3D11ShaderResourceView **ppShaderResourceViews)
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::HSGetShader( ID3D11HullShader **ppHullShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HSGetShader(ppHullShader, ppClassInstances, pNumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::HSGetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HSGetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::HSGetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::DSGetShaderResources( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView **ppShaderResourceViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::DSGetShader( ID3D11DomainShader **ppDomainShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DSGetShader(ppDomainShader, ppClassInstances, pNumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::DSGetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DSGetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::DSGetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    DSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::CSGetShaderResources( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView **ppShaderResourceViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::CSGetUnorderedAccessViews( UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView **ppUnorderedAccessViews )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CSGetUnorderedAccessViews(StartSlot, NumUAVs, ppUnorderedAccessViews);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::CSGetShader( ID3D11ComputeShader **ppComputeShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CSGetShader(ppComputeShader, ppClassInstances, pNumClassInstances);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::CSGetSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CSGetSamplers(StartSlot, NumSamplers, ppSamplers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::CSGetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    CSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::ClearState( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    ClearState();
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11DeviceContextHook::Flush( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    Flush();
    set_vtable(this, vs.down());
}

D3D11_DEVICE_CONTEXT_TYPE STDMETHODCALLTYPE D3D11DeviceContextHook::GetType( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    D3D11_DEVICE_CONTEXT_TYPE r = GetType();
    set_vtable(this, vs.down());
    return r;
}

UINT STDMETHODCALLTYPE D3D11DeviceContextHook::GetContextFlags( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    UINT r = GetContextFlags();
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11DeviceContextHook::FinishCommandList( BOOL RestoreDeferredContextState, ID3D11CommandList **ppCommandList )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = FinishCommandList(RestoreDeferredContextState, ppCommandList);
    set_vtable(this, vs.down());
    return r;
}



template TUnknownHook<ID3D11Asynchronous>;
template TD3D11DeviceChildHook<ID3D11Asynchronous>;
template TD3D11AsynchronousHook<ID3D11Asynchronous>;


template TUnknownHook<ID3D11Query>;
template TD3D11DeviceChildHook<ID3D11Query>;
template TD3D11AsynchronousHook<ID3D11Query>;
template TD3D11QueryHook<ID3D11Query>;


template TUnknownHook<ID3D11Predicate>;
template TD3D11DeviceChildHook<ID3D11Predicate>;
template TD3D11AsynchronousHook<ID3D11Predicate>;
template TD3D11QueryHook<ID3D11Predicate>;


template TUnknownHook<ID3D11BlendState>;
template TD3D11DeviceChildHook<ID3D11BlendState>;

void STDMETHODCALLTYPE D3D11BlendStateHook::GetDesc( D3D11_BLEND_DESC *pDesc )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetDesc(pDesc);
    set_vtable(this, vs.down());
}


template TUnknownHook<ID3D11Counter>;
template TD3D11DeviceChildHook<ID3D11Counter>;
template TD3D11AsynchronousHook<ID3D11Counter>;

void STDMETHODCALLTYPE D3D11CounterHook::GetDesc( D3D11_COUNTER_DESC *pDesc )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetDesc(pDesc);
    set_vtable(this, vs.down());
}


template TUnknownHook<ID3D11CommandList>;
template TD3D11DeviceChildHook<ID3D11CommandList>;

UINT STDMETHODCALLTYPE D3D11CommandListHook::GetContextFlags( void )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    UINT r = GetContextFlags();
    set_vtable(this, vs.down());
    return r;
}


template TUnknownHook<ID3D11DepthStencilState>;
template TD3D11DeviceChildHook<ID3D11DepthStencilState>;

void STDMETHODCALLTYPE D3D11DepthStencilStateHook::GetDesc( D3D11_DEPTH_STENCIL_DESC *pDesc )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetDesc(pDesc);
    set_vtable(this, vs.down());
}


template TUnknownHook<ID3D11InputLayout>;
template TD3D11DeviceChildHook<ID3D11InputLayout>;


template TUnknownHook<ID3D11RasterizerState>;
template TD3D11DeviceChildHook<ID3D11RasterizerState>;

void STDMETHODCALLTYPE D3D11RasterizerStateHook::GetDesc( D3D11_RASTERIZER_DESC *pDesc )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetDesc(pDesc);
    set_vtable(this, vs.down());
}


template TUnknownHook<ID3D11SamplerState>;
template TD3D11DeviceChildHook<ID3D11SamplerState>;

void STDMETHODCALLTYPE D3D11SamplerStateHook::GetDesc( D3D11_SAMPLER_DESC *pDesc )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetDesc(pDesc);
    set_vtable(this, vs.down());
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      resource hook interface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template TUnknownHook<ID3D11Buffer>;
template TD3D11DeviceChildHook<ID3D11Buffer>;
template TD3D11ResourceHook<ID3D11Buffer>;

void STDMETHODCALLTYPE D3D11BufferHook::GetDesc( D3D11_BUFFER_DESC *pDesc )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetDesc(pDesc);
    set_vtable(this, vs.down());
}


template TUnknownHook<ID3D11Texture1D>;
template TD3D11DeviceChildHook<ID3D11Texture1D>;
template TD3D11ResourceHook<ID3D11Texture1D>;

void STDMETHODCALLTYPE D3D11Texture1DHook::GetDesc( D3D11_TEXTURE1D_DESC *pDesc )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetDesc(pDesc);
    set_vtable(this, vs.down());
}


template TUnknownHook<ID3D11Texture2D>;
template TD3D11DeviceChildHook<ID3D11Texture2D>;
template TD3D11ResourceHook<ID3D11Texture2D>;

void STDMETHODCALLTYPE D3D11Texture2DHook::GetDesc( D3D11_TEXTURE2D_DESC *pDesc )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetDesc(pDesc);
    set_vtable(this, vs.down());
}


template TUnknownHook<ID3D11Texture3D>;
template TD3D11DeviceChildHook<ID3D11Texture3D>;
template TD3D11ResourceHook<ID3D11Texture3D>;

void STDMETHODCALLTYPE D3D11Texture3DHook::GetDesc( D3D11_TEXTURE3D_DESC *pDesc )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetDesc(pDesc);
    set_vtable(this, vs.down());
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      view hook interface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template TUnknownHook<ID3D11DepthStencilView>;
template TD3D11DeviceChildHook<ID3D11DepthStencilView>;
template TD3D11ViewHook<ID3D11DepthStencilView>;

void STDMETHODCALLTYPE D3D11DepthStencilViewHook::GetDesc( D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetDesc(pDesc);
    set_vtable(this, vs.down());
}


template TUnknownHook<ID3D11RenderTargetView>;
template TD3D11DeviceChildHook<ID3D11RenderTargetView>;
template TD3D11ViewHook<ID3D11RenderTargetView>;

void STDMETHODCALLTYPE D3D11RenderTargetViewHook::GetDesc( D3D11_RENDER_TARGET_VIEW_DESC *pDesc )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetDesc(pDesc);
    set_vtable(this, vs.down());
}


template TUnknownHook<ID3D11ShaderResourceView>;
template TD3D11DeviceChildHook<ID3D11ShaderResourceView>;
template TD3D11ViewHook<ID3D11ShaderResourceView>;

void STDMETHODCALLTYPE D3D11ShaderResourceViewHook::GetDesc( D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetDesc(pDesc);
    set_vtable(this, vs.down());
}


template TUnknownHook<ID3D11UnorderedAccessView>;
template TD3D11DeviceChildHook<ID3D11UnorderedAccessView>;
template TD3D11ViewHook<ID3D11UnorderedAccessView>;

void STDMETHODCALLTYPE D3D11UnorderedAccessViewHook::GetDesc( D3D11_UNORDERED_ACCESS_VIEW_DESC *pDesc )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetDesc(pDesc);
    set_vtable(this, vs.down());
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      shader hook interface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template TUnknownHook<ID3D11ClassInstance>;
template TD3D11DeviceChildHook<ID3D11ClassInstance>;

void STDMETHODCALLTYPE D3D11ClassInstanceHook::GetClassLinkage( ID3D11ClassLinkage **ppLinkage )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetClassLinkage(ppLinkage);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11ClassInstanceHook::GetDesc( D3D11_CLASS_INSTANCE_DESC *pDesc )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetDesc(pDesc);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11ClassInstanceHook::GetInstanceName( LPSTR pInstanceName, SIZE_T *pBufferLength )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetInstanceName(pInstanceName, pBufferLength);
    set_vtable(this, vs.down());
}

void STDMETHODCALLTYPE D3D11ClassInstanceHook::GetTypeName( LPSTR pTypeName, SIZE_T *pBufferLength )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    GetTypeName(pTypeName, pBufferLength);
    set_vtable(this, vs.down());
}


template TUnknownHook<ID3D11ClassLinkage>;
template TD3D11DeviceChildHook<ID3D11ClassLinkage>;

HRESULT STDMETHODCALLTYPE D3D11ClassLinkageHook::GetClassInstance( LPCSTR pClassInstanceName, UINT InstanceIndex, ID3D11ClassInstance **ppInstance )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = GetClassInstance(pClassInstanceName, InstanceIndex, ppInstance);
    set_vtable(this, vs.down());
    return r;
}

HRESULT STDMETHODCALLTYPE D3D11ClassLinkageHook::CreateClassInstance( LPCSTR pClassTypeName, UINT ConstantBufferOffset, UINT ConstantVectorOffset, UINT TextureOffset, UINT SamplerOffset, ID3D11ClassInstance **ppInstance )
{
    VTableStack &vs = g_vtables[this];
    set_vtable(this, vs.up());
    HRESULT r = CreateClassInstance(pClassTypeName, ConstantBufferOffset, ConstantVectorOffset, TextureOffset, SamplerOffset, ppInstance);
    set_vtable(this, vs.down());
    return r;
}

template TUnknownHook<ID3D11VertexShader>;
template TD3D11DeviceChildHook<ID3D11VertexShader>;

template TUnknownHook<ID3D11PixelShader>;
template TD3D11DeviceChildHook<ID3D11PixelShader>;

template TUnknownHook<ID3D11GeometryShader>;
template TD3D11DeviceChildHook<ID3D11GeometryShader>;

template TUnknownHook<ID3D11HullShader>;
template TD3D11DeviceChildHook<ID3D11HullShader>;

template TUnknownHook<ID3D11DomainShader>;
template TD3D11DeviceChildHook<ID3D11DomainShader>;

template TUnknownHook<ID3D11ComputeShader>;
template TD3D11DeviceChildHook<ID3D11ComputeShader>;
