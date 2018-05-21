#ifndef DEFINITIONS2.H
#define DEFINITIONS2.H

void startup(void) __attribute__((naked)) __attribute__((section (".start_section")) );

//#define SIMULATOR 
#define TARGET_FPS 30
#define MAX_POINTS 20
#define STK_CTRL 	((volatile unsigned int *)(0xE000E010))
#define STK_LOAD	((volatile unsigned int *)(0xE000E010+4))
#define STK_VAL 	((volatile unsigned int *)(0xE000E010+8))
#define B_E				0x40    // Enable
#define B_RST			0x20	// Reset
#define B_CS2			0x10	// Controller Select 2
#define B_CS1			8		// Controller Select 1
#define B_SELECT		4		// 0 Graphics, 1 ASCII
#define B_RW			2		// 0 Write, 1 Read
#define B_RS			1		// 0 Command, 1 Data
#define LCD_ON			0x3F	// Display on
#define LCD_OFF			0x3E	// Display off	
#define LCD_SET_ADD		0x40	// Set horizontal coordinate
#define LCD_SET_PAGE	0xB8	// Set vertical coordinate
#define LCD_DISP_START	0xC0	// Start address
#define LCD_BUSY		0x80	// Read busy status

typedef unsigned int uint32_t;
typedef unsigned char uint8_t, byte;
uint8_t backbuffer[1024]; // 128 * 64 /8
//	GPIO 
typedef struct _gpio { 
	uint32_t 	moder;
	uint32_t	otyper; 
	uint32_t	ospeedr; 
	uint32_t	pupdr; 
	union{ 
		uint32_t	idr; 
		struct{ 
			byte idrLow; 
			byte idrHigh; 
		}; 
	}; 
	union{ 
		uint32_t	odr; 
		struct{ 
			byte odrLow; 
			byte odrHigh; 
		}; 
	}; 
}	GPIO; 
/*may be needed to boot the struct
 * typedef volatile *GPIO gpioPort...*/
#define GPIO_D	(*((volatile GPIO*)	0x40020c00))
#define GPIO_E	(*((volatile GPIO*)	0x40021000))

void startup ( void )
{
__asm volatile(
	" LDR R0,=0x2001C000\n"		/* set stack */
	" MOV SP,R0\n"
	" BL main\n"				/* call main */
	"_exit: B .\n"				/* never return */
	) ;
}
typedef struct tPoint{
        uint8_t x;
        uint8_t y;
} POINT;

typedef struct tGeometry{
        int numpoints;
        int sizex;
        int sizey;
        POINT px[MAX_POINTS];
}GEOMETRY, *PGEOMETRY;

typedef struct tObj {
        PGEOMETRY geo;
        int dirx,diry;
        int posx,posy;
        void(* draw)(struct tObj *);
        void(* clear)(struct tObj *);
        void(* move)(struct tObj *);
        void(* set_speed)(struct tObj *,int,int);
}OBJECT, *POBJECT;

GEOMETRY ball_geometry =
{
	12,		/* numpoints*/
	4,4,		/* sizex, sizey*/
	{
		/* px[0,1,2 ...]*/
		{0,1}, {0,2}, {1,0}, {1,1}, {1,2},
		{1,3}, {2,0}, {2,1}, {2,2}, {2,3},
		{3,1},
		{3,2},
	}
};
void delay_250ns(void);
void delay_500ns(void);
void delay_milli(uint32_t ms);
void delay_micro(uint32_t us);
static void graphic_ctrl_bit_set(uint8_t x); // Ready
static void graphic_ctrl_bit_clear(uint8_t x); // Ready
static void select_controller(uint8_t controller); // Ready
static void graphic_wait_ready(void); // Ready
static uint8_t graphic_read(uint8_t controller); // Ready
static void graphic_write_command(uint8_t command, uint8_t controller); // Ready
static void graphic_write_data(uint8_t data, uint8_t controller); // Ready
static void graphic_write(uint8_t value, uint8_t controller); // Ready
static uint8_t graphic_read_data(uint8_t controller); // Ready
void app_init(void);
void graphic_initialize(void); // Ready
void graphic_clear_screen(void); // Ready
void graphic_draw_screen(void); // Ready?
//void pixel (int x, int y, int set); // Check that one.
void pixel(int x, int y); // autopong
void set_object_speed(POBJECT o,int speedx,int speedy); // Ready
void draw_object(POBJECT o); // Failing
void clear_object(POBJECT o); // Failing
void move_object(POBJECT o); // Ready
void clear_backbuffer(void);
void kbdActivate(uint32_t row);
int kdbGetCol(void);
uint8_t keyb(void);

#endif // DEFINITIONS2.H
