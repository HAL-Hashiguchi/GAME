#include "input.h"
#include "d3dx9.h"
#include "MODEL.h"
#include "camera.h"
#include "State.h"
#include "ca.h"


int g_deduction = 100;
Gear g_MT;
Lever g_AT;
int cnt = 0;
float g_acl = 0;
float curve = 0;
JIKI_DATA g_jiki;

// マニュアル車の挙動
void MTcar(void)
{
	switch (g_MT) {
//***********************************************
//**               ニュートラル                **
//***********************************************
	case Neutral:
		BackCamera(BackMat);
		break;

//***********************************************
//**                   ロー                    **
//***********************************************
	case Low:
		BackCamera(BackMat);
		Move(&g_acl);
		Accelerate(&g_acl);
		Enginebrake(&g_acl);
		if (g_acl > 0.8f)
			g_acl = 0.8f;
		break;

//***********************************************
//**                 セカンド                  **
//***********************************************
	case Second:
		BackCamera(BackMat);
		Move(&g_acl);
		Accelerate(&g_acl);
		Enginebrake(&g_acl);
		if (g_acl > 2.0f)
			g_acl = 2.0f;
		break;

//***********************************************
//**                  サード                   **
//***********************************************
	case Third:
		BackCamera(BackMat);
		Move(&g_acl);
		Accelerate(&g_acl);
		Enginebrake(&g_acl);
		if (g_acl > 3.25f)
			g_acl = 3.25f;
		break;

//***********************************************
//**                  トップ                   **
//***********************************************
	case Top:
		BackCamera(BackMat);
		Move(&g_acl);
		Accelerate(&g_acl);
		Enginebrake(&g_acl);
		if (g_acl > 4)
			g_acl = 4.0f;
		break;

//***********************************************
//**              オーバートップ               **
//***********************************************
	case Overtop:
		BackCamera(BackMat);
		Move(&g_acl);
		Accelerate(&g_acl);
		Enginebrake(&g_acl);
		if (g_acl > 5)
			g_acl = 5.0f;
		break;

//***********************************************
//**                リバース                   **
//***********************************************
	case Back:
		ReverseCamera(ReverseMat);
		Enginebrake(&g_acl);
		if (g_acl < 0) {
			if (GetKeyboardPress(DIK_X)) { //バック時ブレーキ
				g_acl += 0.035f;
			}
		}
		if (GetKeyboardPress(DIK_Z)) { //バック走行
			g_acl -= 0.01f;
		}
		break;
	}

	g_jiki.data.pos.z += g_acl; // 自機のポジションをアクセルに比例

	if (g_acl != 0) {
		if (GetKeyboardPress(DIK_RIGHT)) {
			g_jiki.data.angle.y -= 1.5f;
		}
		if (GetKeyboardPress(DIK_LEFT)) {
			g_jiki.data.angle.y += 1.5f;
		}
	}
}

void ATcar(void)
{
	switch (g_AT) {
//***********************************************
//**                パーキング                 **
//***********************************************
	case Parking:
		BackCamera(BackMat);
		break;

//***********************************************
//**                リバース                   **
//***********************************************
	case Reverse:
		ReverseCamera(ReverseMat);
		if (g_acl > 0) // ドライブ状態から切り替えた場合
		{
			Enginebrake(&g_acl);
		}
		if (g_acl < -0.005f)
		{
			g_acl += 0.005f; //バックの慣性
		}
		if (g_acl < 0)
		{
			if (GetKeyboardPress(DIK_X)) { //バック時ブレーキ
				g_acl += 0.035f;
				if (g_acl > 0) // バック中に前進しない措置
					g_acl = 0;
			}
		}
		if (GetKeyboardPress(DIK_Z)) { //バック走行
			g_acl -= 0.01f;
		}
		if (g_acl < -1.5f) // バック時最高速度
		{
			g_acl = -1.5f;
		}
		g_jiki.data.pos.z = g_acl;  // 自機が進んだ分ををアクセルに比例
		break;

//***********************************************
//**                ドライブ                   **
//***********************************************
	case Drive:
		BackCamera(BackMat);
		if (g_acl < 0.5f) // ブレーキを踏んでいない場合進む(creep)
			g_acl += 0.01f;
		Move(&g_acl);
		Accelerate(&g_acl);
		Enginebrake(&g_acl);
		if (g_acl > 5) // 最高速度
			g_acl = 5;
		if (g_acl < 0)
			g_acl = 0;
		g_jiki.data.pos.z += g_acl;  // 自機が進んだ分ををアクセルに比例
		break;
	}

//***********************************************
//**                全体措置                   **
//***********************************************
	if (g_acl != 0) {
		if (GetKeyboardPress(DIK_RIGHT))
			g_jiki.data.angle.y -= 1.5f;

		if (GetKeyboardPress(DIK_LEFT))
			g_jiki.data.angle.y += 1.5f;
	}
}

