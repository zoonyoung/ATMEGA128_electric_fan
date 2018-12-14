//===========================================================================
//	elctric_fan_project
//===========================================================================

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define EX_CLCD_DATA     (*(volatile unsigned char*) 0x8000)
#define EX_CLCD_CONTROL     (*(volatile unsigned char*) 0x8001)
#define _CLCD
#define		EX_LED	 		(*(volatile unsigned char*) 0x8008)
#define		EX_STEPPER		(*(volatile unsigned char*) 0x8009)
#define EX_FND_DATA     (*(volatile unsigned char*) 0x8002)
#define EX_FND_SELECT   (*(volatile unsigned char*) 0x8003)

float speed;
int play;
int TIMER;
int count;
int TIME;
int seconds;

unsigned  char                fndDigit[4] = { 0, 0, 0, 0 };


void updateClock(int secs);
static void updateFnd();
void updateFndDigits(int i);


void
initDevices(void)
{
	cli();
	MCUCR = 0x80;
	sei();
}

void
delay(int n)
{
	for (volatile int i = 0; i < n; i++) {
		for (volatile int j = 0; j < 600; j++);
	}
}

static unsigned char	smStepPhase1[] = {
	0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x09
};

#define N1STEPS (sizeof(smStepPhase1) / sizeof(unsigned char))
int
main(void)
{
	int i, step = 0;
	int j;
	int TIMER = 0;
	char LED=0x00;
	EX_LED=LED;
	initDevices();
	initLcd();
	while (1) {
        printLcd(1,1,"Sun_Pung_Gi!");
		printLcd(2,1,"I'm Ready!");
		fndDigit[4] = 0;
		fndDigit[3] = 0;
		fndDigit[2] = 0;
		fndDigit[1] = 0;
		updateFnd();

		if (PINB & 0x20) play = 1;
		if (PINB & 0x40) play = 1;
		if (PINB & 0x80) play = 1;
		while (play) {
			updateFnd();
			if (PINB & 0x20) {
				speed = 1;
				LED = 0x20;
				TIMER = 0;
				initLcd();
        		printLcd(1,1,"Sun_Pung_Gi!");
				printLcd(2,1,"quick quick!!");
			}
			if (PINB & 0x40) {
				speed = 2;
				LED = 0x40;
				TIMER = 0;
				initLcd();
        		printLcd(1,1,"Sun_Pung_Gi!");
				printLcd(2,1,"quick!");
			}
			if (PINB & 0x80) {
				speed = 4;
				LED = 0x80;
				TIMER = 0;
				initLcd();
        		printLcd(1,1,"Sun_Pung_Gi!");
				printLcd(2,1,"slow!");
			}
			if (PINB & 0x08) {
				TIMER = 1;
				LED = (LED | 0x08);
				TIME = 33000.0;
				seconds = 30;
				fndDigit[0]=0;
				fndDigit[1]=0;			
				fndDigit[2]=3;
				fndDigit[3]=0;
			}
			if (PINB & 0x04) {
				TIMER = 1;
				LED = (LED | 0x04);
				TIME = 23000.0;
				seconds = 20;
				fndDigit[0]=0;
				fndDigit[1]=0;			
				fndDigit[2]=2;
				fndDigit[3]=0;
			}
			if (PINB & 0x02) {
				TIMER = 1;
				LED = (LED | 0x02);
				TIME = 13000.0;
				seconds = 10;
				fndDigit[0]=0;
				fndDigit[1]=0;			
				fndDigit[2]=1;
				fndDigit[3]=0;
			}

			if (PINB & 0x10) {
				EX_LED = 0x00;
				play = 0;
				TIMER = 0;
				initLcd();
        		printLcd(1,1,"Sun_Pung_Gi!");
				printLcd(2,1,"Good_BYE");
				delay(2300);
				break;
			}
			EX_LED=LED;

			if (TIMER == 0) {
				EX_STEPPER = smStepPhase1[step];
				step = (step + 1) % N1STEPS;
				delay(speed);
			}
			if (TIMER == 1) {
				initLcd();
        		printLcd(1,1,"Sun_Pung_Gi!");
				printLcd(2,1,"Reserved!");
				count = TIME / speed;
				int fndValue;
				fndValue=seconds*10;
				int m=count/fndValue;
				for (j = 1; j < count+1; j++) {
					updateFnd();
					if ((j%m)==0){
						updateFndDigits(fndValue);
						fndValue--;
					}
					EX_STEPPER = smStepPhase1[step];
					step = (step + 1) % N1STEPS;
					delay(speed);
					if (PINB & 0x10) {
						EX_LED = 0x00;
						play = 0;
						TIMER = 0;
						break;
					}
					if (PINB & 0x20) {
						speed = 1;
						EX_LED = 0x20;
						TIMER = 0;
						play = 0;
						break;
					}
					if (PINB & 0x40) {
						speed = 2;
						EX_LED = 0x40;
						TIMER = 0;
						play = 0;
						break;
					}
					if (PINB & 0x80) {
						speed = 4;
						EX_LED = 0x80;
						TIMER = 0;
						play = 0;
						break;
					}
				}
				EX_LED = 0x00;
				play = 0;
			}
		}
	}
}


