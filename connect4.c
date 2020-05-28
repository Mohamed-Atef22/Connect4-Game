// ******* Required Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Blue Nokia 5110
// ---------------
// Signal        (Nokia 5110) LaunchPad pin
// Reset          RST connected to PA7
// SSI0Fss        CE  connected to PA3
// Data/Command   DC  connected to PA6
// SSI0Tx         Din connected to PA5
// SSI0Clk        Clk connected to PA2
// 3.3V           Vcc power 3.3V
// back light     BL  not connected, consists of 4 white LEDs which draw ~80mA total
// Ground         Gnd ground

#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "Random.h"
#include "TExaS.h"
#include "Sound.h"
#include "ADC.h"

void PortF_Init(void);
void DisableInterrupts(void); 
void EnableInterrupts(void);  
void SysTick_Init(void);
void Timer2_Init(unsigned long period);
void Delay100ms(unsigned long count); // time delay in 0.1 seconds
unsigned long TimerCount;
unsigned long Semaphore;

int ADC0_rescale(int val);

void SysTick_Wait10ms(unsigned long delay); 
void SysTick_Wait(unsigned long delay);

void play(int pos , int player); 
void winnerMessage(int p); 

void startingScreen(void) ;
void display();


void Vertical_Check(void); 
void Horizontal_Check(void); 
void Diagonal_Check(void); 



const unsigned char circle[] ={
 0x42, 0x4D, 0x8E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x0F, 0xFF,
 0xF0, 0x00, 0x0F, 0xFF, 0xF0, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,

};

const unsigned char square[] ={
 0x42, 0x4D, 0x8E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xF0, 0x00,
 0x0F, 0x00, 0xF0, 0x00, 0x0F, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF,

};


const unsigned char cursor[] ={
 0x42, 0x4D, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x0F, 0xFF, 0xF0, 0x00, 0xFF, 0xFF,
 0xFF, 0x00, 0xFF,

};


const unsigned char playingBorder[] ={
 0x42, 0x4D, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x0F, 0xF0, 0x00,
 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,

};

const unsigned char nullImage[] ={0};


int p_ver[3] , p_hor[3] , p_dia[3];  
int p_win[3] ; 
 

int roww , roww2 , temp = 0 ; 	

unsigned long PA1,PA2; 
//int cursorPosition = 0 ; 
int ADCdata ; 
int cursorPosition ; 
int PA1_Flag = 1 ;
int PA2_Flag = 1 ; 
int col = 0 , row = 0  ; 
int screenArray[6][7] ;
int turn = 0 ; 
int nextcycle = 0 ; 
int game_mode = 0 ; 
int x = 0 ;



typedef struct gameObject {
	  const unsigned char* spirit ;
}GameObject;




GameObject Chips[3] = {{nullImage},{circle},{square}}; 

//int i , j ; 
int main(void){
	SysTick_Init() ; 
	PortF_Init(); 
	TExaS_Init(SSI0_Real_Nokia5110_Scope); 
	ADC0_Init();    
//Random_Init(1);
  Nokia5110_Init();
	EnableInterrupts();           
  Nokia5110_ClearBuffer();

  while(1){ 	
		
	 for(col=0; col < 7 ; col++){
		Nokia5110_PrintBMP(12 + (col * 8) , 8 , playingBorder , 0);
		for(row=0 ; row < 6 ; row++)
		{
			if (screenArray[row][col] > 0 )
			Nokia5110_PrintBMP(13 +(8*col) ,7 +(8*row), Chips[screenArray[row][col]].spirit, 0); 
			}
	 }
			cursorPosition = ADC0_rescale( ADC0_In());
		
			Nokia5110_PrintBMP( 13 + 8* cursorPosition , 4, cursor, 0);

				 
  		PA1 = (GPIO_PORTF_DATA_R&0x10);    
      PA2 = (GPIO_PORTF_DATA_R&0x01);     
		
		 nextcycle = !nextcycle ;
			
		 if(!PA1 && PA2 && !turn ){              
         if(PA1_Flag)
				 {
					 play(cursorPosition , 1); 
					 PA1_Flag = 0 ; 
					 turn = 1 ;
					 nextcycle = 0 ; 
					 
				 }
      }else{ 
					PA1_Flag = 1 ; 
			}		 
     if ( !PA2 && PA1 && turn  ){     
         if(PA2_Flag)
				 {
					 play(cursorPosition , 2); 
					 PA2_Flag = 0 ; 
					 turn = 0 ; 
				 }
      }else{ 
					PA2_Flag = 1 ; 
			}		
		
			display();			
			
			Vertical_Check();
			Horizontal_Check();
			Diagonal_Check() ; 
			 
			for (x = 1 ; x < 3 ; x++)
			{		
			if(p_win[x] == 1 ) 
			{ 
				winnerMessage(x);
			}
			}

}
}

