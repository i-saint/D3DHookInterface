#ifndef _ist_D3D11LeakChecker_h_
#define _ist_D3D11LeakChecker_h_
#include <D3D11.h>

// DirectX11 のリソースのリークチェック機能を提供します。
// D3D11LeakCheckerPrintLeakInfo() を呼んだ時点で解放されてないリソースを、作成された場所のコールスタックと共に表示します。
// 
// 有効にするには、このファイルを include する前に D3D11LEAKCHECKER_ENABLE を define しておく必要があります。
// D3D11LEAKCHECKER_ENABLE を define しなかった場合、API は空の定義に置き換えられます。
// 
// D3D11LEAKCHECKER_ENABLE_ADDREF_TRACE を define している場合、追加で AddRef() / Release() した場所のコールスタックと呼ばれた回数を表示します。
// D3D11LEAKCHECKER_ENABLE_ADDREF_TRACE は相応のコストがかかると思われます。
// 
// また、NVIDIA Nsight を使用する際 (Nvda.Graphics.Interception.100.dll などが読み込まれてるのが検出された時) は leak checker は無効化されます。
// これはそうしないとクラッシュするためで、おそらく Nsight も D3D11 interface の hook か何かをやっており、競合しているためと予想されます。
// (他にも競合でクラッシュを招くソフトがあるかもしれません。プロファイル系や動画撮影系ソフトは危険候補です。
//  とりあえず PIX、Intel GPA、Dxtory は併用可能なのを確認済みです。)


#define D3D11LEAKCHECKER_MAX_CALLSTACK_SIZE 32
#define D3D11LEAKCHECKER_ENABLE_ADDREF_TRACE


enum D3D11LC_OPTION {
    D3D11LC_NONE = 0,

    // symbol の初期化/終了処理を行うか (SymInitialize()/SymCleanup())
    // デフォルトで有効。他のモジュールと競合する場合などはこのオプションを無効にします。
    D3D11LC_INIT_SYMBOLS = 1,
};

#ifdef D3D11LEAKCHECKER_ENABLE

// opt: D3D11LC_OPTION の bit の組み合わせ
bool _D3D11LeakCheckerInitialize(IDXGISwapChain *pSwapChain, ID3D11Device *pDevice, int opt=D3D11LC_INIT_SYMBOLS);
void _D3D11LeakCheckerFinalize();
void _D3D11LeakCheckerPrintLeakInfo();

#define D3D11LeakCheckerInitialize(...) _D3D11LeakCheckerInitialize(__VA_ARGS__)
#define D3D11LeakCheckerFinalize()      _D3D11LeakCheckerFinalize()
#define D3D11LeakCheckerPrintLeakInfo() _D3D11LeakCheckerPrintLeakInfo()

#else // D3D11LEAKCHECKER_ENABLE

#define D3D11LeakCheckerInitialize(...) 
#define D3D11LeakCheckerFinalize() 
#define D3D11LeakCheckerPrintLeakInfo() 

#endif // D3D11LEAKCHECKER_ENABLE

#endif // _ist_D3D11LeakChecker_h_
