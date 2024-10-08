#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#include "usb_agent.h"

#include "flash_layout.h"
void JumpToApp(void)
{
	uint32_t address = APP_START_ADDR;

#if CONFIG_ARCH_HAS_USERSPACE
	__ASSERT(!(CONTROL_nPRIV_Msk & __get_CONTROL()),
			 "Not in Privileged mode"); // 跳转前需要在特权模式,CONFIG_USERSPACE=y使得线程在非特权模式(用户态)下跑
#endif

	// uninit_used_peripherals();

	__ISB();
	__disable_irq();
	NVIC_Type *nvic = NVIC;
	/* Disable NVIC interrupts */
	for (uint8_t i = 0; i < ARRAY_SIZE(nvic->ICER); i++)
	{
		nvic->ICER[i] = 0xFFFFFFFF;
	}
	/* Clear pending NVIC interrupts */
	for (uint8_t i = 0; i < ARRAY_SIZE(nvic->ICPR); i++)
	{
		nvic->ICPR[i] = 0xFFFFFFFF;
	}

	SysTick->CTRL = 0;

	/* Disable fault handlers used by the bootloader */
	SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;

	/* Activate the MSP if the core is currently running with the PSP */
	if (CONTROL_SPSEL_Msk & __get_CONTROL())
	{
		__set_CONTROL(__get_CONTROL() & ~CONTROL_SPSEL_Msk);
	}

	__DSB(); /* Force Memory Write before continuing */
	__ISB(); /* Flush and refill pipeline with updated permissions */

	SCB->VTOR = address;
	uint32_t *vector_table = (uint32_t *)address;

#if defined(CONFIG_BUILTIN_STACK_GUARD) && \
	defined(CONFIG_CPU_CORTEX_M_HAS_SPLIM)
	/* Reset limit registers to avoid inflicting stack overflow on image
	 * being booted.
	 * CONFIG_BUILTIN_STACK_GUARD 线程和中断栈溢出检测
	 * CONFIG_CPU_CORTEX_M_HAS_SPLIM ARMv8-M之类的型号有SP限制寄存器,用来检测栈溢出,nrf52840为Cortex-M4,Armv7-M,不需要考虑
	 */
	__set_PSPLIM(0);
	__set_MSPLIM(0);
#endif

	/* Set MSP to the new address and clear any information from PSP */
	__set_MSP(vector_table[0]);
	__set_PSP(0);

	/* Call reset handler. */
	((void (*)(void))vector_table[1])();
	CODE_UNREACHABLE;
}

#include "hal/nrf_gpio.h"

#include "dfu_nrf52.h"
int DfuCheck(void)
{
	// todo 按键电平
	nrf_gpio_cfg_input(NRF_GPIO_PIN_MAP(0, 11), NRF_GPIO_PIN_PULLUP);

	// 这里需要延时判断,gpio寄生电容导致电压上升慢
	volatile bool dfu = true;
	for (int i = 0; i < 200; i++)
	{
		if (nrf_gpio_pin_read(NRF_GPIO_PIN_MAP(0, 11)) == 1)
		{
			dfu = false;
			break;
		}
	}

	// if (!dfu)
	if (!dfu && IsAppValid())
	{
		JumpToApp();
	}

	return 0;
}

#include "comm.h"
#include "cl_event_system.h"
#include "dfu.h"

void Thread_Dfu(void)
{
	CL_EventSysInit();

	Comm_Init();
	Dfu_Init();
	while (true)
	{
		Comm_Process();
		Dfu_Process();
	}
}

#include <zephyr/console/console.h>
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
void Thread_Blink(void)
{
	int ret;

	if (!gpio_is_ready_dt(&led))
	{
		return;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0)
	{
		return;
	}

	while (1)
	{
		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0)
		{
			return;
		}

		k_msleep(2000);
		printk("blink");
	}
	return;
}

#define STACKSIZE 1024
K_THREAD_DEFINE(blink_id, STACKSIZE, Thread_Blink, NULL, NULL, NULL,
				1, 0, 0);

K_THREAD_DEFINE(dfu_id, 40960, Thread_Dfu, NULL, NULL, NULL,
				2, 0, 0);

SYS_INIT(DfuCheck, EARLY, 1);
