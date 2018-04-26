#define _CRT_SECURE_NO_WARNINGS
#include "input.h"
#include <stdio.h>
#include <windows.h>
#include "sound.h"
#include "MODEL.h"
#include "ca.h"
#include "State.h"

// 定義
int nState = 0;        // 状態遷移
int nButtonCount = 15; // ボタンカウント
int nCount = 0;
int pose = 0;          // ポーズ画面での選択に用いる
int over = 0;          // ゲームオーバー画面での選択に用いる
int clear = 0;         // ゲームクリア画面での選択に用いる
int ope = 0;           // 説明箇所の分岐にて使用する変数(0.画面の見方)(1.キーマニュアル)(2.車種選択)
int chara = 0;         // 車種選択で用いる一時的な変数
int sel = 0;           // 車種選択(1.AT)(2.MT)
int accident = 0;      // 交通事故用
int score = 0;         // 最終的な得点
DWORD dio = 0;         // 止める時間
DWORD jot = 0;         // 止める時間2
DWORD ora = 0;         // 止めていた時間の合計
int pmeasure = 0;      // ポーズの時にも使用

void UpdateMenu(void)
{
	UpdateInput();
	//Space key もしくは 左クリックでゲーム説明に移る
	if (GetKeyboardTrigger(DIK_SPACE) || GetMouseLeftTrigger() && nButtonCount < 0)
	{
		nButtonCount = 15;
		ope = 2;
		nState = 1;
	}
	//Z key もしくは 右クリックで車種選択画面へ
	if (GetKeyboardTrigger(DIK_Z) || GetMouseRightTrigger() && nButtonCount < 0)
	{
		nButtonCount = 15;
		ope = 2;
		nState = 1;
	}
}
void DrawMenu(void)
{
	//バックバッファ＆Ｚバッファのクリア
	g_pd3dDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(255, 128, 128, 0), 1.0f, 0);

	//Direct3Dによる描画の開始
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

		//Direct3Dによる描画の終了
		g_pd3dDevice->EndScene();
	}
	//バックバッファとフロントバッファの入れ替え
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void Operation(void)
{
	UpdateInput();

	switch (ope) {
	case 0:
	{
		DrawOperation2();
		// 画面の見方
		if (GetKeyboardTrigger(DIK_SPACE) || GetMouseLeftTrigger() && nButtonCount < 0)
		{
			nButtonCount = 15;
			ope = 1;
		}
		// タイトル画面へ
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
		// キーボードマニュアル
		if (GetKeyboardTrigger(DIK_SPACE) || GetMouseLeftTrigger() && nButtonCount < 0)
		{
			nButtonCount = 15;
			ope = 2;
		}
		// おひとつ前の状態
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
	if (GetKeyboardTrigger(DIK_LEFT))//右の方
	{
		//nButtonCount = 15;
		chara -= 1;
	}
	if (GetKeyboardTrigger(DIK_RIGHT))//左の方
	{
		//nButtonCount = 15;
		chara += 1;
	}
	if (GetKeyboardTrigger(DIK_X)) // 戻る
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
		switch (chara) { // 初期化処理してから遷移
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

	//Direct3Dによる描画の開始
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		RECT rect = { 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT };
		TCHAR str[1024] = " press leftkey ... Automatic\npress rightkey ... Manual\nAnd press spacekey ... game start!\n\n...If you back,press Xkey";

		//テキスト描画
		g_font->DrawText(NULL, str, -1, &rect, DT_LEFT, D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff));
		
		//Direct3Dによる描画の終了
		g_pd3dDevice->EndScene();
	}
	//バックバッファとフロントバッファの入れ替え
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void DrawOperation(void)
{
	g_pd3dDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	//Direct3Dによる描画の開始
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		RECT rect = { 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT };
		TCHAR str[1024] = "Key Manual\n\nUp ... Accel\nLeft or Right ... Curve\nDown ... Brake\nLeft Control ... Clutch\nTab ... Neutral\nQ ... Low\nA ... Second\nW ... Third\nS ... Top\nE ... Overtop\nD ... Reverse\n\n~Press X ... Title   ,   Space ... Game Start~\n\nGood luck!!";

		//テキスト描画
		g_font->DrawText(NULL, str, -1, &rect, DT_LEFT, D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff));

		//Direct3Dによる描画の終了
		g_pd3dDevice->EndScene();
	}
	//バックバッファとフロントバッファの入れ替え
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void DrawOperation2(void)
{
	//バックバッファ＆Ｚバッファのクリア
	g_pd3dDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	//Direct3Dによる描画の開始
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		g_pd3dDevice->SetFVF(FVF_VERTEX_2D);
		g_pd3dDevice->SetTexture(0, g_pd3dTextureOperation);
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2,
			g_vertexOperation, sizeof(VERTEX_2D));

		//Direct3Dによる描画の終了
		g_pd3dDevice->EndScene();
	}
	//バックバッファとフロントバッファの入れ替え
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void PoseMenu(void)
{
	UpdateInput();
		if (pmeasure == 0) {
			dio = timeGetTime();
			pmeasure += 1;
		}
		jot = ((timeGetTime() - dio) / 1000); // ポーズ画面に入って何秒経っているかを計測
	if (GetKeyboardTrigger(DIK_UP))//上の方
	{
		nButtonCount = 15;
		pose -= 1;
	}
	if (GetKeyboardTrigger(DIK_DOWN))//下の方
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
	switch (pose) { // 初期化処理してから遷移
	case 0: // ポーズ画面から戻る
		pose = 0;
		pmeasure = 0;
		ora = ora + jot; // pose中の合計経過時間
		dio = 0;
		jot = 0;
		nState = 2;
		break;
	case 1: // リトライ
		pose = 0;
		StateInit();
		initGame();
		nState = 2;
		break;
	case 2: // 車種選択
		pose = 0;
		StateInit();
		initGame();
		ope = 2;
		nState = 1;
		break;
	case 3: // タイトル画面へ戻る
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

	//Direct3Dによる描画の開始
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

		//Direct3Dによる描画の終了
		g_pd3dDevice->EndScene();
	}
	//バックバッファとフロントバッファの入れ替え
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void GameOver(void)
{
	UpdateInput();
	if (GetKeyboardTrigger(DIK_UP))//上の方
	{
		nButtonCount = 15;
		over -= 1;
	}
	if (GetKeyboardTrigger(DIK_DOWN))//下の方
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
		switch (over) { // 初期化処理してから遷移
		case 0: // リトライ
			over = 0;
			StateInit();
			initGame();
			nState = 2;
			break;
		case 1: // 車種選択
			over = 0;
			StateInit();
			initGame();
			ope = 2;
			nState = 1;
			break;
		case 2: // タイトル画面へ戻る
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

	//Direct3Dによる描画の開始
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

		//Direct3Dによる描画の終了
		g_pd3dDevice->EndScene();
	}
	//バックバッファとフロントバッファの入れ替え
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void Result(void)
{
	UpdateInput();
	if (GetKeyboardTrigger(DIK_UP))//上の方
	{
		nButtonCount = 15;
		clear -= 1;
	}
	if (GetKeyboardTrigger(DIK_DOWN))//下の方
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
		switch (clear) { // 初期化処理してから遷移
		case 0: // リトライ
			clear = 0;
			StateInit();
			initGame();
			nState = 2;
			break;
		case 1: // 車種選択
			clear = 0;
			StateInit();
			initGame();
			ope = 2;
			nState = 1;
			break;
		case 2: // タイトル画面へ戻る
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

	//Direct3Dによる描画の開始
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		if (sel == 1)
		{
			score = deduction * (600 - time) * 0.9; // 操作が簡単なATの場合倍率を低くしている
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

		//Direct3Dによる描画の終了
		g_pd3dDevice->EndScene();
	}
	//バックバッファとフロントバッファの入れ替え
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void StateInit(void) // 状態遷移で必要な初期化処理
{
	deduction = 100;                  // スコア(deduct)
	lever = 'P';                      // シフトレバー
	cnt = 'N';                        // ギアカウント
	acl = 0;                          // 速度
	ope = 0;                          // OperationState
	point = 0;                        // チェックポイント
	before = 0;                       // スタート前処理
	bcount = 0;                       // よくある321GO
	timer = 4;                        // スタート前計測用
	measure = 0;                      // 時間計測に使う
	pivot = 0;                        // 時間計測の原点
	time = 0;                         // 現在の時間を取得
	ora = 0;                          // pose中の合計経過時間
}