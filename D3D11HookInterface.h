#ifndef _ist_D3D11HookInterface_h_
#define _ist_D3D11HookInterface_h_
#include <D3D11.h>

// DirectX の interface のメンバ関数呼び出しを、template 引数に指定した class の同メンバ関数にリダイレクトさせます。
// template 引数には HookInterface 系 class を継承した class を指定します。
// 
// DirectX 本来の interface を vtable だけ HookInterface のものに差し替えることで実現しているため、
// 呼ばれるメンバ関数は HookInterface のものでありながら、instance の実体は DirectX の内部実装のもの、という特殊な状況になります。
// このため、HookInterface ではメンバ変数を正常に扱うことができません。(意図しない結果になるか、クラッシュを招きます)
// メンバ変数は global 変数や class 内 static 変数で代用する必要があります。
// 
// DirectX の interface のメンバ関数は stdcall であることに注意。
// HookInterface の継承先もメンバ関数に stdcall を指定する必要があります。
// 
// このヘッダファイルが提供する class (DXGISwapChainHookInterface など) のメンバ関数は、DirectX の本来の API を呼びます。
// なので、継承先で override したメンバ関数は必ず継承元の関数も呼ぶ必要があります。
//
// また、コンストラクタ & デストラクタ は hook できません。
// ID3D11Device の Create 系関数を hook、Release() を hook して 0 を返すタイミングを取る、のようにして代用する必要があります。

// 例:
/*
    class HookTestD3D11DeviceContext : public D3D11DeviceContextHookInterface
    {
    typedef D3D11DeviceContextHookInterface super;
    public:
        // DrawIndexedInstanced() を hook して引数のパラメータをデバッグ出力
        virtual void STDMETHODCALLTYPE DrawIndexedInstanced( UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation)
        {
            char buf[256];
            sprintf(buf, "DrawIndexedInstanced(): IndexCountPerInstance=%d InstanceCount=%d\n", IndexCountPerInstance, InstanceCount);
            OutputDebugStringA(buf);

            // DirectX 本来のメンバ関数呼び出し
            super::DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
        }
    };

    D3D11SetHook<HookTestD3D11DeviceContext>(g_pImmediateContext);
    // 多重 hook も可能。最後に登録したものから順に呼ばれます
    D3D11SetHook<HookTestD3D11DeviceContext>(g_pImmediateContext);
*/

class DXGISwapChainHook;

class D3D11DeviceHook;
class D3D11DeviceContextHook;
class D3D11AsynchronousHook;
class D3D11BlendStateHook;
class D3D11CounterHook;
class D3D11CommandListHook;
class D3D11DepthStencilStateHook;
class D3D11InputLayoutHook;
class D3D11PredicateHook;
class D3D11QueryHook;
class D3D11RasterizerStateHook;
class D3D11SamplerStateHook;

class D3D11BufferHook;
class D3D11Texture1DHook;
class D3D11Texture2DHook;
class D3D11Texture3DHook;

class D3D11DepthStencilViewHook;
class D3D11RenderTargetViewHook;
class D3D11ShaderResourceViewHook;
class D3D11UnorderedAccessViewHook;

class D3D11VertexShaderHook;
class D3D11PixelShaderHook;
class D3D11GeometryShaderHook;
class D3D11HullShaderHook;
class D3D11DomainShaderHook;
class D3D11ComputeShaderHook;
class D3D11ClassInstanceHook;
class D3D11ClassLinkageHook;

template<class HookType> void D3D11SetHook(IDXGISwapChain *pTarget);

template<class HookType> void D3D11SetHook(ID3D11Device *pTarget);
template<class HookType> void D3D11SetHook(ID3D11DeviceContext *pTarget);
template<class HookType> void D3D11SetHook(ID3D11Asynchronous *pTarget);
template<class HookType> void D3D11SetHook(ID3D11BlendState *pTarget);
template<class HookType> void D3D11SetHook(ID3D11Counter *pTarget);
template<class HookType> void D3D11SetHook(ID3D11CommandList *pTarget);
template<class HookType> void D3D11SetHook(ID3D11DepthStencilState *pTarget);
template<class HookType> void D3D11SetHook(ID3D11InputLayout *pTarget);
template<class HookType> void D3D11SetHook(ID3D11Predicate *pTarget);
template<class HookType> void D3D11SetHook(ID3D11Query *pTarget);
template<class HookType> void D3D11SetHook(ID3D11RasterizerState *pTarget);
template<class HookType> void D3D11SetHook(ID3D11SamplerState *pTarget);

template<class HookType> void D3D11SetHook(ID3D11Buffer *pTarget);
template<class HookType> void D3D11SetHook(ID3D11Texture1D *pTarget);
template<class HookType> void D3D11SetHook(ID3D11Texture2D *pTarget);
template<class HookType> void D3D11SetHook(ID3D11Texture3D *pTarget);

template<class HookType> void D3D11SetHook(ID3D11DepthStencilView *pTarget);
template<class HookType> void D3D11SetHook(ID3D11RenderTargetView *pTarget);
template<class HookType> void D3D11SetHook(ID3D11ShaderResourceView *pTarget);
template<class HookType> void D3D11SetHook(ID3D11UnorderedAccessView *pTarget);

template<class HookType> void D3D11SetHook(ID3D11ClassInstance *pTarget);
template<class HookType> void D3D11SetHook(ID3D11ClassLinkage *pTarget);
template<class HookType> void D3D11SetHook(ID3D11VertexShader *pTarget);
template<class HookType> void D3D11SetHook(ID3D11PixelShader *pTarget);
template<class HookType> void D3D11SetHook(ID3D11GeometryShader *pTarget);
template<class HookType> void D3D11SetHook(ID3D11HullShader *pTarget);
template<class HookType> void D3D11SetHook(ID3D11DomainShader *pTarget);
template<class HookType> void D3D11SetHook(ID3D11ComputeShader *pTarget);





template<class T> struct D3D11GetHookType;

template<> struct D3D11GetHookType<IDXGISwapChain>            { typedef DXGISwapChainHook result_type; };

