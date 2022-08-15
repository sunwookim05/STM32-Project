#include "main.h"
#include "led_ring.h"

#ifndef __APP_H_
#define __APP_H_

typedef char *String;
typedef uint8_t boolean;
typedef enum{false, true} _BOOL;
typedef enum {
	OVER, SAFE, OFF, ONN, AUTO, ON
} gstat;
typedef struct {
	boolean over, safe, off, onn, au, on;
} Statflag;

void setUp();
void appLoop();

#endif
