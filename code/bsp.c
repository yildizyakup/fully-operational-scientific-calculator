#include "bsp.h"
#include "stm32g0xx.h"

int pressCounter;

float displayNumber1;
float displayNumber2;
float displayNumberFinal;


int displayNumber1_d1;
int displayNumber1_d2;
int displayNumber1_d3;
int displayNumber1_d4;

int displayNumber2_d1;
int displayNumber2_d2;
int displayNumber2_d3;
int displayNumber2_d4;

int preventBounce;

float displayNumberFinal;

float floatNumberRight;
float floatNumberLeft;

int dotFlag;
int calculationFlag;
int additionFlag;
int substractionFlag;
int multiplicationFlag;
int divisionFlag;
int enterFlag;
int negativeFlag;
int floatingFlag;
int scientificFlag;
int overflowFlag;
int invalidFlag;
int idleFlag =1;
int idleTime;
int negativePoint;

void delay(uint32_t s){
	
    for(;s>0;s--);

    }


void TIM1_BRK_UP_TRG_COM_IRQHandler(void) {

	BSP_keypad_set();
	BSP_ssd_display(pressCounter);

	TIM1->SR &= ~(1U << 1);
	TIM1->SR &= ~(1U << 0);

}



void timer1(void) {

	/* Setup TIM1 */
	RCC->APBENR2 |= (1U << 11);
	TIM1->CR1 = 0;
	TIM1->CR1 |= (1 << 7);
	TIM1->CNT = 0;
	TIM1->PSC = 1;
	TIM1->ARR = 2499;
	TIM1->DIER |= (1 << 0);
	TIM1->CR1 |= (1 << 0);
	NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 1);
	NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);

}


void BSP_system_init(void){

	BSP_keypad_init();
	BSP_ssd_init();

}

void BSP_keypad_init(void){
	
	/* Enable GPIOA and GPIOB clock */
	RCC->IOPENR |=(3U<<0);
	
	/* Setup PB6, PB7, PA15, PB1 as output (rows) */
															 
	GPIOB->MODER &= ~(3U << 2*6);  /* PB6 is output (13-12)  */
	GPIOB->MODER |=  (1U << 2*6);                            
															 
	GPIOB->MODER &= ~(3U << 2*7);  /* PB7 is output (15-14)  */
	GPIOB->MODER |=  (1U << 2*7);
									  
	GPIOA->MODER &= ~(3U << 2*15); /* PA15 is output (31-30) */
	GPIOA->MODER |=  (1U << 2*15);    
									  
	GPIOB->MODER &= ~(3U << 2*1);  /* PB1 is output (03-02)  */
	GPIOB->MODER |=  (1U << 2*1);
	
	/* Setup PA10, PA9, PB0, PB2 as input (columns) */
									  
	GPIOA->MODER &= ~(3U << 2*10); /* PA10 is input (21-20) */
	GPIOA->PUPDR |=  (2U << 2*10);    
									  
	GPIOA->MODER &= ~(3U << 2*9);  /* PA9 is input (21-20) */
	GPIOA->PUPDR |=  (2U << 2*9);                           
									 					   
	GPIOB->MODER &= ~(3U << 2*0);  /* PB0 is input (01-00) */
	GPIOB->PUPDR |=  (2U << 2*0);
									 					   
	GPIOB->MODER &= ~(3U << 2*2);  /* PB2 is input (05-04) */
	GPIOB->PUPDR |=  (2U << 2*2);

	/* Setup interrupts for inputs */
	
	EXTI->EXTICR[2] |= (0U << 8*2);  /*  PA10 */
	EXTI->EXTICR[2] |= (0U << 8*1);  /*  PA9  */
	EXTI->EXTICR[0] |= (1U << 8*0);  /*  PB0  */
	EXTI->EXTICR[0] |= (1U << 8*2);  /*  PB2  */
	
	/* Rising Edge */
	
	EXTI->RTSR1 |= (1U << 10);  /*  PA10 */
	EXTI->RTSR1 |= (1U << 9);   /*  PA9  */
	EXTI->RTSR1 |= (1U << 0);   /*  PB0  */
	EXTI->RTSR1 |= (1U << 2);   /*  PB2  */
	
	/* Mask */
	
	EXTI->IMR1 |= (1U << 10);   /*  PA10 */
	EXTI->IMR1 |= (1U << 9);    /*  PA9  */
	EXTI->IMR1 |= (1U << 0);    /*  PB0  */
	EXTI->IMR1 |= (1U << 2);    /*  PB2  */
	


	NVIC_SetPriority(EXTI0_1_IRQn, 0);
	NVIC_EnableIRQ(EXTI0_1_IRQn);
	
	NVIC_SetPriority(EXTI2_3_IRQn, 0);
	NVIC_EnableIRQ(EXTI2_3_IRQn);

	NVIC_SetPriority(EXTI4_15_IRQn, 0);
	NVIC_EnableIRQ(EXTI4_15_IRQn);

	BSP_keypad_set();
	BSP_ssd_clear();
}

void BSP_keypad_set(void){

	GPIOB->ODR |= (1U << 6);
	GPIOB->ODR |= (1U << 7);
	GPIOA->ODR |= (1U << 15);
	GPIOB->ODR |= (1U << 1);
	
}

void BSP_keypad_clear(void){

	GPIOB->ODR &= ~(1U << 6);
	GPIOB->ODR &= ~(1U << 7);
	GPIOA->ODR &= ~(1U << 15);
	GPIOB->ODR &= ~(1U << 1);
	
}


