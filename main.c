#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "button.h"
volatile uint32 SysTickCount;
void SysTickCallback(void)
{
	SysTickCount++;
}

uint32_t getTickCount(void) {return SysTickCount;}
void initSystick(void)
{
	Cy_SysTick_Init(CY_SYSTICK_CLOCK_SOURCE_CLK_CPU, (SystemCoreClock/1000)-1);
	Cy_SysTick_SetCallback(0,SysTickCallback);
	Cy_SysTick_Enable();
}

button_var button1_obj;

void button_dbg_msg(uint8_t state )
{
	switch(state)
	{
		case PRESSED_btn :
			printf(" Button state : Pressed \r\n");
			break;
		case DOUBLE_PRESSED_btn:
			printf(" Button state : Double Click \r\n");
			break;
		case HELD_btn:
			printf(" Button state : Long press \r\n");
			break;
		case RELEASED_btn :
			printf(" Button state : Released \r\n");
			break;
		default :
			printf(" Button state : waiting for action \r\n");
			break;
	}
}


int main(void)
{
    cy_rslt_t result;

#if defined (CY_DEVICE_SECURE)
    cyhal_wdt_t wdt_obj;

    /* Clear watchdog timer so that it doesn't trigger a reset */
    result = cyhal_wdt_init(&wdt_obj, cyhal_wdt_get_max_timeout_ms());
    CY_ASSERT(CY_RSLT_SUCCESS == result);
    cyhal_wdt_free(&wdt_obj);
#endif

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    initSystick();
    /* Enable global interrupts */
    __enable_irq();
    printf(" button testing \r\n");
    button.setTick(getTickCount);
    if( button.create(&button1_obj, P0_4, BUTTON_LOW, 1000) == CY_RSLT_SUCCESS )
    	printf(" button initialize success \r\n");
    else
    	printf(" button-obj error \r\n");

    uint32 timePrev = getTickCount();
    uint8_t button_state = 0;
    for(;;)
    {
    	button_state =button.check(&button1_obj);
    	if ( button_state != WAITING_btn )
    		button_dbg_msg(button_state);

        if( getTickCount() - timePrev >= 1000)
        {
            uint32_t timeNow = button.tick();
            printf(" duration : %u \r\n", (timeNow - timePrev));
            timePrev = timeNow;
        }
    }
}

/* [] END OF FILE */
