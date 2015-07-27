#include <tros/driver.h>
#include <tros/hal/io.h>
#include <tros/irq.h>
#include <tros/tros.h>
#include <ds/ringbuffer.h>
#include <string.h>
#include <keyboard.h>

#define KEY_DEVICE  0x60
#define KEY_PENDING 0x64


static int _kbd_scancode_map[] = {
    //key           scancode
    KEY_UNKNOWN,    //0
    KEY_ESCAPE,     //1
    KEY_1,          //2
    KEY_2,          //3
    KEY_3,          //4
    KEY_4,          //5
    KEY_5,          //6
    KEY_6,          //7
    KEY_7,          //8
    KEY_8,          //9
    KEY_9,          //0xa
    KEY_0,          //0xb
    KEY_MINUS,      //0xc
    KEY_EQUAL,      //0xd
    KEY_BACKSPACE,  //0xe
    KEY_TAB,        //0xf
    KEY_Q,          //0x10
    KEY_W,          //0x11
    KEY_E,          //0x12
    KEY_R,          //0x13
    KEY_T,          //0x14
    KEY_Y,          //0x15
    KEY_U,          //0x16
    KEY_I,          //0x17
    KEY_O,          //0x18
    KEY_P,          //0x19
    KEY_LEFTBRACKET,//0x1a
    KEY_RIGHTBRACKET,//0x1b
    KEY_RETURN,     //0x1c
    KEY_LCTRL,      //0x1d
    KEY_A,          //0x1e
    KEY_S,          //0x1f
    KEY_D,          //0x20
    KEY_F,          //0x21
    KEY_G,          //0x22
    KEY_H,          //0x23
    KEY_J,          //0x24
    KEY_K,          //0x25
    KEY_L,          //0x26
    KEY_SEMICOLON,  //0x27
    KEY_QUOTE,      //0x28
    KEY_GRAVE,      //0x29
    KEY_LSHIFT,     //0x2a
    KEY_BACKSLASH,  //0x2b
    KEY_Z,          //0x2c
    KEY_X,          //0x2d
    KEY_C,          //0x2e
    KEY_V,          //0x2f
    KEY_B,          //0x30
    KEY_N,          //0x31
    KEY_M,          //0x32
    KEY_COMMA,      //0x33
    KEY_DOT,        //0x34
    KEY_SLASH,      //0x35
    KEY_RSHIFT,     //0x36
    KEY_KP_ASTERISK,//0x37
    KEY_RALT,       //0x38
    KEY_SPACE,      //0x39
    KEY_CAPSLOCK,   //0x3a
    KEY_F1,         //0x3b
    KEY_F2,         //0x3c
    KEY_F3,         //0x3d
    KEY_F4,         //0x3e
    KEY_F5,         //0x3f
    KEY_F6,         //0x40
    KEY_F7,         //0x41
    KEY_F8,         //0x42
    KEY_F9,         //0x43
    KEY_F10,        //0x44
    KEY_KP_NUMLOCK, //0x45
    KEY_SCROLLLOCK, //0x46
    KEY_HOME,       //0x47
    KEY_KP_8,       //0x48	//keypad up arrow
    KEY_PAGEUP,     //0x49
    KEY_KP_2,       //0x50	//keypad down arrow
    KEY_KP_3,       //0x51	//keypad page down
    KEY_KP_0,       //0x52	//keypad insert key
    KEY_KP_DECIMAL, //0x53	//keypad delete key
    KEY_UNKNOWN,    //0x54
    KEY_UNKNOWN,    //0x55
    KEY_UNKNOWN,    //0x56
    KEY_F11,        //0x57
    KEY_F12         //0x58
};


static int _shift;
static int _alt;
static int _ctrl;
static int _capslock;
static char _scancode;

static ringbuffer_t _kb_data;

int kbd_read(char* buffer, unsigned int count);
int kbd_ioctl(unsigned int num, unsigned long param);
int kbd_open();
int kbd_close();
void kbd_irq_handler(cpu_registers_t* regs);

char kbd_ascii_keycode(enum KEYCODE code);

static driver_hid_t __kbdriver = {
    .read = kbd_read,
    .ioctl = kbd_ioctl
};