void BSP_number_control(void){

	if(displayNumberFinal >= 0 && displayNumberFinal < 10000){
		overflowFlag = 0;
		calculationFlag = 0;
		floatNumberRight = modf(displayNumberFinal, &floatNumberLeft);
		floatNumberLeft = displayNumberFinal - floatNumberRight;

				if(floatNumberRight > 0){
						floatingFlag=1;
						pressCounter = numberOfDigits((displayNumberFinal));
				}

				if((negativeFlag == 0) && (floatingFlag == 0)){
					BSP_ssd_final_number();
				}

				//1-Digit
				else if((negativeFlag == 0) && (floatingFlag==1)){


					 if(numberOfDigits(displayNumberFinal) == 1){

						dotFlag = 1;

						displayNumber1_d1 = (int)(floatNumberLeft);
						displayNumber1_d2 = (int)((floatNumberRight*10));
						displayNumber1_d3 = (((int)(floatNumberRight*100))%10);
						displayNumber1_d4 = (((int)(floatNumberRight*1000))%10);

				}

				//2-Digit
				else if (numberOfDigits(displayNumberFinal) == 2){

						dotFlag = 2;

						displayNumber1_d1 = (int)(floatNumberLeft/10);
						displayNumber1_d2 =	((int)(floatNumberLeft)%10);
						displayNumber1_d3 = (int)(floatNumberRight*10);
						displayNumber1_d4 = (((int)(floatNumberRight*100))%10);
				}

				//3-Digit
				else if (numberOfDigits(displayNumberFinal) == 3){

						dotFlag = 3;

						displayNumber1_d1 = (int)(floatNumberLeft/100);
						displayNumber1_d2 =	((((int)(floatNumberLeft))%100)/10);
						displayNumber1_d3 = (((int)floatNumberLeft)%10);
						displayNumber1_d4 = (int)(floatNumberRight*10);
				}

				//4-Digit
				else if (numberOfDigits(displayNumberFinal) == 4)  {
						floatingFlag = 0;
						pressCounter = 4;

						displayNumber1_d4 = (((int)displayNumberFinal)%10);
						displayNumber1_d3 = ((((int)displayNumberFinal)%100)/10);
						displayNumber1_d2 = ((((int)displayNumberFinal)%1000)/100);
						displayNumber1_d1 = (int)((displayNumberFinal/1000));

				}
				}

			}

				else if((displayNumberFinal < 0) && (displayNumberFinal >= -999)){
					overflowFlag = 0;
					calculationFlag = 0;
					displayNumberFinal=displayNumberFinal*-1;
					floatNumberRight = modf(displayNumberFinal, &floatNumberLeft);
					floatNumberLeft = displayNumberFinal - floatNumberRight;
					displayNumberFinal=displayNumberFinal*-1;

					if(floatNumberRight > 0){
							floatingFlag=1;
							pressCounter = numberOfDigits((displayNumberFinal));
						}

					if((negativeFlag == 1) && (floatingFlag == 0)){

						displayNumberFinal=displayNumberFinal*-1;
						BSP_ssd_final_number();
						displayNumberFinal=displayNumberFinal*-1;

					}

					if((negativeFlag == 1) && (floatingFlag == 1)){

						displayNumberFinal=displayNumberFinal*-1;
						if(numberOfDigits(displayNumberFinal)==1){

							dotFlag = 2;

							displayNumber1_d2 = (int)(floatNumberLeft);
							displayNumber1_d3 = (int)((floatNumberRight*10));
							displayNumber1_d4 = (((int)(floatNumberRight*100))%10);

						}

						//2-Digit
						else if (numberOfDigits(displayNumberFinal) == 2){

							dotFlag = 3;

							displayNumber1_d2 = (int)(floatNumberLeft/10);
							displayNumber1_d3 =	((int)(floatNumberLeft)%10);
							displayNumber1_d4 = (int)(floatNumberRight*10);

						}

						//3-Digit
						else if (numberOfDigits(displayNumberFinal) == 3){
							floatingFlag = 0;
							displayNumber1_d2 = (int)(floatNumberLeft/100);
							displayNumber1_d3 =	((((int)(floatNumberLeft))%100)/10);
							displayNumber1_d4 = (((int)floatNumberLeft)%10);

						}

				}


		}

				else
				{
					floatingFlag=0;

					if(invalidFlag==1)
						overflowFlag=0;

					else
						overflowFlag = 1;
				}
}


void BSP_ssd_final_number(void){

		pressCounter = numberOfDigits((displayNumberFinal));

		if(numberOfDigits(displayNumberFinal)==1){


			displayNumber1_d4 = (((int)displayNumberFinal)%10);
		}

		else if(numberOfDigits(displayNumberFinal)==2){

			displayNumber1_d4 = (((int)displayNumberFinal)%10);
			displayNumber1_d3 = ((((int)displayNumberFinal)%100)/10);
		}

		else if(numberOfDigits(displayNumberFinal)==3){


			displayNumber1_d4 = (((int)displayNumberFinal)%10);
			displayNumber1_d3 = ((((int)displayNumberFinal)%100)/10);
			displayNumber1_d2 = ((((int)displayNumberFinal)%1000)/100);
		}

		else if(numberOfDigits(displayNumberFinal)==4){


			displayNumber1_d4 = (((int)displayNumberFinal)%10);
			displayNumber1_d3 = ((((int)displayNumberFinal)%100)/10);
			displayNumber1_d2 = ((((int)displayNumberFinal)%1000)/100);
			displayNumber1_d1 = (int)((displayNumberFinal/1000));
		}
}

void BSP_set_values(void)
{

	displayNumber1_d1 = 0;
	displayNumber1_d2 = 0;
	displayNumber1_d3 = 0;
	displayNumber1_d4 = 0;

    pressCounter = 0;
    overflowFlag = 0;
    invalidFlag  = 0;
    scientificFlag = 0;
    negativeFlag = 0;

	displayNumberFinal = 0;
	displayNumber2_d1 = 0;
	displayNumber2_d2 = 0;
	displayNumber2_d3 = 0;
	displayNumber2_d4 = 0;

	displayNumberFinal = 0;
	displayNumber1 = 0;
	displayNumber2 = 0;

}



void BSP_ssd_enter(void){

		if(enterFlag==1){

			if(scientificFlag != 0)
				BSP_scientific_function(displayNumberFinal);

			BSP_calculation(displayNumber1_d1, displayNumber1_d2, displayNumber1_d3,displayNumber1_d4, displayNumber2_d1,displayNumber2_d2,displayNumber2_d3,displayNumber2_d4);
			BSP_number_control();

		}
}

