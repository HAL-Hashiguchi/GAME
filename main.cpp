//=============================================================================
//
// 3D�Q�[������
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
//�}�N����`
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

// �񎟌��z��ɂđI�������Ǘ�
#define TEXTURE_Gamestart ("assets/Gamestart.png")
#define TEXTURE_Back      ("assets/Back.png")
#define TEXTURE_Retry     ("assets/Retry.png")
#define TEXTURE_Change    ("assets/Change.png")
#define TEXTURE_BackTitle ("assets/Backtitle.png")
#define TEXTURE_Automatic ("assets/Automatic.png")
#define TEXTURE_Manual    ("assets/Manual.png")

LPDIRECT3D9			g_pD3D			= NULL;
LPDIRECT3DDEVICE9	g_pd3dDevice	= NULL;

LPDIRECT3DTEXTURE9 g_pd3dTextureMenu = NULL;      // ���j���[���
LPDIRECT3DTEXTURE9 g_pd3dTextureOperation = NULL; // �������
LPDIRECT3DTEXTURE9 g_pd3dTextureGameover = NULL;  // �Q�[���I�[�o�[
LPDIRECT3DTEXTURE9 g_pd3dTextureAccident = NULL;  // traffic accident
LPDIRECT3DTEXTURE9 g_pd3dTexturePose = NULL;      // �Q�[�����f���
LPDIRECT3DTEXTURE9 g_pd3dTextureClear = NULL;    // �N���A���
LPDIRECT3DTEXTURE9 g_pd3dTextureArrow = NULL;    //
LPDIRECT3DTEXTURE9 g_pd3dTextureSelect[7] = { NULL };   // �I�𕶎�

D3DPRESENT_PARAMETERS		g_D3DPresentParam;		// PRESENT PARAMETERS

D3DXVECTOR3 g_cameraPos = { 0, 0, 1 };  //�J�����̈ʒu{0.0.1}
D3DXVECTOR3 g_cameraLook = { 0, 0, 0 }; //�J�����̖ڕW
D3DXVECTOR3 g_cameraUp = { 0, 1, 0 };   //�J�����̏����{0.1.0}

D3DXMATRIX	g_matcamera;						// �J�����s��
D3DXMATRIX	g_matprojection;					// �v���W�F�N�V�����ϊ��s��

LPD3DXFONT g_font = NULL; //�t�H���g

MODEL_DATA g_training;                  // �Ό���
MODEL_DATA g_man[MAN];                  // �l
MODEL_DATA g_skydome;					// �X�J�C�h�[��
MODEL_DATA g_jimen;						// �n��
MODEL_DATA g_kabe;						// �^�񒆂̕�
MODEL_DATA g_wall1;						// �O�c��
MODEL_DATA g_wall2;						// ������
MODEL_DATA g_wall3;                     // ��c��
MODEL_DATA g_wall4;                     // �E����
MODEL_DATA g_signal;                    // �M���@
VERTEX_2D  g_vertexMenu[4];             // ���j���[�w�i
VERTEX_2D  g_vertexOperation[4];        // �����w�i
VERTEX_2D  g_vertexGameover[4];         // �Q�[���I�[�o�[
VERTEX_2D  g_vertexAccident[4];         // ����
VERTEX_2D  g_vertexPose[4];             // �|�[�Y
VERTEX_2D  g_vertexClear[4];            // �N���A
VERTEX_2D  g_vertexArrow[4];
VERTEX_2D  g_vertexSelect[7][4];           // ����

