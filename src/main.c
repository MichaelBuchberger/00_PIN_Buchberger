/**
 * AIIT STM32F4 Template
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>

#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ioe.h"

typedef enum{
	unaccessed,
	accesscheck,
	accessed,
	access_denied,
	access_wait,
	hub,
}PIN_state;

uint32_t myImage[] =
{
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
		,0x00,0x00,0x00,0x00,0x0f,0xf0,0x00,0x00,0x00
		,0x00,0x00,0x00,0x01,0xff,0xff,0x00,0x00,0x00
		,0x00,0x00,0x00,0x07,0xff,0xff,0xc0,0x00,0x00
		,0x00,0x00,0x00,0x3f,0xff,0xff,0xe0,0x00,0x00
		,0x00,0x00,0x00,0x7f,0xff,0xff,0xf8,0x00,0x00
		,0x00,0x00,0x01,0xff,0xff,0xff,0xfc,0x00,0x00
		,0x00,0x00,0x03,0xff,0xff,0xff,0xfe,0x00,0x00
		,0x00,0x00,0x0f,0xff,0xff,0xff,0xff,0x00,0x00
		,0x00,0x00,0x1f,0xff,0xff,0xff,0xff,0x80,0x00
		,0x00,0x00,0x3f,0xff,0xff,0xff,0xff,0xc0,0x00
		,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xe0,0x00
		,0x00,0x01,0xff,0xff,0xff,0xff,0xff,0xe0,0x00
		,0x00,0x03,0xff,0xff,0xff,0xff,0xff,0xf0,0x00
		,0x00,0x03,0xff,0xff,0xff,0xff,0xff,0xf0,0x00
		,0x00,0x07,0xff,0xff,0xff,0xff,0xff,0xf8,0x00
		,0x00,0x0f,0xff,0xff,0xff,0xff,0xff,0xf8,0x00
		,0x00,0x0f,0xff,0xff,0xff,0xff,0xff,0xfc,0x00
		,0x00,0x1f,0xff,0xff,0xff,0xff,0xff,0xfc,0x00
		,0x00,0x3f,0xff,0xff,0xff,0xff,0xff,0xfc,0x00
		,0x00,0x3f,0xff,0xff,0xff,0xff,0xff,0xfc,0x00
		,0x00,0x7f,0xfc,0x0f,0xff,0xff,0xff,0xfc,0x00
		,0x00,0x7f,0xf8,0x07,0xff,0xff,0xff,0xfe,0x00
		,0x00,0x3f,0xf0,0x03,0xff,0xff,0xff,0xfe,0x00
		,0x00,0x3f,0xf0,0x01,0xff,0xff,0xff,0xfe,0x00
		,0x00,0x3f,0xe0,0x01,0xff,0xff,0xff,0xfe,0x00
		,0x00,0x7f,0xe0,0x01,0xff,0xff,0xff,0xfe,0x00
		,0x00,0x7f,0xe0,0x01,0xff,0xff,0xff,0xfe,0x00
		,0x00,0x7f,0xe0,0x01,0xff,0xff,0xff,0xfe,0x00
		,0x00,0x7f,0xe0,0x01,0xff,0xff,0xff,0xfe,0x00
		,0x00,0x3f,0xe0,0x01,0xff,0xff,0xff,0xfe,0x00
		,0x00,0x0f,0xf0,0x01,0xff,0xff,0xff,0xfe,0x00
		,0x00,0x03,0xf0,0x01,0xff,0xff,0xff,0xfc,0x00
		,0x00,0x01,0xf8,0x01,0xff,0xff,0xff,0xfc,0x00
		,0x00,0x01,0xfe,0x00,0xff,0xff,0xff,0xfc,0x00
		,0x00,0x00,0xff,0xe0,0x7f,0xff,0xff,0xfc,0x00
		,0x00,0x00,0xff,0xe0,0xff,0xff,0xff,0xf8,0x00
		,0x00,0x00,0xff,0xf0,0xff,0xff,0xff,0xf8,0x00
		,0x00,0x00,0xff,0xf3,0xff,0xff,0xff,0xf0,0x00
		,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xf0,0x00
		,0x00,0x01,0xff,0xff,0xff,0xff,0xff,0xf0,0x00
		,0x00,0x01,0xff,0xff,0xff,0xff,0xff,0xe0,0x00
		,0x00,0x01,0xff,0xff,0xff,0xff,0xff,0xe0,0x00
		,0x00,0x03,0xff,0xff,0xff,0xff,0xff,0xc0,0x00
		,0x00,0x07,0xff,0xff,0xff,0xff,0xff,0xc0,0x00
		,0x00,0x0f,0xff,0xff,0xff,0xff,0xff,0x80,0x00
		,0x00,0x1f,0xff,0xff,0xff,0xff,0xff,0x80,0x00
		,0x00,0x7f,0xff,0xff,0xff,0xff,0xff,0x00,0x00
		,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00
		,0x01,0xff,0xff,0xff,0xff,0xff,0xfe,0x00,0x00
		,0x07,0xff,0xff,0xff,0xff,0xff,0xfc,0x00,0x00
		,0x07,0xff,0xff,0xff,0xff,0xff,0xfc,0x00,0x00
		,0x0f,0xff,0xff,0xff,0xff,0xff,0xf8,0x00,0x00
		,0x1f,0xff,0xff,0xff,0xff,0xff,0xf0,0x00,0x00
		,0x1f,0xff,0xff,0xff,0xff,0xff,0xe0,0x00,0x00
		,0x1f,0xff,0xff,0xff,0xff,0xff,0xc0,0x00,0x00
		,0x1f,0xff,0xff,0xff,0xff,0xff,0x80,0x00,0x00
		,0x0f,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00
		,0x0f,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00
		,0x07,0xff,0xff,0xff,0xff,0xf8,0x00,0x00,0x00
		,0x03,0xff,0xff,0xff,0xff,0xe0,0x00,0x00,0x00
		,0x00,0xff,0xff,0xff,0xff,0xc0,0x00,0x00,0x00
		,0x00,0x7f,0xff,0xff,0xfe,0x00,0x00,0x00,0x00
		,0x00,0x1f,0xff,0xff,0xf8,0x00,0x00,0x00,0x00
		,0x00,0x03,0xff,0xff,0xc0,0x00,0x00,0x00,0x00
		,0x00,0x00,0x0f,0xf0,0x00,0x00,0x00,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00


};



volatile int32_t SysTickCnt=0;
volatile int cnt = 0;

// function declarations
static int GetUserButtonPressed(void);
static int GetTouchState (int *xCoord, int *yCoord);
void whatTouch(int* countT, int* NumbPIN, int* t,int* timedelay);
void AccessDeniedBlink(int Blinkcheck, int time);
void PINSetup();
void DisplayAccessDenied(int* Blinkcheck, int* time);
PIN_state AccessBlink(int Blinkcheck, int time);
PIN_state DisplayAccessed(int* Blinkcheck, int* time);
PIN_state unaccessed_NumbtoPIN(int* countT, int* NumbPIN,int* PIN_1, int* PIN_2, int* PIN_3, int* PIN_4);
PIN_state AccessCheckFunc(int PIN_1, int PIN_2, int PIN_3, int PIN_4);
PIN_state FalseWaitTimer(int* falsetimer, int* sec, float* counterfalse, int* time, int* trytime, int* t, int* countT, int* Blinkcheck, int* FalseInput,float SysTickCnt_Hold);
PIN_state CheckTrysRemaining(int* falsetimer, int* sec, float* counterfalse, int* time, int* trytime, int* t, int* countT, int* Blinkcheck, int* FalseInput);

void SysTick_Handler(void) {	// SysTick ISR: will be executed every 10ms (configured by SysTick_Config())
	SysTickCnt++;
	cnt++;
}




int main(void) {
	int t = 0;
	int Blinkcheck = 0;
	int time = 0;
	int countT = 0;
	int NumbPIN;
	int PIN_1,PIN_2,PIN_3,PIN_4;
	int FalseInput = 0;
	int trytime = 0;
	float counterfalse = 0;
	int falsetimer;
	int sec;
	int timedelay = 0;
	PIN_state state=unaccessed;

	RCC_ClocksTypeDef 	Clocks;
	static int32_t SysTickCnt_Hold;
	SysTickCnt = 0;
	SysTickCnt_Hold = 0;

	RCC_GetClocksFreq(&Clocks);

	SysTick_Config(Clocks.HCLK_Frequency/100 - 1);	// 100 Hz ( T=10ms)

	LCD_Init();	        // LCD initialization




	PINSetup();



	while (1) {
		// infinite (main) loop, cycle time 100ms, Timer tick  = 10ms

		if( SysTickCnt - SysTickCnt_Hold < 10 )		  // 10 * 10ms = 100ms
			continue;
		SysTickCnt_Hold += 10;

		// ToDo: start with your code here


		switch(state){
		case unaccessed:

			whatTouch(&countT,&NumbPIN,&t,&timedelay);

			state = unaccessed_NumbtoPIN(&countT,&NumbPIN,&PIN_1,&PIN_2,&PIN_3,&PIN_4);

			break;
		case accesscheck:
			state = AccessCheckFunc(PIN_1, PIN_2, PIN_3, PIN_4);

			break;
		case accessed:

			state = DisplayAccessed(&Blinkcheck, &time);

			break;
		case access_denied:

			DisplayAccessDenied(&Blinkcheck, &time);

			state = CheckTrysRemaining(&falsetimer, &sec, &counterfalse, &time, &trytime, &t, &countT, &Blinkcheck, &FalseInput);

			break;
		case access_wait:

			state = FalseWaitTimer(&falsetimer,&sec,&counterfalse,&time,&trytime,&t,&countT,&Blinkcheck,&FalseInput,SysTickCnt_Hold);

			break;
		case hub:
			LCD_SetPrintPosition(5, 3);
			LCD_DrawMonoPict(&myImage);
			printf("%i",sizeof(myImage));
			break;
		}

	}
} // main

PIN_state CheckTrysRemaining(int* falsetimer, int* sec, float* counterfalse, int* time, int* trytime, int* t, int* countT, int* Blinkcheck, int* FalseInput){
	PIN_state state = access_denied;

	if((*Blinkcheck>=3)&&(*FalseInput<3)){
		LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_RED);
		LCD_SetPrintPosition(8, 4);
		LCD_SetFont(&Font16x24);
		printf("%i try(s) ",(3-*FalseInput));
		LCD_SetPrintPosition(9, 3);
		printf("remaining");
		*trytime= *trytime + 1;

		if(*trytime>=50){
			*time=0;
			*trytime=0;
			*t=0;
			*countT=0;
			*Blinkcheck=0;
			*FalseInput= *FalseInput + 1;
			state = unaccessed;
			PINSetup();
		}
	}
	if((*Blinkcheck>=3)&&(*FalseInput==3)){
		*falsetimer=1;
		*sec=0;
		state = access_wait;
	}


	return state;
}

PIN_state FalseWaitTimer(int* falsetimer, int* sec, float* counterfalse, int* time, int* trytime, int* t, int* countT, int* Blinkcheck, int* FalseInput,float SysTickCnt_Hold){
	PIN_state state = access_wait;

	LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_RED);
	LCD_SetPrintPosition(8, 3);
	LCD_SetFont(&Font16x24);
	printf("FALSE PIN");
	LCD_SetPrintPosition(9, 1);
	printf("WAIT %i:%2.i min",*falsetimer,*sec);


	if((SysTickCnt - SysTickCnt_Hold < 10)){
		*counterfalse = *counterfalse + 0.1;												//um 0.1 erhöhen, für Timer
	}
	if(*sec<=0){
		*counterfalse = 0;
		*falsetimer = *falsetimer - 1;
	}
	else if(*sec<10){
		LCD_SetPrintPosition(9, 8);
		printf("0");
	}
	else if(*falsetimer<0){
		*time=0;
		*trytime=0;
		*t=0;
		*countT=0;
		*Blinkcheck=0;
		*FalseInput=0;
		state = unaccessed;
		PINSetup();
	}
	*sec = 60 - *counterfalse;


	return state;
}

PIN_state DisplayAccessed(int* Blinkcheck, int* time){
	PIN_state state = accessed;
	if(*time>=6){
		*time = 0;
		*Blinkcheck=*Blinkcheck + 1;
	}
	state = AccessBlink(*Blinkcheck, *time);

	*time=*time + 1;

	return state;
}

void DisplayAccessDenied(int* Blinkcheck, int* time){
	if(*time>=6){
		*time = 0;
		*Blinkcheck=*Blinkcheck + 1;
	}
	AccessDeniedBlink(*Blinkcheck, *time);

	*time=*time + 1;

}

void AccessDeniedBlink(int Blinkcheck, int time){
	if((time==0)&&(Blinkcheck<=2)){
		LCD_Clear(LCD_COLOR_BLACK);

		LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
		LCD_SetPrintPosition(6, 1);
		LCD_SetFont(&Font16x24);
		printf("ACCESS DENIED");

	}
	else if((time==3)&&(Blinkcheck<=2)){
		LCD_Clear(LCD_COLOR_RED);

		LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_RED);
		LCD_SetPrintPosition(6, 1);
		LCD_SetFont(&Font16x24);
		printf("ACCESS DENIED");

	}
}

PIN_state AccessBlink(int Blinkcheck, int time){
	PIN_state state = accessed;
	if((time==0)&&(Blinkcheck<=2)){
		LCD_Clear(LCD_COLOR_BLACK);

		LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
		LCD_SetPrintPosition(6, 3);
		LCD_SetFont(&Font16x24);
		printf("ACCESSED");

	}
	else if((time==3)&&(Blinkcheck<=2)){
		LCD_Clear(LCD_COLOR_GREEN);

		LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_GREEN);
		LCD_SetPrintPosition(6, 3);
		LCD_SetFont(&Font16x24);
		printf("ACCESSED");

	}
	else if((Blinkcheck==3)&&(time==5)){
		state = hub;
		LCD_Clear(LCD_COLOR_BLACK);
	}
	return state;
}

void whatTouch(int* countT, int* NumbPIN, int* t,int* timedelay){
	int x,y;

	if(GetTouchState(&x , &y)&&(*t<SysTickCnt)){

		LCD_SetColors(LCD_COLOR_GREY, LCD_COLOR_GREY);
		*timedelay = SysTickCnt + 30;
		LCD_SetFont(&Font16x24);

		if(x>=2&&x<=77&&y>=231&&y<=301){
			*countT= *countT + 1;
			*NumbPIN=1;
			LCD_DrawFullRect(3,20,74,69);
			LCD_SetPrintPosition(2, 2);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
			printf("1");
		}
		else if(x>=82&&x<=157&&y>=231&&y<=301){
			*countT= *countT + 1;
			*NumbPIN=2;
			LCD_DrawFullRect(83,20,74,69);
			LCD_SetPrintPosition(2, 7);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
			printf("2");
		}
		else if(x>=162&&x<=237&&y>=231&&y<=301){
			*countT= *countT + 1;
			*NumbPIN=3;
			LCD_DrawFullRect(163,20,74,69);
			LCD_SetPrintPosition(2, 12);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
			printf("3");
		}
		else if(x>=2&&x<=77&&y>=158&&y<=228){
			*countT= *countT + 1;
			*NumbPIN=4;
			LCD_DrawFullRect(3,93,74,69);
			LCD_SetPrintPosition(5, 2);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
			printf("4");
		}
		else if(x>=82&&x<=157&&y>=158&&y<=228){
			*countT= *countT + 1;
			*NumbPIN=5;
			LCD_DrawFullRect(83,93,74,69);
			LCD_SetPrintPosition(5, 7);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
			printf("5");
		}
		else if(x>=162&&x<=237&&y>=158&&y<=228){
			*countT= *countT + 1;
			*NumbPIN=6;
			LCD_DrawFullRect(163,93,74,69);
			LCD_SetPrintPosition(5, 12);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
			printf("6");
		}
		else if(x>=2&&x<=77&&y>=85&&y<=155){
			*countT= *countT + 1;
			*NumbPIN=7;
			LCD_DrawFullRect(3,165,74,69);
			LCD_SetPrintPosition(8, 2);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
			printf("7");
		}
		else if(x>=82&&x<=157&&y>=85&&y<=155){
			*countT= *countT + 1;
			*NumbPIN=8;
			LCD_DrawFullRect(83,165,74,69);
			LCD_SetPrintPosition(8, 7);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
			printf("8");
		}
		else if(x>=162&&x<=237&&y>=85&&y<=155){
			*countT= *countT + 1;
			*NumbPIN=9;
			LCD_DrawFullRect(163,165,74,69);
			LCD_SetPrintPosition(8, 12);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
			printf("9");
		}
		else if(x>=2&&x<=77&&y>=13&&y<=82){
			*countT= *countT + 1;
			*NumbPIN=10;
			LCD_DrawFullRect(3,238,74,69);
			LCD_SetPrintPosition(11, 2);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
			printf("*");
		}
		else if(x>=82&&x<=157&&y>=12&&y<=82){
			*countT= *countT + 1;
			*NumbPIN=0;
			LCD_DrawFullRect(83,238,74,69);
			LCD_SetPrintPosition(11, 7);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
			printf("0");
		}
		else if(x>=162&&x<=237&&y>=12&&y<=82){
			*countT= *countT + 1;
			*NumbPIN=11;
			LCD_DrawFullRect(163,238,74,69);
			LCD_SetPrintPosition(11, 12);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
			printf("#");
		}
		*t = SysTickCnt + 50;
	}



	if(*timedelay == SysTickCnt){
		LCD_SetFont(&Font16x24);
		LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_BLACK);
		if(*NumbPIN==1){
			LCD_DrawFullRect(3,20,74,69);
			LCD_SetPrintPosition(2, 2);
			LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			printf("1");
		}
		else if(*NumbPIN==2){
			LCD_DrawFullRect(83,20,74,69);
			LCD_SetPrintPosition(2, 7);
			LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			printf("2");
		}
		else if(*NumbPIN==3){
			LCD_DrawFullRect(163,20,74,69);
			LCD_SetPrintPosition(2, 12);
			LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			printf("3");
		}
		else if(*NumbPIN==4){
			LCD_DrawFullRect(3,93,74,69);
			LCD_SetPrintPosition(5, 2);
			LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			printf("4");
		}
		else if(*NumbPIN==5){
			LCD_DrawFullRect(83,93,74,69);
			LCD_SetPrintPosition(5, 7);
			LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			printf("5");
		}
		else if(*NumbPIN==6){
			LCD_DrawFullRect(163,93,74,69);
			LCD_SetPrintPosition(5, 12);
			LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			printf("6");
		}
		else if(*NumbPIN==7){
			LCD_DrawFullRect(3,165,74,69);
			LCD_SetPrintPosition(8, 2);
			LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			printf("7");
		}
		else if(*NumbPIN==8){
			LCD_DrawFullRect(83,165,74,69);
			LCD_SetPrintPosition(8, 7);
			LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			printf("8");
		}
		else if(*NumbPIN==9){
			LCD_DrawFullRect(163,165,74,69);
			LCD_SetPrintPosition(8, 12);
			LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			printf("9");
		}
		else if(*NumbPIN==10){
			LCD_DrawFullRect(3,238,74,69);
			LCD_SetPrintPosition(11, 2);
			LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			printf("*");
		}
		else if(*NumbPIN==0){
			LCD_DrawFullRect(83,238,74,69);
			LCD_SetPrintPosition(11, 7);
			LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			printf("0");
		}
		else if(*NumbPIN==11){
			LCD_DrawFullRect(163,238,74,69);
			LCD_SetPrintPosition(11, 12);
			LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			printf("#");
		}
	}

}

void PINSetup(){
	int i,o;


	LCD_Clear(LCD_COLOR_BLACK);
	LCD_SetFont(&Font16x24);
	LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK); // TextColor, BackColor

	LCD_SetPrintPosition(2, 0);
	printf("  1    2    3");
	LCD_SetPrintPosition(5, 0);
	printf("  4    5    6");
	LCD_SetPrintPosition(8, 0);
	printf("  7    8    9");
	LCD_SetPrintPosition(11, 0);
	printf("  *    0    #");

	LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);

	for(i=19;i<=300;i=i+73){
		for(o=2;o<=200;o=o+80){
			LCD_DrawRect(o,i,70,75);
		}
	}
	LCD_SetFont(&Font8x8);
	LCD_SetPrintPosition(0, 0);
	LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_BLACK);
	printf("     Copyright Buchberger  ");

}

PIN_state unaccessed_NumbtoPIN(int* countT, int* NumbPIN,int* PIN_1, int* PIN_2, int* PIN_3, int* PIN_4){
	PIN_state state=unaccessed;


	if(*countT==1){
		*PIN_1 = *NumbPIN;

	}
	else if(*countT==2){
		*PIN_2 = *NumbPIN;

	}
	else if(*countT==3){
		*PIN_3 = *NumbPIN;

	}
	else if(*countT==4){
		*PIN_4 = *NumbPIN;

		state = accesscheck;
	}

	return state;
}

PIN_state AccessCheckFunc(int PIN_1, int PIN_2, int PIN_3, int PIN_4){

	PIN_state state = accesscheck;

	if((PIN_1==2)&&(PIN_2==1)&&(PIN_3==1)&&(PIN_4==0)){
		state = accessed;
	}
	else{
		state= access_denied;
	}

	return state;
}




/**
 * Check if User Button has been pressed
 * @param none
 * @return 1 if rising edge has been detected
 */
static int GetUserButtonPressed(void) {
	return (GPIOA->IDR & 0x0001);
}

/**
 * Check if touch interface has been used
 * @param xCoord x coordinate of touch event in pixels
 * @param yCoord y coordinate of touch event in pixels
 * @return 1 if touch event has been detected
 */
static int GetTouchState (int* xCoord, int* yCoord) {

	static TP_STATE* TP_State;
	int touchclick = 0;

	TP_State = IOE_TP_GetState();
	if (TP_State->TouchDetected) {
		*xCoord = TP_State->X;
		*yCoord = TP_State->Y;
		touchclick = 1;
	}

	return touchclick;
}

