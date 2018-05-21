#include "definitions2.h"

static OBJECT ball =
{
	&ball_geometry,
	0,0, // direction
	1,1, // position
	draw_object,
	clear_object,
	move_object,
	set_object_speed,
};
/*void main(void)
{
	POBJECT p = &ball;
	app_init();
	graphic_initialize();
#ifndef SIMULATOR
	graphic_clear_screen();
#endif
	p->set_speed(p, 4, 4);
	while(1)
	{
		p->move(p);
		delay_milli(1000/TARGET_FPS); 
	}
}*/
int main(int argc, char **argv) 
{ 
	POBJECT p = &ball;
	app_init(); 
	graphic_initialize(); 
	graphic_clear_screen();
	//p->set_speed(p, 4, 1);
	while(1) 
		{ 
			clear_backbuffer();
			p->move(p);
			graphic_write_command(LCD_ON 		 , B_CS1|B_CS2);
			graphic_draw_screen(); 
			delay_milli(40);
			uint8_t c = keyb();
			switch(c)
			{
				case 6: p->set_speed(p, 2, 0); break;
				case 9: p->set_speed(p, 2, 2); break;
				case 4: p->set_speed(p, -2, 0); break;
				case 1: p->set_speed(p, -2, -2); break;
				case 7: p->set_speed(p, -2, 2); break;
				case 3: p->set_speed(p, 2, -2); break;
				case 5: p->set_speed(p, 0, 0); break;
				case 2: p->set_speed(p, 0, -2); break;
				case 8: p->set_speed(p, 0, 2); break;
			}
		}
}