void BSP_displayNumber2_init(void){

	calculationFlag = 0;

	if (pressCounter==1){
		pressCounter=0;


		if (floatingFlag == 1){

			displayNumber2_d1 = displayNumber1_d1;
			displayNumber2_d2 = displayNumber1_d2;
			displayNumber2_d3 = displayNumber1_d3;
			displayNumber2_d4 = displayNumber1_d4;

			displayNumber1_d1 = 0;
			displayNumber1_d2 = 0;
			displayNumber1_d3 = 0;
			displayNumber1_d4 = 0;
		}
		else{

			displayNumber2_d4 = displayNumber1_d4;
			displayNumber1_d4 = 0;
		}
		}

	else if(pressCounter == 2){

			pressCounter=0;

			if (floatingFlag == 1){

				displayNumber2_d1 = displayNumber1_d1;
				displayNumber2_d2 = displayNumber1_d2;
				displayNumber2_d3 = displayNumber1_d3;
				displayNumber2_d4 = displayNumber1_d4;

				displayNumber1_d1 = 0;
				displayNumber1_d2 = 0;
				displayNumber1_d3 = 0;
				displayNumber1_d4 = 0;
			}
			else{

				displayNumber2_d3 = displayNumber1_d3;
			displayNumber2_d4 = displayNumber1_d4;
			displayNumber1_d3 = 0;
			displayNumber1_d4 = 0;}
	}
	else if(pressCounter == 3){

			pressCounter=0;

			if (floatingFlag == 1){

				displayNumber2_d1 = displayNumber1_d1;
				displayNumber2_d2 = displayNumber1_d2;
				displayNumber2_d3 = displayNumber1_d3;
				displayNumber2_d4 = displayNumber1_d4;

				displayNumber1_d1 = 0;
				displayNumber1_d2 = 0;
				displayNumber1_d3 = 0;
				displayNumber1_d4 = 0;
			}
			else{

				displayNumber2_d2 = displayNumber1_d2;
			displayNumber2_d3 = displayNumber1_d3;
			displayNumber2_d4 = displayNumber1_d4;

			displayNumber1_d2 = 0;
			displayNumber1_d3 = 0;
			displayNumber1_d4 = 0;}
	}
	else if(pressCounter == 4){

			pressCounter=0;

			if (floatingFlag == 1){

				displayNumber2_d1 = displayNumber1_d1;
				displayNumber2_d2 = displayNumber1_d2;
				displayNumber2_d3 = displayNumber1_d3;
				displayNumber2_d4 = displayNumber1_d4;

				displayNumber1_d1 = 0;
				displayNumber1_d2 = 0;
				displayNumber1_d3 = 0;
				displayNumber1_d4 = 0;
			}
			else{

			displayNumber2_d1 = displayNumber1_d1;
			displayNumber2_d2 = displayNumber1_d2;
			displayNumber2_d3 = displayNumber1_d3;
			displayNumber2_d4 = displayNumber1_d4;

			displayNumber1_d1 = 0;
			displayNumber1_d2 = 0;
			displayNumber1_d3 = 0;
			displayNumber1_d4 = 0;}
	}
}

void BSP_keypad_press(int pressCounter, float numberValue){

	if(pressCounter > 4){
		(void)pressCounter;
	}

	else{

			if (pressCounter==1){

				displayNumber1_d4 = numberValue;
			}

			else if (pressCounter==2){

				displayNumber1_d3 = displayNumber1_d4;
				displayNumber1_d4 = numberValue;
			}



			else if (pressCounter==3){

				displayNumber1_d2 = displayNumber1_d3;
				displayNumber1_d3 = displayNumber1_d4;
				displayNumber1_d4 = numberValue;
			}

			else if (pressCounter==4){

				displayNumber1_d1 = displayNumber1_d2;
				displayNumber1_d2 = displayNumber1_d3;
				displayNumber1_d3 = displayNumber1_d4;
				displayNumber1_d4 = numberValue;
			}
		}
	}



void BSP_scientific_function(float number){

	/* EA is for log */
	if((scientificFlag ==1) && (additionFlag == 1)){

		displayNumberFinal = log10( number);
		scientificFlag =0;
		additionFlag = 0;
	}
	/* EB is for ln */
	else if((scientificFlag ==1) && (substractionFlag == 1)){

		displayNumberFinal = log(number);
		scientificFlag =0;
		substractionFlag = 0;
	}

	/* EC is for sqrt */
	else if((scientificFlag ==1) && (multiplicationFlag == 1)){

		displayNumberFinal = sqrt(number);
		scientificFlag =0;
		multiplicationFlag = 0;
		}

	/* ED is for x^2 */
	else if((scientificFlag ==1) && (divisionFlag == 1)){

		displayNumberFinal = pow(number,2);
		scientificFlag =0;
		divisionFlag = 0;
		}

	/* EEA is for sin */
	else if((scientificFlag ==2) && (additionFlag == 1)){

		displayNumberFinal = sin(number);

		if(displayNumberFinal < 0)
			negativeFlag = 1;
		else
			negativeFlag = 0;

		additionFlag = 0;
		}

	/* EEB is for cos */
	else if((scientificFlag ==2) && (substractionFlag == 1)){

		displayNumberFinal = cos(number);
		if(displayNumberFinal < 0)
			negativeFlag = 1;
		else
			negativeFlag = 0;

		scientificFlag =0;
		substractionFlag = 0;
			}

	/* EEC is for tan */
	else if((scientificFlag ==2) && (multiplicationFlag == 1)){

		displayNumberFinal = tan(number);

		if(displayNumberFinal < 0)
			negativeFlag = 1;
		else
			negativeFlag = 0;

		scientificFlag =0;
		multiplicationFlag = 0;
			}

	/* EED is for cot */
	else if((scientificFlag ==2) && (divisionFlag == 1)){

		displayNumberFinal = 1/(tan(number));

		if(displayNumberFinal < 0)
			negativeFlag = 1;
		else
			negativeFlag = 0;

		scientificFlag =0;
		divisionFlag = 0;
			}

	/* EEE is for pi */
	else if(scientificFlag == 3){
		displayNumberFinal = 3.141;
		scientificFlag =0;

			}
}



