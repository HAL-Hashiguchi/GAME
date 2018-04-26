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

//�ȉ����(State)�Ƃ͊֌W�Ȃ�(main.cpp�ɂĒ�`)
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
extern LPDIRECT3DTEXTURE9 g_pd3dTextureMenu;      // ���j���[���
extern LPDIRECT3DTEXTURE9 g_pd3dTextureOperation; // ����
extern LPDIRECT3DTEXTURE9 g_pd3dTextureGameover;  // �Q�[���I�[�o�[
extern LPDIRECT3DTEXTURE9 g_pd3dTextureAccident;  // traffic accident
extern LPDIRECT3DTEXTURE9 g_pd3dTexturePose;      // �|�[�Y���
extern LPDIRECT3DTEXTURE9 g_pd3dTextureClear;     // �R���O���`�����[�V����
extern LPDIRECT3DTEXTURE9 g_pd3dTextureSelect[7];    //
extern LPDIRECT3DTEXTURE9 g_pd3dTextureArrow;
extern LPD3DXFONT g_font;                         // �t�H���g
extern VERTEX_2D  g_vertexMenu[4];                // ���j���[�w�i
extern VERTEX_2D  g_vertexOperation[4];           // ����
extern VERTEX_2D  g_vertexGameover[4];            // �Q�[���I�[�o�[
extern VERTEX_2D  g_vertexAccident[4];            // ���̂̔ߎS��
extern VERTEX_2D  g_vertexPose[4];                // �|�[�Y
extern VERTEX_2D  g_vertexClear[4];               // �N���A
extern VERTEX_2D  g_vertexSelect[7][4];              // 
extern VERTEX_2D  g_vertexArrow[4];

#define SCREEN_WIDTH  (800)
#define SCREEN_HEIGHT (600)
#define FVF_VERTEX_2D (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#endif