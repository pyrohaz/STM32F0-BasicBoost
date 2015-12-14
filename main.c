#include <stm32f0xx_gpio.h>
#include <stm32f0xx_rcc.h>
#include <stm32f0xx_tim.h>

GPIO_InitTypeDef G;
TIM_TimeBaseInitTypeDef TI;
TIM_OCInitTypeDef TO;
TIM_BDTRInitTypeDef TB;

//Output frequency (Hz)
#define OUTPUT_FREQ		150000

//Duty cycle (%)
#define DUTY_CYCLE		75

//1 millisecond time base
volatile uint32_t MSec = 0;

void SysTick_Handler(void){
	MSec++;
}

//Millisecond delay function
void Delay(uint32_t D){
	uint32_t MSS = MSec;
	while((MSec-MSS)<D) __NOP;
}

int main(void)
{
	//Enable clocks!
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	//Enable timer outputs, PA7 is the PWM output and PA6 is the break input
	G.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	G.GPIO_Mode = GPIO_Mode_AF;
	G.GPIO_OType = GPIO_OType_PP;
	G.GPIO_PuPd = GPIO_PuPd_UP;
	G.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &G);

	//Set PA6 and PA7 to alternate functions (TIM1)
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_2);

	//Initialize TIM1 at the required output frequecy
	TI.TIM_ClockDivision = TIM_CKD_DIV1;
	TI.TIM_CounterMode = TIM_CounterMode_Up;
	TI.TIM_Period = SystemCoreClock/OUTPUT_FREQ;
	TI.TIM_Prescaler = 0;
	TI.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TI);

	//Setup the PWM output at the required duty cycle
	TO.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TO.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TO.TIM_OCMode = TIM_OCMode_PWM2;
	TO.TIM_OutputNState = TIM_OutputNState_Enable;
	TO.TIM_Pulse = SystemCoreClock*DUTY_CYCLE/(OUTPUT_FREQ*100);
	TIM_OC1Init(TIM1, &TO);

	//Enable the break input!
	TB.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
	TB.TIM_Break = TIM_Break_Enable;
	TB.TIM_BreakPolarity = TIM_BreakPolarity_High;
	TB.TIM_DeadTime = 0;
	TB.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
	TB.TIM_OSSIState = TIM_OSSIState_Enable;
	TB.TIM_OSSRState = TIM_OSSRState_Enable;
	TIM_BDTRConfig(TIM1, &TB);
	TIM_Cmd(TIM1, ENABLE);

	//Enable PWM output!
	TIM_CtrlPWMOutputs(TIM1, ENABLE);

	//Setup the millisecond timer
	SysTick_Config(SystemCoreClock/1000);

	//Do nothing while the PWM PWMs!
	while(1){
	}
}