void BSP_calculation(int displayNumber1_d1, int displayNumber1_d2, int displayNumber1_d3, int displayNumber1_d4, int displayNumber2_d1, int displayNumber2_d2, int displayNumber2_d3, int displayNumber2_d4){


		BSP_keypad_clear();
		BSP_ssd_clear();


		displayNumber1 = displayNumber1_d4 + displayNumber1_d3*10 + displayNumber1_d2*100 + displayNumber1_d1*1000;

		if (floatingFlag==1){

			if (negativeFlag==1){

			if(dotFlag == 2){
			displayNumber2 = displayNumber2_d4*0.01 + displayNumber2_d3*0.1 + displayNumber2_d2;

			}
			else if(dotFlag == 3){
			displayNumber2 = displayNumber2_d4*0.1 + displayNumber2_d3*1 + displayNumber2_d2*10;

			}

			}

			else{

				if(dotFlag == 1){
				displayNumber2 = displayNumber2_d4*0.001 + displayNumber2_d3*0.01 + displayNumber2_d2*0.1+displayNumber2_d1;

				}
				if(dotFlag == 2){
				displayNumber2 = displayNumber2_d4*0.01 + displayNumber2_d3*0.1 + displayNumber2_d2 + displayNumber2_d1*10;

				}
				if(dotFlag == 3){
				displayNumber2 = displayNumber2_d4*0.1 + displayNumber2_d3*1 + displayNumber2_d2*10 + displayNumber2_d1*100;

			}

			}
		}

		else{

		displayNumber1 = displayNumber1_d4 + displayNumber1_d3*10 + displayNumber1_d2*100 + displayNumber1_d1*1000;
		displayNumber2 = displayNumber2_d4 + displayNumber2_d3*10 + displayNumber2_d2*100 + displayNumber2_d1*1000;

		}

		if(additionFlag == 1){

			if(negativeFlag == 1){
				displayNumberFinal = displayNumber1 + -1*displayNumber2;
				additionFlag = 0;

					if(displayNumberFinal > 0)
						negativeFlag =0;
					else
						negativeFlag =1;
			}

			else{
			displayNumberFinal = displayNumber1 + displayNumber2;
			additionFlag =0 ;
			}
		}

		else if(substractionFlag == 1){

			if(negativeFlag == 1)
			displayNumber2=displayNumber2*-1;


			displayNumberFinal = displayNumber2 - displayNumber1;
			substractionFlag =0;

			if (displayNumberFinal < 0)
				negativeFlag = 1;
			else
				negativeFlag = 0;

		}

		else if(multiplicationFlag == 1){

			if(negativeFlag == 1)
			displayNumber2=displayNumber2*-1;

			displayNumberFinal = displayNumber1 * displayNumber2;

			if (displayNumberFinal < 0)
				negativeFlag = 1;

			else
				negativeFlag = 0;
			multiplicationFlag = 0;
		}

		else if(divisionFlag == 1){

			if(negativeFlag == 1)
			displayNumber2=displayNumber2*-1;

			displayNumberFinal =  displayNumber2 / displayNumber1;
			divisionFlag = 0;


			if (displayNumberFinal < 0)
				negativeFlag = 1;
				else
				negativeFlag = 0;

		}

		if(displayNumberFinal == ((displayNumberFinal)/0)){
			invalidFlag = 1;
		}
	}




void BSP_ssd_init(void){
	
	/* Enable GPIOA and GPIOB clock */
	RCC->IOPENR |=(3U<<0);

    /*Setup PA0-1-4-5-6-7-11-12 for Digits 0011_1100_0000_0011_1111_0000_1100*/
    GPIOA -> MODER &= ~(15U <<2*0);  /* Necessary bits cleared 1111     */
    GPIOA -> MODER &= ~(255U <<2*4); /* Necessary bits cleared 11111111 */
    GPIOA -> MODER &= ~(15U <<2*11); /* Necessary bits cleared 1111     */

    GPIOA -> MODER |= (5U << 2*0);  /* Required pins is activated output" 0101     */
    GPIOA -> MODER |= (85U << 2*4); /* Required pins is activated output" 01010101 */
    GPIOA -> MODER |= (5U << 2*11); /* Required pins is activated output" 0101     */
	
	/* Setup PB4 as output D1 */
	GPIOB->MODER &= ~(3U << 2 * 4);
	GPIOB->MODER |= (1U << 2 * 4);
	
	/* Setup PB5 as output D2 */
	GPIOB->MODER &= ~(3U << 2 * 5);
	GPIOB->MODER |= (1U << 2 * 5);
	
	/* Setup PB8 as output D3 */
	GPIOB->MODER &= ~(3U << 2 * 8);
	GPIOB->MODER |= (1U << 2 * 8);
	
	/* Setup PB9 as output D4 */
	GPIOB->MODER &= ~(3U << 2 * 9);
	GPIOB->MODER |= (1U << 2 * 9);
	
	BSP_ssd_clear();
}





void BSP_ssd_ID_display(void){

	BSP_ssd_clear();
	BSP_ssd_Digit_all_OFF();

			for(int i = 0; i<50;i++){
			BSP_ssd_set(1);
			BSP_ssd_Digit_n_ON(1);
			BSP_ssd_Digit_n_OFF(1);

			BSP_ssd_set(7);
			BSP_ssd_Digit_n_ON(2);
			BSP_ssd_Digit_n_OFF(2);

			BSP_ssd_set(0);
			BSP_ssd_Digit_n_ON(3);
			BSP_ssd_Digit_n_OFF(3);

			BSP_ssd_set(6);
			BSP_ssd_Digit_n_ON(4);
			BSP_ssd_Digit_n_OFF(4);
			}

}

