/*
 * board-common.c: Implement function which is common across
 * different boards.
 *
 * Copyright (c) 2011-2012, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <linux/clk.h>
#include <linux/serial_8250.h>

#include <mach/clk.h>

#include "board.h"
#include "board-common.h"
#include "devices.h"
#include "clock.h"

extern unsigned long  debug_uart_port_base;
extern struct clk *debug_uart_clk;

struct platform_device *uart_console_debug_device = NULL;

struct platform_device vibrator_device = {
	.name = "tegra-vibrator",
	.id = -1,
};

int tegra_vibrator_init(void)
{
	return platform_device_register(&vibrator_device);
}

int uart_console_debug_init(int default_debug_port)
{
	int debug_port_id;

	debug_port_id = get_tegra_uart_debug_port_id();
	if (debug_port_id < 0)
		debug_port_id = default_debug_port;

	if (debug_port_id < 0) {
		pr_warn("No debug console channel\n");
		return -EINVAL;
	}

	switch (debug_port_id) {
	case 0:
		/* UARTA is the debug port. */
		pr_info("Selecting UARTA as the debug console\n");
		debug_uart_clk = clk_get_sys("serial8250.0", "uarta");
		debug_uart_port_base = ((struct plat_serial8250_port *)(
			debug_uarta_device.dev.platform_data))->mapbase;
		uart_console_debug_device = &debug_uarta_device;
		break;

	case 1:
		/* UARTB is the debug port. */
		pr_info("Selecting UARTB as the debug console\n");
		debug_uart_clk =  clk_get_sys("serial8250.0", "uartb");
		debug_uart_port_base = ((struct plat_serial8250_port *)(
			debug_uartb_device.dev.platform_data))->mapbase;
		uart_console_debug_device = &debug_uartb_device;
		break;

	case 2:
		/* UARTC is the debug port. */
		pr_info("Selecting UARTC as the debug console\n");
		debug_uart_clk =  clk_get_sys("serial8250.0", "uartc");
		debug_uart_port_base = ((struct plat_serial8250_port *)(
			debug_uartc_device.dev.platform_data))->mapbase;
		uart_console_debug_device = &debug_uartc_device;
		break;

	case 3:
		/* UARTD is the debug port. */
		pr_info("Selecting UARTD as the debug console\n");
		debug_uart_clk =  clk_get_sys("serial8250.0", "uartd");
		debug_uart_port_base = ((struct plat_serial8250_port *)(
			debug_uartd_device.dev.platform_data))->mapbase;
		uart_console_debug_device = &debug_uartd_device;
		break;

#if !defined(CONFIG_ARCH_TEGRA_2x_SOC)
	case 4:
		/* UARTE is the debug port. */
		pr_info("Selecting UARTE as the debug console\n");
		debug_uart_clk =  clk_get_sys("serial8250.0", "uarte");
		debug_uart_port_base = ((struct plat_serial8250_port *)(
			debug_uarte_device.dev.platform_data))->mapbase;
		uart_console_debug_device = &debug_uarte_device;
		break;
#endif

	default:
		pr_info("The debug console id %d is invalid, Assuming UARTA", debug_port_id);
		debug_uart_clk = clk_get_sys("serial8250.0", "uarta");
		debug_uart_port_base = ((struct plat_serial8250_port *)(
			debug_uarta_device.dev.platform_data))->mapbase;
		uart_console_debug_device = &debug_uarta_device;
		break;
	}

	if (!IS_ERR_OR_NULL(debug_uart_clk)) {
		struct clk *c;
		pr_info("The debug console clock name is %s\n",
						debug_uart_clk->name);
		c = tegra_get_clock_by_name("pll_p");
		if (IS_ERR_OR_NULL(c))
			pr_err("Not getting the parent clock pll_p\n");
		else
			clk_set_parent(debug_uart_clk, c);

		tegra_clk_prepare_enable(debug_uart_clk);
		clk_set_rate(debug_uart_clk, clk_get_rate(c));
	} else {
		pr_err("Not getting the clock for debug consolei %d\n",
			debug_port_id);
	}
	return debug_port_id;
}