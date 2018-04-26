#ifndef state_h_
#define state_h_

void UpdateMenu(void);
void DrawMenu(void);
void Operation(void);
void CharacterSelect(void);
void DrawSelect(void);
void DrawOperation(void);
void DrawOperation2(void);
void PoseMenu(void);
void DrawPose(void);
void StateInit(void);
void GameOver(void);
void DrawGameOver(void);
void Result(void);
void DrawResult(void);

extern int nState;
extern int nButtonCount;

//以下状態(State)とは関係なし(main.cppにて定義)
void initGame(void);

extern int point;
extern int measure;
extern int before;
extern int bcount;
extern int timer;
extern DWORD pivot;
extern DWORD time;
extern DWORD ora;
extern LPDIRECT3D9			g_pD3D;          
extern LPDIRECT3DDEVICE9	g_pd3dDevice;    
extern LPDIRECT3DTEXTURE9 g_pd3dTextureMenu;      // メニュー画面
extern LPDIRECT3DTEXTURE9 g_pd3dTextureOperation; // 説明
extern LPDIRECT3DTEXTURE9 g_pd3dTextureGameover;  // ゲームオーバー
extern LPDIRECT3DTEXTURE9 g_pd3dTextureAccident;  // traffic accident
extern LPDIRECT3DTEXTURE9 g_pd3dTexturePose;      // ポーズ画面
extern LPDIRECT3DTEXTURE9 g_pd3dTextureClear;     // コングラチュレーション
extern LPDIRECT3DTEXTURE9 g_pd3dTextureSelect[7];    //
extern LPDIRECT3DTEXTURE9 g_pd3dTextureArrow;
extern LPD3DXFONT g_font;                         // フォント
extern VERTEX_2D  g_vertexMenu[4];                // メニュー背景
extern VERTEX_2D  g_vertexOperation[4];           // 説明
extern VERTEX_2D  g_vertexGameover[4];            // ゲームオーバー
extern VERTEX_2D  g_vertexAccident[4];            // 事故の悲惨さ
extern VERTEX_2D  g_vertexPose[4];                // ポーズ
extern VERTEX_2D  g_vertexClear[4];               // クリア
extern VERTEX_2D  g_vertexSelect[7][4];              // 
extern VERTEX_2D  g_vertexArrow[4];

#define SCREEN_WIDTH  (800)
#define SCREEN_HEIGHT (600)
#define FVF_VERTEX_2D (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#endif