void display(){
			Nokia5110_DisplayBuffer();
			Nokia5110_ClearBuffer();
		
}

void winnerMessage(int p){
	while(1){
				int x ; 
		for (x =0 ; x <2 ; x++)
			{	Nokia5110_Clear();		
				Nokia5110_SetCursor(1, 0);
				Nokia5110_OutString("Player");
				Nokia5110_SetCursor(7, 0);		 //
				Nokia5110_OutUDec(p);
				Nokia5110_SetCursor(1, 3);
				Nokia5110_OutString("wins !");
		
				SysTick_Wait10ms(40);
				Nokia5110_Clear();

			}
    }

}



void Vertical_Check(void){
					
		for(col=0; col < 7 ; col++){
		for(row=0 ; row < 6 ; row++)
		{
		if(row > 0 && screenArray[row-1][col] != screenArray[row][col] ) 
		{p_ver[screenArray[row-1][col]] = 0 ; }
		 p_ver[screenArray[row][col]]++ ; 
		
			if (p_ver[screenArray[row][col]] >= 4 )
			{p_win[screenArray[row][col]] = 1 ; }
		}		
		p_ver[0] = 0 ;
		p_ver[1] = 0 ;
		p_ver[2] = 0 ;
		
		if(p_win[1] || p_win[2] )
					break ; 
		}
}

void Horizontal_Check(void){

		for(row=0; row < 6 ; row++){
		for(col=0 ; col < 7 ; col++)
		{
		if(col > 0 && screenArray[row][col-1] != screenArray[row][col] ) 
		{p_hor[screenArray[row][col-1]] = 0 ; }
		 p_hor[screenArray[row][col]]++ ; 
		
	
		if(	p_hor[screenArray[row][col]] >= 4 )
				p_win[screenArray[row][col]] = 1 ; 
		
		
		}
		p_hor[0] = 0 ;
		p_hor[1] = 0 ;
		p_hor[2] = 0 ;
		
	if(p_win[1] || p_win[2] )
					break ; 	
		}
}

