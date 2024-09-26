#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#include "usb_agent.h"

int JumpToApp(void)
{
	uint32_t address = 0x25800;

#if CONFIG_ARCH_HAS_USERSPACE
	__ASSERT(!(CONTROL_nPRIV_Msk & __get_CONTROL()),
			 "Not in Privileged mode"); //跳转前需要在特权模式,CONFIG_USERSPACE=y使得线程在非特权模式(用户态)下跑
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
	return 0;
}

#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

void Thread_Dfu(void)
{
	JumpToApp();

	while (true)
	{
		uint8_t buff[64];
		uint32_t recvLen = RecvData(buff, sizeof(buff));
		if (recvLen)
		{
			SendData(buff, recvLen);
		}
	}
}

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

		k_msleep(1000);
	}
	return;
}

#define STACKSIZE 1024
K_THREAD_DEFINE(blink_id, STACKSIZE, Thread_Blink, NULL, NULL, NULL,
				1, 0, 0);

K_THREAD_DEFINE(dfu_id, STACKSIZE, Thread_Dfu, NULL, NULL, NULL,
				2, 0, 0);

SYS_INIT(JumpToApp, EARLY, 1);
