/*
 * Copyright (c) 2016 Intel Corporation
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file Sample app to demonstrate PWM.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));

// #define MIN_PERIOD PWM_SEC(1U) / 128U
// #define MAX_PERIOD PWM_SEC(1U)

#define STEP 100    /* PWM pulse step */
#define MINPULSEWIDTH 500  /* Servo 0 degrees */
#define MIDDLEPULSEWIDTH 1500   /* Servo 90 degrees */
#define MAXPULSEWIDTH 2500  /* Servo 180 degrees */
/* period of servo motor signal ->  20ms (50Hz) */
#define PERIOD (USEC_PER_SEC / 50U)
#define SLEEP_TIME_S 1  /* Pause time in seconds*/

void PWM_control(uint8_t *dir, uint32_t *pulse_width)
{
	if (*dir == 1U)
	{
		if (*pulse_width < MAXPULSEWIDTH)
		{
			*pulse_width += STEP;
		}
		else
		{
			*dir = 0U;
		}
	}
	else if (*dir == 0U)
	{
		if (*pulse_width > MINPULSEWIDTH)
		{
			*pulse_width -= STEP;
		}
		else
		{
			*dir = 1U;
		}
	}
}

void get_deegrees(uint8_t *degrees, uint32_t *pulse_width)
{
	*degrees =  (uint8_t)((float)(*pulse_width - MINPULSEWIDTH) / (float)(MAXPULSEWIDTH - MINPULSEWIDTH) * 180.0);
}

int main(void)
{
	// uint32_t max_period;
	// uint32_t period;
	// uint8_t dir = 0U;
	int ret;
	printk("Servo control program\n");
	/* Set PWM starting positions as 0 degrees */
	uint32_t pulse_width = MINPULSEWIDTH;
	uint8_t dir = 0U;
	uint8_t degrees = 0U;

	printk("PWM-based blinky\n");

	if (!pwm_is_ready_dt(&pwm_led0)) {
		printk("Error: PWM device %s is not ready\n",
		       pwm_led0.dev->name);
		return 0;
	}

	// /*
	//  * In case the default MAX_PERIOD value cannot be set for
	//  * some PWM hardware, decrease its value until it can.
	//  *
	//  * Keep its value at least MIN_PERIOD * 4 to make sure
	//  * the sample changes frequency at least once.
	//  */
	// printk("Calibrating for channel %d...\n", pwm_led0.channel);
	// max_period = MAX_PERIOD;
	// while (pwm_set_dt(&pwm_led0, max_period, max_period / 2U)) {
	// 	max_period /= 2U;
	// 	if (max_period < (4U * MIN_PERIOD)) {
	// 		printk("Error: PWM device "
	// 		       "does not support a period at least %lu\n",
	// 		       4U * MIN_PERIOD);
	// 		return 0;
	// 	}
	// }

	// printk("Done calibrating; maximum/minimum periods %u/%lu nsec\n",
	//        max_period, MIN_PERIOD);

	while (1) {
		// ret = pwm_set_dt(&pwm_led0, period, period / 2U);
		ret = pwm_set_dt(&pwm_led0, PWM_USEC(PERIOD), PWM_USEC(pulse_width));
		if (ret) {
			printk("Error %d: failed to set pulse width\n", ret);
			return 0;
		}

		// period = dir ? (period * 2U) : (period / 2U);
		// if (period > max_period) {
		// 	period = max_period / 2U;
		// 	dir = 0U;
		// } else if (period < MIN_PERIOD) {
		// 	period = MIN_PERIOD * 2U;
		// 	dir = 1U;
		// }

		get_deegrees(&degrees, &pulse_width);

		printk("PWM pulse width: %d\n", pulse_width);
		printk("Degrees: %d\n", degrees);
                
        PWM_control(&dir, &pulse_width);

        printk("\n");

		k_sleep(K_SECONDS(SLEEP_TIME_S));
	}
	return 0;
}
