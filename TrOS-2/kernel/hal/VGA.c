#include <TrOS/hal/VGA.h>
#include <TrOS/hal/io.h>

#define VGA_MEMORY 0xB8000
#define VGA_CURSOR_IREG 0x3D4
#define VGA_CURSOR_DREG 0x3D5

static unsigned int _xPos = 0;
static unsigned int _yPos = 0;
static unsigned char _char_attrib = 0x1F;

static unsigned char _vga_calculate_color(vga_char_attrib_t* c);

void vga_move_cursor(unsigned int x, unsigned int y)
{
    unsigned int mempos = (VGA_COLS * y) + x;
    unsigned char mempos_low = mempos & 0xFF;
    unsigned char mempos_high = (mempos >> 8) & 0xFF;

    outb(0x0F, VGA_CURSOR_IREG);
    outb(mempos_low, VGA_CURSOR_DREG);

    outb(0x0E, VGA_CURSOR_IREG);
    outb(mempos_high, VGA_CURSOR_DREG);
}

void vga_putch(char c)
{
    if(c != 0)
    {
        unsigned char* vgamem = (unsigned char*)VGA_MEMORY;

        if (c == '\n'|| c == '\r' || _xPos > 79)
        {
            _yPos++;
            _xPos = 0;

            //"scroll" if needed
            if(_yPos >= VGA_LINES)
            {
                for (int i = 0; i < (VGA_LINES-1)*(VGA_COLS*2); i+=2)
                {
                    vgamem[i] = vgamem[i+(VGA_COLS*2)];
                    vgamem[i+1] = vgamem[i+(VGA_COLS*2)+1];
                }

                for (int i = (VGA_LINES-1)*(VGA_COLS*2);
                    i < (VGA_COLS*2)*VGA_LINES;
                    i+=2)
                {
                    vgamem[i] = ' ';
                    vgamem[i+1] = _char_attrib;
                }

                _yPos = (VGA_LINES-1);
            }
        }
        else
        {
            unsigned int memloc = (_xPos * 2)+(_yPos * VGA_COLS *2);

            vgamem[memloc] = c;
            vgamem[memloc+1] = _char_attrib;
            _xPos++;
        }
        vga_move_cursor(_xPos, _yPos);
    }
}

void vga_puts(const char* str)
{
    while (*str)
    {
        vga_putch(*str);
        str++;
    }
}

void vga_clear_screen(vga_char_attrib_t* c)
{
    unsigned char* vgamem = (unsigned char*)VGA_MEMORY;
    unsigned char attrib = _vga_calculate_color(c);
    unsigned int size = (VGA_COLS*2)*VGA_LINES;

    for (int i=0; i<size; i+=2)
    {
        vgamem[i] = ' ';
        vgamem[i+1] = attrib;
    }

    _xPos = 0;
    _yPos = 0;
}

unsigned char _vga_calculate_color(vga_char_attrib_t* c)
{
    unsigned char bg = (c->bg << 4);
    return bg | c->font;
}

void vga_set_color(vga_char_attrib_t* c)
{
    _char_attrib = _vga_calculate_color(c);
}

void vga_set_position(unsigned int x, unsigned int y)
{
    _xPos = x;
    _yPos = y;
}

vga_position_t vga_get_position()
{
    vga_position_t pos = {
        .x = _xPos,
        .y = _yPos
    };
    return pos;
}
