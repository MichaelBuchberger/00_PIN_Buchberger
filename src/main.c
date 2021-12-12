/**
 * AIIT STM32F4 Template
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
	beforelogout,
	touchdraw,
	waitforhub,
	snake,
}PIN_state;

typedef enum{
	start,
	stop,
	restart,
	left,
	right,
	up,
	down,

}SnakeDirection;

//NOTE: Creating the test.c file from test.bmp is done with this command:
// xxd -i test.bmp > src/test.c
extern unsigned char test_bmp[];

volatile int32_t SysTickCnt=0;
volatile int cnt = 0;

// function declarations
static int GetUserButtonPressed(void);
static int GetTouchState (int *xCoord, int *yCoord);
void whatTouch(int* countT, int* NumbPIN, int* t,int* time_rdelay);
void AccessDeniedBlink(int Blinkcheck, int time_r);
void PINSetup();
void HubSetup();
void SnakeSetup();
void ArrowLeft();
void ArrowRight();
void ArrowUp();
void ArrowDown();
void StopColorTable();
void TouchPrintSetup(int* color, int* time_r, int* r);
void DisplayAccessDenied(int* Blinkcheck, int* time_r);
PIN_state HubTouch(int* time_r);
PIN_state AccessBlink(int Blinkcheck, int time_r);
PIN_state DisplayAccessed(int* Blinkcheck, int* time_r);
PIN_state unaccessed_NumbtoPIN(int* countT, int* NumbPIN,int* PIN_1, int* PIN_2, int* PIN_3, int* PIN_4);
PIN_state AccessCheckFunc(int PIN_1, int PIN_2, int PIN_3, int PIN_4);
PIN_state FalseWaittimer(int* falsetime_rr, int* sec, float* counterfalse, int* time_r, int* trytime_r, int* t, int* countT, int* Blinkcheck, int* FalseInput,float SysTickCnt_Hold);
PIN_state CheckTrysRemaining(int* falsetime_rr, int* sec, float* counterfalse, int* time_r, int* trytime_r, int* t, int* countT, int* Blinkcheck, int* FalseInput);
PIN_state LogoutFunc(int* falsetime_rr, int* sec, float* counterfalse, int* time_r, int* trytime_r, int* t, int* countT, int* Blinkcheck, int* FalseInput);
PIN_state TouchPrintFunc(int* color, int* time_r, int* r);
PIN_state SnakeFunc(int* time_r, int* a, int* needapple, int* xApple, int* yApple, int* startx, int* starty, int* length, SnakeDirection* Snakestate);

void SysTick_Handler(void) {	// SysTick ISR: will be executed every 10ms (configured by SysTick_Config())
	SysTickCnt++;
	cnt++;
}

int xarr[1000];
int yarr[1000];


int main(void) {
	int t = 0;
	int Blinkcheck = 0;
	int time_r = 0;
	int countT = 0;
	int NumbPIN;
	int PIN_1,PIN_2,PIN_3,PIN_4;
	int FalseInput = 0;
	int trytime_r = 0;
	float counterfalse = 0;
	int falsetime_rr;
	int sec;
	int time_rdelay = 0;
	int r,color,xApple,yApple;
	int a = 0;
	int needapple = 1;
	int length = 0;
	int startx = 110, starty = 107;


	srand((unsigned)time(NULL));




	PIN_state state = unaccessed;
	SnakeDirection Snakestate = start;

	RCC_ClocksTypeDef 	Clocks;
	static int32_t SysTickCnt_Hold;
	SysTickCnt = 0;
	SysTickCnt_Hold = 0;

	RCC_GetClocksFreq(&Clocks);

	SysTick_Config(Clocks.HCLK_Frequency/100 - 1);	// 100 Hz ( T=10ms)

	LCD_Init();	        // LCD initialization


	PINSetup();



	while (1) {
		// infinite (main) loop, cycle time_r 100ms, time_rr tick  = 10ms

		if( SysTickCnt - SysTickCnt_Hold < 10 )		  // 10 * 10ms = 100ms
			continue;
		SysTickCnt_Hold += 10;

		// ToDo: start with your code here

continue;
		switch(state){
		case unaccessed:

			whatTouch(&countT,&NumbPIN,&t,&time_rdelay);

			state = unaccessed_NumbtoPIN(&countT,&NumbPIN,&PIN_1,&PIN_2,&PIN_3,&PIN_4);

			break;
		case accesscheck:
			state = AccessCheckFunc(PIN_1, PIN_2, PIN_3, PIN_4);

			break;
		case accessed:

			state = DisplayAccessed(&Blinkcheck, &time_r);

			break;
		case access_denied:

			DisplayAccessDenied(&Blinkcheck, &time_r);

			state = CheckTrysRemaining(&falsetime_rr, &sec, &counterfalse, &time_r, &trytime_r, &t, &countT, &Blinkcheck, &FalseInput);

			break;
		case access_wait:

			state = FalseWaittimer(&falsetime_rr,&sec,&counterfalse,&time_r,&trytime_r,&t,&countT,&Blinkcheck,&FalseInput,SysTickCnt_Hold);

			break;
		case hub:

			state = HubTouch(&time_r);



			break;
		case beforelogout:

			state = LogoutFunc(&falsetime_rr, &sec, &counterfalse, &time_r, &trytime_r, &t, &countT, &Blinkcheck, &FalseInput);

			break;
		case touchdraw:

			TouchPrintSetup(&color, &time_r, &r);

			state = TouchPrintFunc(&color, &time_r, &r);


			break;
		case waitforhub:

			if(time_r>=2){
				state = hub;
				HubSetup();
			}
			time_r++;

			break;
		case snake:

			state = SnakeFunc(&time_r, &a, &needapple, &xApple, &yApple, &startx, &starty, &length, &Snakestate);

			break;
		}

	}
} // main

PIN_state CheckTrysRemaining(int* falsetime_rr, int* sec, float* counterfalse, int* time_r, int* trytime_r, int* t, int* countT, int* Blinkcheck, int* FalseInput){
	PIN_state state = access_denied;

	if((*Blinkcheck>=3)&&(*FalseInput<3)){
		LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_RED);
		LCD_SetPrintPosition(8, 4);
		LCD_SetFont(&Font16x24);
		printf("%i try(s) ",(3-*FalseInput));
		LCD_SetPrintPosition(9, 3);
		printf("remaining");
		*trytime_r= *trytime_r + 1;

		if(*trytime_r>=50){
			*time_r=0;
			*trytime_r=0;
			*t=0;
			*countT=0;
			*Blinkcheck=0;
			*FalseInput= *FalseInput + 1;
			state = unaccessed;
			PINSetup();
		}
	}
	if((*Blinkcheck>=3)&&(*FalseInput==3)){
		*falsetime_rr=1;
		*sec=0;
		state = access_wait;
	}


	return state;
}

PIN_state FalseWaittimer(int* falsetime_rr, int* sec, float* counterfalse, int* time_r, int* trytime_r, int* t, int* countT, int* Blinkcheck, int* FalseInput,float SysTickCnt_Hold){
	PIN_state state = access_wait;

	LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_RED);
	LCD_SetPrintPosition(8, 3);
	LCD_SetFont(&Font16x24);
	printf("PIN FALSE");
	LCD_SetPrintPosition(9, 1);
	printf("WAIT %i:%2.i min",*falsetime_rr,*sec);


	if((SysTickCnt - SysTickCnt_Hold < 10)){
		*counterfalse = *counterfalse + 0.1;												//um 0.1 erhöhen, für time_r
	}

	if((*falsetime_rr==0)&&(*sec<=0)){														//muss hier geschrieben werden, sonnst kann dazu kommen dass
		*time_r=0;																			//alles null wird und keine Zeit abläuft
		*trytime_r=0;
		*t=0;
		*countT=0;
		*Blinkcheck=0;
		*FalseInput=0;
		state = unaccessed;
		PINSetup();
	}
	else if((*sec<=0)&&(*falsetime_rr>0)){													//wird beim umschalten nichtmehr erhöht
		*counterfalse = 0;
		*falsetime_rr = *falsetime_rr - 1;
	}
	else if((*sec<10)&&(state==access_wait)){												//oben wird auf unaccesed umgeschaltet, bei nichtabprüfung
		LCD_SetPrintPosition(9, 8);															//des states wird printf ausgeführt
		printf("0");
	}


	*sec = 60 - *counterfalse;


	return state;
}

PIN_state DisplayAccessed(int* Blinkcheck, int* time_r){
	PIN_state state = accessed;
	if(*time_r>=6){
		*time_r = 0;
		*Blinkcheck=*Blinkcheck + 1;
	}
	state = AccessBlink(*Blinkcheck, *time_r);

	*time_r=*time_r + 1;

	return state;
}

void DisplayAccessDenied(int* Blinkcheck, int* time_r){
	if(*time_r>=6){
		*time_r = 0;
		*Blinkcheck=*Blinkcheck + 1;
	}
	AccessDeniedBlink(*Blinkcheck, *time_r);

	*time_r=*time_r + 1;

}

void AccessDeniedBlink(int Blinkcheck, int time_r){
	if((time_r==0)&&(Blinkcheck<=2)){
		LCD_Clear(LCD_COLOR_BLACK);

		LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
		LCD_SetPrintPosition(6, 1);
		LCD_SetFont(&Font16x24);
		printf("ACCESS DENIED");

	}
	else if((time_r==3)&&(Blinkcheck<=2)){
		LCD_Clear(LCD_COLOR_RED);

		LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_RED);
		LCD_SetPrintPosition(6, 1);
		LCD_SetFont(&Font16x24);
		printf("ACCESS DENIED");

	}
}

PIN_state AccessBlink(int Blinkcheck, int time_r){
	PIN_state state = accessed;
	if((time_r==0)&&(Blinkcheck<=2)){
		LCD_Clear(LCD_COLOR_BLACK);

		LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
		LCD_SetPrintPosition(6, 3);
		LCD_SetFont(&Font16x24);
		printf("ACCESSED");

	}
	else if((time_r==3)&&(Blinkcheck<=2)){
		LCD_Clear(LCD_COLOR_GREEN);

		LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_GREEN);
		LCD_SetPrintPosition(6, 3);
		LCD_SetFont(&Font16x24);
		printf("ACCESSED");

	}
	else if((Blinkcheck==3)&&(time_r==5)){
		state = hub;
		HubSetup();


	}
	return state;
}

void whatTouch(int* countT, int* NumbPIN, int* t,int* time_rdelay){
	int x,y;

	if(GetTouchState(&x , &y)&&(*t<SysTickCnt)){

		LCD_SetColors(LCD_COLOR_GREY, LCD_COLOR_GREY);
		*time_rdelay = SysTickCnt + 30;
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
			LCD_DrawFullRect(3,166,74,69);
			LCD_SetPrintPosition(8, 2);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
			printf("7");
		}
		else if(x>=82&&x<=157&&y>=85&&y<=155){
			*countT= *countT + 1;
			*NumbPIN=8;
			LCD_DrawFullRect(83,166,74,69);
			LCD_SetPrintPosition(8, 7);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
			printf("8");
		}
		else if(x>=162&&x<=237&&y>=85&&y<=155){
			*countT= *countT + 1;
			*NumbPIN=9;
			LCD_DrawFullRect(163,166,74,69);
			LCD_SetPrintPosition(8, 12);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
			printf("9");
		}
		else if(x>=2&&x<=77&&y>=13&&y<=82){
			*countT= *countT + 1;
			*NumbPIN=10;
			LCD_DrawFullRect(3,239,74,69);
			LCD_SetPrintPosition(11, 2);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
			printf("*");
		}
		else if(x>=82&&x<=157&&y>=12&&y<=82){
			*countT= *countT + 1;
			*NumbPIN=0;
			LCD_DrawFullRect(83,239,74,69);
			LCD_SetPrintPosition(11, 7);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
			printf("0");
		}
		else if(x>=162&&x<=237&&y>=12&&y<=82){
			*countT= *countT + 1;
			*NumbPIN=11;
			LCD_DrawFullRect(163,239,74,69);
			LCD_SetPrintPosition(11, 12);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
			printf("#");
		}
		*t = SysTickCnt + 50;
	}



	if(*time_rdelay == SysTickCnt){
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
			LCD_DrawFullRect(3,166,74,69);
			LCD_SetPrintPosition(8, 2);
			LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			printf("7");
		}
		else if(*NumbPIN==8){
			LCD_DrawFullRect(83,166,74,69);
			LCD_SetPrintPosition(8, 7);
			LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			printf("8");
		}
		else if(*NumbPIN==9){
			LCD_DrawFullRect(163,166,74,69);
			LCD_SetPrintPosition(8, 12);
			LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			printf("9");
		}
		else if(*NumbPIN==10){
			LCD_DrawFullRect(3,239,74,69);
			LCD_SetPrintPosition(11, 2);
			LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			printf("*");
		}
		else if(*NumbPIN==0){
			LCD_DrawFullRect(83,239,74,69);
			LCD_SetPrintPosition(11, 7);
			LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			printf("0");
		}
		else if(*NumbPIN==11){
			LCD_DrawFullRect(163,239,74,69);
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

void HubSetup(){

		LCD_Clear(LCD_COLOR_BLACK);
		LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
		LCD_DrawRect(0,0,70,239);
		LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_RED);
		LCD_DrawFullRect(1, 1, 238, 69);
		LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_RED);
		LCD_SetFont(&Font16x24);
		LCD_SetPrintPosition(1, 4);
		printf("Logout");
		LCD_DrawRect(160,23,20,10);
		LCD_DrawRect(161,24,18,8);
		LCD_DrawUniLine(160, 43, 170, 43);
		LCD_DrawUniLine(160, 42, 170, 42);
		LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_RED);
		LCD_DrawFullRect(166,29,10,9);
		LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_RED);
		LCD_DrawUniLine(166, 32, 178, 32);
		LCD_DrawUniLine(166, 33, 178, 33);
		LCD_DrawUniLine(174, 28, 178, 32);
		LCD_DrawUniLine(174, 29, 177, 33);
		LCD_DrawUniLine(174, 36, 177, 32);
		LCD_DrawUniLine(174, 37, 178, 33);

		//LCD_WriteBMP(&myImage);
		//LCD_DrawRect(30, 200, 150, 150);
					//PICTURE?

		LCD_WriteBMP(test_bmp);


		LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
		LCD_DrawRect(70, 80, 100, 100);
		LCD_SetFont(&Font16x24);
		LCD_SetPrintPosition(5, 5);
		printf("Draw");

		LCD_DrawRect(70, 200, 100, 100);
		LCD_SetFont(&Font16x24);
		LCD_SetPrintPosition(10, 5);
		printf("Snake");

}

PIN_state HubTouch(int* time_r){
	int x,y;
	PIN_state state = hub;

	if(GetTouchState(&x, &y)){
		if(x>=0&&x<=240&&y>=250&&y<=320){
			*time_r = 0;
			state = beforelogout;

		}
		else if(x>=70&&x<=170&&y>=125&&y<=225){
			*time_r = 0;
			state = touchdraw;
		}
		else if(x>=70&&x<=170&&y>=10&&y<=110){
			*time_r = 0;
			state = snake;
			LCD_Clear(LCD_COLOR_WHITE);
			SnakeSetup();
		}
	}
	return state;
}

PIN_state LogoutFunc(int* falsetime_rr, int* sec, float* counterfalse, int* time_r, int* trytime_r, int* t, int* countT, int* Blinkcheck, int* FalseInput){
	PIN_state state = beforelogout;
	*time_r = *time_r + 1;

	if(*time_r==1){
		LCD_SetColors(LCD_COLOR_GREY, LCD_COLOR_GREY);
		LCD_DrawFullRect(1, 1, 238, 69);
		LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
		LCD_SetFont(&Font16x24);
		LCD_SetPrintPosition(1, 4);
		printf("Logout");

		LCD_DrawRect(160,23,20,10);
		LCD_DrawRect(161,24,18,8);
		LCD_DrawUniLine(160, 43, 170, 43);
		LCD_DrawUniLine(160, 42, 170, 42);
		LCD_SetColors(LCD_COLOR_GREY, LCD_COLOR_GREY);
		LCD_DrawFullRect(166,29,10,9);
		LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
		LCD_DrawUniLine(166, 32, 178, 32);
		LCD_DrawUniLine(166, 33, 178, 33);
		LCD_DrawUniLine(174, 28, 178, 32);
		LCD_DrawUniLine(174, 29, 177, 33);
		LCD_DrawUniLine(174, 36, 177, 32);
		LCD_DrawUniLine(174, 37, 178, 33);
	}
	else if(*time_r>=3){
		state = unaccessed;
		*time_r=0;
		*trytime_r=0;
		*t=0;
		*countT=0;
		*Blinkcheck=0;
		*FalseInput=0;
		PINSetup();
	}
	return state;
}

PIN_state TouchPrintFunc(int* color, int* time_r, int* r){
	PIN_state state = touchdraw;
	int x,y;


	if(GetTouchState(&x,&y)){

		if((x>=0&&x<=240&&y>=280&&y<=320)||(x>=200&&x<=240&&y>=0&&y<=320)){
			if(x>205&&x<235&&y>0&&y<30){
				StopColorTable();
				LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
				LCD_DrawRect(96,3,34,64);
				LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
				LCD_DrawCircle(220,300,17);

				LCD_SetColors(LCD_COLOR_YELLOW, LCD_COLOR_YELLOW);
				LCD_DrawFullCircle(170,20,4);
				LCD_DrawFullCircle(188,20,7);
				*color = 1;
			}
			else if(x>205&&x<235&&y>35&&y<65){
				StopColorTable();
				LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
				LCD_DrawRect(96,3,34,64);
				LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
				LCD_DrawCircle(220,265,17);

				LCD_SetColors(LCD_COLOR_GREEN, LCD_COLOR_GREEN);
				LCD_DrawFullCircle(170,20,4);
				LCD_DrawFullCircle(188,20,7);
				*color = 1;
			}
			else if(x>205&&x<235&&y>70&&y<100){
				StopColorTable();
				LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
				LCD_DrawRect(96,3,34,64);
				LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
				LCD_DrawCircle(220,230,17);

				LCD_SetColors(LCD_COLOR_MAGENTA, LCD_COLOR_MAGENTA);
				LCD_DrawFullCircle(170,20,4);
				LCD_DrawFullCircle(188,20,7);
				*color = 1;
			}
			else if(x>205&&x<235&&y>105&&y<135){
				StopColorTable();
				LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
				LCD_DrawRect(96,3,34,64);
				LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
				LCD_DrawCircle(220,195,17);

				LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_RED);
				LCD_DrawFullCircle(170,20,4);
				LCD_DrawFullCircle(188,20,7);
				*color = 1;
			}
			else if(x>205&&x<235&&y>140&&y<170){
				StopColorTable();
				LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
				LCD_DrawRect(96,3,34,64);
				LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
				LCD_DrawCircle(220,160,17);

				LCD_SetColors(LCD_COLOR_CYAN, LCD_COLOR_CYAN);
				LCD_DrawFullCircle(170,20,4);
				LCD_DrawFullCircle(188,20,7);
				*color = 1;
			}
			else if(x>205&&x<235&&y>175&&y<205){
				StopColorTable();
				LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
				LCD_DrawRect(96,3,34,64);
				LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
				LCD_DrawCircle(220,125,17);

				LCD_SetColors(LCD_COLOR_BLUE2, LCD_COLOR_BLUE2);
				LCD_DrawFullCircle(170,20,4);
				LCD_DrawFullCircle(188,20,7);
				*color = 1;
			}
			else if(x>205&&x<235&&y>210&&y<240){
				StopColorTable();
				LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
				LCD_DrawRect(96,3,34,64);
				LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
				LCD_DrawCircle(220,90,17);

				LCD_SetColors(LCD_COLOR_BLUE, LCD_COLOR_BLUE);
				LCD_DrawFullCircle(170,20,4);
				LCD_DrawFullCircle(188,20,7);
				*color = 1;
			}
			else if(x>205&&x<235&&y>245&&y<275){
				StopColorTable();
				LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
				LCD_DrawRect(96,3,34,64);
				LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
				LCD_DrawCircle(220,55,17);

				LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_BLACK);
				LCD_DrawFullCircle(170,20,4);
				LCD_DrawFullCircle(188,20,7);
				*color = 1;
			}
			else if(x>202&&x<238&&y>274&&y<308){

				state = waitforhub;
				*time_r = 0;
			}
			else if(x>5&&x<91&&y>275&&y<305){
				LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
				LCD_DrawFullRect(0, 41, 200, 279);
				LCD_DrawRect(96,3,34,64);
				StopColorTable();
				LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
				LCD_DrawFullCircle(170,20,4);
				LCD_DrawFullCircle(188,20,7);
				LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
				LCD_DrawCircle(170,20,4);
				LCD_DrawCircle(188,20,7);
				LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
				*color = 0;
			}
			else if(x>98&&x<158&&y>275&&y<305){
				StopColorTable();
				LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
				LCD_DrawFullCircle(170,20,4);
				LCD_DrawFullCircle(188,20,7);
				LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
				LCD_DrawCircle(170,20,4);
				LCD_DrawCircle(188,20,7);
				LCD_DrawRect(96,3,34,64);
				LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);

				*color = 1;
			}
			else if(x>160&&x<=177&&y>275&&y<305){
				*r = 4;
				LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
				LCD_DrawCircle(170,20,6);
				StopColorTable();
				LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
				LCD_DrawRect(96,3,34,64);
				LCD_DrawCircle(188,20,9);
				LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
				LCD_DrawFullCircle(170,20,4);
				LCD_DrawFullCircle(188,20,7);
				LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
				LCD_DrawCircle(170,20,4);
				LCD_DrawCircle(188,20,7);
				LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
				*color = 0;

			}
			else if(x>177&&x<200&&y>275&&y<305){
				*r = 7;
				LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
				LCD_DrawCircle(188,20,9);
				StopColorTable();
				LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
				LCD_DrawRect(96,3,34,64);
				LCD_DrawCircle(170,20,6);
				LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
				LCD_DrawFullCircle(170,20,4);
				LCD_DrawFullCircle(188,20,7);
				LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
				LCD_DrawCircle(170,20,4);
				LCD_DrawCircle(188,20,7);
				LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
				*color = 0;

			}

		}
		else if(x>=7&&x<193&&y>=0&&y<258&&*color==1){
			LCD_DrawFullCircle(x,305-y,*r);
		}



	}
	return state;
}

void StopColorTable(){

	LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
	LCD_DrawCircle(220,300,17);
	LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
	LCD_DrawCircle(220,265,17);
	LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
	LCD_DrawCircle(220,230,17);
	LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
	LCD_DrawCircle(220,195,17);
	LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
	LCD_DrawCircle(220,160,17);
	LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
	LCD_DrawCircle(220,125,17);
	LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
	LCD_DrawCircle(220,90,17);
	LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
	LCD_DrawCircle(220,55,17);
	LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
	LCD_DrawRect(202,2,36,35);
}

void TouchPrintSetup(int* color, int* time_r, int* r){
	if(*time_r==1){
		LCD_Clear(LCD_COLOR_WHITE);
		LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
		LCD_DrawUniLine(0,40,200,40);
		LCD_DrawUniLine(200,40,200,320);
		LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_RED);
		LCD_DrawFullRect(203,3,34,35);
		LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
		LCD_DrawRect(202,2,36,35);
		LCD_DrawRect(203,3,34,33);
		LCD_DrawUniLine(202,2,237,38);
		LCD_DrawUniLine(202,3,237,37);
		LCD_DrawUniLine(237,2,202,38);
		LCD_DrawUniLine(237,3,202,37);

		LCD_DrawRect(98,5,30,60);
		LCD_DrawRect(5,5,30,86);
		LCD_SetFont(&Font8x8);
		LCD_SetPrintPosition(2, 1);
		printf("Remove all");
		LCD_SetPrintPosition(2, 13);
		printf("Rubber");


		LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
		LCD_DrawFullCircle(170,20,4);
		LCD_DrawFullCircle(188,20,7);
		LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
		LCD_DrawCircle(170,20,4);
		LCD_DrawCircle(188,20,7);
		LCD_DrawCircle(170,20,6);
		*r = 4;
		*color = 0;

		LCD_SetColors(LCD_COLOR_YELLOW, LCD_COLOR_YELLOW);
		LCD_DrawFullCircle(220,300, 15);
		LCD_SetColors(LCD_COLOR_GREEN, LCD_COLOR_GREEN);
		LCD_DrawFullCircle(220,265, 15);
		LCD_SetColors(LCD_COLOR_MAGENTA, LCD_COLOR_MAGENTA);
		LCD_DrawFullCircle(220,230, 15);
		LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_RED);
		LCD_DrawFullCircle(220,195, 15);
		LCD_SetColors(LCD_COLOR_CYAN, LCD_COLOR_CYAN);
		LCD_DrawFullCircle(220,160, 15);
		LCD_SetColors(LCD_COLOR_BLUE2, LCD_COLOR_BLUE2);
		LCD_DrawFullCircle(220,125, 15);
		LCD_SetColors(LCD_COLOR_BLUE, LCD_COLOR_BLUE);
		LCD_DrawFullCircle(220,90, 15);
		LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_BLACK);
		LCD_DrawFullCircle(220,55, 15);



		LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);

	}
	*time_r = *time_r + 1;
}

void SnakeSetup(){

	LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_BLACK);
	LCD_DrawRect(40, 40, 241, 160);
	LCD_DrawUniLine(0, 40, 240, 40);
	LCD_DrawUniLine(0, 281, 240, 281);
	//Close Window
	LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_RED);
	LCD_DrawFullRect(202, 0, 38, 40);
	LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_BLACK);
	LCD_DrawRect(200, 0, 40, 39);
	LCD_DrawRect(201, 1, 38, 37);
	LCD_DrawUniLine(239, 0, 239, 40);
	LCD_DrawUniLine(238, 0, 238, 40);
	LCD_DrawUniLine(202,2,237,38);
	LCD_DrawUniLine(202,3,237,37);
	LCD_DrawUniLine(237,2,202,38);
	LCD_DrawUniLine(237,3,202,37);
	//PFEIL Links
	LCD_DrawFullRect(5, 158, 30, 4);
	LCD_DrawUniLine(5, 161, 12, 154);
	LCD_DrawUniLine(5, 160, 12, 153);
	LCD_DrawUniLine(5, 159, 12, 152);
	LCD_DrawUniLine(5, 158, 12, 151);
	LCD_DrawUniLine(5, 161, 12, 168);
	LCD_DrawUniLine(5, 160, 12, 167);
	LCD_DrawUniLine(5, 159, 12, 166);
	LCD_DrawUniLine(5, 158, 12, 165);
	//PFEIL Rechts
	LCD_DrawFullRect(205, 158, 30, 4);
	LCD_DrawUniLine(235, 161, 228, 154);
	LCD_DrawUniLine(235, 160, 228, 153);
	LCD_DrawUniLine(235, 159, 228, 152);
	LCD_DrawUniLine(235, 158, 228, 151);
	LCD_DrawUniLine(235, 161, 228, 168);
	LCD_DrawUniLine(235, 160, 228, 167);
	LCD_DrawUniLine(235, 159, 228, 166);
	LCD_DrawUniLine(235, 158, 228, 165);
	//PFEIL Oben
	LCD_DrawFullRect(118, 5, 4, 30);
	LCD_DrawUniLine(118, 5, 125, 12);
	LCD_DrawUniLine(119, 5, 126, 12);
	LCD_DrawUniLine(120, 5, 127, 12);
	LCD_DrawUniLine(121, 5, 128, 12);
	LCD_DrawUniLine(118, 5, 111, 12);
	LCD_DrawUniLine(119, 5, 112, 12);
	LCD_DrawUniLine(120, 5, 113, 12);
	LCD_DrawUniLine(121, 5, 114, 12);
	//PFEIL Unten
	LCD_DrawFullRect(118, 285, 4, 30);
	LCD_DrawUniLine(118, 315, 125, 308);
	LCD_DrawUniLine(119, 315, 126, 308);
	LCD_DrawUniLine(120, 315, 127, 308);
	LCD_DrawUniLine(121, 315, 128, 308);
	LCD_DrawUniLine(118, 315, 111, 308);
	LCD_DrawUniLine(119, 315, 112, 308);
	LCD_DrawUniLine(120, 315, 113, 308);
	LCD_DrawUniLine(121, 315, 114, 308);

}

PIN_state SnakeFunc(int* time_r, int* a, int* needapple, int* xApple, int* yApple, int* startx, int* starty, int* length, SnakeDirection* Snakestate){
	PIN_state state = snake;
	int x,y,n;



	if(GetTouchState(&x, &y) && (*Snakestate != stop)&& (*Snakestate != restart)){
		if(x>=0&&x<200&&y>=280&&y<=320&&(*Snakestate!=down)){										//drücken wohin
			//up
			*Snakestate = up;
			*time_r = 2;
			LCD_SetColors(LCD_COLOR_GREEN, LCD_COLOR_GREEN);
			LCD_DrawFullRect(0,0,200,40);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
			ArrowUp();
		}
		else if(x>=0&&x<=240&&y>=0&&y<=40&&(*Snakestate!=up)){
			//down
			*Snakestate = down;
			*time_r = 2;
			LCD_SetColors(LCD_COLOR_GREEN, LCD_COLOR_GREEN);
			LCD_DrawFullRect(0,282,240,38);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
			ArrowDown();
		}
		else if(x>=0&&x<=40&&y>40&&y<280&&(*Snakestate!=right)){
			//left
			*Snakestate = left;
			*time_r = 2;
			LCD_SetColors(LCD_COLOR_GREEN, LCD_COLOR_GREEN);
			LCD_DrawFullRect(0,41,40,240);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
			ArrowLeft();
		}
		else if(x>=200&&x<=240&&y>40&&y<280&&(*Snakestate!=left)){
			//right
			*Snakestate = right;
			*time_r = 2;
			LCD_SetColors(LCD_COLOR_GREEN, LCD_COLOR_GREEN);
			LCD_DrawFullRect(201,41,39,240);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
			ArrowRight();
		}
		else if(x>=200&&x<=240&&y>=280&&y<=320){
			state = waitforhub;
			*Snakestate = start;
			*time_r = 0;
			*a=0;
			*needapple = 1;
			*length = 0;
			*xApple = 0;
			*yApple = 0;
			*startx = 110;
			*starty = 107;

		}
	}

	if((*Snakestate != stop) && (*Snakestate != restart)&&(*time_r>=5)){

		if(*Snakestate == start){
			LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_RED);
			LCD_DrawFullRect(*startx,*starty, 21, 20);
		}
		else if((*Snakestate == down)&&(*Snakestate != up)){													//für runter
			*starty = *starty + 22;
			LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
			LCD_DrawFullRect(201,41,39,240);
			LCD_DrawFullRect(0,41,40,240);
			LCD_DrawFullRect(0,282,240,38);
			LCD_DrawFullRect(0,0,200,40);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
			ArrowDown();
			ArrowUp();
			ArrowRight();
			ArrowLeft();


		}
		else if((*Snakestate == up)&&(*Snakestate != down)){													//für rauf
			*starty = *starty - 22;
			LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
			LCD_DrawFullRect(201,41,39,240);
			LCD_DrawFullRect(0,41,40,240);
			LCD_DrawFullRect(0,282,240,38);
			LCD_DrawFullRect(0,0,200,40);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
			ArrowDown();
			ArrowUp();
			ArrowRight();
			ArrowLeft();
		}
		else if((*Snakestate == right)&&(*Snakestate != left)){													//für rechts
			*startx = *startx + 23;
			LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
			LCD_DrawFullRect(201,41,39,240);
			LCD_DrawFullRect(0,41,40,240);
			LCD_DrawFullRect(0,282,240,38);
			LCD_DrawFullRect(0,0,200,40);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
			ArrowDown();
			ArrowUp();
			ArrowRight();
			ArrowLeft();
		}
		else if((*Snakestate == left)&&(*Snakestate != right)){													//für links
			*startx = *startx - 23;
			LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);
			LCD_DrawFullRect(201,41,39,240);
			LCD_DrawFullRect(0,41,40,240);
			LCD_DrawFullRect(0,282,240,38);
			LCD_DrawFullRect(0,0,200,40);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
			ArrowDown();
			ArrowUp();
			ArrowRight();
			ArrowLeft();
		}

		if(*startx>=41&&*startx<=179&&*starty>=41&&*starty<=261){
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_BLACK);
			LCD_DrawFullRect(*startx, *starty, 21, 20);
			xarr[*a] = *startx;																				//für merken wo langgeschlängel
			yarr[*a] = *starty;
			if(*length>=1){																					//apfel schwarz dass kopf bleibt
				LCD_SetColors(LCD_COLOR_GREEN, LCD_COLOR_GREEN);											//wird sowieo im nächsten kästchen
				LCD_DrawFullRect(xarr[*a-1], yarr[*a-1], 21, 20);												//grün übermalt
			}

			for(n=0;n<=*length;n++){
				if((*startx==xarr[*a-(1+n)])&&(*starty==yarr[*a-(1+n)])&&*length!=0){
					*Snakestate = stop;																		//wenn in sich selbst fahren dann stop
				}
				if((*xApple==xarr[*a-(1+n)])&&(*yApple==yarr[*a-(1+n)])){										//bei apfel in schlange zeichnen neu apfel
					*needapple = 1;
					*xApple = 33;
					*yApple = 33;
				}
			}

		}
		else{
			*Snakestate = stop;																				//wenn wand dann stop
		}


		if(*Snakestate!=start && *Snakestate!=stop && *a!=0){													//bei weiterschlängeln
			LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_WHITE);												//letzts schwanzteil löschen
			LCD_DrawFullRect(xarr[*a-(1+*length)], yarr[*a-(1+*length)], 21, 20);
		}



		if((*xApple==41 || *xApple==64 || *xApple==87 || *xApple==110 || *xApple==133 || *xApple==156 || *xApple==179)&&(*yApple==41 || *yApple==63 || *yApple==85 || *yApple==107 || *yApple==129 || *yApple==151 || *yApple==173 || *yApple==195 || *yApple==217 || *yApple==239 || *yApple==261)){
			LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_RED);													//Apfelzeichnen
			LCD_DrawFullRect(*xApple, *yApple, 21, 20);
		}

		if((*xApple == *startx)&&(*yApple == *starty)){
			*length = *length + 1;
			*needapple = 1;
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_BLACK);												//Apfel unsichtbar machen
			LCD_DrawFullRect(*xApple, *yApple, 21, 20);
		}


		*a = *a + 1;
		*time_r = 0;
	}
	else if(*Snakestate == stop){																			//wenn stop dann gameover
		if(*time_r<=3){
			LCD_SetColors(LCD_COLOR_GREY, LCD_COLOR_GREY);
			LCD_DrawFullRect(6,121, 229, 79);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
			LCD_DrawRect(5,120, 80, 230);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
			LCD_SetPrintPosition(6, 3);
			LCD_SetFont(&Font16x24);
			printf("GAME OVER");
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREEN);
			LCD_DrawRect(59,164, 28, 121);
			LCD_SetColors(LCD_COLOR_GREEN, LCD_COLOR_GREEN);
			LCD_DrawFullRect(60,165, 120, 27);
			LCD_SetPrintPosition(7, 5);
			LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREEN);
			printf("RETRY");
		}
		if(GetTouchState(&x, &y)){
			if(x>=200&&x<=240&&y>=280&&y<=320){

				state = waitforhub;
				*Snakestate = start;
				*time_r = 0;
				*a=0;
				*needapple = 1;
				*length = 0;
				*xApple = 0;
				*yApple = 0;
				*startx = 110;
				*starty = 107;
			}
			else if(x>=50&&x<=190&&y>=105&&y<=155){
				LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
				LCD_DrawRect(59,164, 28, 121);
				LCD_SetColors(LCD_COLOR_GREY, LCD_COLOR_GREY);
				LCD_DrawFullRect(60,165, 120, 27);
				LCD_SetPrintPosition(7, 5);
				LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_GREY);
				printf("RETRY");
				*Snakestate = restart;
				*time_r = 0;

			}
		}

	}
	else if((*Snakestate == restart)&&(*time_r>=5)){

		*Snakestate = start;
		*time_r = 0;
		LCD_Clear(LCD_COLOR_WHITE);
		SnakeSetup();
		*a=0;
		*needapple = 1;
		*length = 0;
		*xApple = 0;
		*yApple = 0;
		*startx = 110;
		*starty = 107;
	}
	if((*Snakestate != stop) &&((*needapple==1)||!(*xApple==41 || *xApple==64 || *xApple==87 || *xApple==110 || *xApple==133 || *xApple==156 || *xApple==179))){
		*xApple = ((rand()%((199+1)-41))+41);																			//Bedingungen und berechnungen für Apfel
	}																													//in x-Richtung darunter y-Richtung

	if((*Snakestate != stop) && ((*needapple==1)||!(*yApple==41 || *yApple==63 || *yApple==85 || *yApple==107 || *yApple==129 || *yApple==151 || *yApple==173 || *yApple==195 || *yApple==217 || *yApple==239 || *yApple==261))){
		*yApple = ((rand()%((279+1)-41))+41);
		*needapple = 0;
	}



	*time_r = *time_r + 1;
	return state;
}

void ArrowLeft(){
	LCD_DrawFullRect(5, 158, 30, 4);
	LCD_DrawUniLine(5, 161, 12, 154);
	LCD_DrawUniLine(5, 160, 12, 153);
	LCD_DrawUniLine(5, 159, 12, 152);
	LCD_DrawUniLine(5, 158, 12, 151);
	LCD_DrawUniLine(5, 161, 12, 168);
	LCD_DrawUniLine(5, 160, 12, 167);
	LCD_DrawUniLine(5, 159, 12, 166);
	LCD_DrawUniLine(5, 158, 12, 165);
}

void ArrowRight(){
	LCD_DrawFullRect(205, 158, 30, 4);
	LCD_DrawUniLine(235, 161, 228, 154);
	LCD_DrawUniLine(235, 160, 228, 153);
	LCD_DrawUniLine(235, 159, 228, 152);
	LCD_DrawUniLine(235, 158, 228, 151);
	LCD_DrawUniLine(235, 161, 228, 168);
	LCD_DrawUniLine(235, 160, 228, 167);
	LCD_DrawUniLine(235, 159, 228, 166);
	LCD_DrawUniLine(235, 158, 228, 165);
}

void ArrowUp(){
	LCD_DrawFullRect(118, 5, 4, 30);
	LCD_DrawUniLine(118, 5, 125, 12);
	LCD_DrawUniLine(119, 5, 126, 12);
	LCD_DrawUniLine(120, 5, 127, 12);
	LCD_DrawUniLine(121, 5, 128, 12);
	LCD_DrawUniLine(118, 5, 111, 12);
	LCD_DrawUniLine(119, 5, 112, 12);
	LCD_DrawUniLine(120, 5, 113, 12);
	LCD_DrawUniLine(121, 5, 114, 12);
}

void ArrowDown(){
	LCD_DrawFullRect(118, 285, 4, 30);
	LCD_DrawUniLine(118, 315, 125, 308);
	LCD_DrawUniLine(119, 315, 126, 308);
	LCD_DrawUniLine(120, 315, 127, 308);
	LCD_DrawUniLine(121, 315, 128, 308);
	LCD_DrawUniLine(118, 315, 111, 308);
	LCD_DrawUniLine(119, 315, 112, 308);
	LCD_DrawUniLine(120, 315, 113, 308);
	LCD_DrawUniLine(121, 315, 114, 308);
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
