#include "uart0.h"

int main()
{
    uart0_init(9600);

    uart0_tx_string("UART0 Driver Demo\r\n");

    uart0_integer(12345);

    uart0_tx_string("\r\n");

    uart0_float(123.456);

    while(1);
}
