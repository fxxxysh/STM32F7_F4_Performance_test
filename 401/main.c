/**
  ******************************************************************************
  * @file    UART/UART_Printf/Src/main.c 
  * @author  MCD Application Team
  * @version V1.2.4
  * @date    29-January-2016
  * @brief   This example shows how to retarget the C library printf function 
  *          to the UART.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "arm_math.h"
/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup UART_Printf
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* UART handler declaration */
UART_HandleTypeDef UartHandle;

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
static void SystemClock_Config(void);
static void Error_Handler(void);

/* Private functions ---------------------------------------------------------*/
#define MAX_BLOCKSIZE   32//测试数据个数
	//单精度浮点测试数据
 float32_t testInput_f32[MAX_BLOCKSIZE] =
{
  -1.244916875853235400f,  -4.793533929171324800f,   0.360705030233248850f,   0.827929644170887320f,  -3.299532218312426900f,   3.427441903227623800f,   3.422401784294607700f,  -0.108308165334010680f,
   0.941943896490312180f,   0.502609575000365850f,  -0.537345278736373500f,   2.088817392965764500f,  -1.693168684143455700f,   6.283185307179590700f,  -0.392545884746175080f,   0.327893095115825040f,
   3.070147440456292300f,   0.170611405884662230f,  -0.275275082396073010f,  -2.395492805446796300f,   0.847311163536506600f,  -3.845517018083148800f,   2.055818378415868300f,   4.672594161978930800f,
  -1.990923030266425800f,   2.469305197656249500f,   3.609002606064021000f,  -4.586736582331667500f,  -4.147080139136136300f,   1.643756718868359500f,  -1.150866392366494800f,   1.985805026477433800f

};
//双精度浮点测试数据
 float64_t testInput_f64[MAX_BLOCKSIZE] =
{
  -1.244916875853235400,  -4.793533929171324800,   0.360705030233248850,   0.827929644170887320,  -3.299532218312426900,   3.427441903227623800,   3.422401784294607700f,  -0.108308165334010680,
   0.941943896490312180,   0.502609575000365850,  -0.537345278736373500,   2.088817392965764500,  -1.693168684143455700,   6.283185307179590700,  -0.392545884746175080f,   0.327893095115825040,
   3.070147440456292300,   0.170611405884662230,  -0.275275082396073010,  -2.395492805446796300,   0.847311163536506600,  -3.845517018083148800,   2.055818378415868300f,   4.672594161978930800,
  -1.990923030266425800,   2.469305197656249500,   3.609002606064021000,  -4.586736582331667500,  -4.147080139136136300,   1.643756718868359500,  -1.150866392366494800f,   1.985805026477433800

};
//32位整形测试数据
 int32_t testInput_q32[MAX_BLOCKSIZE] =
{
  244916875,  171324800,   33248850,   70887320, 83124269,   27623800,   94607700, 4010680,
  240312180,  500365850,  36373500,  965764500,  143455700,  79590700,  46175080,   278930951,
 56292300,   884662230, 39607010,  446796300,   65066009,  83148808,   868300111,  78930800,
 266425800,  6249500,   64021000,  586736582,  36136300,  868359500,   664948001,  77433800

};
//运算输出值
uint32_t blockSize = 32;
float32_t  cosOutput;
float32_t  sinOutput;
uint32_t  Output_q32;
float32_t  Output_f32;
float32_t  Output_f32_A[32]={0};
float64_t  Output_f64;

uint32_t tickstart = 0;//每次测试开始值
uint32_t tickfor = 0;//for循环耗时
uint32_t tick_sincos = 0;//sincos函数不使用DSP库耗时
uint32_t tick_sincos_dsp = 0;//sincos函数使用DSP库耗时
uint32_t tick_mult_q32 = 0;//32整形位整形做乘法运算耗时
uint32_t tick_mult_f32 = 0;//单精度浮点数做乘法运算耗时
uint32_t tick_mult_f32_dsp = 0;//单精度浮点数使用DSP库做乘法运算耗时
uint32_t tick_mult_f64 = 0;//双精度浮点数做乘法运算耗时



