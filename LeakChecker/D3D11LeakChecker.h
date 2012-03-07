#ifndef _ist_D3D11LeakChecker_h_
#define _ist_D3D11LeakChecker_h_

// DirectX11 のリソースのリークチェック機能を提供します。
// D3D11LeakCheckerPrintLeakInfo() を呼んだ時点で解放されてないリソースを、作成された場所のコールスタックと共に表示します。
// 
// D3D11LEAKCHECKER_ENABLE_ADDREF_TRACE を define している場合、追加で AddRef() / Release() した場所のコールスタックと呼ばれた回数を表示します。
// D3D11LEAKCHECKER_ENABLE_ADDREF_TRACE は相応のコストがかかると思われます。


#define D3D11LEAKCHECKER_MAX_CALLSTACK_SIZE 32
#define D3D11LEAKCHECKER_ENABLE_ADDREF_TRACE

enum D3D11LC_OPTION {
    D3D11LC_NONE = 0,

    // symbol の初期化/終了処理を行うか (SymInitialize()/SymCleanup())
    // 他のモジュールと競合する場合などはこのオプションを無効にする。
    D3D11LC_INIT_SYMBOLS = 1,

    // このオプションを入れると、D3D11SetHook() ではなく D3DAddHook() で hook を仕込む。
    // 他のモジュールが既に hook を仕込んでいる場合有効にする。
    D3D11LC_HOOK_ADD = 2,
};

// opt: D3D11LC_OPTION の bit の組み合わせ
void D3D11LeakCheckerInitialize(IDXGISwapChain *pSwapChain, ID3D11Device *pDevice, int opt=D3D11LC_INIT_SYMBOLS);
void D3D11LeakCheckerFinalize();

void D3D11LeakCheckerPrintLeakInfo();

#endif // _ist_D3D11LeakChecker_h_