// LPDIRECTSOUNDBUFFER8 g_pBGM;            // BGM
int point = 0;                          // �`�F�b�N�|�C���g
int before = 0;
int bcount = 0;
int timer = 4;
int measure = 0;                        // ���Ԏ擾�̈׈�x�����ǂފ֐��ɗp����
int kmcount = 0;                        // ���x����
DWORD pivot = 0;                        // ���Ԃ̌��_
DWORD time = 0;                         // ���݂̎��Ԃ��擾

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
	HWND			hwnd;						// �E�C���h�E�n���h��
	MSG				msg;						// ���b�Z�[�W�\����
	WNDCLASSEX		wcex;						// �E�C���h�E�N���X�\����
	int				width = SCREEN_WIDTH;			// �E�C���h�E�̕� �v�Z�p���[�N
	int				height = SCREEN_HEIGHT;			// �E�C���h�E�̍��� �v�Z�p���[�N
	DWORD oldtime = timeGetTime();

	UNREFERENCED_PARAMETER(hPrevInstance);      //�����Ă��������ǌx�����o��(���g�p�錾)
	UNREFERENCED_PARAMETER(lpCmdLine);          //�����Ă��������ǌx�����o��(���g�p�錾)

	// �E�C���h�E�N���X���̃Z�b�g
	wcex.hInstance = hInstance;			// �C���X�^���X�l�̃Z�b�g
	wcex.lpszClassName = CLASS_NAME;					// �N���X��
	wcex.lpfnWndProc = (WNDPROC)WndProc;		// �E�C���h�E���b�Z�[�W�֐�
	wcex.style = 0;					// �E�C���h�E�X�^�C��
	wcex.cbSize = sizeof(WNDCLASSEX);	// �\���̂̃T�C�Y
	wcex.hIcon = LoadIcon((HINSTANCE)NULL, IDI_APPLICATION);	// ���[�W�A�C�R��
	wcex.hIconSm = LoadIcon((HINSTANCE)NULL, IDI_WINLOGO);		// �X���[���A�C�R��
	wcex.hCursor = LoadCursor((HINSTANCE)NULL, IDC_ARROW);		// �J�[�\���X�^�C��
	wcex.lpszMenuName = 0; 					// ���j���[�Ȃ�
	wcex.cbClsExtra = 0;					// �G�L�X�g���Ȃ�
	wcex.cbWndExtra = 0;
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);		// �w�i�F��

	if (!RegisterClassEx(&wcex)) return FALSE;	// �E�C���h�E�N���X�̓o�^

	if (FULLSCREEN){
		hwnd = CreateWindow(
			CLASS_NAME,							// �E�B���h�E�N���X�̖��O
			WINDOW_NAME,							// �^�C�g��
			WS_VISIBLE | WS_POPUP,			// �E�B���h�E�X�^�C��
			0, 0,							// �E�B���h�E�ʒu �c, ��
			SCREEN_WIDTH, SCREEN_HEIGHT,				// �E�B���h�E�T�C�Y
			NULL,							// �e�E�B���h�E�Ȃ�
			(HMENU)NULL,					// ���j���[�Ȃ�
			hInstance,						// �C���X�^���X�n���h��
			(LPVOID)NULL);					// �ǉ������Ȃ�
	}
	else{
		RECT	rWindow, rClient;

		hwnd = CreateWindow(
			CLASS_NAME,							// �E�B���h�E�N���X�̖��O
			WINDOW_NAME,							// �^�C�g��
			WS_CAPTION | WS_SYSMENU,		// �E�B���h�E�X�^�C��
			0, 0,							// �E�B���h�E�ʒu �c, ��(���ƂŒ����Ɉړ������܂�)
			SCREEN_WIDTH, SCREEN_HEIGHT,				// �E�B���h�E�T�C�Y
			HWND_DESKTOP,					// �e�E�B���h�E�Ȃ�
			(HMENU)NULL,					// ���j���[�Ȃ�
			hInstance,						// �C���X�^���X�n���h��
			(LPVOID)NULL);					// �ǉ������Ȃ�

		// �E�C���h�E�T�C�Y���Čv�Z�iMetrics�����ł́A�t���[���f�U�C���ŃN���C�A���g�̈�T�C�Y���ς���Ă��܂��̂Łj
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

	//�E�C���h�E�̕\��
	ShowWindow(hwnd, nCmdShow);//�w�肳�ꂽ�E�B���h�E�̕\����Ԃ�ݒ�(�E�B���h�E��\��)
	UpdateWindow(hwnd);       //�E�B���h�E�̏�Ԃ𒼂��ɔ��f(�E�B���h�E�̃N���C�A���g�̈�ɂ�`��)

	// DirectX������
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

	// DIRECTINPUT������
	HRESULT hr = InitInput(hInstance, hwnd);
	if (FAILED(hr)){
		MessageBox(hwnd, "error", "DIRECT INPUT INIT ERROR", MB_OK);
		return 0;
	}

	//���b�Z�[�W���[�v
	timeBeginPeriod(1);			// �^�C�}�̕���\�͂��P�����ɂ���     2014.05.01 �ꏊ��GameInit�ȍ~�Ɉړ�

	initGame();

	msg.message = WM_NULL;
	while (WM_QUIT != msg.message){

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);//���z�L�[���b�Z�[�W�𕶎����b�Z�[�W�֌���
			DispatchMessage(&msg); //�E�B���h�E�v���V�[�W���փ��b�Z�[�W�𑗏o			
		}
		else{
			DWORD nowtime = timeGetTime();
			if (nowtime >= oldtime + 16) {

				nButtonCount--;
				switch (nState) {
				case 0: // �X�^�[�g���
					UpdateMenu();
					DrawMenu();
					break;
				case 1: // �������
					Operation();
					break;
				case 2: // �Q�[���{��
					// PlaySound(g_pBGM, 1);
					UpdateInput();			// ���͌��m
					GameMain();				// �`��
					oldtime = nowtime;
					break;
				case 3:
					PoseMenu();
					DrawPose();
					break;
				case 4: // �Q�[���I�[�o�[
					GameOver();
					DrawGameOver();
					break;
				case 5: // �Q�[���N���A
					Result();
					DrawResult();
					break;

				}
			}
		}
	}

	timeEndPeriod(1);							// �^�C�}�̕���\�͂��Ƃɖ߂�

	//�E�B���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// DirectINPUT �I������
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
			case VK_ESCAPE:        // ESCkey��������܂���
				// StopSound(g_pBGM); // �ꎞ��~
				if (nState == 2) //�Q�[�����ł����
				{
					nState = 3; // �|�[�Y��ʑJ��
					break;
				}
				else {
					yesno = MessageBox(hWnd, "�I�����܂���?", "�I���m�F", MB_YESNO);
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

//����������
bool Init(HWND hWnd, bool bFullScreen, int width, int height)
{
	HRESULT	hr;
	D3DDISPLAYMODE				disp;				// �f�B�X�v���C���[�h
	int							adapter = 0;			// �f�B�X�v���C�A�_�v�^�ԍ�

	//BGM
	/*
	InitSound(hWnd); //������
	g_pBGM = LoadSound(BGM_00);
	*/

	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);	// Direct3D9�C���^�[�t�F�[�X�擾
	if (g_pD3D == NULL)	return false;

	// �A�_�v�^�̌��݂̃f�B�X�v���C���[�h���擾����
	hr = g_pD3D->GetAdapterDisplayMode(adapter, &disp);
	if (hr != D3D_OK)	return false;

	memset(&g_D3DPresentParam, 0, sizeof(g_D3DPresentParam));			// �[���N���A
	g_D3DPresentParam.BackBufferFormat = disp.Format;	// ���݂̃r�b�g�[�x
	g_D3DPresentParam.BackBufferWidth = width;			// �o�b�N�o�b�t�@�̕����Z�b�g
	g_D3DPresentParam.BackBufferHeight = height;			// �o�b�N�o�b�t�@�̍������Z�b�g
	g_D3DPresentParam.BackBufferCount = 1;				// �o�b�N�o�b�t�@�̐�
	g_D3DPresentParam.SwapEffect = D3DSWAPEFFECT_FLIP;
	g_D3DPresentParam.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;	// �o�b�N�o�b�t�@�����b�N�\�ɂ���

	if (bFullScreen)	g_D3DPresentParam.Windowed = FALSE;	// �t���X�N���[�����[�h
	else					g_D3DPresentParam.Windowed = TRUE;	// �E�C���h�E���[�h

	// �y�o�b�t�@�̎����쐬�i�r�b�g�[�x16�j
	g_D3DPresentParam.EnableAutoDepthStencil = 1;
	g_D3DPresentParam.AutoDepthStencilFormat = D3DFMT_D24S8;
	g_D3DPresentParam.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	g_D3DPresentParam.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;	// VSYNC��҂��Ȃ�

	// �f�o�C�X�쐬
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

	// �e�N�X�`���X�e�[�W�X�e�[�g
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	 
	// �y�o�b�t�@�L��
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
	sts = g_kabe.xfile.LoadXFile("assets/kabe.x", g_pd3dDevice); //�^��
	if (!sts) {
		MessageBox(NULL, "load x(kabe.x) file error", "load x file error", MB_OK);
	};
	sts = g_wall1.xfile.LoadXFile("assets/wall.x", g_pd3dDevice); //�c
	if (!sts) {
		MessageBox(NULL, "load x(wall.x) file error", "load x file error", MB_OK);
	};
	sts = g_wall2.xfile.LoadXFile("assets/wall2.x", g_pd3dDevice); //��
	if (!sts) {
		MessageBox(NULL, "load x(wall2.x) file error", "load x file error", MB_OK);
	};
	sts = g_wall3.xfile.LoadXFile("assets/wall.x", g_pd3dDevice); //�c
	if (!sts) {
		MessageBox(NULL, "load x(wall.x) file error", "load x file error", MB_OK);
	};
	sts = g_wall4.xfile.LoadXFile("assets/wall2.x", g_pd3dDevice); //��
	if (!sts) {
		MessageBox(NULL, "load x(wall2.x) file error", "load x file error", MB_OK);
	};
	sts = g_signal.xfile.LoadXFile("assets/signal.x", g_pd3dDevice);
	if (!sts) {
		MessageBox(NULL, "load x(signal.x) file error", "load x file error", MB_OK);
	};

	// Menu���
	
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
	
	// �������
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

	// �Q�[���I�[�o�[���1
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

	// �Q�[���I�[�o�[���2
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

	// �|�[�Y���

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

	// �N���A���

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

	// �I��

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


	// �����I��
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

	//�t�H���g������
	D3DXCreateFont(g_pd3dDevice, 20, 10, 0, 0, 0, 0, 0, 0, 0, "Times New Roman", &g_font);

	return(true);
}


