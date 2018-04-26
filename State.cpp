#define _CRT_SECURE_NO_WARNINGS
#include "input.h"
#include <stdio.h>
#include <windows.h>
#include "sound.h"
#include "MODEL.h"
#include "ca.h"
#include "State.h"

// ��`
int nState = 0;        // ��ԑJ��
int nButtonCount = 15; // �{�^���J�E���g
int nCount = 0;
int pose = 0;          // �|�[�Y��ʂł̑I���ɗp����
int over = 0;          // �Q�[���I�[�o�[��ʂł̑I���ɗp����
int clear = 0;         // �Q�[���N���A��ʂł̑I���ɗp����
int ope = 0;           // �����ӏ��̕���ɂĎg�p����ϐ�(0.��ʂ̌���)(1.�L�[�}�j���A��)(2.�Ԏ�I��)
int chara = 0;         // �Ԏ�I���ŗp����ꎞ�I�ȕϐ�
int sel = 0;           // �Ԏ�I��(1.AT)(2.MT)
int accident = 0;      // ��ʎ��̗p
int score = 0;         // �ŏI�I�ȓ��_
DWORD dio = 0;         // �~�߂鎞��
DWORD jot = 0;         // �~�߂鎞��2
DWORD ora = 0;         // �~�߂Ă������Ԃ̍��v
int pmeasure = 0;      // �|�[�Y�̎��ɂ��g�p

void UpdateMenu(void)
{
	UpdateInput();
	//Space key �������� ���N���b�N�ŃQ�[�������Ɉڂ�
	if (GetKeyboardTrigger(DIK_SPACE) || GetMouseLeftTrigger() && nButtonCount < 0)
	{
		nButtonCount = 15;
		ope = 2;
		nState = 1;
	}
	//Z key �������� �E�N���b�N�ŎԎ�I����ʂ�
	if (GetKeyboardTrigger(DIK_Z) || GetMouseRightTrigger() && nButtonCount < 0)
	{
		nButtonCount = 15;
		ope = 2;
		nState = 1;
	}
}
void DrawMenu(void)
{
	//�o�b�N�o�b�t�@���y�o�b�t�@�̃N���A
	g_pd3dDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(255, 128, 128, 0), 1.0f, 0);

	//Direct3D�ɂ��`��̊J�n
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		g_pd3dDevice->SetFVF(FVF_VERTEX_2D);
		g_pd3dDevice->SetTexture(0, g_pd3dTextureMenu);
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2,
			g_vertexMenu, sizeof(VERTEX_2D));

		g_pd3dDevice->SetFVF(FVF_VERTEX_2D);
		g_pd3dDevice->SetTexture(0, g_pd3dTextureSelect[0]);
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2,
			g_vertexSelect[0], sizeof(VERTEX_2D));

			g_vertexSelect[0][0].vtx = D3DXVECTOR3(200.0f, 200.0f, 0.0f);
			g_vertexSelect[0][1].vtx = D3DXVECTOR3(400.0f, 200.0f, 0.0f);
			g_vertexSelect[0][2].vtx = D3DXVECTOR3(200.0f, 300.0f, 0.0f);
			g_vertexSelect[0][3].vtx = D3DXVECTOR3(400.0f, 300.0f, 0.0f);

		//Direct3D�ɂ��`��̏I��
		g_pd3dDevice->EndScene();
	}
	//�o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�̓���ւ�
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void Operation(void)
{
	UpdateInput();

	switch (ope) {
	case 0:
	{
		DrawOperation2();
		// ��ʂ̌���
		if (GetKeyboardTrigger(DIK_SPACE) || GetMouseLeftTrigger() && nButtonCount < 0)
		{
			nButtonCount = 15;
			ope = 1;
		}
		// �^�C�g����ʂ�
		if (GetKeyboardTrigger(DIK_X) || GetMouseRightTrigger() && nButtonCount < 0)
		{
			nButtonCount = 15;
			nState = 0;
		}
		break;
	}
	case 1:
	{
		DrawOperation();
		// �L�[�{�[�h�}�j���A��
		if (GetKeyboardTrigger(DIK_SPACE) || GetMouseLeftTrigger() && nButtonCount < 0)
		{
			nButtonCount = 15;
			ope = 2;
		}
		// ���ЂƂO�̏��
		if (GetKeyboardTrigger(DIK_X) || GetMouseRightTrigger() && nButtonCount < 0)
		{
			nButtonCount = 15;
			ope = 0;
			nState = 1;
		}
		break;
	}
	case 2:
	{
		DrawSelect();
		CharacterSelect();
		break;
	}
	}
}