void Diagonal_Check(void){
	temp = 0; 
	
	for (row = 0 ; row <  6 ; row++)
	{ roww = row ;
		for(col=0; col < 7 ; col++){ 
			
			if (roww > 5  || roww <0) 
				continue ; 
			
			
			if ( temp != screenArray[roww][col])
			p_dia[temp] = 0 ; 
			
			
			p_dia[screenArray[roww][col]]++ ; 

			if (p_dia[screenArray[roww][col]] >= 4 )
			{p_win[screenArray[roww][col]] = 1 ;}		
			temp = screenArray[roww][col] ; 
			roww--;
		}
		p_dia[0] = 0 ;
		p_dia[1] = 0 ;
		p_dia[2] = 0 ;
		temp = 0 ; 
	
		if(p_win[1] || p_win[2] )
					break ; 
	}

		for (row = 0 ; row <  6 ; row++)
		{ roww = row ; 
		for(col= 6; col >= 1 ; col--){
		if (roww > 5  || roww <0) 
				continue ; 
			
		
			if (temp != screenArray[roww][col])
				p_dia[temp] = 0 ;
			
			p_dia[screenArray[roww][col]]++ ; 

			if (p_dia[screenArray[roww][col]] >= 4 )
			{p_win[screenArray[roww][col]] = 1 ; }
			
			temp = screenArray[roww][col] ;
			roww++;
			 
		}
		p_dia[0] = 0 ;
		p_dia[1] = 0 ;
		p_dia[2] = 0 ;
		temp = 0 ; 
		if(p_win[1] || p_win[2] )
					break ;
	}

	for (row = 5 ; row >= 0  ; row--)
	{	roww2 = row ; 
		for(col=0; col < 7 ; col++){
	if (roww2 > 5  || roww2 <0) 
				continue ; 
					
			if (temp != screenArray[roww][col])
				p_dia[temp] = 0 ; 
			
			
			p_dia[screenArray[roww2][col]]++ ; 

			if (p_dia[screenArray[roww2][col]] >= 4 )
			{p_win[screenArray[roww2][col]] = 1 ; }
			
			temp = screenArray[roww2][col] ;
			roww2++;
			 
		}
		p_dia[0] = 0 ;
		p_dia[1] = 0 ;
		p_dia[2] = 0 ;
		temp = 0 ; 
		if(p_win[1] || p_win[2] )
					break ;
	}
		
		
	for (row = 5 ; row >= 0  ; row--)
	{	roww2 = row ; 
	for(col= 6; col >= 1 ; col--){
	if (roww2 > 5  || roww2 <0) 
				continue ; 
			
		if (temp != screenArray[roww2][col])
			p_dia[temp] = 0 ; 

			
			p_dia[screenArray[roww2][col]]++ ; 

			if (p_dia[screenArray[roww2][col]] >= 4 )
			{p_win[screenArray[roww2][col]] = 1 ; }
			
			temp = screenArray[roww2][col] ;
			roww2--;
			 
		}
		p_dia[0] = 0 ;
		p_dia[1] = 0 ;
		p_dia[2] = 0 ;
		temp = 0 ; 
		if(p_win[1] || p_win[2] )
					break ;
	
	}
	
	}

void play(int pos , int player){
		
		for(row=5 ; row > 0 ; row--)
		{	
			if (screenArray[row][pos] > 0  ) 
					continue ;
			
			screenArray[row][pos] = player ; 
			Nokia5110_PrintBMP( 13 +(8 * pos) ,7 + (8 * row) , Chips[player].spirit, 0); // player ship middle bottom 
			break ;
		}
}



void SysTick_Init(void){

  NVIC_ST_CTRL_R = 0;              
	NVIC_ST_CTRL_R = 0x00000005;    	
}


void SysTick_Wait(unsigned long delay){

  NVIC_ST_RELOAD_R = delay-1;  
  NVIC_ST_CURRENT_R = 0;      

  while((NVIC_ST_CTRL_R&0x00010000)==0){ 

  }

}


void SysTick_Wait10ms(unsigned long delay){

  unsigned long i;
	
  for(i=0; i<delay; i++){

    SysTick_Wait(800000);  

  }

}



int ADC0_rescale(int val){

if (val < 585 )
	return 0 ; 
else if (val < 1170)
	return 1 ; 
else if (val < 1745)
	return 2 ; 
else if (val < 2330)
	return 3 ; 
else if (val < 2905)
	return 4 ; 
else if (val < 3490) 
	return 5 ; 
else if (val < 4100)
	return 6 ; 
else 
	return 6 ; 
}


void Timer2_Init(unsigned long period){ 

  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  delay = SYSCTL_RCGCTIMER_R;
  TimerCount = 0;
  Semaphore = 0;
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
}
void Timer2A_Handler(void){ 
  TIMER2_ICR_R = 0x00000001;   // acknowledge timer2A timeout
  TimerCount++;
  Semaphore = 1; // trigger
}


void Delay100ms(unsigned long count){unsigned long volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0        
}