void BSP_ssd_overflow_display(void){

	BSP_ssd_clear();
	BSP_ssd_Digit_all_OFF();


			BSP_ssd_set(0);
			BSP_ssd_Digit_n_ON(1);
			BSP_ssd_Digit_n_OFF(1);

			BSP_ssd_set(16);
			BSP_ssd_Digit_n_ON(2);
			BSP_ssd_Digit_n_OFF(2);

			BSP_ssd_set(13);
			BSP_ssd_Digit_n_ON(3);
			BSP_ssd_Digit_n_OFF(3);

			BSP_ssd_set(14);
			BSP_ssd_Digit_n_ON(4);
			BSP_ssd_Digit_n_OFF(4);

}

void BSP_ssd_invalid_display(void){

	BSP_ssd_clear();
	BSP_ssd_Digit_all_OFF();


			BSP_ssd_set(1);
			BSP_ssd_Digit_n_ON(1);
			BSP_ssd_Digit_n_OFF(1);

			BSP_ssd_set(15);
			BSP_ssd_Digit_n_ON(2);
			BSP_ssd_Digit_n_OFF(2);

			BSP_ssd_set(16);
			BSP_ssd_Digit_n_ON(3);
			BSP_ssd_Digit_n_OFF(3);

			BSP_ssd_set(14);
			BSP_ssd_Digit_n_ON(4);
			BSP_ssd_Digit_n_OFF(4);

}



void BSP_ssd_set(int number){

	BSP_keypad_clear();
	BSP_ssd_clear();

		if (number == 0) {

			GPIOA->ODR &= ~(0x1872U);
		} 
		
		else if (number == 1) {

			GPIOA->ODR &= ~(0x30U);
		} 
		
		else if (number == 2) {

			GPIOA->ODR &= ~(0x1892U);
		}
		
		else if (number == 3) {

			GPIOA->ODR &= ~(0x10B2U);
		} 
		
		else if (number == 4) {

			GPIOA->ODR &= ~(0xF0U);
		} 
		
		else if (number == 5) {

			GPIOA->ODR &= ~(0x10E2U);
		} 
		
		else if (number == 6) {

			GPIOA->ODR &= ~(0x18E2U);
		} 
		
		else if (number == 7) {

			GPIOA->ODR &= ~(0x32U);
		} 
		
		else if (number == 8) {

			GPIOA->ODR &= ~(0x18F2U);
		} 
		
		else if (number == 9) {

			GPIOA->ODR &= ~(0x10F2U);
		} 
		//- Negative Symbol
		else if (number == 10) {

			GPIOA->ODR &= ~(0x80U);
		} 

		//Decimal
		else if (number == 11) {

			GPIOA->ODR &= ~(0x1U);
		} 
		//U
		else if (number == 12) {

			GPIOA->ODR &= ~(0X1870U);
		}
		//0000_1000_1100_0010 = 0x1C4U //F
		else if (number == 13) {

			GPIOA->ODR &= ~(0x8C2U);

		} 
		//0001_1000_0100_0000 = 0x1840U //L
		else if (number == 14) {

			GPIOA->ODR &= ~(0x1840U);

		} 
		// 0000_1000_1010_0000 = 0x08A0 //n
		else if (number == 15) {

			GPIOA->ODR &= ~(0x08A0U);

		}
		// 0001_1000_0010_0000 = 0x1820 //u
		else if (number == 16) {

			GPIOA->ODR &= ~(0x1820U);

		}
		//d 0001_1000_1011_0000 = 0x18B0 //d
		else if (number == 17) {

			GPIOA->ODR &= ~(0x18B0U);

		}

}

void BSP_ssd_clear(void){
	GPIOA->ODR |= (0x18F3U);
}