//=============================================================================
// �J�����ƃI�u�W�F�N�g��������
//=============================================================================
void initGame(void)
{
	//�n��
	initModel(&g_jimen);
	g_jimen.yFg = true; //�\����L����

	//�X�J�C�h�[��
	initModel(&g_skydome);
	g_skydome.yFg = true; //�\����L����

	//�^�񒆂̕�
	initModel(&g_kabe);
	g_kabe.yFg = true; //�\����L����
	
	//�O�c��
	initModel(&g_wall1);
	g_wall1.pos.z = 750;
	g_wall1.yFg = true; //�\����L����

	//������			   
	initModel(&g_wall2);
	g_wall2.pos.x = -1000;
	g_wall2.yFg = true; //�\����L����

	//�O�c��
	initModel(&g_wall3);
	g_wall3.pos.z = -750;
	g_wall3.yFg = true; //�\����L����

	//������			   
	initModel(&g_wall4);
	g_wall4.pos.x = 1000;
	g_wall4.yFg = true; //�\����L����

	// �Ό���
	initModel(&g_training);
	g_training.r = 10.0f;
	g_training.pos.x = 800;
	g_training.pos.z = -550;
	g_training.yFg = true; //�\����L����

	//�ʍs�lA
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
		g_man[i].yFg = true; //�\����L����
	}

	//�M���@
	initModel(&g_signal);
	g_signal.r = 0.1f;
	g_signal.pos.x = 985;
	g_signal.pos.y = 1;
	g_signal.pos.z = -160;
	g_signal.yFg = true; //�\����L����

	//���@
	initModel(&g_jiki.data);
	g_jiki.data.r = 10.0f;
	g_jiki.data.pos.x = -950;
	g_jiki.data.pos.y = 1;
	g_jiki.data.pos.z = -10;
	g_jiki.data.yFg = true; //�\����L����
	for (int i = 0; i < 100; i++){
		g_jiki.posLog[i].x = 0; //�ʒu(�ߋ����O)
		g_jiki.posLog[i].y = 0; //�ʒu(�ߋ����O)
		g_jiki.posLog[i].z = 0; //�ʒu(�ߋ����O)
	}
	g_jiki.logNum = 0; //�ʒu(�ߋ����O)
	g_jiki.state = 1;

}