void CharacterSelect(void)
{

	UpdateInput();
	if (GetKeyboardTrigger(DIK_LEFT))//�E�̕�
	{
		//nButtonCount = 15;
		chara -= 1;
	}
	if (GetKeyboardTrigger(DIK_RIGHT))//���̕�
	{
		//nButtonCount = 15;
		chara += 1;
	}
	if (GetKeyboardTrigger(DIK_X)) // �߂�
	{
		chara = 0;
		ope = 1;
		nState = 1;
	}
	if (chara > 1)
		chara = 1;
	if (chara < 0)
		chara = 0;
	if (GetKeyboardTrigger(DIK_SPACE))
	{
		//nButtonCount = 15;
		switch (chara) { // �������������Ă���J��
		case 0: // at
			chara = 0;
			sel = 1;
			nState = 2;
			break;
		case 1: // mt
			chara = 0;
			sel = 2;
			nState = 2;
			break;
		}
	}

}

void DrawSelect(void)
{
	g_pd3dDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	//Direct3D�ɂ��`��̊J�n
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		RECT rect = { 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT };
		TCHAR str[1024] = " press leftkey ... Automatic\npress rightkey ... Manual\nAnd press spacekey ... game start!\n\n...If you back,press Xkey";

		//�e�L�X�g�`��
		g_font->DrawText(NULL, str, -1, &rect, DT_LEFT, D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff));
		
		//Direct3D�ɂ��`��̏I��
		g_pd3dDevice->EndScene();
	}
	//�o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�̓���ւ�
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void DrawOperation(void)
{
	g_pd3dDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	//Direct3D�ɂ��`��̊J�n
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		RECT rect = { 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT };
		TCHAR str[1024] = "Key Manual\n\nUp ... Accel\nLeft or Right ... Curve\nDown ... Brake\nLeft Control ... Clutch\nTab ... Neutral\nQ ... Low\nA ... Second\nW ... Third\nS ... Top\nE ... Overtop\nD ... Reverse\n\n~Press X ... Title   ,   Space ... Game Start~\n\nGood luck!!";

		//�e�L�X�g�`��
		g_font->DrawText(NULL, str, -1, &rect, DT_LEFT, D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff));

		//Direct3D�ɂ��`��̏I��
		g_pd3dDevice->EndScene();
	}
	//�o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�̓���ւ�
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void DrawOperation2(void)
{
	//�o�b�N�o�b�t�@���y�o�b�t�@�̃N���A
	g_pd3dDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	//Direct3D�ɂ��`��̊J�n
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		g_pd3dDevice->SetFVF(FVF_VERTEX_2D);
		g_pd3dDevice->SetTexture(0, g_pd3dTextureOperation);
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2,
			g_vertexOperation, sizeof(VERTEX_2D));

		//Direct3D�ɂ��`��̏I��
		g_pd3dDevice->EndScene();
	}
	//�o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�̓���ւ�
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void PoseMenu(void)
{
	UpdateInput();
		if (pmeasure == 0) {
			dio = timeGetTime();
			pmeasure += 1;
		}
		jot = ((timeGetTime() - dio) / 1000); // �|�[�Y��ʂɓ����ĉ��b�o���Ă��邩���v��
	if (GetKeyboardTrigger(DIK_UP))//��̕�
	{
		nButtonCount = 15;
		pose -= 1;
	}
	if (GetKeyboardTrigger(DIK_DOWN))//���̕�
	{
		nButtonCount = 15;
		pose += 1;
	}
	if (pose > 3)
		pose = 3;
	if (pose < 0)
		pose = 0;
	if (GetKeyboardTrigger(DIK_SPACE))
	{
	switch (pose) { // �������������Ă���J��
	case 0: // �|�[�Y��ʂ���߂�
		pose = 0;
		pmeasure = 0;
		ora = ora + jot; // pose���̍��v�o�ߎ���
		dio = 0;
		jot = 0;
		nState = 2;
		break;
	case 1: // ���g���C
		pose = 0;
		StateInit();
		initGame();
		nState = 2;
		break;
	case 2: // �Ԏ�I��
		pose = 0;
		StateInit();
		initGame();
		ope = 2;
		nState = 1;
		break;
	case 3: // �^�C�g����ʂ֖߂�
		pose = 0;
		StateInit();
		initGame();
		nState = 0;
		break;
	}
	}
}