void delay_250ns( void ) 
{   // SystemCoreClock = 168000000   
	*STK_CTRL = 0;   
	*STK_LOAD = ( (168/4) -1 );
    *STK_VAL = 0;   
	*STK_CTRL = 5;   
	while( (*STK_CTRL & 0x10000 )== 0 ){
		}   
	*STK_CTRL = 0; 
} 
void delay_500ns()
{
#ifndef SIMULATOR
	delay_250ns();
	delay_250ns();
#endif
}
void delay_micro(uint32_t us)
{
	while(us--)
	{
		delay_250ns();
		delay_250ns();
		delay_250ns();
		delay_250ns();
	}
}
void delay_milli( uint32_t ms ) 
{ 
	#ifdef SIMULATOR
	while( ms-- )   {
		delay_micro(1);
	}
	#else
	while( ms-- )   {
		delay_micro(1000);
	}
	#endif
}
uint8_t graphic_read_data(uint8_t controller)
{
	(void) graphic_read(controller); /*returnerar nonsense*/
	return graphic_read(controller); /*returnerar korrekt data*/
}
static void graphic_ctrl_bit_set(uint8_t x)
{
	uint8_t c;
	c = GPIO_E.odrLow; 
	c &= ~B_SELECT;
	c |= (~B_SELECT & x);
	GPIO_E.odrLow = c;
}
static void graphic_ctrl_bit_clear(uint8_t x)
{
	uint8_t c;
	c = GPIO_E.odrLow;
	c &= ~B_SELECT;
	c &= ~x;
	GPIO_E.odrLow = c;
}
static void select_controller(uint8_t controller)
{
	switch(controller)
	{
		case 0:
			graphic_ctrl_bit_clear(B_CS1|B_CS2);
			break;
		case B_CS1:
			graphic_ctrl_bit_set(B_CS1);
			graphic_ctrl_bit_clear(B_CS2);
			break;
		case B_CS2:
			graphic_ctrl_bit_set(B_CS2);
			graphic_ctrl_bit_clear(B_CS1);
			break;
		case B_CS1|B_CS2:
			graphic_ctrl_bit_set(B_CS1|B_CS2);
			break;
	}
}
void graphic_initialize(void)
{
	graphic_ctrl_bit_set(B_E);
	delay_micro(10);
	
	graphic_ctrl_bit_clear(B_CS1|B_CS2|B_RST|B_E);
	delay_milli(30);
	graphic_ctrl_bit_set(B_RST);
	delay_milli(100);
	graphic_write_command(LCD_OFF 		 , B_CS1|B_CS2);
	//graphic_write_command(LCD_ON 		 , B_CS1|B_CS2);
	graphic_write_command(LCD_DISP_START , B_CS1|B_CS2);
	graphic_write_command(LCD_SET_ADD 	 , B_CS1|B_CS2);
	graphic_write_command(LCD_SET_PAGE 	 , B_CS1|B_CS2);
	select_controller(0);
}
static void graphic_wait_ready(void)
{
	uint8_t c;
	graphic_ctrl_bit_clear(B_E);
	GPIO_E.moder = 0x00005555; // 15-8 inputs, 7-0 outputs
	graphic_ctrl_bit_clear(B_RS);
	graphic_ctrl_bit_set(B_RW);
	delay_500ns();
	
	while(1)
	{
		graphic_ctrl_bit_set(B_E);
		delay_500ns();
		c = GPIO_E.idrHigh & LCD_BUSY;
		graphic_ctrl_bit_clear(B_E);
		delay_500ns();
		if(c == 0) break;
	}
	GPIO_E.moder = 0x55555555; // 15-0 outputs
}
static uint8_t graphic_read(uint8_t controller)
{
	uint8_t c;
	graphic_ctrl_bit_clear(B_E);
	GPIO_E.moder = 0x00005555; // 15-8 in, 7-0 out
	graphic_ctrl_bit_set(B_RS|B_RW);
	select_controller(controller);
	delay_500ns();
	graphic_ctrl_bit_set(B_E);
	delay_500ns();
	c = GPIO_E.idrHigh;
	graphic_ctrl_bit_clear(B_E);
	GPIO_E.moder = 0x55555555; // 15-0 out
	
	if(controller & B_CS1)
	{
		select_controller(B_CS1);
		graphic_wait_ready();
	}
	if(controller & B_CS2)
	{
		select_controller(B_CS2);
		graphic_wait_ready();
	}
	return c;
}
static void graphic_write(uint8_t value, uint8_t controller)
{
	GPIO_E.odrHigh = value;
	select_controller(controller);
	delay_500ns();
	graphic_ctrl_bit_set(B_E);
	delay_500ns();
	graphic_ctrl_bit_clear(B_E);
	
	if(controller & B_CS1)
	{
		select_controller(B_CS1);
		graphic_wait_ready();
	}
	if(controller & B_CS2)
	{
		select_controller(B_CS2);
		graphic_wait_ready();
	}
}
static void graphic_write_command(uint8_t command, uint8_t controller)
{
	graphic_ctrl_bit_clear(B_E);
	select_controller(controller);
	graphic_ctrl_bit_clear(B_RS|B_RW);
	graphic_write(command, controller);
}
static void graphic_write_data(uint8_t data, uint8_t controller)
{
	graphic_ctrl_bit_clear(B_E);
	select_controller(controller);
	graphic_ctrl_bit_set(B_RS);
	graphic_ctrl_bit_clear(B_RW);
	graphic_write(data, controller);
}
void graphic_clear_screen(void)
{
	uint8_t i,j;
	
	for(j = 0; j < 8; j++)
	{
		graphic_write_command(LCD_SET_PAGE| j, B_CS1| B_CS2);
		graphic_write_command(LCD_SET_ADD| 0, B_CS1| B_CS2);
		for(i = 0; i <= 63; i++)
		{
			graphic_write_data(0, B_CS1|B_CS2);
		}
	}
}
void graphic_draw_screen(void)
{
	uint8_t i, j, controller, c;
	uint32_t k = 0;
	
	for(c = 0; c < 2; c++) 
	{
		controller = (c == 0) ? B_CS1 : B_CS2;
		for(j = 0; j < 8; j++) 
		{
			graphic_write_command(LCD_SET_PAGE | j, controller);
			graphic_write_command(LCD_SET_ADD  | 0, controller);
			for(i = 0; i <= 63; i++, k++) 
			{
				graphic_write_data(backbuffer[k], controller);
			}
		}
	}
}
void draw_object(POBJECT o)
{
     for(int i = 0; i < MAX_POINTS; i++)
	 {
		 pixel(o->geo->px[i].x+o->posx,o->geo->px[i].y+o->posy);//, 1);
	 }   
}
void set_object_speed(POBJECT o,int speedx,int speedy)
{
	o->dirx = speedx;
	o->diry = speedy;
}
void clear_object(POBJECT o)
{
	for(int i = 0; i < MAX_POINTS; i++)
	 {
		 pixel(o->geo->px[i].x+o->posx,o->geo->px[i].y+o->posy);//, 0);
	 }  
}
void move_object(POBJECT o)
{
	clear_object(o);
	o->posx += o->dirx;
	o->posy += o->diry;
	if(o->posx<1 || o->posx>126)
	{
		o->dirx = -o->dirx;
	}
	if(o->posy<1 || o->posy>62)
	{
		o->diry = -o->diry;
	}
	draw_object(o);
}
//uint8_t backbuffer[1024]; // 128 * 64 /8
void clear_backbuffer(void)
{
	int i;
	for( i = 0; i < 1024; i++)
	{
		backbuffer[i] = 0;
	}
}
void pixel(int x, int y)
{
	uint8_t mask;
	int index = 0;
	if((x>128)||(x<1)||(y>64)||(y<1)) return;
	
	mask = 1 << ((y-1)%8);
	
	if(x>64)
	{
		x-=65;
		index = 512;
	}
	index += x + ((y-1)/8)*64;
	
	backbuffer[index] |= mask;
}
void kbdActivate(unsigned int row)
{
	/* Aktivera angiven rad, eller deaktivera samtliga */
	switch(row)
	{
		case 1: {(GPIO_D.odrHigh) = 0x10; break;} // 0001 0000
		case 2: {(GPIO_D.odrHigh) = 0x20; break;} // 0010 0000
		case 3: {(GPIO_D.odrHigh) = 0x40; break;} // 0100 0000
		case 4: {(GPIO_D.odrHigh) = 0x80; break;} // 1000 0000
		case 0: {(GPIO_D.odrHigh) = 0x00; break;}
	}
}
int kdbGetCol(void)
{
	/* om någon tangent (iaktivera rad) är nedtryckt,
	 * returnera dess kolumnnummer, annars returnera 0 */
	 unsigned char c;
	 c = GPIO_D.idrHigh;
	 if(c & 0x8) {return 4;}
	 if(c & 0x4) {return 3;}
	 if(c & 0x2) {return 2;}
     if(c & 0x1) {return 1;}
	 return 0;
}
uint8_t key[] = {1,2,3,10,4,5,6,11,7,8,9,12,14,0,15,13};
uint8_t keyb(void)
{
	//uint8_t key[] = {1,2,3,10,4,5,6,11,7,8,9,12,14,0,15,13};
	int row, col;
	for(row = 1; row <= 4; row++)
	{
		kbdActivate(row);
		if((col = kdbGetCol()))
		{
			kbdActivate(0);
			return key [4*(row-1)+(col-1)];
		}
	}
	kbdActivate(0);
	return(0xFF);
}
void app_init()
{
#ifdef USBDM
	/*starta klockor port D och E */
	* ((unsigned long *) 0x40023830) = 0x18;
	__asm volatile ("LDR R0,=0x08000209\n BLX R0\n");
#endif
	GPIO_E.moder = 0x55555555;
	GPIO_D.moder = 0x55005555; // GPIO-D port D8-15 ... 7 segment display till port D0-7
	GPIO_D.otyper &= 0x00FF;	// THIS IS A SHORT.
	GPIO_D.pupdr &= 0xFF00FFFF;
	GPIO_D.pupdr |= 0x00AA0000;
}
