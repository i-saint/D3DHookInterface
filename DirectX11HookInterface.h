#ifndef _ist_DirectX11HookInterface_h_
#define _ist_DirectX11HookInterface_h_
#include <d3d11.h>

class DXGISwapChainHookInterface;
class D3D11DeviceHookInterface;
class D3D11DeviceContextHookInterface;

class D3D11BufferHookInterface;
class D3D11Texture1DHookInterface;
class D3D11Texture2DHookInterface;
class D3D11Texture3DHookInterface;

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

    SetD3D11DeviceContextHookInterface<HookTestD3D11DeviceContext>(g_pImmediateContext);
    // 多重 hook も可能。最後に登録したものから順に呼ばれます
    SetD3D11DeviceContextHookInterface<HookTestD3D11DeviceContext>(g_pImmediateContext);
*/

template<class HookInterfaceType>
void SetDXGISwapChainHookInterface(IDXGISwapChain *pTarget);

template<class HookInterfaceType>
void SetD3D11DeviceHookInterface(ID3D11Device *pTarget);

template<class HookInterfaceType>
void SetD3D11DeviceContextHookInterface(ID3D11DeviceContext *pTarget);


template<class HookInterfaceType>
void SetD3D11BufferHookInterface(ID3D11Buffer *pTarget);

template<class HookInterfaceType>
void SetD3D11Texture1DHookInterface(ID3D11Texture1D *pTarget);

template<class HookInterfaceType>
void SetD3D11Texture2DHookInterface(ID3D11Texture2D *pTarget);

template<class HookInterfaceType>
void SetD3D11Texture3DHookInterface(ID3D11Texture3D *pTarget);




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      template implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
class TUnknownHookInterface : public T
{
public:
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
        REFIID riid,
        __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject);

    virtual ULONG STDMETHODCALLTYPE AddRef( void);

    virtual ULONG STDMETHODCALLTYPE Release( void);
};

template<class T>
class TDXGIObjectHookInterface : public TUnknownHookInterface<T>
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
class TDXGIDeviceSubObjectHookInterface : public TDXGIObjectHookInterface<T>
{
public:
    virtual HRESULT STDMETHODCALLTYPE GetDevice( 
        REFIID riid,
        void **ppDevice);
};

template<class T>
class TD3D11DeviceChildHookInterface : public TUnknownHookInterface<T>
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
class TD3D11ResourceHookInterface : public TD3D11DeviceChildHookInterface<T>
{
public:
    virtual void STDMETHODCALLTYPE GetType( 
        D3D11_RESOURCE_DIMENSION *pResourceDimension);

    virtual void STDMETHODCALLTYPE SetEvictionPriority( 
        UINT EvictionPriority);

