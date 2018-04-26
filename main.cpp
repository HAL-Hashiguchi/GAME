//=============================================================================
//
// 3Dゲーム制作
//
//=============================================================================
#define _CRT_SECURE_NO_WARNINGS
#define SAFE_RELEASE(ptr){if(ptr){(ptr)->Release();(ptr)=NULL;}}
#include <windows.h>
#include <d3dx9.h>
#include <stdio.h>
#include <math.h>
#include "input.h"
// #include "sound.h"
#include "MODEL.h"
#include "ca.h"
#include "State.h"

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"winmm.lib")

//************************************************************************
//マクロ定義
//************************************************************************
#define FULLSCREEN     0
#define CLASS_NAME    "Racing"
#define WINDOW_NAME   "First Run"


#define TEXTURE_MENU ("assets/Title.jpg")
#define TEXTURE_OPERATION ("assets/Operation.png")
#define TEXTURE_GAMEOVER  ("assets/Gameover.png")
#define TEXTURE_ACCIDENT ("assets/Injured.png")
#define TEXTURE_POSE ("assets/Pose.png")
#define TEXTURE_CLEAR ("assets/Clear.png")
#define TEXTURE_ARROW ("assets/Arrow.png")

// 二次元配列にて選択肢を管理
#define TEXTURE_Gamestart ("assets/Gamestart.png")
#define TEXTURE_Back      ("assets/Back.png")
#define TEXTURE_Retry     ("assets/Retry.png")
#define TEXTURE_Change    ("assets/Change.png")
#define TEXTURE_BackTitle ("assets/Backtitle.png")
#define TEXTURE_Automatic ("assets/Automatic.png")
#define TEXTURE_Manual    ("assets/Manual.png")

LPDIRECT3D9			g_pD3D			= NULL;
LPDIRECT3DDEVICE9	g_pd3dDevice	= NULL;

LPDIRECT3DTEXTURE9 g_pd3dTextureMenu = NULL;      // メニュー画面
LPDIRECT3DTEXTURE9 g_pd3dTextureOperation = NULL; // 説明画面
LPDIRECT3DTEXTURE9 g_pd3dTextureGameover = NULL;  // ゲームオーバー
LPDIRECT3DTEXTURE9 g_pd3dTextureAccident = NULL;  // traffic accident
LPDIRECT3DTEXTURE9 g_pd3dTexturePose = NULL;      // ゲーム中断画面
LPDIRECT3DTEXTURE9 g_pd3dTextureClear = NULL;    // クリア画面
LPDIRECT3DTEXTURE9 g_pd3dTextureArrow = NULL;    //
LPDIRECT3DTEXTURE9 g_pd3dTextureSelect[7] = { NULL };   // 選択文字

D3DPRESENT_PARAMETERS		g_D3DPresentParam;		// PRESENT PARAMETERS

D3DXVECTOR3 g_cameraPos = { 0, 0, 1 };  //カメラの位置{0.0.1}
D3DXVECTOR3 g_cameraLook = { 0, 0, 0 }; //カメラの目標
D3DXVECTOR3 g_cameraUp = { 0, 1, 0 };   //カメラの上方向{0.1.0}

D3DXMATRIX	g_matcamera;						// カメラ行列
D3DXMATRIX	g_matprojection;					// プロジェクション変換行列

LPD3DXFONT g_font = NULL; //フォント

MODEL_DATA g_training;                  // 対向車
MODEL_DATA g_man[MAN];                  // 人
MODEL_DATA g_skydome;					// スカイドーム
MODEL_DATA g_jimen;						// 地面
MODEL_DATA g_kabe;						// 真ん中の壁
MODEL_DATA g_wall1;						// 前縦壁
MODEL_DATA g_wall2;						// 左横壁
MODEL_DATA g_wall3;                     // 後縦壁
MODEL_DATA g_wall4;                     // 右横壁
MODEL_DATA g_signal;                    // 信号機
VERTEX_2D  g_vertexMenu[4];             // メニュー背景
VERTEX_2D  g_vertexOperation[4];        // 説明背景
VERTEX_2D  g_vertexGameover[4];         // ゲームオーバー
VERTEX_2D  g_vertexAccident[4];         // 事故
VERTEX_2D  g_vertexPose[4];             // ポーズ
VERTEX_2D  g_vertexClear[4];            // クリア
VERTEX_2D  g_vertexArrow[4];
VERTEX_2D  g_vertexSelect[7][4];           // 文字

// LPDIRECTSOUNDBUFFER8 g_pBGM;            // BGM
int point = 0;                          // チェックポイント
int before = 0;
int bcount = 0;
int timer = 4;
int measure = 0;                        // 時間取得の為一度だけ読む関数に用いる
int kmcount = 0;                        // 速度制限
DWORD pivot = 0;                        // 時間の原点
DWORD time = 0;                         // 現在の時間を取得

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
bool Init(HWND hWnd, bool bFullScreen, int width, int height);
void GameMain(void);
void Uninit(void);
void initGame(void);
void initModel(MODEL_DATA*);
void exportWorldMatrix(D3DXMATRIX&, MODEL_DATA&);
void course(void);
bool HitSphere(MODEL_DATA&, MODEL_DATA&);

