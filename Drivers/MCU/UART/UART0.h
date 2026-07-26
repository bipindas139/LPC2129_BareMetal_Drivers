#ifndef UART0_H
#define UART0_H

// ****** UART0 ********

#include <lpc21xx.h>

typedef unsigned int  u32;
typedef  int s32;
typedef unsigned char u8;
typedef char s8;

void uart0_init(u32 );
void uart0_tx(u8 );
void uart0_tx_string(const s8 *);
u8  uart0_rx(void);
void uart0_rx_string(s8 *, u32 );
u8 uart0_rx_timeout(u8 *, u32 );
void uart0_float(float );
void uart0_integer(s32 );
void uart0_hex32(u32);
void uart0_hex(u8 );

#endif
