#ifndef MODEL_h_
#define MODEL_h_
#include "d3dx9.h"
#include "CDirect3DXFile.h"

#define MAN (5)

//*********************************************************
//**                     構造体宣言                      **
//*********************************************************

//two dimensions dealed
typedef struct
{
	D3DXVECTOR3 vtx;
	float rhw;
	D3DCOLOR diffuse;
	D3DXVECTOR2 tex;
} VERTEX_2D;

//一般的なモデルデータ
typedef struct {
	bool yFg; //処理を行うか否かフラグ
	CDirect3DXFile xfile; // Xファイル
	float r;  //半径
	D3DXVECTOR3 posSpeed; // 位置移動速度
	D3DXVECTOR3 posBef; // 位置(1コマ前)
	D3DXVECTOR3 pos; // 位置
//	D3DXVECTOR3 angleSpeed; //向き移動速度　※未使用
	D3DXVECTOR3 angleBef; // 向き(1コマ前)
	D3DXVECTOR3 angle; // 向き
	D3DXMATRIX  mat;   // 積算行列
}MODEL_DATA;

//自機管理するデータを纏めた構造体
typedef struct {
public:
	MODEL_DATA data; // モデルを管理するデータ
	D3DXVECTOR3 posLog[100]; // 位置(過去ログ)
	int state;  // 状態　1:地面上　2:ジャンプ中
	int logNum; // 位置(過去ログ)番号
}JIKI_DATA;

// 宣言
extern JIKI_DATA g_jiki;						// 自機

#endif