template<> struct D3D11GetHookType<ID3D11Device>              { typedef D3D11DeviceHook result_type; };
template<> struct D3D11GetHookType<ID3D11DeviceContext>       { typedef D3D11DeviceContextHook result_type; };
template<> struct D3D11GetHookType<ID3D11Asynchronous>        { typedef D3D11AsynchronousHook result_type; };
template<> struct D3D11GetHookType<ID3D11BlendState>          { typedef D3D11BlendStateHook result_type; };
template<> struct D3D11GetHookType<ID3D11Counter>             { typedef D3D11CounterHook result_type; };
template<> struct D3D11GetHookType<ID3D11CommandList>         { typedef D3D11CommandListHook result_type; };
template<> struct D3D11GetHookType<ID3D11DepthStencilState>   { typedef D3D11DepthStencilStateHook result_type; };
template<> struct D3D11GetHookType<ID3D11InputLayout>         { typedef D3D11InputLayoutHook result_type; };
template<> struct D3D11GetHookType<ID3D11Predicate>           { typedef D3D11PredicateHook result_type; };
template<> struct D3D11GetHookType<ID3D11Query>               { typedef D3D11QueryHook result_type; };
template<> struct D3D11GetHookType<ID3D11RasterizerState>     { typedef D3D11RasterizerStateHook result_type; };
template<> struct D3D11GetHookType<ID3D11SamplerState>        { typedef D3D11SamplerStateHook result_type; };

template<> struct D3D11GetHookType<ID3D11Buffer>              { typedef D3D11BufferHook result_type; };
template<> struct D3D11GetHookType<ID3D11Texture1D>           { typedef D3D11Texture1DHook result_type; };
template<> struct D3D11GetHookType<ID3D11Texture2D>           { typedef D3D11Texture2DHook result_type; };
template<> struct D3D11GetHookType<ID3D11Texture3D>           { typedef D3D11Texture3DHook result_type; };

template<> struct D3D11GetHookType<ID3D11DepthStencilView>    { typedef D3D11DepthStencilViewHook result_type; };
template<> struct D3D11GetHookType<ID3D11RenderTargetView>    { typedef D3D11RenderTargetViewHook result_type; };
template<> struct D3D11GetHookType<ID3D11ShaderResourceView>  { typedef D3D11ShaderResourceViewHook result_type; };
template<> struct D3D11GetHookType<ID3D11UnorderedAccessView> { typedef D3D11UnorderedAccessViewHook result_type; };

template<> struct D3D11GetHookType<ID3D11VertexShader>        { typedef D3D11VertexShaderHook result_type; };
template<> struct D3D11GetHookType<ID3D11PixelShader>         { typedef D3D11PixelShaderHook result_type; };
template<> struct D3D11GetHookType<ID3D11GeometryShader>      { typedef D3D11GeometryShaderHook result_type; };
template<> struct D3D11GetHookType<ID3D11HullShader>          { typedef D3D11HullShaderHook result_type; };
template<> struct D3D11GetHookType<ID3D11DomainShader>        { typedef D3D11DomainShaderHook result_type; };
template<> struct D3D11GetHookType<ID3D11ComputeShader>       { typedef D3D11ComputeShaderHook result_type; };
template<> struct D3D11GetHookType<ID3D11ClassInstance>       { typedef D3D11ClassInstanceHook result_type; };
template<> struct D3D11GetHookType<ID3D11ClassLinkage>        { typedef D3D11ClassLinkageHook result_type; };


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      template implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
class TUnknownHook : public T
{
public:
    typedef T base_type;

    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
        REFIID riid,
        __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject);

    virtual ULONG STDMETHODCALLTYPE AddRef(void);

    virtual ULONG STDMETHODCALLTYPE Release(void);
};

template<class T>
class TDXGIObjectHook : public TUnknownHook<T>
{
public:
    virtual HRESULT STDMETHODCALLTYPE SetPrivateData( 
        REFGUID Name,
        UINT DataSize,
        const void *pData);

    virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface( 
        REFGUID Name,
        const IUnknown *pUnknown);

    virtual HRESULT STDMETHODCALLTYPE GetPrivateData( 
        REFGUID Name,
        UINT *pDataSize,
        void *pData);

    virtual HRESULT STDMETHODCALLTYPE GetParent( 
        REFIID riid,
        void **ppParent);
};

template<class T>
class TDXGIDeviceSubObjectHook : public TDXGIObjectHook<T>
{
public:
    virtual HRESULT STDMETHODCALLTYPE GetDevice( 
        REFIID riid,
        void **ppDevice);
};

template<class T>
class TD3D11DeviceChildHook : public TUnknownHook<T>
{
public:
    virtual void STDMETHODCALLTYPE GetDevice( 
        ID3D11Device **ppDevice);

    virtual HRESULT STDMETHODCALLTYPE GetPrivateData( 
        REFGUID guid,
        UINT *pDataSize,
        void *pData);

    virtual HRESULT STDMETHODCALLTYPE SetPrivateData( 
        REFGUID guid,
        UINT DataSize,
        const void *pData);

    virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface( 
        REFGUID guid,
        const IUnknown *pData);
};

template<class T>
class TD3D11ResourceHook : public TD3D11DeviceChildHook<T>
{
public:
    virtual void STDMETHODCALLTYPE GetType( 
        D3D11_RESOURCE_DIMENSION *pResourceDimension);

    virtual void STDMETHODCALLTYPE SetEvictionPriority( 
        UINT EvictionPriority);

    virtual UINT STDMETHODCALLTYPE GetEvictionPriority(void);
};

template<class T>
class TD3D11ViewHook : public TD3D11DeviceChildHook<T>
{
public:
    virtual void STDMETHODCALLTYPE GetResource( 
        ID3D11Resource **ppResource);
};


template<class T>
class TD3D11AsynchronousHook : public TD3D11DeviceChildHook<T>
{
public:
    virtual UINT STDMETHODCALLTYPE GetDataSize(void);
};