void BSP_ssd_display(int pressCounter){

	BSP_ssd_clear();
	preventBounce = 0;

	if(idleTime > 500000) {

		floatingFlag = 0;
		idleFlag = 1;
		BSP_set_values();

	}

	if(overflowFlag == 1)
	{
		idleTime+=6;
		BSP_ssd_Digit_all_OFF();
		BSP_ssd_overflow_display();
	}

	else if(idleFlag == 1)
	{

		BSP_ssd_Digit_all_OFF();
		BSP_ssd_ID_display();
	}

	else if(invalidFlag == 1)
	{
		idleTime+=6;
		BSP_ssd_Digit_all_OFF();
		BSP_ssd_invalid_display();
	}


	else if(floatingFlag == 0){
		 if (pressCounter == 0){
			 idleTime+=1;
			BSP_ssd_clear();
			BSP_ssd_Digit_all_OFF();
			 }

	else if (pressCounter == 1){
		for(int i = 0; i<40;i++){

			if(negativeFlag==1){
				idleTime+=10;
				BSP_ssd_Digit_all_OFF();
				BSP_ssd_set(10);
				BSP_ssd_Digit_n_ON(3);
				BSP_ssd_Digit_n_OFF(3);
				BSP_ssd_set(displayNumber1_d4);
				BSP_ssd_Digit_n_ON(4);
				delay(400);
		}

		else{
			idleTime+=10;
		BSP_ssd_Digit_all_OFF();
		BSP_ssd_set(displayNumber1_d4);
		BSP_ssd_Digit_n_ON(4);
		delay(400);
		}

		}
	}

	else if (pressCounter == 2){


			if(negativeFlag==1){
				idleTime+=40;
				BSP_ssd_Digit_all_OFF();
				BSP_ssd_set(10);
				BSP_ssd_Digit_n_ON(2);
				BSP_ssd_Digit_n_OFF(2);
				delay(400);
				BSP_ssd_set(displayNumber1_d3);
				BSP_ssd_Digit_n_ON(3);
				BSP_ssd_Digit_n_OFF(3);
				delay(400);
				BSP_ssd_set(displayNumber1_d4);
				BSP_ssd_Digit_n_ON(4);
				delay(400);

			}
			else{
				idleTime+=10;
				BSP_ssd_Digit_all_OFF();
				BSP_ssd_set(displayNumber1_d3);
				BSP_ssd_Digit_n_ON(3);
				BSP_ssd_Digit_n_OFF(3);
				delay(400);
				BSP_ssd_set(displayNumber1_d4);
				BSP_ssd_Digit_n_ON(4);
				BSP_ssd_Digit_n_OFF(4);
			}

	}

	else if (pressCounter == 3){


			if(negativeFlag==1){
				idleTime+=40;
				BSP_ssd_Digit_all_OFF();
				BSP_ssd_set(10);
				BSP_ssd_Digit_n_ON(1);
				BSP_ssd_Digit_n_OFF(1);
				delay(400);
				BSP_ssd_set(displayNumber1_d2);
				BSP_ssd_Digit_n_ON(2);
				BSP_ssd_Digit_n_OFF(2);
				delay(400);
				BSP_ssd_set(displayNumber1_d3);
				BSP_ssd_Digit_n_ON(3);
				BSP_ssd_Digit_n_OFF(3);
				delay(400);
				BSP_ssd_set(displayNumber1_d4);
				BSP_ssd_Digit_n_ON(4);
				BSP_ssd_Digit_n_OFF(4);
			}

			else{
				idleTime+=40;
				BSP_ssd_Digit_all_OFF();
				BSP_ssd_set(displayNumber1_d2);
				BSP_ssd_Digit_n_ON(2);
				BSP_ssd_Digit_n_OFF(2);
				delay(400);
				BSP_ssd_set(displayNumber1_d3);
				BSP_ssd_Digit_n_ON(3);
				BSP_ssd_Digit_n_OFF(3);
				delay(400);
				BSP_ssd_set(displayNumber1_d4);
				BSP_ssd_Digit_n_ON(4);
				BSP_ssd_Digit_n_OFF(4);
			}


	}

	else if (pressCounter >= 4){

			idleTime+=40;
			BSP_ssd_Digit_all_OFF();
			BSP_ssd_set(displayNumber1_d1);
			BSP_ssd_Digit_n_ON(1);
			BSP_ssd_Digit_n_OFF(1);
			delay(400);
			BSP_ssd_set(displayNumber1_d2);
			BSP_ssd_Digit_n_ON(2);
			BSP_ssd_Digit_n_OFF(2);
			delay(400);
			BSP_ssd_set(displayNumber1_d3);
			BSP_ssd_Digit_n_ON(3);
			BSP_ssd_Digit_n_OFF(3);
			delay(400);
			BSP_ssd_set(displayNumber1_d4);
			BSP_ssd_Digit_n_ON(4);
			BSP_ssd_Digit_n_OFF(4);


	}
	}
	//Floating Flag- Display Positive or Negative Float Numbers

	else{

		 if(pressCounter == 1){

			if(negativeFlag==1){

				idleTime+=50;
				BSP_ssd_Digit_all_OFF();

				BSP_ssd_set(10);
				BSP_ssd_Digit_n_ON(1);
				BSP_ssd_Digit_n_OFF(1);
				delay(400);


				BSP_ssd_set(displayNumber1_d2);
				BSP_ssd_Digit_n_ON(2);
				BSP_ssd_Digit_n_OFF(2);
				delay(400);

				BSP_ssd_set(11);
				BSP_ssd_Digit_n_ON(2);
				BSP_ssd_Digit_n_OFF(2);
				delay(400);

				BSP_ssd_set(displayNumber1_d3);
				BSP_ssd_Digit_n_ON(3);
				BSP_ssd_Digit_n_OFF(3);
				delay(400);


				BSP_ssd_set(displayNumber1_d4);
				BSP_ssd_Digit_n_ON(4);
				BSP_ssd_Digit_n_OFF(4);
				delay(400);
			}

			else{

				idleTime+=50;
			BSP_ssd_Digit_all_OFF();
			BSP_ssd_set(displayNumber1_d1);
			BSP_ssd_Digit_n_ON(1);
			BSP_ssd_Digit_n_OFF(1);
			delay(400);

			BSP_ssd_set(11);
			BSP_ssd_Digit_n_ON(1);
			BSP_ssd_Digit_n_OFF(1);
			delay(400);

			BSP_ssd_set(displayNumber1_d2);
			BSP_ssd_Digit_n_ON(2);
			BSP_ssd_Digit_n_OFF(2);
			delay(400);

			BSP_ssd_set(displayNumber1_d3);
			BSP_ssd_Digit_n_ON(3);
			BSP_ssd_Digit_n_OFF(3);
			delay(400);

			BSP_ssd_set(displayNumber1_d4);
			BSP_ssd_Digit_n_ON(4);
			BSP_ssd_Digit_n_OFF(4);
			}
		}

		else if(pressCounter == 2){

			if(negativeFlag==1){

				idleTime+=50;
				BSP_ssd_Digit_all_OFF();

				BSP_ssd_set(10);
				BSP_ssd_Digit_n_ON(1);
				BSP_ssd_Digit_n_OFF(1);
				delay(400);


				BSP_ssd_set(displayNumber1_d2);
				BSP_ssd_Digit_n_ON(2);
				BSP_ssd_Digit_n_OFF(2);
				delay(400);

				BSP_ssd_set(displayNumber1_d3);
				BSP_ssd_Digit_n_ON(3);
				BSP_ssd_Digit_n_OFF(3);
				delay(400);

				BSP_ssd_set(11);
				BSP_ssd_Digit_n_ON(3);
				BSP_ssd_Digit_n_OFF(3);
				delay(400);


				BSP_ssd_set(displayNumber1_d4);
				BSP_ssd_Digit_n_ON(4);
				BSP_ssd_Digit_n_OFF(4);
				delay(400);
			}

			else{

				idleTime+=50;
				BSP_ssd_Digit_all_OFF();
				BSP_ssd_set(displayNumber1_d1);
				BSP_ssd_Digit_n_ON(1);
				BSP_ssd_Digit_n_OFF(1);
				delay(400);

				BSP_ssd_set(displayNumber1_d2);
				BSP_ssd_Digit_n_ON(2);
				BSP_ssd_Digit_n_OFF(2);
				delay(400);

				BSP_ssd_set(11);
				BSP_ssd_Digit_n_ON(2);
				BSP_ssd_Digit_n_OFF(2);
				delay(400);

				BSP_ssd_set(displayNumber1_d3);
				BSP_ssd_Digit_n_ON(3);
				BSP_ssd_Digit_n_OFF(3);
				delay(400);

				BSP_ssd_set(displayNumber1_d4);
				BSP_ssd_Digit_n_ON(4);
				BSP_ssd_Digit_n_OFF(4);
				delay(400);
			}
		}

		else if(pressCounter == 3){

			if(negativeFlag==1){

				idleTime+=50;
				BSP_ssd_Digit_all_OFF();

				BSP_ssd_set(10);
				BSP_ssd_Digit_n_ON(1);
				BSP_ssd_Digit_n_OFF(1);
				delay(400);

				BSP_ssd_set(displayNumber1_d2);
				BSP_ssd_Digit_n_ON(2);
				BSP_ssd_Digit_n_OFF(2);
				delay(400);

				BSP_ssd_set(displayNumber1_d3);
				BSP_ssd_Digit_n_ON(3);
				BSP_ssd_Digit_n_OFF(3);
				delay(400);

				BSP_ssd_set(displayNumber1_d4);
				BSP_ssd_Digit_n_ON(4);
				BSP_ssd_Digit_n_OFF(4);
				delay(400);


			}
			else{

				idleTime+=50;
			BSP_ssd_Digit_all_OFF();
			BSP_ssd_set(displayNumber1_d1);
			BSP_ssd_Digit_n_ON(1);
			BSP_ssd_Digit_n_OFF(1);
			delay(400);
			BSP_ssd_set(displayNumber1_d2);
			BSP_ssd_Digit_n_ON(2);
			BSP_ssd_Digit_n_OFF(2);
			delay(400);
			BSP_ssd_set(displayNumber1_d3);
			BSP_ssd_Digit_n_ON(3);
			BSP_ssd_Digit_n_OFF(3);
			delay(400);
			BSP_ssd_set(11);
			BSP_ssd_Digit_n_ON(3);
			BSP_ssd_Digit_n_OFF(3);
			delay(400);
			BSP_ssd_set(displayNumber1_d4);
			BSP_ssd_Digit_n_ON(4);
			BSP_ssd_Digit_n_OFF(4);
			}

			}
		}

	}