void DrawPose(void)
{
	g_pd3dDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	//Direct3D�ɂ��`��̊J�n
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		g_pd3dDevice->SetFVF(FVF_VERTEX_2D);
		g_pd3dDevice->SetTexture(0, g_pd3dTexturePose);
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2,
			g_vertexPose, sizeof(VERTEX_2D));

		g_pd3dDevice->SetFVF(FVF_VERTEX_2D);
		g_pd3dDevice->SetTexture(0, g_pd3dTextureArrow);
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2,
			g_vertexArrow, sizeof(VERTEX_2D));

		for (int i = 1; i < 5; i++) {
			g_pd3dDevice->SetFVF(FVF_VERTEX_2D);
			g_pd3dDevice->SetTexture(0, g_pd3dTextureSelect[i]);
			g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2,
				g_vertexSelect[i], sizeof(VERTEX_2D));
		}

		switch (pose) {
		case 0:
			g_vertexArrow[0].vtx = D3DXVECTOR3(160.0f, 190.0f, 0.0f);
			g_vertexArrow[1].vtx = D3DXVECTOR3(360.0f, 190.0f, 0.0f);
			g_vertexArrow[2].vtx = D3DXVECTOR3(160.0f, 290.0f, 0.0f);
			g_vertexArrow[3].vtx = D3DXVECTOR3(360.0f, 290.0f, 0.0f);
			break;
		case 1:
			g_vertexArrow[0].vtx = D3DXVECTOR3(160.0f, 240.0f, 0.0f);
			g_vertexArrow[1].vtx = D3DXVECTOR3(360.0f, 240.0f, 0.0f);
			g_vertexArrow[2].vtx = D3DXVECTOR3(160.0f, 340.0f, 0.0f);
			g_vertexArrow[3].vtx = D3DXVECTOR3(360.0f, 340.0f, 0.0f);
			break;
		case 2:
			g_vertexArrow[0].vtx = D3DXVECTOR3(160.0f, 290.0f, 0.0f);
			g_vertexArrow[1].vtx = D3DXVECTOR3(360.0f, 290.0f, 0.0f);
			g_vertexArrow[2].vtx = D3DXVECTOR3(160.0f, 390.0f, 0.0f);
			g_vertexArrow[3].vtx = D3DXVECTOR3(360.0f, 390.0f, 0.0f);
			break;
		case 3:
			g_vertexArrow[0].vtx = D3DXVECTOR3(160.0f, 340.0f, 0.0f);
			g_vertexArrow[1].vtx = D3DXVECTOR3(360.0f, 340.0f, 0.0f);
			g_vertexArrow[2].vtx = D3DXVECTOR3(160.0f, 440.0f, 0.0f);
			g_vertexArrow[3].vtx = D3DXVECTOR3(360.0f, 440.0f, 0.0f);
			break;
		}

		g_vertexSelect[1][0].vtx = D3DXVECTOR3(200.0f, 200.0f, 0.0f);
		g_vertexSelect[1][1].vtx = D3DXVECTOR3(400.0f, 200.0f, 0.0f);
		g_vertexSelect[1][2].vtx = D3DXVECTOR3(200.0f, 300.0f, 0.0f);
		g_vertexSelect[1][3].vtx = D3DXVECTOR3(400.0f, 300.0f, 0.0f);

		g_vertexSelect[2][0].vtx = D3DXVECTOR3(200.0f, 250.0f, 0.0f);
		g_vertexSelect[2][1].vtx = D3DXVECTOR3(400.0f, 250.0f, 0.0f);
		g_vertexSelect[2][2].vtx = D3DXVECTOR3(200.0f, 350.0f, 0.0f);
		g_vertexSelect[2][3].vtx = D3DXVECTOR3(400.0f, 350.0f, 0.0f);

		g_vertexSelect[3][0].vtx = D3DXVECTOR3(200.0f, 300.0f, 0.0f);
		g_vertexSelect[3][1].vtx = D3DXVECTOR3(400.0f, 300.0f, 0.0f);
		g_vertexSelect[3][2].vtx = D3DXVECTOR3(200.0f, 400.0f, 0.0f);
		g_vertexSelect[3][3].vtx = D3DXVECTOR3(400.0f, 400.0f, 0.0f);

		g_vertexSelect[4][0].vtx = D3DXVECTOR3(200.0f, 350.0f, 0.0f);
		g_vertexSelect[4][1].vtx = D3DXVECTOR3(400.0f, 350.0f, 0.0f);
		g_vertexSelect[4][2].vtx = D3DXVECTOR3(200.0f, 450.0f, 0.0f);
		g_vertexSelect[4][3].vtx = D3DXVECTOR3(400.0f, 450.0f, 0.0f);

		//Direct3D�ɂ��`��̏I��
		g_pd3dDevice->EndScene();
	}
	//�o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�̓���ւ�
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void GameOver(void)
{
	UpdateInput();
	if (GetKeyboardTrigger(DIK_UP))//��̕�
	{
		nButtonCount = 15;
		over -= 1;
	}
	if (GetKeyboardTrigger(DIK_DOWN))//���̕�
	{
		nButtonCount = 15;
		over += 1;
	}
	if (over > 2)
		over = 2;
	if (over < 0)
		over = 0;
	if (GetKeyboardTrigger(DIK_SPACE))
	{
		switch (over) { // �������������Ă���J��
		case 0: // ���g���C
			over = 0;
			StateInit();
			initGame();
			nState = 2;
			break;
		case 1: // �Ԏ�I��
			over = 0;
			StateInit();
			initGame();
			ope = 2;
			nState = 1;
			break;
		case 2: // �^�C�g����ʂ֖߂�
			over = 0;
			StateInit();
			initGame();
			nState = 0;
			break;
		}
	}
}
void DrawGameOver(void)
{
	g_pd3dDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(255, 128, 128, 0), 1.0f, 0);

	//Direct3D�ɂ��`��̊J�n
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{	
		if (deduction < 70)
		{
			g_pd3dDevice->SetFVF(FVF_VERTEX_2D);
			g_pd3dDevice->SetTexture(0, g_pd3dTextureGameover);
			g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2,
				g_vertexGameover, sizeof(VERTEX_2D));
		}
		else
		{
			g_pd3dDevice->SetFVF(FVF_VERTEX_2D);
			g_pd3dDevice->SetTexture(0, g_pd3dTextureAccident);
			g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2,
				g_vertexAccident, sizeof(VERTEX_2D));
		}

		g_pd3dDevice->SetFVF(FVF_VERTEX_2D);
		g_pd3dDevice->SetTexture(0, g_pd3dTextureArrow);
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2,
			g_vertexArrow, sizeof(VERTEX_2D));

		for (int i = 2; i < 5; i++) {
			g_pd3dDevice->SetFVF(FVF_VERTEX_2D);
			g_pd3dDevice->SetTexture(0, g_pd3dTextureSelect[i]);
			g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2,
				g_vertexSelect[i], sizeof(VERTEX_2D));
		}

		switch (over) {
			case 0:
				g_vertexArrow[0].vtx = D3DXVECTOR3(160.0f, 240.0f, 0.0f);
				g_vertexArrow[1].vtx = D3DXVECTOR3(360.0f, 240.0f, 0.0f);
				g_vertexArrow[2].vtx = D3DXVECTOR3(160.0f, 340.0f, 0.0f);
				g_vertexArrow[3].vtx = D3DXVECTOR3(360.0f, 340.0f, 0.0f);
				break;
			case 1:
				g_vertexArrow[0].vtx = D3DXVECTOR3(160.0f, 290.0f, 0.0f);
				g_vertexArrow[1].vtx = D3DXVECTOR3(360.0f, 290.0f, 0.0f);
				g_vertexArrow[2].vtx = D3DXVECTOR3(160.0f, 390.0f, 0.0f);
				g_vertexArrow[3].vtx = D3DXVECTOR3(360.0f, 390.0f, 0.0f);
				break;
			case 2:
				g_vertexArrow[0].vtx = D3DXVECTOR3(160.0f, 340.0f, 0.0f);
				g_vertexArrow[1].vtx = D3DXVECTOR3(360.0f, 340.0f, 0.0f);
				g_vertexArrow[2].vtx = D3DXVECTOR3(160.0f, 440.0f, 0.0f);
				g_vertexArrow[3].vtx = D3DXVECTOR3(360.0f, 440.0f, 0.0f);
				break;
			}
		g_vertexSelect[2][0].vtx = D3DXVECTOR3(200.0f, 250.0f, 0.0f);
		g_vertexSelect[2][1].vtx = D3DXVECTOR3(400.0f, 250.0f, 0.0f);
		g_vertexSelect[2][2].vtx = D3DXVECTOR3(200.0f, 350.0f, 0.0f);
		g_vertexSelect[2][3].vtx = D3DXVECTOR3(400.0f, 350.0f, 0.0f);

		g_vertexSelect[3][0].vtx = D3DXVECTOR3(200.0f, 300.0f, 0.0f);
		g_vertexSelect[3][1].vtx = D3DXVECTOR3(400.0f, 300.0f, 0.0f);
		g_vertexSelect[3][2].vtx = D3DXVECTOR3(200.0f, 400.0f, 0.0f);
		g_vertexSelect[3][3].vtx = D3DXVECTOR3(400.0f, 400.0f, 0.0f);

		g_vertexSelect[4][0].vtx = D3DXVECTOR3(200.0f, 350.0f, 0.0f);
		g_vertexSelect[4][1].vtx = D3DXVECTOR3(400.0f, 350.0f, 0.0f);
		g_vertexSelect[4][2].vtx = D3DXVECTOR3(200.0f, 450.0f, 0.0f);
		g_vertexSelect[4][3].vtx = D3DXVECTOR3(400.0f, 450.0f, 0.0f);

		//Direct3D�ɂ��`��̏I��
		g_pd3dDevice->EndScene();
	}
	//�o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�̓���ւ�
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void Result(void)
{
	UpdateInput();
	if (GetKeyboardTrigger(DIK_UP))//��̕�
	{
		nButtonCount = 15;
		clear -= 1;
	}
	if (GetKeyboardTrigger(DIK_DOWN))//���̕�
	{
		nButtonCount = 15;
		clear += 1;
	}
	if (clear > 2)
		clear = 2;
	if (clear < 0)
		clear = 0;
	if (GetKeyboardTrigger(DIK_SPACE))
	{
		switch (clear) { // �������������Ă���J��
		case 0: // ���g���C
			clear = 0;
			StateInit();
			initGame();
			nState = 2;
			break;
		case 1: // �Ԏ�I��
			clear = 0;
			StateInit();
			initGame();
			ope = 2;
			nState = 1;
			break;
		case 2: // �^�C�g����ʂ֖߂�
			clear = 0;
			StateInit();
			initGame();
			nState = 0;
			break;
		}
	}
}
void DrawResult(void)
{
	g_pd3dDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(255, 128, 128, 0), 1.0f, 0);

	//Direct3D�ɂ��`��̊J�n
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		if (sel == 1)
		{
			score = deduction * (600 - time) * 0.9; // ���삪�ȒP��AT�̏ꍇ�{����Ⴍ���Ă���
		}
		if (sel == 2)
		{
			score = deduction * (600 - time);
		}
		g_pd3dDevice->SetFVF(FVF_VERTEX_2D);
		g_pd3dDevice->SetTexture(0, g_pd3dTextureClear);
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2,
			g_vertexClear, sizeof(VERTEX_2D));

		g_pd3dDevice->SetFVF(FVF_VERTEX_2D);
		g_pd3dDevice->SetTexture(0, g_pd3dTextureArrow);
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2,
			g_vertexArrow, sizeof(VERTEX_2D));

		for (int i = 2; i < 5; i++) {
			g_pd3dDevice->SetFVF(FVF_VERTEX_2D);
			g_pd3dDevice->SetTexture(0, g_pd3dTextureSelect[i]);
			g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2,
				g_vertexSelect[i], sizeof(VERTEX_2D));
		}

		switch (clear) {
		case 0:
			g_vertexArrow[0].vtx = D3DXVECTOR3(160.0f, 290.0f, 0.0f);
			g_vertexArrow[1].vtx = D3DXVECTOR3(360.0f, 290.0f, 0.0f);
			g_vertexArrow[2].vtx = D3DXVECTOR3(160.0f, 390.0f, 0.0f);
			g_vertexArrow[3].vtx = D3DXVECTOR3(360.0f, 390.0f, 0.0f);
			break;
		case 1:
			g_vertexArrow[0].vtx = D3DXVECTOR3(160.0f, 340.0f, 0.0f);
			g_vertexArrow[1].vtx = D3DXVECTOR3(360.0f, 340.0f, 0.0f);
			g_vertexArrow[2].vtx = D3DXVECTOR3(160.0f, 440.0f, 0.0f);
			g_vertexArrow[3].vtx = D3DXVECTOR3(360.0f, 440.0f, 0.0f);
			break;
		case 2:
			g_vertexArrow[0].vtx = D3DXVECTOR3(160.0f, 390.0f, 0.0f);
			g_vertexArrow[1].vtx = D3DXVECTOR3(360.0f, 390.0f, 0.0f);
			g_vertexArrow[2].vtx = D3DXVECTOR3(160.0f, 490.0f, 0.0f);
			g_vertexArrow[3].vtx = D3DXVECTOR3(360.0f, 490.0f, 0.0f);
			break;
		}
		g_vertexSelect[2][0].vtx = D3DXVECTOR3(200.0f, 300.0f, 0.0f);
		g_vertexSelect[2][1].vtx = D3DXVECTOR3(400.0f, 300.0f, 0.0f);
		g_vertexSelect[2][2].vtx = D3DXVECTOR3(200.0f, 400.0f, 0.0f);
		g_vertexSelect[2][3].vtx = D3DXVECTOR3(400.0f, 400.0f, 0.0f);

		g_vertexSelect[3][0].vtx = D3DXVECTOR3(200.0f, 350.0f, 0.0f);
		g_vertexSelect[3][1].vtx = D3DXVECTOR3(400.0f, 350.0f, 0.0f);
		g_vertexSelect[3][2].vtx = D3DXVECTOR3(200.0f, 450.0f, 0.0f);
		g_vertexSelect[3][3].vtx = D3DXVECTOR3(400.0f, 450.0f, 0.0f);

		g_vertexSelect[4][0].vtx = D3DXVECTOR3(200.0f, 400.0f, 0.0f);
		g_vertexSelect[4][1].vtx = D3DXVECTOR3(400.0f, 400.0f, 0.0f);
		g_vertexSelect[4][2].vtx = D3DXVECTOR3(200.0f, 500.0f, 0.0f);
		g_vertexSelect[4][3].vtx = D3DXVECTOR3(400.0f, 500.0f, 0.0f);

		RECT moji001;
		moji001.left = 100;
		moji001.top = 200;
		moji001.right = 640;
		moji001.bottom = 320;
		char buf[100];
		sprintf(buf, "\n\nscore %d", score);
		g_font->DrawTextA(NULL, buf, -1, &moji001, NULL, 0xFFFF0000);

		//Direct3D�ɂ��`��̏I��
		g_pd3dDevice->EndScene();
	}
	//�o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�̓���ւ�
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void StateInit(void) // ��ԑJ�ڂŕK�v�ȏ���������
{
	deduction = 100;                  // �X�R�A(deduct)
	lever = 'P';                      // �V�t�g���o�[
	cnt = 'N';                        // �M�A�J�E���g
	acl = 0;                          // ���x
	ope = 0;                          // OperationState
	point = 0;                        // �`�F�b�N�|�C���g
	before = 0;                       // �X�^�[�g�O����
	bcount = 0;                       // �悭����321GO
	timer = 4;                        // �X�^�[�g�O�v���p
	measure = 0;                      // ���Ԍv���Ɏg��
	pivot = 0;                        // ���Ԍv���̌��_
	time = 0;                         // ���݂̎��Ԃ��擾
	ora = 0;                          // pose���̍��v�o�ߎ���
}