// dma.h
// Simple implementation of the 8237A ISA DMAC


#ifndef INCLUDE_TROS_DMA_H
#define INCLUDE_TROS_DMA_H

void dma_channel_set_mode(unsigned char channel, unsigned char mode);
void dma_channel_set_read(unsigned char channel);
void dma_channel_set_write(unsigned char channel);
void dma_channel_set_address(unsigned char channel, unsigned short address);
void dma_channel_set_count(unsigned char channel, unsigned short count);
void dma_channel_mask(unsigned char channel);
void dma_channel_unmask(unsigned char channel);
void dma_flipflop_reset(int dma);
void dma_reset();
void dma_set_external_page_register(unsigned char reg, unsigned char value);
void dma_unmask();

#endif
