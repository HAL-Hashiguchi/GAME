#ifndef car_h_
#define car_h_

typedef enum {
	SELECT = 0,
	AUTOMATIC = 1,
	MANUAL = 2
}Car;

typedef enum {
	Neutral,
	Low,
	Second,
	Third,
	Top,
	Overtop,
	Back
}Gear;

typedef enum {
	Parking,
	Reverse,
	Drive
}Lever;

extern Car g_sel;
extern Gear g_MT;
extern Lever g_AT;
extern float g_acl;
extern int g_deduction;

void MTcar();
void ATcar();
void Move(float *speed);
void Enginebrake(float *speed);
void Accelerate(float *speed);
void course();
void transmission();
void BackCamera(D3DXMATRIX back);
void ReverseCamera(D3DXMATRIX reverse);

#endif