void BSP_ssd_Digit_n_ON(int n){

	if(n==1)
	GPIOB->ODR |= (1 << 4);

	else if (n==2)
	GPIOB->ODR |= (1 << 5);

	else if (n==3)
	GPIOB->ODR |= (1 << 9);

	else if (n==4)
	GPIOB->ODR |= (1 << 8);
	

}


void BSP_ssd_Digit_n_OFF(int n){

	if(n==1)
	GPIOB->BRR |= (1 << 4);

	else if (n==2)
	GPIOB->BRR |= (1 << 5);

	else if (n==3)
	GPIOB->BRR |= (1 << 9);

	else if (n==4)
	GPIOB->BRR |= (1 << 8);

}

void BSP_ssd_Digit_all_OFF(void){

	GPIOB->BRR |= (1 << 4);
	GPIOB->BRR |= (1 << 5);
	GPIOB->BRR |= (1 << 9);
	GPIOB->BRR |= (1 << 8);

}

int numberOfDigits(float n){

	if (n >= 0){

	if(n < 10){
	  return 1;
	}

	else if(n < 100){
	  return 2;
	}

	else if(n < 1000){
	  return 3;
	}

	else if(n < 10000){
	 return 4;
	}

	}

	else if (n < 0){

		if (n>-10)
	    return 1;

		else if (n>-100)
		return 2;

		else if (n>-1000)
		return 3;

		else if (n>-10000)
		return 3;

	}
}


void EXTI0_1_IRQHandler(void) /* PB0 */ {

	BSP_keypad_clear();

	/*** PB1 - # ***/
	GPIOB->ODR ^= (1U<<1);

	if(((GPIOB->IDR >> 0) & 1)  && (preventBounce == 0) ){
		enterFlag = 1;
		preventBounce = 1;
		idleFlag = 0;
		idleTime = 0;
		BSP_ssd_enter();
		delay(400000);
	}

	GPIOB->ODR ^= (1U<<1);

	/*** PA15 - 9 ***/
	GPIOA->ODR ^= (1U<<15);

	if(((GPIOB->IDR >> 0) & 1) && (preventBounce == 0) ){

		pressCounter++;
		preventBounce = 1;
		idleFlag = 0;
		idleTime = 0;
		displayNumber1 = 9;
		BSP_keypad_press(pressCounter,displayNumber1);
		delay(80000);
	}

	GPIOA->ODR ^= (1U<<15);

	/*** PB7 - 6 ***/
	GPIOB->ODR ^= (1U<<7);

	if(((GPIOB->IDR >> 0) & 1)  && (preventBounce == 0) ){

		pressCounter++;
		preventBounce = 1;
		idleFlag = 0;
		idleTime = 0;
		displayNumber1 = 6;
		BSP_keypad_press(pressCounter,displayNumber1);
		delay(80000);
	}

	GPIOB->ODR ^= (1U<<7);

	/*** PB6 - 3 ***/
	GPIOB->ODR ^= (1U<<6);

	if(((GPIOB->IDR >> 0) & 1)  && (preventBounce == 0) ){

		pressCounter++;
		preventBounce = 1;
		idleFlag = 0;
		idleTime = 0;
		displayNumber1 = 3;
		BSP_keypad_press(pressCounter,displayNumber1);
		delay(80000);
	}

	GPIOB->ODR ^= (1U<<6);

	EXTI->RPR1 |= (1U <<0); /* Clear interrupt flag */

	BSP_keypad_set();
}

