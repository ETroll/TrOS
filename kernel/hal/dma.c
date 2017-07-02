#include <tros/hal/dma.h>
#include <tros/hal/io.h>
#include <tros/tros.h>

#define DMA_MAX_CHANNELS        16
#define DMA_CHANNELS_PER_DMAC   8


//DMA0 IO Registers
enum DMA0_CHANNEL_IO
{
    DMA0_REG_CHAN0_ADDR     = 0,
    DMA0_REG_CHAN0_COUNT    = 1,
    DMA0_REG_CHAN1_ADDR     = 2,
    DMA0_REG_CHAN1_COUNT    = 3,
    DMA0_REG_CHAN2_ADDR     = 4,
    DMA0_REG_CHAN2_COUNT    = 5,
    DMA0_REG_CHAN3_ADDR     = 6,
    DMA0_REG_CHAN3_COUNT    = 7,
};

//DMA0 Generic Registers
enum DMA0_IO
{
    DMA0_REG_STATUS             = 0x08,
    DMA0_REG_COMMAND            = 0x08,
    DMA0_REG_REQUEST            = 0x09,
    DMA0_REG_CHANMASK           = 0x0A,
    DMA0_REG_MODE               = 0x0B,
    DMA0_REG_CLEARBYTE_FLIPFLOP = 0x0C,
    DMA0_REG_TEMP               = 0x0D,
    DMA0_REG_MASTER_CLEAR       = 0x0D,
    DMA0_REG_CLEAR_MASK         = 0x0E,
    DMA0_REG_MASK               = 0x0F
};


//DMA1 IO Registers
enum DMA1_CHANNEL_IO
{
    DMA1_REG_CHAN4_ADDR     = 0xC0,
    DMA1_REG_CHAN4_COUNT    = 0xC2,
    DMA1_REG_CHAN5_ADDR     = 0xC4,
    DMA1_REG_CHAN5_COUNT    = 0xC6,
    DMA1_REG_CHAN6_ADDR     = 0xC8,
    DMA1_REG_CHAN6_COUNT    = 0xCA,
    DMA1_REG_CHAN7_ADDR     = 0xCC,
    DMA1_REG_CHAN7_COUNT    = 0xCE,
};

// DMA Generic registers
enum DMA1_IO
{
    DMA1_REG_STATUS             = 0xD0,
    DMA1_REG_COMMAND            = 0xD0,
    DMA1_REG_REQUEST            = 0xD2,
    DMA1_REG_CHANMASK           = 0xD4,
    DMA1_REG_MODE               = 0xD6,
    DMA1_REG_CLEARBYTE_FLIPFLOP = 0xD8,
    DMA1_REG_INTER              = 0xDA,
    DMA1_REG_UNMASK_ALL         = 0xDC,
    DMA1_REG_MASK               = 0xDE
};

// DMA External Page Registers
enum DMA0_PAGE_REG
{
    DMA_PAGE_EXTRA0             = 0x80,
    DMA_PAGE_CHAN2_ADDRBYTE2    = 0x81,
    DMA_PAGE_CHAN3_ADDRBYTE2    = 0x82,
    DMA_PAGE_CHAN1_ADDRBYTE2    = 0x83,
    DMA_PAGE_EXTRA1             = 0x84,
    DMA_PAGE_EXTRA2             = 0x85,
    DMA_PAGE_EXTRA3             = 0x86,
    DMA_PAGE_CHAN6_ADDRBYTE2    = 0x87,
    DMA_PAGE_CHAN7_ADDRBYTE2    = 0x88,
    DMA_PAGE_CHAN5_ADDRBYTE2    = 0x89,
    DMA_PAGE_EXTRA4             = 0x8C,
    DMA_PAGE_EXTRA5             = 0x8D,
    DMA_PAGE_EXTRA6             = 0x8E,
    DMA_PAGE_DRAM_REFRESH       = 0x8F
};


enum DMA_MODE_REG_MASK
{
    DMA_MODE_MASK_SEL           = 3,
    DMA_MODE_MASK_TRA           = 0x0C,
    DMA_MODE_SELF_TEST          = 0,
    DMA_MODE_READ_TRANSFER      = 4,
    DMA_MODE_WRITE_TRANSFER     = 8,
    DMA_MODE_MASK_AUTO          = 0x10,
    DMA_MODE_MASK_IDEC          = 0x20,
    DMA_MODE_MASK               = 0xC0,
    DMA_MODE_TRANSFER_ON_DEMAND = 0,
    DMA_MODE_TRANSFER_SINGLE    = 0x40,
    DMA_MODE_TRANSFER_BLOCK     = 0x80,
    DMA_MODE_TRANSFER_CASCADE   = 0xC0
};

enum DMA_CMD_REG_MASK
{
    DMA_CMD_MASK_MEMTOMEM       = 1,
    DMA_CMD_MASK_CHAN0ADDRHOLD  = 2,
    DMA_CMD_MASK_ENABLE         = 4,
    DMA_CMD_MASK_TIMING         = 8,
    DMA_CMD_MASK_PRIORITY       = 0x10,
    DMA_CMD_MASK_WRITESEL       = 0x20,
    DMA_CMD_MASK_DREQ           = 0x40,
    DMA_CMD_MASK_DACK           = 0x80
};



