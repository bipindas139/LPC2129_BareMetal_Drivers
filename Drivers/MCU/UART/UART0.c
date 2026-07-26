
/******************************************************************************
 * File        : UART0.c
 * Description : UART0 Driver for LPC2129
 * MCU         : LPC2129
 * Author      : Bipindas K V
 * Date        : July 2026
 ******************************************************************************/

#include "UART_header.h"

#define UART0_THRE (U0LSR & (1 << 5))
#define UART0_RDR (U0LSR & (1 << 0))
#define UART0_DECIMAL 3

/******************************************************************************
 * @brief  Initialize UART0 peripheral.
 * @param  baud_rate Desired baud rate (e.g., 9600, 115200).
 * @return None.
 ******************************************************************************/
void uart0_init(u32 baud_rate)
{
  u32 pclk, d;
  if (VPBDIV == 0)
    pclk = 15000000;
  else if (VPBDIV == 1)
    pclk = 60000000;
  else if (VPBDIV == 2)
    pclk = 30000000;

  PINSEL0 |= (1 << 0) | (1 << 2); // P0.0 -> TxD, P0.1 -> RxD

  d = pclk / (16 * baud_rate); //// Baud rate setting formula
  U0LCR = 0x83;                // Enable access to Divisor Latches(Bit 7 = 1), Word Length Select: 8 bit character length(bit 1:0 = 3)
  U0DLL = d & 0xff;
  U0DLM = (d >> 8) & 0xff;
  U0LCR = 0x03; // Disable access to Divisor Latches(Bit 7 = 1), Word Length Select: 8 bit character length(bit 1:0 = 3)
}

/******************************************************************************
 * @brief  Transmit a single 8-bit character over UART0 (blocking).
 * @param  data 8-bit unsigned integer to transmit.
 * @return None.
 ******************************************************************************/
void uart0_tx(u8 data)
{
  while (UART0_THRE == 0)
    ;
  U0THR = data;
}

/******************************************************************************
 * @brief  Receive a single 8-bit character from UART0 (blocking).
 * @param  None.
 * @return Received 8-bit unsigned character.
 ******************************************************************************/
u8 uart0_rx(void)
{
  while (UART0_RDR == 0)
    ;
  return U0RBR;
}

/******************************************************************************
 * @brief  Receive a single character from UART0 with a millisecond timeout.
 * @param  data Pointer to store the received 8-bit character.
 * @param  timeout_ms Maximum time to wait in milliseconds.
 * @return 1 on successful reception, 0 if timeout occurs.
 ******************************************************************************/
u8 uart0_rx_timeout(u8 *data, u32 timeout_ms)
{

  if (data == 0)
  {
    return 0;
  }
  while (1)
  {
    if (UART0_RDR)
    {
      *data = U0RBR;
      return 1;
    }

    if (timeout_ms == 0)
    {
      break;
    }

    delay_ms(1);
    timeout_ms--;
  }

  return 0;
}

/******************************************************************************
 * @brief  Transmit a null-terminated string over UART0 (blocking).
 * @param  p Pointer to the null-terminated string.
 * @return None.
 ******************************************************************************/
void uart0_tx_string(const s8 *p)
{
  while (*p)
  {
    uart0_tx(*p);
    p++;
  }
}

/******************************************************************************
 * @brief  Receive a string from UART0 until a carriage return ('\r') is encountered or the buffer is full.
 * @param  s Pointer to the character buffer.
 * @param  buf_size Maximum capacity of the buffer including the null terminator.
 * @return None.
 ******************************************************************************/
void uart0_rx_string(s8 *s, u32 buf_size)
{
  s32 i = 0;
  u8 temp;
  if (buf_size < 2)
  {
    return;
  }
  while ((temp = uart0_rx()) != '\r')
  {
    uart0_tx(temp);
    s[i] = temp;
    i++;
    if (i == buf_size - 1)
    {
      break;
    }
  }
  s[i] = 0;
}

/******************************************************************************
 * @brief  Convert and transmit a floating-point number as an ASCII string over UART0.
 * @param  num Floating-point value to transmit.
 * @return None.
 ******************************************************************************/
void uart0_float(float num)
{
  s32 integer_part, fraction, num_of_decimal = 1;
  u8 count = UART0_DECIMAL;

  if (num < 0)
  {
    uart0_tx('-');
    num = -num;
  }

  while (count-- > 0)
  {
    num_of_decimal *= 10;
  }

  integer_part = (s32)num;
  fraction = (s32)((num - integer_part) * num_of_decimal + 0.5);

  if (fraction >= num_of_decimal)
  {
    integer_part++;
    fraction = 0;
  }

  uart0_integer(integer_part);

  uart0_tx('.');

  while ((fraction < (num_of_decimal /= 10)) && num_of_decimal > 1)
  {
    uart0_tx('0');
  }

  uart0_integer(fraction);
}

/******************************************************************************
 * @brief  Convert and transmit a signed 32-bit integer as an ASCII string over UART0.
 * @param  num 32-bit signed integer to transmit.
 * @return None.
 ******************************************************************************/
void uart0_integer(s32 num)
{
  u8 a[11];
  s32 i = 0;

  if (num == 0)
  {
    uart0_tx('0');
    return;
  }

  if (num < 0)
  {
    uart0_tx('-');
    num = -num;
  }

  while (num)
  {
    a[i++] = (num % 10) + '0';
    num /= 10;
  }

  while (i)
  {
    uart0_tx(a[--i]);
  }
}

/******************************************************************************
 * @brief  Transmit an 8-bit value as 2 ASCII hexadecimal characters over UART0 (blocking).
 * @param  num 8-bit unsigned integer to convert and transmit.
 * @return None.
 ******************************************************************************/
void uart0_hex(u8 num)
{
  u8 t;
  t = (num >> 4 & 0x0f);

  if (t < 10)
  {
    uart0_tx(t + 48);
  }
  else
  {
    uart0_tx(t - 10 + 65);
  }
  t = (num & 0x0f);

  if (t < 10)
  {
    uart0_tx(t + 48);
  }
  else
  {
    uart0_tx(t - 10 + 65);
  }
}

/******************************************************************************
 * @brief  Transmit a 32-bit value as 8 ASCII hexadecimal characters over UART0 (blocking).
 * @param  num 32-bit unsigned integer to convert and transmit.
 * @return None.
 ******************************************************************************/
void uart0_hex32(u32 num)
{
  s32 i;
  u8 nibble;

  for (i = 7; i >= 0; i--)
  {
    nibble = (num >> (i * 4)) & 0x0F;

    if (nibble < 10)
      uart0_tx(nibble + '0');
    else
      uart0_tx(nibble - 10 + 'A');
  }
}
