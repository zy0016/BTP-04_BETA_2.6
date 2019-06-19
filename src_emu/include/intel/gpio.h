#ifndef __GPIO_H
#define __GPIO_H

#define FALLING_EDGE	0x11
#define RISING_EDGE		0x22
#define BOTH_EDGE	0x33
#define GPIO_NUM	121

int register_gpio_irq(int gpio_number, int edge, void(* irq_handler)(int, void *, struct pt_regs *));
int unregester_gpio_irq(int gpio_number);

#endif
