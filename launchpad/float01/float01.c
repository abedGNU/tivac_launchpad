
//------------------------------------------------------------------------
//------------------------------------------------------------------------

extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );

extern unsigned int fun ( unsigned int, unsigned int );
extern unsigned int ffun ( unsigned int, unsigned int );
extern void fpu_init ( void );

#define RCGCGPIO    0x400FE608
#define PRGPIO      0x400FEA08

#define CM4_BASE    0xE000E000
#define STCTRL      (CM4_BASE+0x010)
#define STRELOAD    (CM4_BASE+0x014)
#define STCURRENT   (CM4_BASE+0x018)

#define RCGCUART    0x400FE618
#define PRUART      0x400FEA18

#define GPIOA_BASE  0x40004000
#define GPIOAFSEL   (GPIOA_BASE+0x420)
#define GPIODEN     (GPIOA_BASE+0x51C)

#define UART0_BASE  0x4000C000
#define UARTDR      (UART0_BASE+0x000)
#define UARTFR      (UART0_BASE+0x018)
#define UARTIBRD    (UART0_BASE+0x024)
#define UARTFBRD    (UART0_BASE+0x028)
#define UARTLCRH    (UART0_BASE+0x02C)
#define UARTCTL     (UART0_BASE+0x030)
#define UARTCC      (UART0_BASE+0xFC8)

//------------------------------------------------------------------------
void uart_init ( void )
{
    unsigned int ra;
    //16000000 / (16 * 115200 ) = 1000000 / 115200 = 8.6805
    //0.6805... * 64 = 43.5   ~ 44
    //use 8 and 44
    ra=GET32(RCGCUART);
    ra|=1; //UART0
    PUT32(RCGCUART,ra);
    ra=GET32(RCGCGPIO);
    ra|=1; //GPIOA
    PUT32(RCGCGPIO,ra);

    ra=GET32(GPIOAFSEL);
    ra|=3;
    PUT32(GPIOAFSEL,ra);
    ra=GET32(GPIODEN);
    ra|=3;
    PUT32(GPIODEN,ra);

    PUT32(UARTCTL,0x00000000);
    PUT32(UARTIBRD, 8);
    PUT32(UARTFBRD, 44);
    PUT32(UARTLCRH,0x00000060);
    PUT32(UARTCC,0x00000005); //PIOSC
    PUT32(UARTFR,0);
    PUT32(UARTCTL,0x00000301);
}
//------------------------------------------------------------------------
void uart_flush ( void )
{
    while(1)
    {
        if(GET32(UARTFR)&0x80) break;
    }
    //PUT32(UARTDR,x);
}
//------------------------------------------------------------------------
void uart_send ( unsigned int x )
{
    while(1)
    {
        if(GET32(UARTFR)&0x80) break;
    }
    PUT32(UARTDR,x);
}
//------------------------------------------------------------------------
unsigned int uart_recv ( void )
{
    while(1)
    {
        if(GET32(UARTFR)&0x40) break;
    }
    return(GET32(UARTDR)&0xFF);;
}
//------------------------------------------------------------------------
void hexstrings ( unsigned int d )
{
    //unsigned int ra;
    unsigned int rb;
    unsigned int rc;

    rb=32;
    while(1)
    {
        rb-=4;
        rc=(d>>rb)&0xF;
        if(rc>9) rc+=0x37; else rc+=0x30;
        uart_send(rc);
        if(rb==0) break;
    }
    uart_send(0x20);
}
//------------------------------------------------------------------------
void hexstring ( unsigned int d )
{
    hexstrings(d);
    uart_send(0x0D);
    uart_send(0x0A);
}
//------------------------------------------------------------------------
void notmain ( void )
{
    unsigned int ra;

    uart_init();
    hexstring(0x87654321);
    hexstring(0x12345678);
    uart_flush();
    fpu_init();
    ra=fun(200,1000);
    hexstring(ra);
    ra=ffun(200,1000);
    hexstring(ra);

}
//------------------------------------------------------------------------
//------------------------------------------------------------------------