void initModel(MODEL_DATA* mdata)
{
	mdata->yFg = false; //�������s�����ۂ��t���O
	mdata->r = 0.0f;
	mdata->posSpeed = { 0, 0, 0 }; //�ʒu�ړ����x
	mdata->posBef = { 0, 0, 0 }; //�ʒu(1�R�}�O)
	mdata->pos = { 0, 0, 0 }; //�ʒu
//	mdata->angleSpeed = { 0, 0, 0 }; //�����ړ����x
	mdata->angleBef = { 0, 0, 0 }; //����(1�R�}�O)
	mdata->angle = { 0, 0, 0 }; //����
	mdata->mat = { 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1 }; // �ώZ�s��

	return;
}

//=============================================================================
// GameMain�֐�
//=============================================================================
void GameMain(void)
{
	if (before == 0)
	{
		D3DXMATRIX BackMat = { 1, 0, 0, 0, //�J�����Ǐ]
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
	// �X�^�[�g�O�̏������I�����Ă���
	if (0 < before)
	{
		if (measure == 0) // ��x�����ʂ�
		{
			pivot = timeGetTime(); // ���Ԃ̌��_���擾
		    ora = 0; // �X�^�[�g�O�ɊJ����pose���Ԃ�0�ɐݒ�
			measure += 1;
		}

		// 1�b����(�o�ߎ��� - �X�^�[�g�������� - �|�[�Y��ʂɂ�������)
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
	if (g_deduction < 70) // ���_���ꂷ�����ꍇ
	{
		nState = 4; // �Q�[���I�[�o�[
	}
	if (g_acl > 3.05f) // �����̑��x��60km�ȉ��A
	{
		kmcount++;
			if (kmcount > 60)
			{
				g_deduction = g_deduction - 1;
				kmcount = 0;
			}
	}
	if (g_jiki.data.pos.x > 750 && g_jiki.data.pos.z < -100) //���`�F�b�N�|�C���g
	{
		// ������ʂ邱�ƂŃS�[�����\�ƂȂ�
		point = 1;
	}

	if (point == 1) // ���N���A����
	{
		if (g_jiki.data.pos.x < -750 && g_jiki.data.pos.z > -10)
		{
			nState = 5; // �^�Q�[���N���A
		}
	}
	for (int i = 0; i < MAN; i++) {
		if (g_jiki.data.pos.x > 750 && g_jiki.data.pos.z < 200)
		{
			g_man[i].pos.x += 0.55f; // �l�̓���
			g_man[3].pos.x -= 0.2f;
			g_man[4].pos.x -= 0.2f;
		}
	}

	for (int i = 0; i < MAN; i++) {
		bool hit = HitSphere(g_jiki.data, g_man[i]); // �������l�ɂԂ����Ă��܂�����H
		if (hit == true)
		{
			// ���̂ɂ͋C��t���悤
			nState = 4; // �Q�[���I�[�o�[
		}
	}
	course();
	g_training.pos.z += 1.0;
	bool hit2 = HitSphere(g_jiki.data, g_signal); // �������M���ɂԂ����Ă��܂�����H
	if (hit2 == true)
	{
		// ���̂ɂ͋C��t���悤
		g_deduction -= 10; // ���_
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

		D3DXMatrixPerspectiveFovLH(&g_matprojection, D3DX_PI / 2.0f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 10000.0f);		// �v���W�F�N�V�����s��(�ˉe�s��)�쐬
		g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &g_matprojection);								// �v���W�F�N�V�����s����O���t�B�b�N�J�[�h�ɐݒ�

		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
		if (SUCCEEDED(g_pd3dDevice->BeginScene()))	// �`��J�n
		{
			//�n�ʂɑ΂��Ă̈ʒu�֍��W�ϊ�(���[���h�ϊ�)
			exportWorldMatrix(g_jimen.mat, g_jiki.data); // ���@
			exportWorldMatrix(g_jimen.mat, g_training);
			exportWorldMatrix(g_jimen.mat, g_kabe);
			exportWorldMatrix(g_jimen.mat, g_wall1); //kabe
			exportWorldMatrix(g_jimen.mat, g_wall2);
			exportWorldMatrix(g_jimen.mat, g_wall3);
			exportWorldMatrix(g_jimen.mat, g_wall4);
			for (int i = 0; i < MAN ; i++)
			exportWorldMatrix(g_jimen.mat, g_man[i]); // ���ԂȂ��l
			exportWorldMatrix(g_jimen.mat, g_signal); //�M���@
		
			//���@���J�����̖ڕW�ɂ���
			g_cameraLook.x = g_jiki.data.mat._41;
			g_cameraLook.y = g_jiki.data.mat._42 + 5;
			g_cameraLook.z = g_jiki.data.mat._43;

			//�J�����̈ʒu���Z�b�g6y7y
			D3DXMatrixLookAtLH(&g_matcamera, &g_cameraPos, &g_cameraLook, &g_cameraUp);		// �J�����s��(�r���[�s��)���쐬
			g_pd3dDevice->SetTransform(D3DTS_VIEW, &g_matcamera);		// �J�����s����O���t�B�b�N�J�[�h�ɃZ�b�g

			//�n�ʕ\��
			D3DXMatrixIdentity(&g_jimen.mat); //�P�ʍs��쐬
			if (g_jimen.yFg == true) {
				g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_jimen.mat);
				g_jimen.xfile.Draw(g_pd3dDevice);
			}

			//�X�J�C�h�[���\��
			if (g_skydome.yFg == true) {
				g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_skydome.mat);
				g_skydome.xfile.Draw(g_pd3dDevice);
			}

			//�l�\��
			for (int i = 0; i < MAN; i++) {
				if (g_man[i].yFg == true) {
					g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_man[i].mat);
					g_man[i].xfile.Draw(g_pd3dDevice);
				}
			}

			//���@�\��
			if (g_jiki.data.yFg == true) {
				g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_jiki.data.mat);
				g_jiki.data.xfile.Draw(g_pd3dDevice);
			}

			//���@�\��
			if (g_training.yFg == true) {
				g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_training.mat);
				g_training.xfile.Draw(g_pd3dDevice);
			}

			//�Ǖ\��
			if (g_kabe.yFg == true) {
				g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_kabe.mat);
				g_kabe.xfile.Draw(g_pd3dDevice);
			}

			//��1�\��
			if (g_wall1.yFg == true) {
				g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_wall1.mat);
				g_wall1.xfile.Draw(g_pd3dDevice);
			}

			//��2�\��
			if (g_wall2.yFg == true) {
				g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_wall2.mat);
				g_wall2.xfile.Draw(g_pd3dDevice);
			}

			//��3�\��
			if (g_wall3.yFg == true) {
				g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_wall3.mat);
				g_wall3.xfile.Draw(g_pd3dDevice);
			}

			//��4�\��
			if (g_wall4.yFg == true) {
				g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_wall4.mat);
				g_wall4.xfile.Draw(g_pd3dDevice);
			}


			//�M���@�\��
			if (g_signal.yFg == true) {
				g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_signal.mat);
				g_signal.xfile.Draw(g_pd3dDevice);
			}

			//�����\��
			RECT moji001;
			moji001.left = 100;
			moji001.top = 100;
			moji001.right = 640;
			moji001.bottom = 320;
			char buf[100];
			if (g_sel == 1) // ���o�[�\��
			{
				sprintf(buf, "Speed : %d km\nLever : %c\nScore : %d\nTime %dsec\n point : %d\n timer : %d", (int)((float)g_acl * 20), lever, g_deduction, time, point , timer); //point�̓`�F�b�N�|�C���g(��)
			}
			if (g_sel == 2) // �M�A�\��
			{
				sprintf(buf, "Speed : %d km\nGear : %c\nScore : %d\nTime %dsec\n point : %d timer : %d", (int)((float)g_acl * 20), cnt, g_deduction, time, point, timer);
			}
			g_font->DrawTextA(NULL, buf, -1, &moji001, NULL, 0xFFFF0000);

			g_pd3dDevice->EndScene(); // �`��I��
		}
		g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
	}