template<class T>
class TD3D11QueryHook : public TD3D11AsynchronousHook<T>
{
public:
    virtual void STDMETHODCALLTYPE GetDesc( 
        D3D11_QUERY_DESC *pDesc);
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      DXGISwapChainHookInterface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class DXGISwapChainHook : public TDXGIDeviceSubObjectHook<IDXGISwapChain>
{
public:
    virtual HRESULT STDMETHODCALLTYPE Present( 
        UINT SyncInterval,
        UINT Flags);

    virtual HRESULT STDMETHODCALLTYPE GetBuffer( 
        UINT Buffer,
        REFIID riid,
        void **ppSurface);

    virtual HRESULT STDMETHODCALLTYPE SetFullscreenState( 
        BOOL Fullscreen,
        IDXGIOutput *pTarget);

    virtual HRESULT STDMETHODCALLTYPE GetFullscreenState( 
        BOOL *pFullscreen,
        IDXGIOutput **ppTarget);

    virtual HRESULT STDMETHODCALLTYPE GetDesc( 
        DXGI_SWAP_CHAIN_DESC *pDesc);

    virtual HRESULT STDMETHODCALLTYPE ResizeBuffers( 
        UINT BufferCount,
        UINT Width,
        UINT Height,
        DXGI_FORMAT NewFormat,
        UINT SwapChainFlags);

    virtual HRESULT STDMETHODCALLTYPE ResizeTarget( 
        const DXGI_MODE_DESC *pNewTargetParameters);

    virtual HRESULT STDMETHODCALLTYPE GetContainingOutput( 
        IDXGIOutput **ppOutput);

    virtual HRESULT STDMETHODCALLTYPE GetFrameStatistics( 
        DXGI_FRAME_STATISTICS *pStats);

    virtual HRESULT STDMETHODCALLTYPE GetLastPresentCount( 
        UINT *pLastPresentCount);
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      D3D11DeviceHookInterface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class D3D11DeviceHook : public TUnknownHook<ID3D11Device>
{
public:
    virtual HRESULT STDMETHODCALLTYPE CreateBuffer( 
        const D3D11_BUFFER_DESC *pDesc,
        const D3D11_SUBRESOURCE_DATA *pInitialData,
        ID3D11Buffer **ppBuffer);

    virtual HRESULT STDMETHODCALLTYPE CreateTexture1D( 
        const D3D11_TEXTURE1D_DESC *pDesc,
        const D3D11_SUBRESOURCE_DATA *pInitialData,
        ID3D11Texture1D **ppTexture1D);

    virtual HRESULT STDMETHODCALLTYPE CreateTexture2D( 
        const D3D11_TEXTURE2D_DESC *pDesc,
        const D3D11_SUBRESOURCE_DATA *pInitialData,
        ID3D11Texture2D **ppTexture2D);

    virtual HRESULT STDMETHODCALLTYPE CreateTexture3D( 
        const D3D11_TEXTURE3D_DESC *pDesc,
        const D3D11_SUBRESOURCE_DATA *pInitialData,
        ID3D11Texture3D **ppTexture3D);

    virtual HRESULT STDMETHODCALLTYPE CreateShaderResourceView( 
        ID3D11Resource *pResource,
        const D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc,
        ID3D11ShaderResourceView **ppSRView);

    virtual HRESULT STDMETHODCALLTYPE CreateUnorderedAccessView( 
        ID3D11Resource *pResource,
        const D3D11_UNORDERED_ACCESS_VIEW_DESC *pDesc,
        ID3D11UnorderedAccessView **ppUAView);

    virtual HRESULT STDMETHODCALLTYPE CreateRenderTargetView( 
        ID3D11Resource *pResource,
        const D3D11_RENDER_TARGET_VIEW_DESC *pDesc,
        ID3D11RenderTargetView **ppRTView);

    virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilView( 
        ID3D11Resource *pResource,
        const D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc,
        ID3D11DepthStencilView **ppDepthStencilView);

    virtual HRESULT STDMETHODCALLTYPE CreateInputLayout( 
        const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,
        UINT NumElements,
        const void *pShaderBytecodeWithInputSignature,
        SIZE_T BytecodeLength,
        ID3D11InputLayout **ppInputLayout);

    virtual HRESULT STDMETHODCALLTYPE CreateVertexShader( 
        const void *pShaderBytecode,
        SIZE_T BytecodeLength,
        ID3D11ClassLinkage *pClassLinkage,
        ID3D11VertexShader **ppVertexShader);

    virtual HRESULT STDMETHODCALLTYPE CreateGeometryShader( 
        const void *pShaderBytecode,
        SIZE_T BytecodeLength,
        ID3D11ClassLinkage *pClassLinkage,
        ID3D11GeometryShader **ppGeometryShader);

    virtual HRESULT STDMETHODCALLTYPE CreateGeometryShaderWithStreamOutput( 
        const void *pShaderBytecode,
        SIZE_T BytecodeLength,
        const D3D11_SO_DECLARATION_ENTRY *pSODeclaration,
        UINT NumEntries,
        const UINT *pBufferStrides,
        UINT NumStrides,
        UINT RasterizedStream,
        ID3D11ClassLinkage *pClassLinkage,
        ID3D11GeometryShader **ppGeometryShader);

    virtual HRESULT STDMETHODCALLTYPE CreatePixelShader( 
        const void *pShaderBytecode,
        SIZE_T BytecodeLength,
        ID3D11ClassLinkage *pClassLinkage,
        ID3D11PixelShader **ppPixelShader);

    virtual HRESULT STDMETHODCALLTYPE CreateHullShader( 
        const void *pShaderBytecode,
        SIZE_T BytecodeLength,
        ID3D11ClassLinkage *pClassLinkage,
        ID3D11HullShader **ppHullShader);

    virtual HRESULT STDMETHODCALLTYPE CreateDomainShader( 
        const void *pShaderBytecode,
        SIZE_T BytecodeLength,
        ID3D11ClassLinkage *pClassLinkage,
        ID3D11DomainShader **ppDomainShader);

    virtual HRESULT STDMETHODCALLTYPE CreateComputeShader( 
        const void *pShaderBytecode,
        SIZE_T BytecodeLength,
        ID3D11ClassLinkage *pClassLinkage,
        ID3D11ComputeShader **ppComputeShader);

    virtual HRESULT STDMETHODCALLTYPE CreateClassLinkage( 
        ID3D11ClassLinkage **ppLinkage);

    virtual HRESULT STDMETHODCALLTYPE CreateBlendState( 
        const D3D11_BLEND_DESC *pBlendStateDesc,
        ID3D11BlendState **ppBlendState);

    virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilState( 
        const D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc,
        ID3D11DepthStencilState **ppDepthStencilState);

    virtual HRESULT STDMETHODCALLTYPE CreateRasterizerState( 
        const D3D11_RASTERIZER_DESC *pRasterizerDesc,
        ID3D11RasterizerState **ppRasterizerState);

    virtual HRESULT STDMETHODCALLTYPE CreateSamplerState( 
        const D3D11_SAMPLER_DESC *pSamplerDesc,
        ID3D11SamplerState **ppSamplerState);

    virtual HRESULT STDMETHODCALLTYPE CreateQuery( 
        const D3D11_QUERY_DESC *pQueryDesc,
        ID3D11Query **ppQuery);

    virtual HRESULT STDMETHODCALLTYPE CreatePredicate( 
        const D3D11_QUERY_DESC *pPredicateDesc,
        ID3D11Predicate **ppPredicate);

    virtual HRESULT STDMETHODCALLTYPE CreateCounter( 
        const D3D11_COUNTER_DESC *pCounterDesc,
        ID3D11Counter **ppCounter);

    virtual HRESULT STDMETHODCALLTYPE CreateDeferredContext( 
        UINT ContextFlags,
        ID3D11DeviceContext **ppDeferredContext);

    virtual HRESULT STDMETHODCALLTYPE OpenSharedResource( 
        HANDLE hResource,
        REFIID ReturnedInterface,
        void **ppResource);

    virtual HRESULT STDMETHODCALLTYPE CheckFormatSupport( 
        DXGI_FORMAT Format,
        UINT *pFormatSupport);

    virtual HRESULT STDMETHODCALLTYPE CheckMultisampleQualityLevels( 
        DXGI_FORMAT Format,
        UINT SampleCount,
        UINT *pNumQualityLevels);

    virtual void STDMETHODCALLTYPE CheckCounterInfo( 
        D3D11_COUNTER_INFO *pCounterInfo);

    virtual HRESULT STDMETHODCALLTYPE CheckCounter( 
        const D3D11_COUNTER_DESC *pDesc,
        D3D11_COUNTER_TYPE *pType,
        UINT *pActiveCounters,
        LPSTR szName,
        UINT *pNameLength,
        LPSTR szUnits,
        UINT *pUnitsLength,
        LPSTR szDescription,
        UINT *pDescriptionLength);

    virtual HRESULT STDMETHODCALLTYPE CheckFeatureSupport( 
        D3D11_FEATURE Feature,
        void *pFeatureSupportData,
        UINT FeatureSupportDataSize);

    virtual HRESULT STDMETHODCALLTYPE GetPrivateData( 
        REFGUID guid,
        UINT *pDataSize,
        void *pData);

    virtual HRESULT STDMETHODCALLTYPE SetPrivateData( 
        REFGUID guid,
        UINT DataSize,
        const void *pData);

    virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface( 
        REFGUID guid,
        const IUnknown *pData);

    virtual D3D_FEATURE_LEVEL STDMETHODCALLTYPE GetFeatureLevel(void);

    virtual UINT STDMETHODCALLTYPE GetCreationFlags(void);

    virtual HRESULT STDMETHODCALLTYPE GetDeviceRemovedReason(void);

    virtual void STDMETHODCALLTYPE GetImmediateContext( 
        ID3D11DeviceContext **ppImmediateContext);

    virtual HRESULT STDMETHODCALLTYPE SetExceptionMode( 
        UINT RaiseFlags);

    virtual UINT STDMETHODCALLTYPE GetExceptionMode(void);
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      D3D11DeviceContextHookInterface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class D3D11DeviceContextHook : public TD3D11DeviceChildHook<ID3D11DeviceContext>
{
typedef ID3D11DeviceContext super;
public:
    virtual void STDMETHODCALLTYPE VSSetConstantBuffers( 
        UINT StartSlot,
        UINT NumBuffers,
        ID3D11Buffer *const *ppConstantBuffers);

    virtual void STDMETHODCALLTYPE PSSetShaderResources( 
        UINT StartSlot,
        UINT NumViews,
        ID3D11ShaderResourceView *const *ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE PSSetShader( 
        ID3D11PixelShader *pPixelShader,
        ID3D11ClassInstance *const *ppClassInstances,
        UINT NumClassInstances);

    virtual void STDMETHODCALLTYPE PSSetSamplers( 
        UINT StartSlot,
        UINT NumSamplers,
        ID3D11SamplerState *const *ppSamplers);

    virtual void STDMETHODCALLTYPE VSSetShader( 
        ID3D11VertexShader *pVertexShader,
        ID3D11ClassInstance *const *ppClassInstances,
        UINT NumClassInstances);

    virtual void STDMETHODCALLTYPE DrawIndexed( 
        UINT IndexCount,
        UINT StartIndexLocation,
        INT BaseVertexLocation);

    virtual void STDMETHODCALLTYPE Draw( 
        UINT VertexCount,
        UINT StartVertexLocation);

    virtual HRESULT STDMETHODCALLTYPE Map( 
        ID3D11Resource *pResource,
        UINT Subresource,
        D3D11_MAP MapType,
        UINT MapFlags,
        D3D11_MAPPED_SUBRESOURCE *pMappedResource);

    virtual void STDMETHODCALLTYPE Unmap( 
        ID3D11Resource *pResource,
        UINT Subresource);

    virtual void STDMETHODCALLTYPE PSSetConstantBuffers( 
        UINT StartSlot,
        UINT NumBuffers,
        ID3D11Buffer *const *ppConstantBuffers);

    virtual void STDMETHODCALLTYPE IASetInputLayout( 
        ID3D11InputLayout *pInputLayout);

    virtual void STDMETHODCALLTYPE IASetVertexBuffers( 
        UINT StartSlot,
        UINT NumBuffers,
        ID3D11Buffer *const *ppVertexBuffers,
        const UINT *pStrides,
        const UINT *pOffsets);

    virtual void STDMETHODCALLTYPE IASetIndexBuffer( 
        ID3D11Buffer *pIndexBuffer,
        DXGI_FORMAT Format,
        UINT Offset);

    virtual void STDMETHODCALLTYPE DrawIndexedInstanced( 
        UINT IndexCountPerInstance,
        UINT InstanceCount,
        UINT StartIndexLocation,
        INT BaseVertexLocation,
        UINT StartInstanceLocation);

    virtual void STDMETHODCALLTYPE DrawInstanced( 
        UINT VertexCountPerInstance,
        UINT InstanceCount,
        UINT StartVertexLocation,
        UINT StartInstanceLocation);

    virtual void STDMETHODCALLTYPE GSSetConstantBuffers( 
        UINT StartSlot,
        UINT NumBuffers,
        ID3D11Buffer *const *ppConstantBuffers);

    virtual void STDMETHODCALLTYPE GSSetShader( 
        ID3D11GeometryShader *pShader,
        ID3D11ClassInstance *const *ppClassInstances,
        UINT NumClassInstances);

    virtual void STDMETHODCALLTYPE IASetPrimitiveTopology( 
        D3D11_PRIMITIVE_TOPOLOGY Topology);

    virtual void STDMETHODCALLTYPE VSSetShaderResources( 
        UINT StartSlot,
        UINT NumViews,
        ID3D11ShaderResourceView *const *ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE VSSetSamplers( 
        UINT StartSlot,
        UINT NumSamplers,
        ID3D11SamplerState *const *ppSamplers);

    virtual void STDMETHODCALLTYPE Begin( 
        ID3D11Asynchronous *pAsync);

    virtual void STDMETHODCALLTYPE End( 
        ID3D11Asynchronous *pAsync);

    virtual HRESULT STDMETHODCALLTYPE GetData( 
        ID3D11Asynchronous *pAsync,
        void *pData,
        UINT DataSize,
        UINT GetDataFlags);

    virtual void STDMETHODCALLTYPE SetPredication( 
        ID3D11Predicate *pPredicate,
        BOOL PredicateValue);

    virtual void STDMETHODCALLTYPE GSSetShaderResources( 
        UINT StartSlot,
        UINT NumViews,
        ID3D11ShaderResourceView *const *ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE GSSetSamplers( 
        UINT StartSlot,
        UINT NumSamplers,
        ID3D11SamplerState *const *ppSamplers);

    virtual void STDMETHODCALLTYPE OMSetRenderTargets( 
        UINT NumViews,
        ID3D11RenderTargetView *const *ppRenderTargetViews,
        ID3D11DepthStencilView *pDepthStencilView);

    virtual void STDMETHODCALLTYPE OMSetRenderTargetsAndUnorderedAccessViews( 
        UINT NumRTVs,
        ID3D11RenderTargetView *const *ppRenderTargetViews,
        ID3D11DepthStencilView *pDepthStencilView,
        UINT UAVStartSlot,
        UINT NumUAVs,
        ID3D11UnorderedAccessView *const *ppUnorderedAccessViews,
        const UINT *pUAVInitialCounts);

    virtual void STDMETHODCALLTYPE OMSetBlendState( 
        ID3D11BlendState *pBlendState,
        const FLOAT BlendFactor[ 4 ],
        UINT SampleMask);

    virtual void STDMETHODCALLTYPE OMSetDepthStencilState( 
        ID3D11DepthStencilState *pDepthStencilState,
        UINT StencilRef);

    virtual void STDMETHODCALLTYPE SOSetTargets( 
        UINT NumBuffers,
        ID3D11Buffer *const *ppSOTargets,
        const UINT *pOffsets);

    virtual void STDMETHODCALLTYPE DrawAuto( void);

    virtual void STDMETHODCALLTYPE DrawIndexedInstancedIndirect( 
        ID3D11Buffer *pBufferForArgs,
        UINT AlignedByteOffsetForArgs);

    virtual void STDMETHODCALLTYPE DrawInstancedIndirect( 
        ID3D11Buffer *pBufferForArgs,
        UINT AlignedByteOffsetForArgs);

    virtual void STDMETHODCALLTYPE Dispatch( 
        UINT ThreadGroupCountX,
        UINT ThreadGroupCountY,
        UINT ThreadGroupCountZ);

    virtual void STDMETHODCALLTYPE DispatchIndirect( 
        ID3D11Buffer *pBufferForArgs,
        UINT AlignedByteOffsetForArgs);

    virtual void STDMETHODCALLTYPE RSSetState( 
        ID3D11RasterizerState *pRasterizerState);

    virtual void STDMETHODCALLTYPE RSSetViewports( 
        UINT NumViewports,
        const D3D11_VIEWPORT *pViewports);

    virtual void STDMETHODCALLTYPE RSSetScissorRects( 
        UINT NumRects,
        const D3D11_RECT *pRects);

    virtual void STDMETHODCALLTYPE CopySubresourceRegion( 
        ID3D11Resource *pDstResource,
        UINT DstSubresource,
        UINT DstX,
        UINT DstY,
        UINT DstZ,
        ID3D11Resource *pSrcResource,
        UINT SrcSubresource,
        const D3D11_BOX *pSrcBox);

    virtual void STDMETHODCALLTYPE CopyResource( 
        ID3D11Resource *pDstResource,
        ID3D11Resource *pSrcResource);

    virtual void STDMETHODCALLTYPE UpdateSubresource( 
        ID3D11Resource *pDstResource,
        UINT DstSubresource,
        const D3D11_BOX *pDstBox,
        const void *pSrcData,
        UINT SrcRowPitch,
        UINT SrcDepthPitch);

    virtual void STDMETHODCALLTYPE CopyStructureCount( 
        ID3D11Buffer *pDstBuffer,
        UINT DstAlignedByteOffset,
        ID3D11UnorderedAccessView *pSrcView);

    virtual void STDMETHODCALLTYPE ClearRenderTargetView( 
        ID3D11RenderTargetView *pRenderTargetView,
        const FLOAT ColorRGBA[ 4 ]);

    virtual void STDMETHODCALLTYPE ClearUnorderedAccessViewUint( 
        ID3D11UnorderedAccessView *pUnorderedAccessView,
        const UINT Values[ 4 ]);

    virtual void STDMETHODCALLTYPE ClearUnorderedAccessViewFloat( 
        ID3D11UnorderedAccessView *pUnorderedAccessView,
        const FLOAT Values[ 4 ]);

    virtual void STDMETHODCALLTYPE ClearDepthStencilView( 
        ID3D11DepthStencilView *pDepthStencilView,
        UINT ClearFlags,
        FLOAT Depth,
        UINT8 Stencil);

    virtual void STDMETHODCALLTYPE GenerateMips( 
        ID3D11ShaderResourceView *pShaderResourceView);

    virtual void STDMETHODCALLTYPE SetResourceMinLOD( 
        ID3D11Resource *pResource,
        FLOAT MinLOD);

    virtual FLOAT STDMETHODCALLTYPE GetResourceMinLOD( 
        ID3D11Resource *pResource);

    virtual void STDMETHODCALLTYPE ResolveSubresource( 
        ID3D11Resource *pDstResource,
        UINT DstSubresource,
        ID3D11Resource *pSrcResource,
        UINT SrcSubresource,
        DXGI_FORMAT Format);

    virtual void STDMETHODCALLTYPE ExecuteCommandList( 
        ID3D11CommandList *pCommandList,
        BOOL RestoreContextState);

    virtual void STDMETHODCALLTYPE HSSetShaderResources( 
        UINT StartSlot,
        UINT NumViews,
        ID3D11ShaderResourceView *const *ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE HSSetShader( 
        ID3D11HullShader *pHullShader,
        ID3D11ClassInstance *const *ppClassInstances,
        UINT NumClassInstances);

    virtual void STDMETHODCALLTYPE HSSetSamplers( 
        UINT StartSlot,
        UINT NumSamplers,
        ID3D11SamplerState *const *ppSamplers);

    virtual void STDMETHODCALLTYPE HSSetConstantBuffers( 
        UINT StartSlot,
        UINT NumBuffers,
        ID3D11Buffer *const *ppConstantBuffers);

    virtual void STDMETHODCALLTYPE DSSetShaderResources( 
        UINT StartSlot,
        UINT NumViews,
        ID3D11ShaderResourceView *const *ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE DSSetShader( 
        ID3D11DomainShader *pDomainShader,
        ID3D11ClassInstance *const *ppClassInstances,
        UINT NumClassInstances);

    virtual void STDMETHODCALLTYPE DSSetSamplers( 
        UINT StartSlot,
        UINT NumSamplers,
        ID3D11SamplerState *const *ppSamplers);

    virtual void STDMETHODCALLTYPE DSSetConstantBuffers( 
        UINT StartSlot,
        UINT NumBuffers,
        ID3D11Buffer *const *ppConstantBuffers);

    virtual void STDMETHODCALLTYPE CSSetShaderResources( 
        UINT StartSlot,
        UINT NumViews,
        ID3D11ShaderResourceView *const *ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE CSSetUnorderedAccessViews( 
        UINT StartSlot,
        UINT NumUAVs,
        ID3D11UnorderedAccessView *const *ppUnorderedAccessViews,
        const UINT *pUAVInitialCounts);

    virtual void STDMETHODCALLTYPE CSSetShader( 
        ID3D11ComputeShader *pComputeShader,
        ID3D11ClassInstance *const *ppClassInstances,
        UINT NumClassInstances);

    virtual void STDMETHODCALLTYPE CSSetSamplers( 
        UINT StartSlot,
        UINT NumSamplers,
        ID3D11SamplerState *const *ppSamplers);

    virtual void STDMETHODCALLTYPE CSSetConstantBuffers( 
        UINT StartSlot,
        UINT NumBuffers,
        ID3D11Buffer *const *ppConstantBuffers);

    virtual void STDMETHODCALLTYPE VSGetConstantBuffers( 
        UINT StartSlot,
        UINT NumBuffers,
        ID3D11Buffer **ppConstantBuffers);

    virtual void STDMETHODCALLTYPE PSGetShaderResources( 
        UINT StartSlot,
        UINT NumViews,
        ID3D11ShaderResourceView **ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE PSGetShader( 
        ID3D11PixelShader **ppPixelShader,
        ID3D11ClassInstance **ppClassInstances,
        UINT *pNumClassInstances);

    virtual void STDMETHODCALLTYPE PSGetSamplers( 
        UINT StartSlot,
        UINT NumSamplers,
        ID3D11SamplerState **ppSamplers);

    virtual void STDMETHODCALLTYPE VSGetShader( 
        ID3D11VertexShader **ppVertexShader,
        ID3D11ClassInstance **ppClassInstances,
        UINT *pNumClassInstances);

    virtual void STDMETHODCALLTYPE PSGetConstantBuffers( 
        UINT StartSlot,
        UINT NumBuffers,
        ID3D11Buffer **ppConstantBuffers);

    virtual void STDMETHODCALLTYPE IAGetInputLayout( 
        ID3D11InputLayout **ppInputLayout);

    virtual void STDMETHODCALLTYPE IAGetVertexBuffers( 
        UINT StartSlot,
        UINT NumBuffers,
        ID3D11Buffer **ppVertexBuffers,
        UINT *pStrides,
        UINT *pOffsets);

    virtual void STDMETHODCALLTYPE IAGetIndexBuffer( 
        ID3D11Buffer **pIndexBuffer,
        DXGI_FORMAT *Format,
        UINT *Offset);

    virtual void STDMETHODCALLTYPE GSGetConstantBuffers( 
        UINT StartSlot,
        UINT NumBuffers,
        ID3D11Buffer **ppConstantBuffers);

    virtual void STDMETHODCALLTYPE GSGetShader( 
        ID3D11GeometryShader **ppGeometryShader,
        ID3D11ClassInstance **ppClassInstances,
        UINT *pNumClassInstances);

    virtual void STDMETHODCALLTYPE IAGetPrimitiveTopology( 
        D3D11_PRIMITIVE_TOPOLOGY *pTopology);

    virtual void STDMETHODCALLTYPE VSGetShaderResources( 
        UINT StartSlot,
        UINT NumViews,
        ID3D11ShaderResourceView **ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE VSGetSamplers( 
        UINT StartSlot,
        UINT NumSamplers,
        ID3D11SamplerState **ppSamplers);

    virtual void STDMETHODCALLTYPE GetPredication( 
        ID3D11Predicate **ppPredicate,
        BOOL *pPredicateValue);

    virtual void STDMETHODCALLTYPE GSGetShaderResources( 
        UINT StartSlot,
        UINT NumViews,
        ID3D11ShaderResourceView **ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE GSGetSamplers( 
        UINT StartSlot,
        UINT NumSamplers,
        ID3D11SamplerState **ppSamplers);

    virtual void STDMETHODCALLTYPE OMGetRenderTargets( 
        UINT NumViews,
        ID3D11RenderTargetView **ppRenderTargetViews,
        ID3D11DepthStencilView **ppDepthStencilView);

    virtual void STDMETHODCALLTYPE OMGetRenderTargetsAndUnorderedAccessViews( 
        UINT NumRTVs,
        ID3D11RenderTargetView **ppRenderTargetViews,
        ID3D11DepthStencilView **ppDepthStencilView,
        UINT UAVStartSlot,
        UINT NumUAVs,
        ID3D11UnorderedAccessView **ppUnorderedAccessViews);

    virtual void STDMETHODCALLTYPE OMGetBlendState( 
        ID3D11BlendState **ppBlendState,
        FLOAT BlendFactor[ 4 ],
        UINT *pSampleMask);

    virtual void STDMETHODCALLTYPE OMGetDepthStencilState( 
        ID3D11DepthStencilState **ppDepthStencilState,
        UINT *pStencilRef);

    virtual void STDMETHODCALLTYPE SOGetTargets( 
        UINT NumBuffers,
        ID3D11Buffer **ppSOTargets);

    virtual void STDMETHODCALLTYPE RSGetState( 
        ID3D11RasterizerState **ppRasterizerState);

    virtual void STDMETHODCALLTYPE RSGetViewports( 
        UINT *pNumViewports,
        D3D11_VIEWPORT *pViewports);

    virtual void STDMETHODCALLTYPE RSGetScissorRects( 
        UINT *pNumRects,
        D3D11_RECT *pRects);

    virtual void STDMETHODCALLTYPE HSGetShaderResources( 
        UINT StartSlot,
        UINT NumViews,
        ID3D11ShaderResourceView **ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE HSGetShader( 
        ID3D11HullShader **ppHullShader,
        ID3D11ClassInstance **ppClassInstances,
        UINT *pNumClassInstances);

    virtual void STDMETHODCALLTYPE HSGetSamplers( 
        UINT StartSlot,
        UINT NumSamplers,
        ID3D11SamplerState **ppSamplers);

    virtual void STDMETHODCALLTYPE HSGetConstantBuffers( 
        UINT StartSlot,
        UINT NumBuffers,
        ID3D11Buffer **ppConstantBuffers);

    virtual void STDMETHODCALLTYPE DSGetShaderResources( 
        UINT StartSlot,
        UINT NumViews,
        ID3D11ShaderResourceView **ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE DSGetShader( 
        ID3D11DomainShader **ppDomainShader,
        ID3D11ClassInstance **ppClassInstances,
        UINT *pNumClassInstances);

    virtual void STDMETHODCALLTYPE DSGetSamplers( 
        UINT StartSlot,
        UINT NumSamplers,
        ID3D11SamplerState **ppSamplers);

    virtual void STDMETHODCALLTYPE DSGetConstantBuffers( 
        UINT StartSlot,
        UINT NumBuffers,
        ID3D11Buffer **ppConstantBuffers);

    virtual void STDMETHODCALLTYPE CSGetShaderResources( 
        UINT StartSlot,
        UINT NumViews,
        ID3D11ShaderResourceView **ppShaderResourceViews);

    virtual void STDMETHODCALLTYPE CSGetUnorderedAccessViews( 
        UINT StartSlot,
        UINT NumUAVs,
        ID3D11UnorderedAccessView **ppUnorderedAccessViews);

    virtual void STDMETHODCALLTYPE CSGetShader( 
        ID3D11ComputeShader **ppComputeShader,
        ID3D11ClassInstance **ppClassInstances,
        UINT *pNumClassInstances);

    virtual void STDMETHODCALLTYPE CSGetSamplers( 
        UINT StartSlot,
        UINT NumSamplers,
        ID3D11SamplerState **ppSamplers);

    virtual void STDMETHODCALLTYPE CSGetConstantBuffers( 
        UINT StartSlot,
        UINT NumBuffers,
        ID3D11Buffer **ppConstantBuffers);

    virtual void STDMETHODCALLTYPE ClearState( void);

    virtual void STDMETHODCALLTYPE Flush( void);

    virtual D3D11_DEVICE_CONTEXT_TYPE STDMETHODCALLTYPE GetType( void);

    virtual UINT STDMETHODCALLTYPE GetContextFlags( void);

    virtual HRESULT STDMETHODCALLTYPE FinishCommandList( 
        BOOL RestoreDeferredContextState,
        ID3D11CommandList **ppCommandList);
};


class D3D11AsynchronousHook : public TD3D11AsynchronousHook<ID3D11Asynchronous>
{
public:
};

class D3D11QueryHook : public TD3D11QueryHook<ID3D11Query>
{
public:
};

class D3D11PredicateHook : public TD3D11QueryHook<ID3D11Predicate>
{
public:
};

class D3D11BlendStateHook : public TD3D11DeviceChildHook<ID3D11BlendState>
{
public:
    virtual void STDMETHODCALLTYPE GetDesc( 
        D3D11_BLEND_DESC *pDesc);
};

class D3D11CounterHook : public TD3D11AsynchronousHook<ID3D11Counter>
{
public:
    virtual void STDMETHODCALLTYPE GetDesc( 
        D3D11_COUNTER_DESC *pDesc);
};

class D3D11CommandListHook : public TD3D11DeviceChildHook<ID3D11CommandList>
{
public:
    virtual UINT STDMETHODCALLTYPE GetContextFlags(void);
};

class D3D11DepthStencilStateHook : public TD3D11DeviceChildHook<ID3D11DepthStencilState>
{
public:
    virtual void STDMETHODCALLTYPE GetDesc( 
        D3D11_DEPTH_STENCIL_DESC *pDesc);
};

class D3D11InputLayoutHook : public TD3D11DeviceChildHook<ID3D11InputLayout>
{
public:
};

class D3D11RasterizerStateHook : public TD3D11DeviceChildHook<ID3D11RasterizerState>
{
public:
    virtual void STDMETHODCALLTYPE GetDesc( 
        D3D11_RASTERIZER_DESC *pDesc);
};

class D3D11SamplerStateHook : public TD3D11DeviceChildHook<ID3D11SamplerState>
{
public:
    virtual void STDMETHODCALLTYPE GetDesc( 
        D3D11_SAMPLER_DESC *pDesc);
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      resource hook interface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class D3D11BufferHook : public TD3D11ResourceHook<ID3D11Buffer>
{
public:
    virtual void STDMETHODCALLTYPE GetDesc( 
        D3D11_BUFFER_DESC *pDesc);
};

class D3D11Texture1DHook : public TD3D11ResourceHook<ID3D11Texture1D>
{
public:
    virtual void STDMETHODCALLTYPE GetDesc( 
        D3D11_TEXTURE1D_DESC *pDesc);
};

class D3D11Texture2DHook : public TD3D11ResourceHook<ID3D11Texture2D>
{
public:
    virtual void STDMETHODCALLTYPE GetDesc( 
        D3D11_TEXTURE2D_DESC *pDesc);
};

class D3D11Texture3DHook : public TD3D11ResourceHook<ID3D11Texture3D>
{
public:
    virtual void STDMETHODCALLTYPE GetDesc( 
        D3D11_TEXTURE3D_DESC *pDesc);
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      view hook interface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class D3D11DepthStencilViewHook : public TD3D11ViewHook<ID3D11DepthStencilView>
{
public:
    virtual void STDMETHODCALLTYPE GetDesc( 
        D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc);
};

class D3D11RenderTargetViewHook : public TD3D11ViewHook<ID3D11RenderTargetView>
{
public:
    virtual void STDMETHODCALLTYPE GetDesc( 
        D3D11_RENDER_TARGET_VIEW_DESC *pDesc);
};

class D3D11ShaderResourceViewHook : public TD3D11ViewHook<ID3D11ShaderResourceView>
{
public:
    virtual void STDMETHODCALLTYPE GetDesc( 
        D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc);
};

class D3D11UnorderedAccessViewHook : public TD3D11ViewHook<ID3D11UnorderedAccessView>
{
public:
    virtual void STDMETHODCALLTYPE GetDesc( 
        D3D11_UNORDERED_ACCESS_VIEW_DESC *pDesc);
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      shader hook interface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class D3D11ClassInstanceHook : public TD3D11DeviceChildHook<ID3D11ClassInstance>
{
public:
    virtual void STDMETHODCALLTYPE GetClassLinkage( 
        ID3D11ClassLinkage **ppLinkage);

    virtual void STDMETHODCALLTYPE GetDesc( 
        D3D11_CLASS_INSTANCE_DESC *pDesc);

    virtual void STDMETHODCALLTYPE GetInstanceName( 
        LPSTR pInstanceName,
        SIZE_T *pBufferLength);

    virtual void STDMETHODCALLTYPE GetTypeName( 
        LPSTR pTypeName,
        SIZE_T *pBufferLength);
};

class D3D11ClassLinkageHook : public TD3D11DeviceChildHook<ID3D11ClassLinkage>
{
public:
    virtual HRESULT STDMETHODCALLTYPE GetClassInstance( 
        LPCSTR pClassInstanceName,
        UINT InstanceIndex,
        ID3D11ClassInstance **ppInstance);

    virtual HRESULT STDMETHODCALLTYPE CreateClassInstance( 
        LPCSTR pClassTypeName,
        UINT ConstantBufferOffset,
        UINT ConstantVectorOffset,
        UINT TextureOffset,
        UINT SamplerOffset,
        ID3D11ClassInstance **ppInstance);
};

class D3D11VertexShaderHook : public TD3D11DeviceChildHook<ID3D11VertexShader>
{
public:
};

class D3D11PixelShaderHook : public TD3D11DeviceChildHook<ID3D11PixelShader>
{
public:
};

class D3D11GeometryShaderHook : public TD3D11DeviceChildHook<ID3D11GeometryShader>
{
public:
};

class D3D11HullShaderHook : public TD3D11DeviceChildHook<ID3D11HullShader>
{
public:
};

class D3D11DomainShaderHook : public TD3D11DeviceChildHook<ID3D11DomainShader>
{
public:
};

class D3D11ComputeShaderHook : public TD3D11DeviceChildHook<ID3D11ComputeShader>
{
public:
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      Hook API
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void D3D11SetHookDirect(IUnknown *pTarget, void **vtable);

// IUnknown* とかで一括処理できるが、ミスが起きやすそうな部分なので型チェックのため個別に用意
void D3D11SetHookInstanciated(IDXGISwapChain *pTarget, IDXGISwapChain *pHook);

void D3D11SetHookInstanciated(ID3D11Device *pTarget, ID3D11Device *pHook);
void D3D11SetHookInstanciated(ID3D11DeviceContext *pTarget, ID3D11DeviceContext *pHook);
void D3D11SetHookInstanciated(ID3D11Asynchronous *pTarget, ID3D11Asynchronous *pHook);
void D3D11SetHookInstanciated(ID3D11BlendState *pTarget, ID3D11BlendState *pHook);
void D3D11SetHookInstanciated(ID3D11Counter *pTarget, ID3D11Counter *pHook);
void D3D11SetHookInstanciated(ID3D11CommandList *pTarget, ID3D11CommandList *pHook);
void D3D11SetHookInstanciated(ID3D11DepthStencilState *pTarget, ID3D11DepthStencilState *pHook);
void D3D11SetHookInstanciated(ID3D11InputLayout *pTarget, ID3D11InputLayout *pHook);
void D3D11SetHookInstanciated(ID3D11Predicate *pTarget, ID3D11Predicate *pHook);
void D3D11SetHookInstanciated(ID3D11Query *pTarget, ID3D11Query *pHook);
void D3D11SetHookInstanciated(ID3D11RasterizerState *pTarget, ID3D11RasterizerState *pHook);
void D3D11SetHookInstanciated(ID3D11SamplerState *pTarget, ID3D11SamplerState *pHook);

void D3D11SetHookInstanciated(ID3D11Buffer *pTarget, ID3D11Buffer *pHook);
void D3D11SetHookInstanciated(ID3D11Texture1D *pTarget, ID3D11Texture1D *pHook);
void D3D11SetHookInstanciated(ID3D11Texture2D *pTarget, ID3D11Texture2D *pHook);
void D3D11SetHookInstanciated(ID3D11Texture3D *pTarget, ID3D11Texture3D *pHook);

void D3D11SetHookInstanciated(ID3D11DepthStencilView *pTarget, ID3D11DepthStencilView *pHook);
void D3D11SetHookInstanciated(ID3D11RenderTargetView *pTarget, ID3D11RenderTargetView *pHook);
void D3D11SetHookInstanciated(ID3D11ShaderResourceView *pTarget, ID3D11ShaderResourceView *pHook);
void D3D11SetHookInstanciated(ID3D11UnorderedAccessView *pTarget, ID3D11UnorderedAccessView *pHook);

void D3D11SetHookInstanciated(ID3D11ClassInstance *pTarget, ID3D11ClassInstance *pHook);
void D3D11SetHookInstanciated(ID3D11ClassLinkage *pTarget, ID3D11ClassLinkage *pHook);
void D3D11SetHookInstanciated(ID3D11VertexShader *pTarget, ID3D11VertexShader *pHook);
void D3D11SetHookInstanciated(ID3D11PixelShader *pTarget, ID3D11PixelShader *pHook);
void D3D11SetHookInstanciated(ID3D11GeometryShader *pTarget, ID3D11GeometryShader *pHook);
void D3D11SetHookInstanciated(ID3D11HullShader *pTarget, ID3D11HullShader *pHook);
void D3D11SetHookInstanciated(ID3D11DomainShader *pTarget, ID3D11DomainShader *pHook);
void D3D11SetHookInstanciated(ID3D11ComputeShader *pTarget, ID3D11ComputeShader *pHook);


void D3D11RemoveHookDirect(IUnknown *pTarget, void **vtable);
void D3D11RemoveHookInstanciated(IUnknown *pTarget, IUnknown *pHook);
template<class HookType> inline void D3D11RemoveHook(IUnknown *pTarget) { HookType v; D3D11RemoveHookInstanciated(pTarget, &v); }
void D3D11RemoveAllHooks(IUnknown *pTarget);


template<class HookType> inline void D3D11SetHook(IDXGISwapChain *pTarget)              { HookType v; D3D11SetHookInstanciated(pTarget, &v); }

template<class HookType> inline void D3D11SetHook(ID3D11Device *pTarget)                { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11DeviceContext *pTarget)         { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11Asynchronous *pTarget)          { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11BlendState *pTarget)            { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11Counter *pTarget)               { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11CommandList *pTarget)           { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11DepthStencilState *pTarget)     { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11InputLayout *pTarget)           { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11Predicate *pTarget)             { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11Query *pTarget)                 { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11RasterizerState *pTarget)       { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11SamplerState *pTarget)          { HookType v; D3D11SetHookInstanciated(pTarget, &v); }

template<class HookType> inline void D3D11SetHook(ID3D11Buffer *pTarget)                { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11Texture1D *pTarget)             { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11Texture2D *pTarget)             { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11Texture3D *pTarget)             { HookType v; D3D11SetHookInstanciated(pTarget, &v); }

template<class HookType> inline void D3D11SetHook(ID3D11DepthStencilView *pTarget)      { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11RenderTargetView *pTarget)      { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11ShaderResourceView *pTarget)    { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11UnorderedAccessView *pTarget)   { HookType v; D3D11SetHookInstanciated(pTarget, &v); }

template<class HookType> inline void D3D11SetHook(ID3D11ClassInstance *pTarget)         { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11ClassLinkage *pTarget)          { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11VertexShader *pTarget)          { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11PixelShader *pTarget)           { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11GeometryShader *pTarget)        { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11HullShader *pTarget)            { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11DomainShader *pTarget)          { HookType v; D3D11SetHookInstanciated(pTarget, &v); }
template<class HookType> inline void D3D11SetHook(ID3D11ComputeShader *pTarget)         { HookType v; D3D11SetHookInstanciated(pTarget, &v); }

#endif // _ist_D3D11HookInterface_h_