int kbd_driver_initialize()
{
    device_driver_t drv = {
        .name = "kbd",
        .type = DRV_HID,
        .driver = &__kbdriver
    };

    _shift = 0;
    _alt = 0;
    _ctrl = 0;
    _capslock = 0;

    printk("** Initalizing generic keyboard driver\n");
    rb_init(&_kb_data);
    irq_register_handler(33, &kbd_irq_handler);
	return driver_register(&drv);
}

void kbd_driver_remove()
{
    irq_remove_handler(33);
}

int kbd_read(char* buffer, unsigned int count)
{
    //TODO: Make this a *blocking* IO call
    unsigned int read = 0;
	while(read < count)
	{
		if(rb_len(&_kb_data) > 0)
		{
			rb_pop(&_kb_data, &buffer[read++]);
		}
	}
	return read;
}

int kbd_ioctl(unsigned int num, unsigned long param)
{
    return 0;
}

void kbd_irq_handler(cpu_registers_t* regs)
{
    static int _extended = 0;

    while(inb(KEY_PENDING) & 2);
    int scancode = inb(KEY_DEVICE);

    if (scancode == 0xE0 || scancode == 0xE1)
    {
        _extended = 1;
        //printk("extended\n");
    }
    else
    {
        _extended = 0;
        if (scancode & 0x80)
        {
            scancode -= 0x80;

            int key = _kbd_scancode_map[scancode];

            switch(key)
            {
                case KEY_LCTRL:
                case KEY_RCTRL:
                    _ctrl = 0;
                    break;

                case KEY_LSHIFT:
                case KEY_RSHIFT:
                    _shift = 0;
                    break;

                case KEY_LALT:
                case KEY_RALT:
                    _alt = 0;
                    break;
            }
        }
        else
        {
            _scancode = scancode;
            int key = _kbd_scancode_map[scancode];

            switch (key)
            {
                case KEY_LCTRL:
                case KEY_RCTRL:
                    _ctrl = 1;
                    break;

                case KEY_LSHIFT:
                case KEY_RSHIFT:
                    _shift = 1;
                    break;

                case KEY_LALT:
                case KEY_RALT:
                    _alt = 1;
                    break;
                case KEY_CAPSLOCK:
                    _capslock = (_capslock) ? 0 : 1;
                    break;
            }

            key = kbd_ascii_keycode(key);

            rb_push(&_kb_data, key);
            //printk("%c", key);
        }
    }
    irq_eoi(1);
}

char kbd_ascii_keycode(enum KEYCODE code)
{
    unsigned char key = code;

    if (key <= 0x7F)
    {
        if ((_shift || _capslock) && key >= 'a' && key <= 'z')
        {
            key -= 32;
        }
        else
        {
            if(_shift)
            {
                switch (key)
                {
                    case '0':
                        key = KEY_RIGHTPARENTHESIS;
                        break;
                    case '1':
                        key = KEY_EXCLAMATION;
                        break;
                    case '2':
                        key = KEY_AT;
                        break;
                    case '3':
                        key = KEY_EXCLAMATION;
                        break;
                    case '4':
                        key = KEY_HASH;
                        break;
                    case '5':
                        key = KEY_PERCENT;
                        break;
                    case '6':
                        key = KEY_CARRET;
                        break;
                    case '7':
                        key = KEY_AMPERSAND;
                        break;
                    case '8':
                        key = KEY_ASTERISK;
                        break;
                    case '9':
                        key = KEY_LEFTPARENTHESIS;
                        break;
                    case KEY_COMMA:
                        key = KEY_LESS;
                        break;
                    case KEY_DOT:
                        key = KEY_GREATER;
                        break;
                    case KEY_SLASH:
                        key = KEY_QUESTION;
                        break;
                    case KEY_SEMICOLON:
                        key = KEY_COLON;
                        break;
                    case KEY_QUOTE:
                        key = KEY_QUOTEDOUBLE;
                        break;
                    case KEY_LEFTBRACKET :
                        key = KEY_LEFTCURL;
                        break;
                    case KEY_RIGHTBRACKET :
                        key = KEY_RIGHTCURL;
                        break;
                    case KEY_GRAVE:
                        key = KEY_TILDE;
                        break;
                    case KEY_MINUS:
                        key = KEY_UNDERSCORE;
                        break;
                    case KEY_PLUS:
                        key = KEY_EQUAL;
                        break;
                    case KEY_BACKSLASH:
                        key = KEY_BAR;
                        break;
                }
            }
        }

        return key;
    }
    return 0;
}