// WINMAIN
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND			hwnd;						// ウインドウハンドル
	MSG				msg;						// メッセージ構造体
	WNDCLASSEX		wcex;						// ウインドウクラス構造体
	int				width = SCREEN_WIDTH;			// ウインドウの幅 計算用ワーク
	int				height = SCREEN_HEIGHT;			// ウインドウの高さ 計算用ワーク
	DWORD oldtime = timeGetTime();

	UNREFERENCED_PARAMETER(hPrevInstance);      //無くてもいいけど警告が出る(未使用宣言)
	UNREFERENCED_PARAMETER(lpCmdLine);          //無くてもいいけど警告が出る(未使用宣言)

	// ウインドウクラス情報のセット
	wcex.hInstance = hInstance;			// インスタンス値のセット
	wcex.lpszClassName = CLASS_NAME;					// クラス名
	wcex.lpfnWndProc = (WNDPROC)WndProc;		// ウインドウメッセージ関数
	wcex.style = 0;					// ウインドウスタイル
	wcex.cbSize = sizeof(WNDCLASSEX);	// 構造体のサイズ
	wcex.hIcon = LoadIcon((HINSTANCE)NULL, IDI_APPLICATION);	// ラージアイコン
	wcex.hIconSm = LoadIcon((HINSTANCE)NULL, IDI_WINLOGO);		// スモールアイコン
	wcex.hCursor = LoadCursor((HINSTANCE)NULL, IDC_ARROW);		// カーソルスタイル
	wcex.lpszMenuName = 0; 					// メニューなし
	wcex.cbClsExtra = 0;					// エキストラなし
	wcex.cbWndExtra = 0;
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);		// 背景色白

	if (!RegisterClassEx(&wcex)) return FALSE;	// ウインドウクラスの登録

	if (FULLSCREEN){
		hwnd = CreateWindow(
			CLASS_NAME,							// ウィンドウクラスの名前
			WINDOW_NAME,							// タイトル
			WS_VISIBLE | WS_POPUP,			// ウィンドウスタイル
			0, 0,							// ウィンドウ位置 縦, 横
			SCREEN_WIDTH, SCREEN_HEIGHT,				// ウィンドウサイズ
			NULL,							// 親ウィンドウなし
			(HMENU)NULL,					// メニューなし
			hInstance,						// インスタンスハンドル
			(LPVOID)NULL);					// 追加引数なし
	}
	else{
		RECT	rWindow, rClient;

		hwnd = CreateWindow(
			CLASS_NAME,							// ウィンドウクラスの名前
			WINDOW_NAME,							// タイトル
			WS_CAPTION | WS_SYSMENU,		// ウィンドウスタイル
			0, 0,							// ウィンドウ位置 縦, 横(あとで中央に移動させます)
			SCREEN_WIDTH, SCREEN_HEIGHT,				// ウィンドウサイズ
			HWND_DESKTOP,					// 親ウィンドウなし
			(HMENU)NULL,					// メニューなし
			hInstance,						// インスタンスハンドル
			(LPVOID)NULL);					// 追加引数なし

		// ウインドウサイズを再計算（Metricsだけでは、フレームデザインでクライアント領域サイズが変わってしまうので）
		GetWindowRect(hwnd, &rWindow);
		GetClientRect(hwnd, &rClient);
		width = (rWindow.right - rWindow.left) - (rClient.right - rClient.left) + SCREEN_WIDTH;
		height = (rWindow.bottom - rWindow.top) - (rClient.bottom - rClient.top) + SCREEN_HEIGHT;
		SetWindowPos(
			hwnd,
			NULL,
			GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2,
			GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2,
			width - 1,
			height - 1,
			SWP_NOZORDER);
	}

	if (!hwnd) return FALSE;

	//ウインドウの表示
	ShowWindow(hwnd, nCmdShow);//指定されたウィンドウの表示状態を設定(ウィンドウを表示)
	UpdateWindow(hwnd);       //ウィンドウの状態を直ちに反映(ウィンドウのクライアント領域にを描画)

	// DirectX初期化
	if (FULLSCREEN){
		bool sts = Init(hwnd, true, width, height);
		if (!sts){
			MessageBox(hwnd, "error", "DIRECTX INIT ERROR", MB_OK);
			return 0;
		}
	}
	else{
		bool sts = Init(hwnd, false, width, height);
		if (!sts){
			MessageBox(hwnd, "error", "DIRECTX INIT ERROR", MB_OK);
			return 0;
		}
	}

	// DIRECTINPUT初期化
	HRESULT hr = InitInput(hInstance, hwnd);
	if (FAILED(hr)){
		MessageBox(hwnd, "error", "DIRECT INPUT INIT ERROR", MB_OK);
		return 0;
	}

	//メッセージループ
	timeBeginPeriod(1);			// タイマの分解能力を１ｍｓにする     2014.05.01 場所をGameInit以降に移動

	initGame();

	msg.message = WM_NULL;
	while (WM_QUIT != msg.message){

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);//仮想キーメッセージを文字メッセージへ交換
			DispatchMessage(&msg); //ウィンドウプロシージャへメッセージを送出			
		}
		else{
			DWORD nowtime = timeGetTime();
			if (nowtime >= oldtime + 16) {

				nButtonCount--;
				switch (nState) {
				case 0: // スタート画面
					UpdateMenu();
					DrawMenu();
					break;
				case 1: // 操作説明
					Operation();
					break;
				case 2: // ゲーム本編
					// PlaySound(g_pBGM, 1);
					UpdateInput();			// 入力検知
					GameMain();				// 描画
					oldtime = nowtime;
					break;
				case 3:
					PoseMenu();
					DrawPose();
					break;
				case 4: // ゲームオーバー
					GameOver();
					DrawGameOver();
					break;
				case 5: // ゲームクリア
					Result();
					DrawResult();
					break;

				}
			}
		}
	}

	timeEndPeriod(1);							// タイマの分解能力もとに戻す

	//ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// DirectINPUT 終了処理
	UninitInput();

	Uninit();

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	int yesno;
	switch( message ){
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_ESCAPE:        // ESCkeyが押されました
				// StopSound(g_pBGM); // 一時停止
				if (nState == 2) //ゲーム中であれば
				{
					nState = 3; // ポーズ画面遷移
					break;
				}
				else {
					yesno = MessageBox(hWnd, "終了しますか?", "終了確認", MB_YESNO);
					if (yesno == IDYES) {
						DestroyWindow(hWnd);
					}
					break;
				}
			}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//初期化処理