//=============================================================================
// �I�������֐�
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
// ���W�ϊ��֐�
//=============================================================================
void exportWorldMatrix(D3DXMATRIX& matworld, MODEL_DATA& mdata)
{
	D3DXMATRIX	matA;
	D3DXMATRIX	mattrans;

	//��ΓI�Ɉړ�
	{
		//�ʒu�ύX
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
		mdata.posBef = mdata.pos; //1�R�}�O���L�^���Ă���

		//�����ύX
		float angleX = mdata.angle.x;
		float angleY = mdata.angle.y;
		float angleZ = mdata.angle.z;

		D3DXMatrixTranslation(&mattrans, posX, posY, posZ);
		D3DXMatrixRotationYawPitchRoll(&matA, angleY*D3DX_PI* -1 / 180.0f, angleX*D3DX_PI / 180.0f, angleZ*D3DX_PI / 180.0f);
		matworld = matA * mattrans;
		mdata.mat = { 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1 }; // �ώZ�s��
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

bool HitSphere(MODEL_DATA& car, MODEL_DATA& object) //���Ƌ��ł̓����蔻��
{
	float hit = (object.pos.x - car.pos.x) * (object.pos.x - car.pos.x) + (object.pos.y - car.pos.y) * (object.pos.y - car.pos.y) + (object.pos.z - car.pos.z) * (object.pos.z - car.pos.z);
	if ((hit) * (hit) <= (car.r + object.r) * (car.r + object.r))
	{
		return true; // �Փ˂��Ă���
	}
	else
	{
		return false;
	}
}