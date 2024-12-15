#include "servo.h"

uint16_t adc_buff[2]={0};

void servo_init(void)
{
    HAL_TIM_Base_Start(&htim2);
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
    HAL_ADC_Start_DMA(&hadc2,(uint32_t *)adc_buff,2);

}
void servo_test(void)
{
    
}