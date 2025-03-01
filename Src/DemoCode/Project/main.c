/*************************
 * stm32 uart main.c
 *************************/
 
#include "stm32f10x.h"
 
/* User defined function prototypes */
void GPIOD_Init(void);
void USART1_Init(void);
void led_toggle(void);
 
int main(void)
{
    /* Initialize GPIOA PIN8 */
    GPIOD_Init();
    /* Initialize USART1 */
    USART1_Init();
 
    while(1)
    {
				USART_SendData(USART1, 'a');
        /* Do nothing, all happens in ISR */
    }
}   
 
/***********************************************
 * Initialize GPIOA PIN8 as push-pull output
 ***********************************************/
void GPIOD_Init(void)
{
    /* Bit configuration structure for GPIOA PIN8 */
    GPIO_InitTypeDef gpiod_init_struct = { GPIO_Pin_2, GPIO_Speed_50MHz, 
                                           GPIO_Mode_Out_PP };
                                             
    /* Enable PORT A clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    /* Initialize GPIOA: 50MHz, PIN8, Push-pull Output */
    GPIO_Init(GPIOD, &gpiod_init_struct);   
     
    /* Turn off LED to start with */
    GPIO_SetBits(GPIOD, GPIO_Pin_2);
}
 
/*****************************************************
 * Initialize USART1: enable interrupt on reception
 * of a character
 *****************************************************/
void USART1_Init(void)
{
    /* USART configuration structure for USART1 */
    USART_InitTypeDef usart1_init_struct;
    /* Bit configuration structure for GPIOA PIN9 and PIN10 */
    GPIO_InitTypeDef gpioa_init_struct;
     
    /* Enalbe clock for USART1, AFIO and GPIOA */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO | 
                           RCC_APB2Periph_GPIOA, ENABLE);
                            
    /* GPIOA PIN9 alternative function Tx */
    gpioa_init_struct.GPIO_Pin = GPIO_Pin_9;
    gpioa_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpioa_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpioa_init_struct);
    /* GPIOA PIN9 alternative function Rx */
    gpioa_init_struct.GPIO_Pin = GPIO_Pin_10;
    gpioa_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpioa_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpioa_init_struct);
 
    /* Enable USART1 */
    USART_Cmd(USART1, ENABLE);  
    /* Baud rate 9600, 8-bit data, One stop bit
     * No parity, Do both Rx and Tx, No HW flow control
     */
    usart1_init_struct.USART_BaudRate = 9600;   
    usart1_init_struct.USART_WordLength = USART_WordLength_8b;  
    usart1_init_struct.USART_StopBits = USART_StopBits_1;   
    usart1_init_struct.USART_Parity = USART_Parity_No ;
    usart1_init_struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart1_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    /* Configure USART1 */
    USART_Init(USART1, &usart1_init_struct);
    /* Enable RXNE interrupt */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    /* Enable USART1 global interrupt */
    NVIC_EnableIRQ(USART1_IRQn);
}
 
/*******************************************
 * Toggle LED 
 *******************************************/
void led_toggle(void)
{
    /* Read LED output (GPIOD PIN2) status */
    uint8_t led_bit = GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_2);
     
    /* If LED output set, clear it */
    if(led_bit == (uint8_t)Bit_SET)
    {
        GPIO_ResetBits(GPIOD, GPIO_Pin_2);
    }
    /* If LED output clear, set it */
    else
    {
        GPIO_SetBits(GPIOD, GPIO_Pin_2);
    }
}
 
/**********************************************************
 * USART1 interrupt request handler: on reception of a 
 * character 't', toggle LED and transmit a character 'T'
 *********************************************************/
void USART1_IRQHandler(void)
{
    /* RXNE handler */
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
				char t = (char)USART_ReceiveData(USART1);
        /* If received 't', toggle LED and transmit 'T' */
        if( t == 't')
        {
            led_toggle();
						USART_ClearITPendingBit(USART1, USART_IT_RXNE); 
            USART_SendData(USART1, 'A');
            /* Wait until Tx data register is empty, not really 
             * required for this example but put in here anyway.
             */
            
            //while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
           // {
            //}
        }
				// USART_ClearITPendingBit(USART1, USART_IT_RXNE); 
    }
     
    /* ------------------------------------------------------------ */
    /* Other USART1 interrupts handler can go here ...             */
}