    virtual UINT STDMETHODCALLTYPE GetEvictionPriority( void);
};



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      DXGISwapChainHookInterface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class DXGISwapChainHookInterface : public TDXGIDeviceSubObjectHookInterface<IDXGISwapChain>
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
class D3D11DeviceHookInterface : public TUnknownHookInterface<ID3D11Device>
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

    virtual D3D_FEATURE_LEVEL STDMETHODCALLTYPE GetFeatureLevel( void);

    virtual UINT STDMETHODCALLTYPE GetCreationFlags( void);

    virtual HRESULT STDMETHODCALLTYPE GetDeviceRemovedReason( void);

    virtual void STDMETHODCALLTYPE GetImmediateContext( 
        ID3D11DeviceContext **ppImmediateContext);

    virtual HRESULT STDMETHODCALLTYPE SetExceptionMode( 
        UINT RaiseFlags);

    virtual UINT STDMETHODCALLTYPE GetExceptionMode( void);

};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      D3D11DeviceContextHookInterface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class D3D11DeviceContextHookInterface : public TD3D11DeviceChildHookInterface<ID3D11DeviceContext>
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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      D3D11BufferHookInterface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class D3D11BufferHookInterface : public TD3D11ResourceHookInterface<ID3D11Buffer>
{
public:
    virtual void STDMETHODCALLTYPE GetDesc( 
        D3D11_BUFFER_DESC *pDesc);
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      D3D11Texture1DHookInterface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class D3D11Texture1DHookInterface : public TD3D11ResourceHookInterface<ID3D11Texture1D>
{
public:
    virtual void STDMETHODCALLTYPE GetDesc( 
        D3D11_TEXTURE1D_DESC *pDesc);
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      D3D11Texture2DHookInterface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class D3D11Texture2DHookInterface : public TD3D11ResourceHookInterface<ID3D11Texture2D>
{
public:
    virtual void STDMETHODCALLTYPE GetDesc( 
        D3D11_TEXTURE2D_DESC *pDesc);
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      D3D11Texture3DHookInterface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class D3D11Texture3DHookInterface : public TD3D11ResourceHookInterface<ID3D11Texture3D>
{
public:
    virtual void STDMETHODCALLTYPE GetDesc( 
        D3D11_TEXTURE3D_DESC *pDesc);
};



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      Hook API
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// void* で一括処理できるが、ミスが起きやすそうな部分なので型チェックのため個別に用意
void _SetDXGISwapChainHookInterface(IDXGISwapChain *pTarget, IDXGISwapChain *pHook);
void _SetD3D11DeviceHookInterface(ID3D11Device *pTarget, ID3D11Device *pHook);
void _SetD3D11DeviceContextHookInterface(ID3D11DeviceContext *pTarget, ID3D11DeviceContext *pHook);

void _SetD3D11BufferHookInterface(ID3D11Buffer *pTarget, ID3D11Buffer *pHook);
void _SetD3D11Texture1DHookInterface(ID3D11Texture1D *pTarget, ID3D11Texture1D *pHook);
void _SetD3D11Texture2DHookInterface(ID3D11Texture2D *pTarget, ID3D11Texture2D *pHook);
void _SetD3D11Texture3DHookInterface(ID3D11Texture3D *pTarget, ID3D11Texture3D *pHook);


template<class HookInterfaceType>
inline void SetDXGISwapChainHookInterface(IDXGISwapChain *pTarget)
{
    HookInterfaceType instanciated;
    _SetDXGISwapChainHookInterface(pTarget, &instanciated);
}

template<class HookInterfaceType>
inline void SetD3D11DeviceHookInterface(ID3D11Device *pTarget)
{
    HookInterfaceType instanciated;
    _SetD3D11DeviceHookInterface(pTarget, &instanciated);
}

template<class HookInterfaceType>
inline void SetD3D11DeviceContextHookInterface(ID3D11DeviceContext *pTarget)
{
    HookInterfaceType instanciated;
    _SetD3D11DeviceContextHookInterface(pTarget, &instanciated);
}


template<class HookInterfaceType>
void SetD3D11BufferHookInterface(ID3D11Buffer *pTarget)
{
    HookInterfaceType instanciated;
    _SetD3D11BufferHookInterface(pTarget, &instanciated);
}

template<class HookInterfaceType>
void SetD3D11Texture1DHookInterface(ID3D11Texture1D *pTarget)
{
    HookInterfaceType instanciated;
    _SetD3D11Texture1DHookInterface(pTarget, &instanciated);
}

template<class HookInterfaceType>
void SetD3D11Texture2DHookInterface(ID3D11Texture2D *pTarget)
{
    HookInterfaceType instanciated;
    _SetD3D11Texture2DHookInterface(pTarget, &instanciated);
}

template<class HookInterfaceType>
void SetD3D11Texture3DHookInterface(ID3D11Texture3D *pTarget)
{
    HookInterfaceType instanciated;
    _SetD3D11Texture3DHookInterface(pTarget, &instanciated);
}

#endif // _ist_DirectX11HookInterface_h_