// 移動を管理
void Move(float *speed)
{
	if (GetKeyboardPress(DIK_Z)) { // 前へ加速
		*speed += 0.005f;
	}
	if (GetKeyboardPress(DIK_X)) { // ブレーキ
// 制動距離計算
/*
        float brake = 0;
		float coefficient = 0.8f; // 摩擦係数を0.8とする
		float speedLog = *speed; //現在のスピードを保存
		float realspeed = speedLog * 20; // 速度を実際の数値に近づける
		float stop = (pow(realspeed, 2.0) / (coefficient * 254)) / 20; // 制動距離の計算
*/
		*speed -= 0.035f; // 今回はブレーキの処理を定数で処理
		if (*speed < 0)
			*speed = 0;
	}
}

// アクセルから離した場合の調整
void Enginebrake(float *speed)
{
	// エンジンブレーキの数値は車種によって違う為、
	// 速度によって適度な数値を放り込んである。
	if (g_MT == Back || g_AT == Reverse) {
		if (*speed >= -0.8f)
			*speed += 0.003f;

		if (*speed >= -2.0f && *speed < -0.8f)
			*speed += 0.0025f;

		if (*speed > 0)
			*speed = 0;
	}
	else {
		if (GetKeyboardPress(DIK_Z))
		{
			// エンジンを踏んでいない時は反応させない。
		}
		else {
			if (*speed <= 0.8f)
				*speed -= 0.003f;

			if (*speed <= 2.0f && *speed > 0.8f)
				*speed -= 0.0025f;

			if (*speed <= 3.0f && *speed > 2.0f)
				*speed -= 0.001f;

			if (*speed <= 3.5f && *speed > 3.0f)
				*speed -= 0.0005f;

			if (*speed <= 4.0f && *speed > 3.5f)
				*speed -= 0.00025f;

			if (*speed > 4.0f)
				*speed -= 0.0000125f;

			if (*speed < 0)
				*speed = 0;
		}
	}
}

// 加速の調整
void Accelerate(float *speed) //
{
	if (GetKeyboardPress(DIK_Z)) {
			switch (g_MT) {
			case Neutral:
				break;
			case Low:
				*speed += 0.005f;
				break;
			case Second:
				*speed += 0.0035f;
				break;
			case Third:
				*speed += 0.0015f;
				break;
			case Top:
				*speed += 0.00095f;
				break;
			case Overtop:
				*speed += 0.00075f;
				break;
			}
		switch (g_AT) {
		case Drive:

			break;
		}
	}
	/*
	
加速度
=
力（フォース）÷質量

=
(トルク×重力加速度×ギヤ比／タイヤの半径)÷車重（質量）

=
(50kgf･m x 9.8m/s2×12/0.35ｍ)÷1730kg

=
9.7ｍ/s2
*/
	//トルク　＝ 20 // 固定
	// * 9.8m/s2 // 重力加速度
	// 
	//
	// / 1500kg
}

// 変速の調整
void transmission(void)
{
	switch (g_sel) {
	case AUTOMATIC:
		if (-0.05f < g_acl && g_acl < 0.05f){
			if (GetKeyboardPress(DIK_Q)) {
				g_AT = Parking;                      // パーキング
				g_acl = 0; // 速度を0に直す
			}
		}
		if (GetKeyboardPress(DIK_W))
			g_AT = Reverse;                          // リバース
		if (GetKeyboardPress(DIK_E))
			g_AT = Drive;                            // ドライブ
		break;
	case MANUAL:
		if (GetKeyboardPress(DIK_LSHIFT)) {
			if (g_acl < 0.05f) {
				if (GetKeyboardTrigger(DIK_TAB))
					g_MT = Neutral;                  // ニュートラル
			}
				if (GetKeyboardTrigger(DIK_Q))
					g_MT = Low;                      // ロー
				if (GetKeyboardTrigger(DIK_A))
					g_MT = Second;                   // セカンド
				if (GetKeyboardTrigger(DIK_W))
					g_MT = Third;                    // サード
				if (GetKeyboardTrigger(DIK_S))
					g_MT = Top;                      // トップ
				if (GetKeyboardTrigger(DIK_E))
					g_MT = Overtop;                  // オーバートップ
				if (GetKeyboardTrigger(DIK_D))
					g_MT = Back;                     // バック
				break;
			}
		}
	}

void course(void)
{
	if (g_jiki.data.pos.z > 748)
	{
		if (g_acl > 50)
		{
			g_acl = -50;
		}
		else
		{
			g_acl = 0;
		}
	}
	if (g_jiki.data.pos.x > 1000)
	{
		if (g_acl > 50)
		{
			g_acl = -50;
		}
		else
		{
			g_acl = 0;
		}
	}
	if (g_jiki.data.pos.x < -998)
	{
		if (g_acl > 50)
		{
			g_acl = -50;
		}
		else
		{
			g_acl = 0;
		}
	}
}

// 進行時に背後に追従するカメラ
void BackCamera(D3DXMATRIX back) {
	back = back * g_jiki.data.mat;
	g_cameraPos.x = back._41;
	g_cameraPos.y = back._42;
	g_cameraPos.z = back._43;
}

// 退行時に背後を見るためのカメラ
void ReverseCamera(D3DXMATRIX reverse) {
	reverse = reverse * g_jiki.data.mat;
	g_cameraPos.x = reverse._41;
	g_cameraPos.y = reverse._42;
	g_cameraPos.z = reverse._43;
}