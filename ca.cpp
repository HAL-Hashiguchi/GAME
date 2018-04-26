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

// �}�j���A���Ԃ̋���
void MTcar(void)
{
	switch (g_MT) {
//***********************************************
//**               �j���[�g����                **
//***********************************************
	case Neutral:
		BackCamera(BackMat);
		break;

//***********************************************
//**                   ���[                    **
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
//**                 �Z�J���h                  **
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
//**                  �T�[�h                   **
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
//**                  �g�b�v                   **
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
//**              �I�[�o�[�g�b�v               **
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
//**                ���o�[�X                   **
//***********************************************
	case Back:
		ReverseCamera(ReverseMat);
		Enginebrake(&g_acl);
		if (g_acl < 0) {
			if (GetKeyboardPress(DIK_X)) { //�o�b�N���u���[�L
				g_acl += 0.035f;
			}
		}
		if (GetKeyboardPress(DIK_Z)) { //�o�b�N���s
			g_acl -= 0.01f;
		}
		break;
	}

	g_jiki.data.pos.z += g_acl; // ���@�̃|�W�V�������A�N�Z���ɔ��

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
//**                �p�[�L���O                 **
//***********************************************
	case Parking:
		BackCamera(BackMat);
		break;

//***********************************************
//**                ���o�[�X                   **
//***********************************************
	case Reverse:
		ReverseCamera(ReverseMat);
		if (g_acl > 0) // �h���C�u��Ԃ���؂�ւ����ꍇ
		{
			Enginebrake(&g_acl);
		}
		if (g_acl < -0.005f)
		{
			g_acl += 0.005f; //�o�b�N�̊���
		}
		if (g_acl < 0)
		{
			if (GetKeyboardPress(DIK_X)) { //�o�b�N���u���[�L
				g_acl += 0.035f;
				if (g_acl > 0) // �o�b�N���ɑO�i���Ȃ��[�u
					g_acl = 0;
			}
		}
		if (GetKeyboardPress(DIK_Z)) { //�o�b�N���s
			g_acl -= 0.01f;
		}
		if (g_acl < -1.5f) // �o�b�N���ō����x
		{
			g_acl = -1.5f;
		}
		g_jiki.data.pos.z = g_acl;  // ���@���i�񂾕������A�N�Z���ɔ��
		break;

//***********************************************
//**                �h���C�u                   **
//***********************************************
	case Drive:
		BackCamera(BackMat);
		if (g_acl < 0.5f) // �u���[�L�𓥂�ł��Ȃ��ꍇ�i��(creep)
			g_acl += 0.01f;
		Move(&g_acl);
		Accelerate(&g_acl);
		Enginebrake(&g_acl);
		if (g_acl > 5) // �ō����x
			g_acl = 5;
		if (g_acl < 0)
			g_acl = 0;
		g_jiki.data.pos.z += g_acl;  // ���@���i�񂾕������A�N�Z���ɔ��
		break;
	}

//***********************************************
//**                �S�̑[�u                   **
//***********************************************
	if (g_acl != 0) {
		if (GetKeyboardPress(DIK_RIGHT))
			g_jiki.data.angle.y -= 1.5f;

		if (GetKeyboardPress(DIK_LEFT))
			g_jiki.data.angle.y += 1.5f;
	}
}

// �ړ����Ǘ�
void Move(float *speed)
{
	if (GetKeyboardPress(DIK_Z)) { // �O�։���
		*speed += 0.005f;
	}
	if (GetKeyboardPress(DIK_X)) { // �u���[�L
// ���������v�Z
/*
        float brake = 0;
		float coefficient = 0.8f; // ���C�W����0.8�Ƃ���
		float speedLog = *speed; //���݂̃X�s�[�h��ۑ�
		float realspeed = speedLog * 20; // ���x�����ۂ̐��l�ɋ߂Â���
		float stop = (pow(realspeed, 2.0) / (coefficient * 254)) / 20; // ���������̌v�Z
*/
		*speed -= 0.035f; // ����̓u���[�L�̏�����萔�ŏ���
		if (*speed < 0)
			*speed = 0;
	}
}

// �A�N�Z�����痣�����ꍇ�̒���
void Enginebrake(float *speed)
{
	// �G���W���u���[�L�̐��l�͎Ԏ�ɂ���ĈႤ�ׁA
	// ���x�ɂ���ēK�x�Ȑ��l����荞��ł���B
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
			// �G���W���𓥂�ł��Ȃ����͔��������Ȃ��B
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

// �����̒���
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
	
�����x
=
�́i�t�H�[�X�j������

=
(�g���N�~�d�͉����x�~�M����^�^�C���̔��a)���ԏd�i���ʁj

=
(50kgf�m x 9.8m/s2�~12/0.35��)��1730kg

=
9.7��/s2
*/
	//�g���N�@�� 20 // �Œ�
	// * 9.8m/s2 // �d�͉����x
	// 
	//
	// / 1500kg
}

// �ϑ��̒���
void transmission(void)
{
	switch (g_sel) {
	case AUTOMATIC:
		if (-0.05f < g_acl && g_acl < 0.05f){
			if (GetKeyboardPress(DIK_Q)) {
				g_AT = Parking;                      // �p�[�L���O
				g_acl = 0; // ���x��0�ɒ���
			}
		}
		if (GetKeyboardPress(DIK_W))
			g_AT = Reverse;                          // ���o�[�X
		if (GetKeyboardPress(DIK_E))
			g_AT = Drive;                            // �h���C�u
		break;
	case MANUAL:
		if (GetKeyboardPress(DIK_LSHIFT)) {
			if (g_acl < 0.05f) {
				if (GetKeyboardTrigger(DIK_TAB))
					g_MT = Neutral;                  // �j���[�g����
			}
				if (GetKeyboardTrigger(DIK_Q))
					g_MT = Low;                      // ���[
				if (GetKeyboardTrigger(DIK_A))
					g_MT = Second;                   // �Z�J���h
				if (GetKeyboardTrigger(DIK_W))
					g_MT = Third;                    // �T�[�h
				if (GetKeyboardTrigger(DIK_S))
					g_MT = Top;                      // �g�b�v
				if (GetKeyboardTrigger(DIK_E))
					g_MT = Overtop;                  // �I�[�o�[�g�b�v
				if (GetKeyboardTrigger(DIK_D))
					g_MT = Back;                     // �o�b�N
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

// �i�s���ɔw��ɒǏ]����J����
void BackCamera(D3DXMATRIX back) {
	back = back * g_jiki.data.mat;
	g_cameraPos.x = back._41;
	g_cameraPos.y = back._42;
	g_cameraPos.z = back._43;
}

// �ލs���ɔw������邽�߂̃J����
void ReverseCamera(D3DXMATRIX reverse) {
	reverse = reverse * g_jiki.data.mat;
	g_cameraPos.x = reverse._41;
	g_cameraPos.y = reverse._42;
	g_cameraPos.z = reverse._43;
}