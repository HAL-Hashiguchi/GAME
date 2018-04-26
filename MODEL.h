#ifndef MODEL_h_
#define MODEL_h_
#include "d3dx9.h"
#include "CDirect3DXFile.h"

#define MAN (5)

//*********************************************************
//**                     �\���̐錾                      **
//*********************************************************

//two dimensions dealed
typedef struct
{
	D3DXVECTOR3 vtx;
	float rhw;
	D3DCOLOR diffuse;
	D3DXVECTOR2 tex;
} VERTEX_2D;

//��ʓI�ȃ��f���f�[�^
typedef struct {
	bool yFg; //�������s�����ۂ��t���O
	CDirect3DXFile xfile; // X�t�@�C��
	float r;  //���a
	D3DXVECTOR3 posSpeed; // �ʒu�ړ����x
	D3DXVECTOR3 posBef; // �ʒu(1�R�}�O)
	D3DXVECTOR3 pos; // �ʒu
//	D3DXVECTOR3 angleSpeed; //�����ړ����x�@�����g�p
	D3DXVECTOR3 angleBef; // ����(1�R�}�O)
	D3DXVECTOR3 angle; // ����
	D3DXMATRIX  mat;   // �ώZ�s��
}MODEL_DATA;

//���@�Ǘ�����f�[�^��Z�߂��\����
typedef struct {
public:
	MODEL_DATA data; // ���f�����Ǘ�����f�[�^
	D3DXVECTOR3 posLog[100]; // �ʒu(�ߋ����O)
	int state;  // ��ԁ@1:�n�ʏ�@2:�W�����v��
	int logNum; // �ʒu(�ߋ����O)�ԍ�
}JIKI_DATA;

// �錾
extern JIKI_DATA g_jiki;						// ���@

#endif