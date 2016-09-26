
//------------------------------------------------------------------------
//------------------------------------------------------------------------

extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );
extern void dummy ( unsigned int );

#define RCGCGPIO 0x400FE608
#define PRGPIO   0x400FEA08

#define CM4_BASE 0xE000E000
#define STCTRL      (CM4_BASE+0x010)
#define STRELOAD    (CM4_BASE+0x014)
#define STCURRENT   (CM4_BASE+0x018)

#define RCGCUART 0x400FE618
#define PRUART   0x400FEA18

#define GPIOA_BASE 0x40004000
#define GPIOAFSEL (GPIOA_BASE+0x420)
#define GPIODEN   (GPIOA_BASE+0x51C)

#define UART0_BASE 0x4000C000
#define UARTDR   (UART0_BASE+0x000)
#define UARTFR   (UART0_BASE+0x018)
#define UARTIBRD (UART0_BASE+0x024)
#define UARTFBRD (UART0_BASE+0x028)
#define UARTLCRH (UART0_BASE+0x02C)
#define UARTCTL  (UART0_BASE+0x030)
#define UARTCC   (UART0_BASE+0xFC8)

#define SYSTEM_CONTROL 0x400FE000
#define MISC (SYSTEM_CONTROL+0x58)
#define RCC (SYSTEM_CONTROL+0x60)
#define RCC2 (SYSTEM_CONTROL+0x70)
#define RIS (SYSTEM_CONTROL+0x50)
#define MOSCCTL (SYSTEM_CONTROL+0x7C)

//------------------------------------------------------------------------
static void switch_to_mosc ( void )
{
    unsigned int ra;

    PUT32(MOSCCTL,3);

    ra=GET32(RCC);
    ra|=1<<11; //BYPASS
    ra&=~(1<<22); //USESYS
    PUT32(RCC,ra);

    PUT32(MISC,1<<8);

    ra=GET32(RCC);
    ra&=~(1<<0); //MOSCDIS
    PUT32(RCC,ra);

    while(1) if(GET32(RIS)&(1<<8)) break;

    //0x15  16mhz
    ra=GET32(RCC);
    ra&=~(0x1F<<6); //XTAL
    ra|=0x15<<6; //XTAL
    ra&=~(0x3<<4); //OSCSRC
    ra|=0x0<<4; //OSCSRC
    ra|=(1<<13); //PWRDN
    PUT32(RCC,ra);
}
//------------------------------------------------------------------------
static void uart_init ( void )
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
static void uart_send ( unsigned int x )
{
    while(1)
    {
        if(GET32(UARTFR)&0x80) break;
    }
    PUT32(UARTDR,x);
}
//------------------------------------------------------------------------
//static
unsigned int uart_recv ( void )
{
    while(1)
    {
        if(GET32(UARTFR)&0x40) break;
    }
    return(GET32(UARTDR)&0xFF);;
}
//------------------------------------------------------------------------
static void hexstrings ( unsigned int d )
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
static void hexstring ( unsigned int d )
{
    hexstrings(d);
    uart_send(0x0D);
    uart_send(0x0A);
}
//------------------------------------------------------------------------
void notmain ( void )
{
    unsigned int ra;
    unsigned int rb;
    uart_init();
    hexstring(0x87654321);
    switch_to_mosc();
    hexstring(0x12345678);


}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