unsigned  char                fndPosition = 0;


static  const unsigned  char  fndSegment[]  = {
  0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7c, 0x07, 0x7f, 0x67
};


static void
updateFnd(void)
{
  EX_FND_SELECT = 0x0f;   
  EX_FND_DATA = fndSegment[fndDigit[fndPosition]];
  EX_FND_SELECT = ~(0x01 << fndPosition);   

  fndPosition++;
  fndPosition %= 4;
}

void
updateFndDigits(int i)
{
  int fndValue=i;
  fndValue=i;
  fndValue  %=  10000;	
  fndDigit[0] = fndValue % 100000 / 1000;
  fndDigit[1] = fndValue % 10000 / 1000;
  fndDigit[2] = fndValue % 1000 / 100;
  fndDigit[3] = fndValue % 100 / 10;
}

static char clcdControl = 0;

#define LCD_EN_ON (EX_CLCD_CONTROL = (clcdControl |= 0x04))
#define LCD_EN_OFF (EX_CLCD_CONTROL = (clcdControl &= ~0x04))
#define LCD_RW_ON (EX_CLCD_CONTROL = (clcdControl |= 0x02))
#define LCD_RW_OFF (EX_CLCD_CONTROL = (clcdControl &= ~0x02))
#define LCD_RS_ON (EX_CLCD_CONTROL = (clcdControl |= 0x01))
#define LCD_RS_OFF (EX_CLCD_CONTROL = (clcdControl &= ~0x01))




void
writeData(char value)
{
  LCD_EN_OFF;
  delay(1);
  LCD_RS_ON;
  delay(1);
  LCD_RW_OFF;
  delay(1);
  LCD_EN_ON;
  delay(1);
  EX_CLCD_DATA = value;
  delay(1);
  LCD_EN_OFF;
  delay(1);
}



void
writeCommand(char value)
{
  LCD_EN_OFF;
  delay(1);
  LCD_RS_OFF;
  delay(1);
  LCD_RW_OFF;
  delay(1);
  LCD_EN_ON;
  delay(1);
  EX_CLCD_DATA = value;
  delay(1);
  LCD_EN_OFF;
  delay(1);
 }

void
initLcd(void)
{
   writeCommand(0x38); 
   writeCommand(0x0e);  
   writeCommand(0x06);  
   writeCommand(0x01);  
   delay(100);
   writeCommand(0x02);  
   delay(100);
}


 void
 gotoLcdXY(unsigned char x, unsigned char y)
 {
   switch(y){
      case 1:
        writeCommand(0x80 + x + 1);
        break;
      case 2:
        writeCommand(0xc0 + x + 1);
        break;
   }
 }
  void
 writeString(char *str)
 {
   while(*str)
    writeData(*str++);
 }

void
printLcd(int row, int col, char *str)
{
  gotoLcdXY(col,row);
  writeString(str);
}
