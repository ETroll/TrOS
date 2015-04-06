//#include "ILI9340.h"
//#include "SPI.h"
//
//#include <Mystdlib.h>
//#include <Kernel/Mmio.h>
//#include <Kernel/Kernel.h>
//#include <Kernel/Arch/Raspberry.h>
//
//
//void ili9340_init()
//{
//    ili9340_cmd(0xEF);
//    ili9340_data(0x03);
//    ili9340_data(0x80);
//    ili9340_data(0x02);
//
//    ili9340_cmd(0xCF);
//    ili9340_data(0x00);
//    ili9340_data(0XC1);
//    ili9340_data(0X30);
//
//    ili9340_cmd(0xED);
//    ili9340_data(0x64);
//    ili9340_data(0x03);
//    ili9340_data(0X12);
//    ili9340_data(0X81);
//
//    ili9340_cmd(0xE8);
//    ili9340_data(0x85);
//    ili9340_data(0x00);
//    ili9340_data(0x78);
//
//    ili9340_cmd(0xCB);
//    ili9340_data(0x39);
//    ili9340_data(0x2C);
//    ili9340_data(0x00);
//    ili9340_data(0x34);
//    ili9340_data(0x02);
//
//    ili9340_cmd(0xF7);
//    ili9340_data(0x20);
//
//    ili9340_cmd(0xEA);
//    ili9340_data(0x00);
//    ili9340_data(0x00);
//
//    ili9340_cmd(ILI9340_PWCTR1);
//    ili9340_data(0x23);
//
//    ili9340_cmd(ILI9340_PWCTR2);
//    ili9340_data(0x10);
//
//    ili9340_cmd(ILI9340_VMCTR1);
//    ili9340_data(0x3e);
//    ili9340_data(0x28);
//
//    ili9340_cmd(ILI9340_VMCTR2);
//    ili9340_data(0x86);
//
//    ili9340_cmd(ILI9340_MADCTL);
//    ili9340_data(ILI9340_MADCTL_MX | ILI9340_MADCTL_BGR);
//
//    ili9340_cmd(ILI9340_PIXFMT);
//    ili9340_data(0x55);
//
//    ili9340_cmd(ILI9340_FRMCTR1);
//    ili9340_data(0x00);
//    ili9340_data(0x18);
//
//    ili9340_cmd(ILI9340_DFUNCTR);
//    ili9340_data(0x08);
//    ili9340_data(0x82);
//    ili9340_data(0x27);
//
//    ili9340_cmd(0xF2);
//    ili9340_data(0x00);
//
//    ili9340_cmd(ILI9340_GAMMASET);
//    ili9340_data(0x01);
//
//    ili9340_cmd(ILI9340_GMCTRP1);
//    ili9340_data(0x0F);
//    ili9340_data(0x31);
//    ili9340_data(0x2B);
//    ili9340_data(0x0C);
//    ili9340_data(0x0E);
//    ili9340_data(0x08);
//    ili9340_data(0x4E);
//    ili9340_data(0xF1);
//    ili9340_data(0x37);
//    ili9340_data(0x07);
//    ili9340_data(0x10);
//    ili9340_data(0x03);
//    ili9340_data(0x0E);
//    ili9340_data(0x09);
//    ili9340_data(0x00);
//
//    ili9340_cmd(ILI9340_GMCTRN1);
//    ili9340_data(0x00);
//    ili9340_data(0x0E);
//    ili9340_data(0x14);
//    ili9340_data(0x03);
//    ili9340_data(0x11);
//    ili9340_data(0x07);
//    ili9340_data(0x31);
//    ili9340_data(0xC1);
//    ili9340_data(0x48);
//    ili9340_data(0x08);
//    ili9340_data(0x0F);
//    ili9340_data(0x0C);
//    ili9340_data(0x31);
//    ili9340_data(0x36);
//    ili9340_data(0x0F);
//
//    ili9340_cmd(ILI9340_SLPOUT);
//    for(int i=0; i<20000; i++) dummy(i);
//    ili9340_cmd(ILI9340_DISPON);
//}
//
//void ili9340_data(unsigned char byte)
//{
//    mmio_write(GPIO_SET_0, 1<<25);
//    spi_send(byte);
//}
//
//void ili9340_cmd(unsigned char byte)
//{
//    mmio_write(GPIO_CLR_0, 1<<25);
//    spi_send(byte);
//}
//
//void setAddrWindow(unsigned short x0, unsigned short y0,
//    unsigned short x1, unsigned short y1) {
//
//    ili9340_cmd(ILI9340_CASET); // Column addr set
//    ili9340_data(x0 >> 8);
//    ili9340_data(x0 & 0xFF);     // XSTART
//    ili9340_data(x1 >> 8);
//    ili9340_data(x1 & 0xFF);     // XEND
//
//    ili9340_cmd(ILI9340_PASET); // Row addr set
//    ili9340_data(y0>>8);
//    ili9340_data(y0);     // YSTART
//    ili9340_data(y1>>8);
//    ili9340_data(y1);     // YEND
//
//    ili9340_cmd(ILI9340_RAMWR); // write to RAM
//}
//
//void ili9340_fillrect(unsigned short x0, unsigned short y0,
//		unsigned short width, unsigned short height, unsigned int color)
//{
//	printk("Drawing rect at %d %d with size %d, %d", x0, y0, width, height);
//
//	setAddrWindow(x0, y0, width, height);
//	unsigned short hi = color >> 8, lo = color;
//
//	mmio_write(GPIO_SET_0, 1<<25);
//	for(int y=height; y>0; y--)
//	{
//		for(int x=width; x>0; x--)
//		{
//
//			spi_send(hi);
//			spi_send(lo);
//		}
//	}
//	mmio_write(GPIO_CLR_0, 1<<25);
//}
//
//void ili9340_fillscreen(unsigned short color)
//{
//	ili9340_fillrect(0,0,ILI9340_TFTWIDTH, ILI9340_TFTHEIGHT, color);
//}
