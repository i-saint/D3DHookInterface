#ifndef _ist_D3D11LeakChecker_h_
#define _ist_D3D11LeakChecker_h_

// DirectX11 のリソースのリークチェック機能を提供します。
// 


#define D3D11LEAKCHECKER_MAX_CALLSTACK_SIZE 32
#define D3D11LEAKCHECKER_MAX_ADDREF_TRACE 16
#define D3D11LEAKCHECKER_ENABLE_ADDREF_TRACE

void D3D11LeakCheckerInitialize(IDXGISwapChain *pSwapChain, ID3D11Device *pDevice);
void D3D11LeakCheckerFinalize();

void D3D11LeakCheckerPrintLeakInfo();

#endif // _ist_D3D11LeakChecker_h_
