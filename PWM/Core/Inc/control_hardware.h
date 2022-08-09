/*
 * for Elevator
 */

#ifndef __CONTROL_HARDWARE_H
#define __CONTROL_HARDWARE_H

#define NS400 asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP")
#define NS800 asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP"); asm("NOP")

void led_ring_bit(uint8_t x);
void led_ring_update(uint32_t *data_24bit);
uint32_t led_ring_rgb(uint8_t red, uint8_t green, uint8_t blue);

#endif