/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
  HAL_Init();
  
  /* Configure the system clock to 84 MHz */
  SystemClock_Config();
   
	  HAL_InitTick(TICK_INT_PRIORITY);
	
	
  /*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART1 configured as follow:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = ODD parity
      - BaudRate = 9600 baud
      - Hardware flow control disabled (RTS and CTS signals) */
  UartHandle.Instance          = USARTx;
  
  UartHandle.Init.BaudRate     = 9600;
  UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits     = UART_STOPBITS_1;
  UartHandle.Init.Parity       = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode         = UART_MODE_TX_RX;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    
  if(HAL_UART_Init(&UartHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(); 
  }
  
  /* Output a message on Hyperterminal using printf function */
//  printf("\n\r UART Printf Example: retarget the C library printf function to the UART\n\r");
  /* Infinite loop */ 
  while (1)
  {
		//1000次sincos运算不使用DSP库耗时
			tickstart=HAL_GetTick();
	for(int j=0;j<1000;j++){
	 for(int i=0; i< blockSize; i++)
		{
    cosOutput = cos(testInput_f32[i]);
    sinOutput = sin(testInput_f32[i]);
		}
	}
	tick_sincos=HAL_GetTick()-tickstart;
	
			//1000次sincos运算使用DSP库耗时
		tickstart=HAL_GetTick();
	for(int j=0;j<1000;j++){
	 for(int i=0; i< blockSize; i++)
		{
    cosOutput = arm_cos_f32(testInput_f32[i]);
    sinOutput = arm_sin_f32(testInput_f32[i]);
		}
	}
	tick_sincos_dsp=HAL_GetTick()-tickstart;
	
		//10000次32位数据相乘运算耗时
		tickstart=HAL_GetTick();
	for(int j=0;j<10000;j++){
	 for(int i=0; i< blockSize; i++)
		{
    Output_q32 = testInput_q32[i]*testInput_q32[i];
		}
	}
	tick_mult_q32=HAL_GetTick()-tickstart;
	
		//10000次单精度数据相乘运算耗时
			tickstart=HAL_GetTick();
	for(int j=0;j<10000;j++){
	 for(int i=0; i< blockSize; i++)
		{
    Output_f32 = testInput_f32[i]*testInput_f32[i];
		}
	}
	tick_mult_f32=HAL_GetTick()-tickstart;
		
	//10000次双精度数据相乘运算耗时
			tickstart=HAL_GetTick();
	for(int j=0;j<10000;j++){
	 for(int i=0; i< blockSize; i++)
		{
    Output_f64 = testInput_f64[i]*testInput_f64[i];
		}
	}
	tick_mult_f64=HAL_GetTick()-tickstart;
	
		//10000次单精度数据相乘运算使用DSP库耗时
		tickstart=HAL_GetTick();
		for(int j=0;j<10000;j++){
			 arm_mult_f32(&testInput_f32[0], &testInput_f32[0], &Output_f32_A[0], blockSize);
		}
		tick_mult_f32_dsp=HAL_GetTick()-tickstart;
		
		//10000次for循环耗时
			tickstart=HAL_GetTick();
		for(int j=0;j<10000;j++){
		}
		tickfor=HAL_GetTick()-tickstart;
		
		//输出测试结果
		printf("tick_sincos=%d,tick_sincos_dsp=%d,tick_mult_q32=%d,tick_mult_f32=%d,tick_mult_f64=%d,tick_mult_f32_dsp=%d\n\r",tick_sincos,tick_sincos_dsp,tick_mult_q32,tick_mult_f32,tick_mult_f64,tick_mult_f32_dsp);

  }
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xFFFF); 

  return ch;
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 84000000
  *            HCLK(Hz)                       = 84000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 16000000
  *            PLL_M                          = 16
  *            PLL_N                          = 336
  *            PLL_P                          = 4
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale2 mode
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  
  /* Enable HSI Oscillator and activate PLL with HSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 0x10;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
 
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* Turn LED2 on */
  BSP_LED_On(LED2);
  while(1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
