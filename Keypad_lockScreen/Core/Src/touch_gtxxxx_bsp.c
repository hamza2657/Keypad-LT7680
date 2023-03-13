#include "main.h"

#include "touch_gtxxxx_defs.h"
#include "touch_gtxxxx.h"



//HW configuration
#define CTP_GPIO_RCC_ENABLE   __HAL_RCC_GPIOD_CLK_ENABLE()
#define CTP_RST_PIN           GPIO_PIN_0
#define CTP_RST_PORT          GPIOD
#define CTP_INT_PIN           GPIO_PIN_1
#define CTP_INT_PORT          GPIOD
#define CTP_INT_IRQn          EXTI1_IRQn
#define CTP_INT_IRQHandler    EXTI1_IRQHandler

extern I2C_HandleTypeDef hi2c3;  //main.c from CubeMX
void MX_I2C3_Init(void);         //main.c from CubeMX
void CTP_TS_Work_Func(void);     //touch_gtxxxx.c



//OS dependet functions

  //No OS
  static int16_t ctX=0;
  static int16_t ctY=0;
  static uint8_t ctPress=0;

  void ctDelay(uint32_t ms){
    HAL_Delay(ms);
  }
  void ctMessagePut(uint16_t x,uint16_t y,uint8_t press){
    ctX=x; ctY=y; ctPress=press;
  }
  uint8_t ctMessageGet(uint16_t* x,uint16_t* y,uint8_t* press){
    HAL_NVIC_DisableIRQ(CTP_INT_IRQn);
    *x=ctX; *y=ctY; *press=ctPress;
    HAL_NVIC_EnableIRQ(CTP_INT_IRQn);
    return 1;
  }


//MCU dependet functions


uint8_t Exti_EN=1;

void I2C_ResetChip(void){
    if(Exti_EN){
      HAL_NVIC_DisableIRQ(CTP_INT_IRQn);
    }
    GPIO_InitTypeDef GPIO_Initure;
    //1. Reset out low
    CTP_RST_PORT->BSRR=CTP_RST_PIN<<16; //RST = 0
	ctDelay(5);
    //2. Int out low / high
    GPIO_Initure.Pin=CTP_INT_PIN;            
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;             
	HAL_GPIO_Init(CTP_INT_PORT,&GPIO_Initure);
    CTP_INT_PORT->BSRR=CTP_INT_PIN<<16; //INT = 0
    ctDelay(2); //>100us
    //3. Reset out high
	CTP_RST_PORT->BSRR=CTP_RST_PIN<<0;  //RST = 1
	ctDelay(7); //5-10 ms
    //4. INT out low
    CTP_INT_PORT->BSRR=CTP_INT_PIN<<16; //INT = 0
    ctDelay(55); //>50 ms
    //5. INT input floating
	GPIO_Initure.Pin=CTP_INT_PIN;
    GPIO_Initure.Mode = GPIO_MODE_IT_FALLING;
	GPIO_Initure.Mode=GPIO_MODE_INPUT;
	HAL_GPIO_Init(CTP_INT_PORT,&GPIO_Initure);
    if(Exti_EN){
      HAL_NVIC_EnableIRQ(CTP_INT_IRQn);
    }
}

void EXTI_IRQ_enable(){
    Exti_EN=1;
    HAL_NVIC_EnableIRQ(CTP_INT_IRQn);
}



//In CubeMX exti must be swihed on
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  if(GPIO_Pin == CTP_INT_PIN){

	  CTP_TS_Work_Func();

  }
}

void I2C_Touch_Init(void){
    HAL_NVIC_DisableIRQ(CTP_INT_IRQn);
    HAL_NVIC_SetPriority(CTP_INT_IRQn, 15, 15);
    GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOD_CLK_ENABLE();		//Turn on the GPIOD clock
	
    GPIO_Initure.Pin=CTP_RST_PIN;            
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;
    GPIO_Initure.Pull=GPIO_NOPULL;
	HAL_GPIO_Init(CTP_RST_PORT,&GPIO_Initure);
    CTP_RST_PORT->BSRR=CTP_RST_PIN<<16; //RST = 0
    ctDelay(10);

    I2C_ResetChip();
    ctDelay(50);
    MX_I2C3_Init();
}

int I2C_Transfer( struct i2c_msg *msgs,int num){
	int im = 0;
	int ret = 0;
	for (im = 0; ret == 0 && im != num; im++){
		if ((msgs[im].flags&CTP_I2C_M_RD)){
			HAL_I2C_Master_Receive(&hi2c3,CTP_ADDRESS+CTP_I2C_M_RD /*msgs[im].addr*/, msgs[im].buf, msgs[im].len,15+msgs[im].len/10);
            ret=0;
		}else{
			HAL_I2C_Master_Transmit(&hi2c3,CTP_ADDRESS /*msgs[im].addr*/,  msgs[im].buf, msgs[im].len,15+msgs[im].len/10);
            ret=0;
		}
	}

	if(ret)
		return ret;

	return im;   													
}