bool Init(HWND hWnd, bool bFullScreen, int width, int height)
{
	HRESULT	hr;
	D3DDISPLAYMODE				disp;				// ディスプレイモード
	int							adapter = 0;			// ディスプレイアダプタ番号

	//BGM
	/*
	InitSound(hWnd); //初期化
	g_pBGM = LoadSound(BGM_00);
	*/

	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);	// Direct3D9インターフェース取得
	if (g_pD3D == NULL)	return false;

	// アダプタの現在のディスプレイモードを取得する
	hr = g_pD3D->GetAdapterDisplayMode(adapter, &disp);
	if (hr != D3D_OK)	return false;

	memset(&g_D3DPresentParam, 0, sizeof(g_D3DPresentParam));			// ゼロクリア
	g_D3DPresentParam.BackBufferFormat = disp.Format;	// 現在のビット深度
	g_D3DPresentParam.BackBufferWidth = width;			// バックバッファの幅をセット
	g_D3DPresentParam.BackBufferHeight = height;			// バックバッファの高さをセット
	g_D3DPresentParam.BackBufferCount = 1;				// バックバッファの数
	g_D3DPresentParam.SwapEffect = D3DSWAPEFFECT_FLIP;
	g_D3DPresentParam.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;	// バックバッファをロック可能にする

	if (bFullScreen)	g_D3DPresentParam.Windowed = FALSE;	// フルスクリーンモード
	else					g_D3DPresentParam.Windowed = TRUE;	// ウインドウモード

	// Ｚバッファの自動作成（ビット深度16）
	g_D3DPresentParam.EnableAutoDepthStencil = 1;
	g_D3DPresentParam.AutoDepthStencilFormat = D3DFMT_D24S8;
	g_D3DPresentParam.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	g_D3DPresentParam.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;	// VSYNCを待たない

	// デバイス作成
	hr = g_pD3D->CreateDevice(adapter,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
		&g_D3DPresentParam,
		&g_pd3dDevice);
	if (hr != D3D_OK){
		hr = g_pD3D->CreateDevice(adapter,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
			&g_D3DPresentParam,
			&g_pd3dDevice);
		if (hr != D3D_OK){
			hr = g_pD3D->CreateDevice(adapter,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
				&g_D3DPresentParam,
				&g_pd3dDevice);
			if (hr != D3D_OK)		return(false);
		}
	}

	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// テクスチャステージステート
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	 
	// Ｚバッファ有効
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	
	bool sts = g_jiki.data.xfile.LoadXFile("assets/kyu.x", g_pd3dDevice);

	sts = g_training.xfile.LoadXFile("assets/training.x", g_pd3dDevice);
	if (!sts) {
		MessageBox(NULL, "load x(training.x) file error", "load x file error", MB_OK);
	}

	if (!sts){
		MessageBox(NULL, "load x(car.x) file error", "load x file error", MB_OK);
	}
	for (int i = 0; i < MAN; i++)
	{
		sts = g_man[i].xfile.LoadXFile("assets/man.x", g_pd3dDevice);
		if (!sts) {
			MessageBox(NULL, "load x(man.x) file error", "load x file error", MB_OK);
		}
	}

	sts = g_skydome.xfile.LoadXFile("assets/skydome.x", g_pd3dDevice);
	if (!sts){
		MessageBox(NULL, "load x(skydome.x) file error", "load x file error", MB_OK);
	}
	sts = g_jimen.xfile.LoadXFile("assets/course.x", g_pd3dDevice);
	if (!sts){
		MessageBox(NULL, "load x(course.x) file error", "load x file error", MB_OK);
	}
	sts = g_kabe.xfile.LoadXFile("assets/kabe.x", g_pd3dDevice); //真ん中
	if (!sts) {
		MessageBox(NULL, "load x(kabe.x) file error", "load x file error", MB_OK);
	};
	sts = g_wall1.xfile.LoadXFile("assets/wall.x", g_pd3dDevice); //縦
	if (!sts) {
		MessageBox(NULL, "load x(wall.x) file error", "load x file error", MB_OK);
	};
	sts = g_wall2.xfile.LoadXFile("assets/wall2.x", g_pd3dDevice); //横
	if (!sts) {
		MessageBox(NULL, "load x(wall2.x) file error", "load x file error", MB_OK);
	};
	sts = g_wall3.xfile.LoadXFile("assets/wall.x", g_pd3dDevice); //縦
	if (!sts) {
		MessageBox(NULL, "load x(wall.x) file error", "load x file error", MB_OK);
	};
	sts = g_wall4.xfile.LoadXFile("assets/wall2.x", g_pd3dDevice); //横
	if (!sts) {
		MessageBox(NULL, "load x(wall2.x) file error", "load x file error", MB_OK);
	};
	sts = g_signal.xfile.LoadXFile("assets/signal.x", g_pd3dDevice);
	if (!sts) {
		MessageBox(NULL, "load x(signal.x) file error", "load x file error", MB_OK);
	};

	// Menu画面
	
	g_vertexMenu[0].vtx = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_vertexMenu[1].vtx = D3DXVECTOR3( SCREEN_WIDTH + 50, 0.0f, 0.0f);
	g_vertexMenu[2].vtx = D3DXVECTOR3(0.0f, SCREEN_HEIGHT + 50, 0.0f);
	g_vertexMenu[3].vtx = D3DXVECTOR3( SCREEN_WIDTH + 50, SCREEN_HEIGHT + 50, 0.0f);

	g_vertexMenu[0].rhw =
		g_vertexMenu[1].rhw =
		g_vertexMenu[2].rhw =
		g_vertexMenu[3].rhw = 1.0f;

	g_vertexMenu[0].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	g_vertexMenu[1].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	g_vertexMenu[2].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	g_vertexMenu[3].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);

	g_vertexMenu[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	g_vertexMenu[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	g_vertexMenu[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	g_vertexMenu[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	D3DXCreateTextureFromFile(g_pd3dDevice,
		TEXTURE_MENU,
		&g_pd3dTextureMenu);
	
	// 説明画面
	g_vertexOperation[0].vtx = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_vertexOperation[1].vtx = D3DXVECTOR3(SCREEN_WIDTH + 50, 0.0f, 0.0f);
	g_vertexOperation[2].vtx = D3DXVECTOR3(0.0f, SCREEN_HEIGHT + 50, 0.0f);
	g_vertexOperation[3].vtx = D3DXVECTOR3(SCREEN_WIDTH + 50, SCREEN_HEIGHT + 50, 0.0f);

	g_vertexOperation[0].rhw =
		g_vertexOperation[1].rhw =
		g_vertexOperation[2].rhw =
		g_vertexOperation[3].rhw = 1.0f;

	g_vertexOperation[0].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	g_vertexOperation[1].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	g_vertexOperation[2].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	g_vertexOperation[3].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);

	g_vertexOperation[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	g_vertexOperation[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	g_vertexOperation[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	g_vertexOperation[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	D3DXCreateTextureFromFile(g_pd3dDevice,
		TEXTURE_OPERATION,
		&g_pd3dTextureOperation);

	// ゲームオーバー画面1
	g_vertexGameover[0].vtx = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_vertexGameover[1].vtx = D3DXVECTOR3(SCREEN_WIDTH + 50, 0.0f, 0.0f);
	g_vertexGameover[2].vtx = D3DXVECTOR3(0.0f, SCREEN_HEIGHT + 50, 0.0f);
	g_vertexGameover[3].vtx = D3DXVECTOR3(SCREEN_WIDTH + 50, SCREEN_HEIGHT + 50, 0.0f);

	g_vertexGameover[0].rhw =
		g_vertexGameover[1].rhw =
		g_vertexGameover[2].rhw =
		g_vertexGameover[3].rhw = 1.0f;

	g_vertexGameover[0].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	g_vertexGameover[1].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	g_vertexGameover[2].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	g_vertexGameover[3].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);

	g_vertexGameover[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	g_vertexGameover[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	g_vertexGameover[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	g_vertexGameover[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	D3DXCreateTextureFromFile(g_pd3dDevice,
		TEXTURE_GAMEOVER,
		&g_pd3dTextureGameover);

	// ゲームオーバー画面2
	g_vertexAccident[0].vtx = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_vertexAccident[1].vtx = D3DXVECTOR3(SCREEN_WIDTH + 50, 0.0f, 0.0f);
	g_vertexAccident[2].vtx = D3DXVECTOR3(0.0f, SCREEN_HEIGHT + 50, 0.0f);
	g_vertexAccident[3].vtx = D3DXVECTOR3(SCREEN_WIDTH + 50, SCREEN_HEIGHT + 50, 0.0f);

	g_vertexAccident[0].rhw =
		g_vertexAccident[1].rhw =
		g_vertexAccident[2].rhw =
		g_vertexAccident[3].rhw = 1.0f;

	g_vertexAccident[0].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	g_vertexAccident[1].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	g_vertexAccident[2].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	g_vertexAccident[3].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);

	g_vertexAccident[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	g_vertexAccident[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	g_vertexAccident[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	g_vertexAccident[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	D3DXCreateTextureFromFile(g_pd3dDevice,
		TEXTURE_ACCIDENT,
		&g_pd3dTextureAccident);

	// ポーズ画面

	g_vertexPose[0].vtx = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_vertexPose[1].vtx = D3DXVECTOR3(SCREEN_WIDTH + 40, 0.0f, 0.0f);
	g_vertexPose[2].vtx = D3DXVECTOR3(0.0f, SCREEN_HEIGHT + 50, 0.0f);
	g_vertexPose[3].vtx = D3DXVECTOR3(SCREEN_WIDTH + 40, SCREEN_HEIGHT + 40, 0.0f);

	g_vertexPose[0].rhw =
		g_vertexPose[1].rhw =
		g_vertexPose[2].rhw =
		g_vertexPose[3].rhw = 1.0f;

	g_vertexPose[0].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	g_vertexPose[1].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	g_vertexPose[2].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	g_vertexPose[3].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);

	g_vertexPose[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	g_vertexPose[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	g_vertexPose[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	g_vertexPose[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	D3DXCreateTextureFromFile(g_pd3dDevice,
		TEXTURE_POSE,
		&g_pd3dTexturePose);

	// クリア画面

	g_vertexClear[0].vtx = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_vertexClear[1].vtx = D3DXVECTOR3(SCREEN_WIDTH + 40, 0.0f, 0.0f);
	g_vertexClear[2].vtx = D3DXVECTOR3(0.0f, SCREEN_HEIGHT + 50, 0.0f);
	g_vertexClear[3].vtx = D3DXVECTOR3(SCREEN_WIDTH + 40, SCREEN_HEIGHT + 40, 0.0f);

	g_vertexClear[0].rhw =
		g_vertexClear[1].rhw =
		g_vertexClear[2].rhw =
		g_vertexClear[3].rhw = 1.0f;

	g_vertexClear[0].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	g_vertexClear[1].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	g_vertexClear[2].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	g_vertexClear[3].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);

	g_vertexClear[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	g_vertexClear[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	g_vertexClear[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	g_vertexClear[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	D3DXCreateTextureFromFile(g_pd3dDevice,
		TEXTURE_CLEAR,
		&g_pd3dTextureClear);

	// 選択

	g_vertexArrow[0].rhw =
		g_vertexArrow[1].rhw =
		g_vertexArrow[2].rhw =
		g_vertexArrow[3].rhw = 1.0f;

	g_vertexArrow[0].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	g_vertexArrow[1].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	g_vertexArrow[2].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
	g_vertexArrow[3].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);

	g_vertexArrow[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	g_vertexArrow[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	g_vertexArrow[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	g_vertexArrow[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	D3DXCreateTextureFromFile(g_pd3dDevice,
		TEXTURE_ARROW,
		&g_pd3dTextureArrow);


	// 文字選択
	for (int i = 0; i < 7; i++) {
		/*
		g_vertexSelect[i][0].vtx = D3DXVECTOR3(200.0f, 200.0f, 0.0f);
		g_vertexSelect[i][1].vtx = D3DXVECTOR3(400.0f, 200.0f, 0.0f);
		g_vertexSelect[i][2].vtx = D3DXVECTOR3(200.0f, 300.0f, 0.0f);
		g_vertexSelect[i][3].vtx = D3DXVECTOR3(400.0f, 300.0f, 0.0f);
		*/

		g_vertexSelect[i][0].rhw =
			g_vertexSelect[i][1].rhw =
			g_vertexSelect[i][2].rhw =
			g_vertexSelect[i][3].rhw = 1.0f;

		g_vertexSelect[i][0].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
		g_vertexSelect[i][1].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
		g_vertexSelect[i][2].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);
		g_vertexSelect[i][3].diffuse = D3DCOLOR_RGBA(255, 255, 255, 255);

		g_vertexSelect[i][0].tex = D3DXVECTOR2(0.0f, 0.0f);
		g_vertexSelect[i][1].tex = D3DXVECTOR2(1.0f, 0.0f);
		g_vertexSelect[i][2].tex = D3DXVECTOR2(0.0f, 1.0f);
		g_vertexSelect[i][3].tex = D3DXVECTOR2(1.0f, 1.0f);
	}
	D3DXCreateTextureFromFile(g_pd3dDevice,
		TEXTURE_Gamestart,
		&g_pd3dTextureSelect[0]);

	D3DXCreateTextureFromFile(g_pd3dDevice,
		TEXTURE_Back,
		&g_pd3dTextureSelect[1]);

	D3DXCreateTextureFromFile(g_pd3dDevice,
		TEXTURE_Retry,
		&g_pd3dTextureSelect[2]);

	D3DXCreateTextureFromFile(g_pd3dDevice,
		TEXTURE_Change,
		&g_pd3dTextureSelect[3]);

	D3DXCreateTextureFromFile(g_pd3dDevice,
		TEXTURE_BackTitle,
		&g_pd3dTextureSelect[4]);

	D3DXCreateTextureFromFile(g_pd3dDevice,
		TEXTURE_Automatic,
		&g_pd3dTextureSelect[5]);

	D3DXCreateTextureFromFile(g_pd3dDevice,
		TEXTURE_Manual,
		&g_pd3dTextureSelect[6]);

	//フォント初期化
	D3DXCreateFont(g_pd3dDevice, 20, 10, 0, 0, 0, 0, 0, 0, 0, "Times New Roman", &g_font);

	return(true);
}


//=============================================================================
// カメラとオブジェクトを初期化
//=============================================================================
void initGame(void)
{
	//地面
	initModel(&g_jimen);
	g_jimen.yFg = true; //表示を有効化

	//スカイドーム
	initModel(&g_skydome);
	g_skydome.yFg = true; //表示を有効化

	//真ん中の壁
	initModel(&g_kabe);
	g_kabe.yFg = true; //表示を有効化
	
	//前縦壁
	initModel(&g_wall1);
	g_wall1.pos.z = 750;
	g_wall1.yFg = true; //表示を有効化

	//左横壁			   
	initModel(&g_wall2);
	g_wall2.pos.x = -1000;
	g_wall2.yFg = true; //表示を有効化

	//前縦壁
	initModel(&g_wall3);
	g_wall3.pos.z = -750;
	g_wall3.yFg = true; //表示を有効化

	//左横壁			   
	initModel(&g_wall4);
	g_wall4.pos.x = 1000;
	g_wall4.yFg = true; //表示を有効化

	// 対向車
	initModel(&g_training);
	g_training.r = 10.0f;
	g_training.pos.x = 800;
	g_training.pos.z = -550;
	g_training.yFg = true; //表示を有効化

	//通行人A
	for (int i = 0; i < MAN; i++) {
		initModel(&g_man[i]);
		g_man[i].r = 3.5f;
		g_man[0].pos.x = 880;
		g_man[1].pos.x = 890;
		g_man[2].pos.x = 900;
		g_man[3].pos.x = 1010;
		g_man[4].pos.x = 1080;
		g_man[i].pos.y = 1;
		g_man[i].pos.z = -90;
		g_man[3].pos.z = -120;
		g_man[4].pos.z = -120;
		g_man[i].yFg = true; //表示を有効化
	}

	//信号機
	initModel(&g_signal);
	g_signal.r = 0.1f;
	g_signal.pos.x = 985;
	g_signal.pos.y = 1;
	g_signal.pos.z = -160;
	g_signal.yFg = true; //表示を有効化

	//自機
	initModel(&g_jiki.data);
	g_jiki.data.r = 10.0f;
	g_jiki.data.pos.x = -950;
	g_jiki.data.pos.y = 1;
	g_jiki.data.pos.z = -10;
	g_jiki.data.yFg = true; //表示を有効化
	for (int i = 0; i < 100; i++){
		g_jiki.posLog[i].x = 0; //位置(過去ログ)
		g_jiki.posLog[i].y = 0; //位置(過去ログ)
		g_jiki.posLog[i].z = 0; //位置(過去ログ)
	}
	g_jiki.logNum = 0; //位置(過去ログ)
	g_jiki.state = 1;

}

void initModel(MODEL_DATA* mdata)
{
	mdata->yFg = false; //処理を行うか否かフラグ
	mdata->r = 0.0f;
	mdata->posSpeed = { 0, 0, 0 }; //位置移動速度
	mdata->posBef = { 0, 0, 0 }; //位置(1コマ前)
	mdata->pos = { 0, 0, 0 }; //位置
//	mdata->angleSpeed = { 0, 0, 0 }; //向き移動速度
	mdata->angleBef = { 0, 0, 0 }; //向き(1コマ前)
	mdata->angle = { 0, 0, 0 }; //向き
	mdata->mat = { 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1 }; // 積算行列

	return;
}

//=============================================================================
// GameMain関数
//=============================================================================
void GameMain(void)
{
	if (before == 0)
	{
		D3DXMATRIX BackMat = { 1, 0, 0, 0, //カメラ追従
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 10, -10, 1 };

		BackMat = BackMat * g_jiki.data.mat;
		g_cameraPos.x = BackMat._41;
		g_cameraPos.y = BackMat._42;
		g_cameraPos.z = BackMat._43;


		bcount++; // 3,2,1,go!
			switch (bcount) {
			case 60:
			{
				timer -= 1;
				break;
			}
			case 120:
			{
				timer -= 1;
				break;
			}
			case 180:
			{
				timer -= 1;
				break;
			}
			case 240:
			{
				timer -= 1;
				before += 1;
				bcount = 0;
				break;
			}
			}
		}
	// スタート前の処理が終了してから
	if (0 < before)
	{
		if (measure == 0) // 一度だけ通る
		{
			pivot = timeGetTime(); // 時間の原点を取得
		    ora = 0; // スタート前に開いたpose時間を0に設定
			measure += 1;
		}

		// 1秒刻み(経過時間 - スタートした時間 - ポーズ画面にいた時間)
		DWORD nowtime = timeGetTime();
		time = ((nowtime - pivot) / 1000) - ora;

		if (g_sel == 1)
		{
			ATcar();
		}
		if (g_sel == 2)
		{
			MTcar();
		}
	}
	transmission();
	UpdateInput();
	if (GetKeyboardPress(DIK_0))
	{
		nState = 3;
	}
	if (g_deduction < 70) // 減点されすぎた場合
	{
		nState = 4; // ゲームオーバー
	}
	if (g_acl > 3.05f) // 公道の速度は60km以下、
	{
		kmcount++;
			if (kmcount > 60)
			{
				g_deduction = g_deduction - 1;
				kmcount = 0;
			}
	}
	if (g_jiki.data.pos.x > 750 && g_jiki.data.pos.z < -100) //仮チェックポイント
	{
		// ここを通ることでゴールが可能となる
		point = 1;
	}

	if (point == 1) // 仮クリア条件
	{
		if (g_jiki.data.pos.x < -750 && g_jiki.data.pos.z > -10)
		{
			nState = 5; // 真ゲームクリア
		}
	}
	for (int i = 0; i < MAN; i++) {
		if (g_jiki.data.pos.x > 750 && g_jiki.data.pos.z < 200)
		{
			g_man[i].pos.x += 0.55f; // 人の動き
			g_man[3].pos.x -= 0.2f;
			g_man[4].pos.x -= 0.2f;
		}
	}

	for (int i = 0; i < MAN; i++) {
		bool hit = HitSphere(g_jiki.data, g_man[i]); // もしも人にぶつかってしまったら？
		if (hit == true)
		{
			// 事故には気を付けよう
			nState = 4; // ゲームオーバー
		}
	}
	course();
	g_training.pos.z += 1.0;
	bool hit2 = HitSphere(g_jiki.data, g_signal); // もしも信号にぶつかってしまったら？
	if (hit2 == true)
	{
		// 事故には気を付けよう
		g_deduction -= 10; // 減点
		if (g_acl > 50)
		{
			g_acl = -50;
		}
		else
		{
			g_acl = 0;
		}
	}

		g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
		g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		D3DXMatrixPerspectiveFovLH(&g_matprojection, D3DX_PI / 2.0f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 10000.0f);		// プロジェクション行列(射影行列)作成
		g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &g_matprojection);								// プロジェクション行列をグラフィックカードに設定

		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
		if (SUCCEEDED(g_pd3dDevice->BeginScene()))	// 描画開始
		{
			//地面に対しての位置へ座標変換(ワールド変換)
			exportWorldMatrix(g_jimen.mat, g_jiki.data); // 自機
			exportWorldMatrix(g_jimen.mat, g_training);
			exportWorldMatrix(g_jimen.mat, g_kabe);
			exportWorldMatrix(g_jimen.mat, g_wall1); //kabe
			exportWorldMatrix(g_jimen.mat, g_wall2);
			exportWorldMatrix(g_jimen.mat, g_wall3);
			exportWorldMatrix(g_jimen.mat, g_wall4);
			for (int i = 0; i < MAN ; i++)
			exportWorldMatrix(g_jimen.mat, g_man[i]); // あぶない人
			exportWorldMatrix(g_jimen.mat, g_signal); //信号機
		
			//自機をカメラの目標にする
			g_cameraLook.x = g_jiki.data.mat._41;
			g_cameraLook.y = g_jiki.data.mat._42 + 5;
			g_cameraLook.z = g_jiki.data.mat._43;

			//カメラの位置をセット6y7y
			D3DXMatrixLookAtLH(&g_matcamera, &g_cameraPos, &g_cameraLook, &g_cameraUp);		// カメラ行列(ビュー行列)を作成
			g_pd3dDevice->SetTransform(D3DTS_VIEW, &g_matcamera);		// カメラ行列をグラフィックカードにセット

			//地面表示
			D3DXMatrixIdentity(&g_jimen.mat); //単位行列作成
			if (g_jimen.yFg == true) {
				g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_jimen.mat);
				g_jimen.xfile.Draw(g_pd3dDevice);
			}

			//スカイドーム表示
			if (g_skydome.yFg == true) {
				g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_skydome.mat);
				g_skydome.xfile.Draw(g_pd3dDevice);
			}

			//人表示
			for (int i = 0; i < MAN; i++) {
				if (g_man[i].yFg == true) {
					g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_man[i].mat);
					g_man[i].xfile.Draw(g_pd3dDevice);
				}
			}

			//自機表示
			if (g_jiki.data.yFg == true) {
				g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_jiki.data.mat);
				g_jiki.data.xfile.Draw(g_pd3dDevice);
			}

			//自機表示
			if (g_training.yFg == true) {
				g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_training.mat);
				g_training.xfile.Draw(g_pd3dDevice);
			}

			//壁表示
			if (g_kabe.yFg == true) {
				g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_kabe.mat);
				g_kabe.xfile.Draw(g_pd3dDevice);
			}

			//壁1表示
			if (g_wall1.yFg == true) {
				g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_wall1.mat);
				g_wall1.xfile.Draw(g_pd3dDevice);
			}

			//壁2表示
			if (g_wall2.yFg == true) {
				g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_wall2.mat);
				g_wall2.xfile.Draw(g_pd3dDevice);
			}

			//壁3表示
			if (g_wall3.yFg == true) {
				g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_wall3.mat);
				g_wall3.xfile.Draw(g_pd3dDevice);
			}

			//壁4表示
			if (g_wall4.yFg == true) {
				g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_wall4.mat);
				g_wall4.xfile.Draw(g_pd3dDevice);
			}


			//信号機表示
			if (g_signal.yFg == true) {
				g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_signal.mat);
				g_signal.xfile.Draw(g_pd3dDevice);
			}

			//文字表示
			RECT moji001;
			moji001.left = 100;
			moji001.top = 100;
			moji001.right = 640;
			moji001.bottom = 320;
			char buf[100];
			if (g_sel == 1) // レバー表示
			{
				sprintf(buf, "Speed : %d km\nLever : %c\nScore : %d\nTime %dsec\n point : %d\n timer : %d", (int)((float)g_acl * 20), lever, g_deduction, time, point , timer); //pointはチェックポイント(仮)
			}
			if (g_sel == 2) // ギア表示
			{
				sprintf(buf, "Speed : %d km\nGear : %c\nScore : %d\nTime %dsec\n point : %d timer : %d", (int)((float)g_acl * 20), cnt, g_deduction, time, point, timer);
			}
			g_font->DrawTextA(NULL, buf, -1, &moji001, NULL, 0xFFFF0000);

			g_pd3dDevice->EndScene(); // 描画終了
		}
		g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
	}


//=============================================================================
// 終了処理関数
//=============================================================================
void  Uninit(void)
{

	// SAFE_RELEASE(g_pBGM);
	// UninitSound();
	if( g_pd3dDevice != NULL )
	{
		g_pd3dDevice->Release();
	}
	if( g_pD3D != NULL )
	{
		g_pD3D->Release();
	}
	if (g_pd3dTextureMenu != NULL)
	{
		g_pd3dTextureMenu->Release();
		g_pd3dTextureMenu = NULL;
	}
	if (g_pd3dTextureOperation != NULL)
	{
		g_pd3dTextureOperation->Release();
		g_pd3dTextureOperation = NULL;
	}
	if (g_pd3dTextureGameover != NULL)
	{
		g_pd3dTextureGameover->Release();
		g_pd3dTextureGameover = NULL;
	}
	if (g_pd3dTextureAccident != NULL)
	{
		g_pd3dTextureAccident->Release();
		g_pd3dTextureAccident = NULL;
	}
	if (g_pd3dTexturePose != NULL)
	{
		g_pd3dTexturePose->Release();
		g_pd3dTexturePose = NULL;
	}
	if (g_pd3dTextureClear != NULL)
	{
		g_pd3dTextureClear->Release();
		g_pd3dTextureClear = NULL;
	}
	if (g_pd3dTextureArrow != NULL)
	{
		g_pd3dTextureArrow->Release();
		g_pd3dTextureArrow = NULL;
	}
	for (int i = 0; i < 7; i++) {
		if (g_pd3dTextureSelect[i] != NULL)
		{
			g_pd3dTextureSelect[i]->Release();
			g_pd3dTextureSelect[i] = NULL;
		}
	}
	if (g_font != NULL)
	{
		g_font->Release();
	}
}


///============================================================================
// 座標変換関数
//=============================================================================
void exportWorldMatrix(D3DXMATRIX& matworld, MODEL_DATA& mdata)
{
	D3DXMATRIX	matA;
	D3DXMATRIX	mattrans;

	//絶対的に移動
	{
		//位置変更
		float posX = mdata.posBef.x;
		float posY = mdata.posBef.y;
		float posZ = mdata.posBef.z;

		posX -= sin(mdata.angle.y*D3DX_PI / 180.f)*(mdata.pos.z - mdata.posBef.z);
		posZ += cos(mdata.angle.y*D3DX_PI / 180.f)*(mdata.pos.z - mdata.posBef.z);

		posX += cos(mdata.angle.y*D3DX_PI / 180.f)*(mdata.pos.x - mdata.posBef.x);
		posZ += sin(mdata.angle.y*D3DX_PI / 180.f)*(mdata.pos.x - mdata.posBef.x);

		posY = mdata.pos.y;

		mdata.pos.x = posX;
		mdata.pos.y = posY;
		mdata.pos.z = posZ;
		mdata.posBef = mdata.pos; //1コマ前を記録しておく

		//向き変更
		float angleX = mdata.angle.x;
		float angleY = mdata.angle.y;
		float angleZ = mdata.angle.z;

		D3DXMatrixTranslation(&mattrans, posX, posY, posZ);
		D3DXMatrixRotationYawPitchRoll(&matA, angleY*D3DX_PI* -1 / 180.0f, angleX*D3DX_PI / 180.0f, angleZ*D3DX_PI / 180.0f);
		matworld = matA * mattrans;
		mdata.mat = { 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1 }; // 積算行列
		mdata.mat = matworld * mdata.mat;
	}
	return;
}


void course()
{

	if (g_jiki.data.pos.z > 748)
	{
		g_acl = 0;
		g_acl -= 0.1f;
	}
	if (g_jiki.data.pos.x > 998)
	{
		g_acl = 0;
		g_acl -= 0.1f;
	}
	if (g_jiki.data.pos.x < -998)
	{
		g_acl = 0;
		g_acl -= 0.1f;
	}
	if (g_jiki.data.pos.z < -748)
	{
		g_acl = 0;
		g_acl -= 0.1f;
	}
}

bool HitSphere(MODEL_DATA& car, MODEL_DATA& object) //球と球での当たり判定
{
	float hit = (object.pos.x - car.pos.x) * (object.pos.x - car.pos.x) + (object.pos.y - car.pos.y) * (object.pos.y - car.pos.y) + (object.pos.z - car.pos.z) * (object.pos.z - car.pos.z);
	if ((hit) * (hit) <= (car.r + object.r) * (car.r + object.r))
	{
		return true; // 衝突している
	}
	else
	{
		return false;
	}
}