void EXTI2_3_IRQHandler(void) /* PB2 */ {

	BSP_keypad_clear();

	/*** PB1 - D ***/
	GPIOB->ODR ^= (1U<<1);

	if(((GPIOB->IDR >> 2) & 1)  && (preventBounce == 0)){

		preventBounce = 1;
		idleFlag = 0;
		idleTime = 0;
		divisionFlag=1;
		calculationFlag = 1;
		BSP_displayNumber2_init();
		delay(16000);
	}

	GPIOB->ODR ^= (1U<<1);

	/*** PA15 - C ***/
	GPIOA->ODR ^= (1U<<15);

	if(((GPIOB->IDR >> 2) & 1) && (preventBounce == 0)){

		preventBounce = 1;
		idleFlag = 0;
		idleTime = 0;
		multiplicationFlag = 1;
		calculationFlag = 1;
		BSP_displayNumber2_init();
		delay(16000);
	}

	GPIOA->ODR ^= (1U<<15);

	/*** PB7 - B ***/
	GPIOB->ODR ^= (1U<<7);

	if(((GPIOB->IDR >> 2) & 1)  && (preventBounce == 0) ){

		preventBounce = 1;
		idleFlag = 0;
		idleTime = 0;
		substractionFlag = 1;
		calculationFlag = 1;
		BSP_displayNumber2_init();
		delay(16000);
	}

	GPIOB->ODR ^= (1U<<7);

	/*** PB6 - A ***/
	GPIOB->ODR ^= (1U<<6);

	if(((GPIOB->IDR >> 2) & 1)  && (preventBounce == 0) ){

		preventBounce = 1;
		idleFlag = 0;
		idleTime = 0;
		additionFlag = 1;
		calculationFlag = 1;
		BSP_displayNumber2_init();
		delay(16000);
	}

	GPIOB->ODR ^= (1U<<6);

	EXTI->RPR1 |= (1U <<2); /* Clear interrupt flag */
	BSP_keypad_set();
}

void EXTI4_15_IRQHandler(void) /* PA9 & PA10 */ {

	/*^* PA-10 *^*/
	if((EXTI->RPR1 & (1<<10))){

	BSP_keypad_clear();

	/*** PB1 - * ***/
	GPIOB->ODR ^= (1U<<1);

	if(((GPIOA->IDR >> 10) & 1)  && (preventBounce == 0)){
		/*???*/
		preventBounce = 1;
		idleFlag = 0;
		idleTime = 0;
		scientificFlag++;
		delay(16000);
	}

	GPIOB->ODR ^= (1U<<1);

	/*** PA15 - 7 ***/
	GPIOA->ODR ^= (1U<<15);

	if(((GPIOA->IDR >> 10) & 1)  && (preventBounce == 0) ){

		pressCounter++;
		preventBounce = 1;
		idleFlag = 0;
		idleTime = 0;
		displayNumber1 = 7;
		BSP_keypad_press(pressCounter,displayNumber1);
		delay(16000);
	}

	GPIOA->ODR ^= (1U<<15);

	/*** PB7 - 4 ***/
	GPIOB->ODR ^= (1U<<7);

	if(((GPIOA->IDR >> 10) & 1)  && (preventBounce == 0)){

		pressCounter++;
		preventBounce = 1;
		idleFlag = 0;
		idleTime = 0;
		displayNumber1 = 4;
		BSP_keypad_press(pressCounter,displayNumber1);
		delay(16000);
	}

	GPIOB->ODR ^= (1U<<7);

	/*** PB6 - 1 ***/
	GPIOB->ODR ^= (1U<<6);

	if(((GPIOA->IDR >> 10) & 1)  && (preventBounce == 0)){

		pressCounter++;
		preventBounce = 1;
		idleFlag = 0;
		idleTime = 0;
		displayNumber1 = 1;
		BSP_keypad_press(pressCounter,displayNumber1);
		delay(16000);
	}

	GPIOB->ODR ^= (1U<<6);

	EXTI->RPR1 |= (1U <<10); /* Clear interrupt flag */
	BSP_keypad_set();
	}

	/*^* PA-9 *^*/
	if((EXTI->RPR1 & (1<<9))){

	BSP_keypad_clear();

	/*** PB1 - 0 ***/
	GPIOB->ODR ^= (1U<<1);

	if(((GPIOA->IDR >> 9) & 1) && (preventBounce == 0) ){

		pressCounter++;
		preventBounce = 1;
		idleFlag = 0;
		idleTime = 0;
		displayNumber1 = 0;
		BSP_keypad_press(pressCounter,displayNumber1);
		delay(16000);
	}

	GPIOB->ODR ^= (1U<<1);

	/*** PA15 - 8 ***/
	GPIOA->ODR ^= (1U<<15);

	if(((GPIOA->IDR >> 9) & 1) && (preventBounce == 0) ){

		pressCounter++;
		preventBounce = 1;
		idleFlag = 0;
		idleTime = 0;
		displayNumber1 = 8;
		BSP_keypad_press(pressCounter,displayNumber1);
		delay(16000);
	}

	GPIOA->ODR ^= (1U<<15);

	/*** PB7 - 5 ***/
	GPIOB->ODR ^= (1U<<7);

	if(((GPIOA->IDR >> 9) & 1)  && (preventBounce == 0) ){

		pressCounter++;
		preventBounce = 1;
		idleFlag = 0;
		idleTime = 0;
		displayNumber1 = 5;
		BSP_keypad_press(pressCounter,displayNumber1);
		delay(16000);
	}

	GPIOB->ODR ^= (1U<<7);

	/*** PB6 - 2 ***/
	GPIOB->ODR ^= (1U<<6);

	if(((GPIOA->IDR >> 9) & 1)  && (preventBounce == 0) ){

		pressCounter++;
		preventBounce = 1;
		idleFlag = 0;
		idleTime = 0;
		displayNumber1 = 2;
		BSP_keypad_press(pressCounter,displayNumber1);
		delay(16000);
	}

	GPIOB->ODR ^= (1U<<6);

	EXTI->RPR1 |= (1U <<9); /* Clear interrupt flag */
	BSP_keypad_set();
	}
}