void dma_channel_set_mode(unsigned char channel, unsigned char mode)
{
    int dma = (channel < 4) ? 0 : 1;
    int chan = (int)((dma==0) ? channel : channel-4);

    dma_channel_mask(channel);
    pio_outb((channel < 4) ? DMA0_REG_MODE : DMA1_REG_MODE, chan | mode);
    dma_unmask(dma);
}

void dma_channel_set_read(unsigned char channel)
{
    dma_channel_set_mode(channel, DMA_MODE_READ_TRANSFER | DMA_MODE_TRANSFER_SINGLE);
}

void dma_channel_set_write(unsigned char channel)
{
    dma_channel_set_mode(channel, DMA_MODE_WRITE_TRANSFER | DMA_MODE_TRANSFER_SINGLE);
}

void dma_channel_set_address(unsigned char channel, unsigned short address)
{
    if(channel < 9)
    {
        unsigned short port = 0;
        switch(channel)
        {
            case 0:
            {
                port = DMA0_REG_CHAN0_ADDR;
            } break;
            case 1:
            {
                port = DMA0_REG_CHAN1_ADDR;
            } break;
            case 2:
            {
                port = DMA0_REG_CHAN2_ADDR;
            } break;
            case 3:
            {
                port = DMA0_REG_CHAN3_ADDR;
            } break;
            case 4:
            {
                port = DMA1_REG_CHAN4_ADDR;
            } break;
            case 5:
            {
                port = DMA1_REG_CHAN5_ADDR;
            } break;
            case 6:
            {
                port = DMA1_REG_CHAN6_ADDR;
            } break;
            case 7:
            {
                port = DMA1_REG_CHAN7_ADDR;
            } break;
	   }
       pio_outb(port, address & 0xFF);
       pio_outb(port, (address >> 8) & 0xFF);
    }
}

void dma_channel_set_count(unsigned char channel, unsigned short count)
{
    if(channel < 9)
    {
        unsigned short port = 0;
        switch(channel)
        {
            case 0:
            {
                port = DMA0_REG_CHAN0_COUNT;
            } break;
            case 1:
            {
                port = DMA0_REG_CHAN1_COUNT;
            } break;
            case 2:
            {
                port = DMA0_REG_CHAN2_COUNT;
            } break;
            case 3:
            {
                port = DMA0_REG_CHAN3_COUNT;
            } break;
            case 4:
            {
                port = DMA1_REG_CHAN4_COUNT;
            } break;
            case 5:
            {
                port = DMA1_REG_CHAN5_COUNT;
            } break;
            case 6:
            {
                port = DMA1_REG_CHAN6_COUNT;
            } break;
            case 7:
            {
                port = DMA1_REG_CHAN7_COUNT;
            } break;
	   }
       pio_outb(port, count & 0xFF);
       pio_outb(port, (count >> 8) & 0xFF);
    }
}

void dma_channel_mask(unsigned char channel)
{
    if(channel < 5)
    {
        pio_outb(DMA0_REG_CHANMASK, (1 << (channel-1)));
    }
    else
    {
        pio_outb(DMA1_REG_CHANMASK, (1 << (channel-5)));
    }
}

void dma_channel_unmask(unsigned char channel)
{
    if(channel < 5)
    {
        pio_outb(DMA0_REG_CHANMASK, channel);
    }
    else
    {
        pio_outb(DMA1_REG_CHANMASK, channel);
    }
}

void dma_flipflop_reset(int dma)
{
    if(dma < 2)
    {
        pio_outb(dma == 0 ? DMA0_REG_CLEARBYTE_FLIPFLOP : DMA1_REG_CLEARBYTE_FLIPFLOP, 0xFF);
    }
}

void dma_reset()
{
    pio_outb(DMA0_REG_TEMP, 0xFF);
}

void dma_set_external_page_register(unsigned char reg, unsigned char value)
{
    if(reg < 15)
    {
        unsigned short port = 0;
        switch(reg)
        {
            case 1:
            {
                port = DMA_PAGE_CHAN1_ADDRBYTE2;
            } break;
            case 2:
            {
                port = DMA_PAGE_CHAN2_ADDRBYTE2;
            } break;
            case 3:
            {
                port = DMA_PAGE_CHAN3_ADDRBYTE2;
            } break;
            case 4:
                return;
            case 5:
            {
                port = DMA_PAGE_CHAN5_ADDRBYTE2;
            } break;
            case 6:
            {
                port = DMA_PAGE_CHAN6_ADDRBYTE2;
            } break;
            case 7:
            {
                port = DMA_PAGE_CHAN7_ADDRBYTE2;
            } break;
	   }

       pio_outb(port, value);
    }
}

void dma_unmask()
{
    pio_outb(DMA1_REG_UNMASK_ALL, 0xFF);
}
