#include "draw.h"
#include <string.h>
#include "resources.h"
#include "driver_oled_hardware.h"

#define animation_active() (false)
#define animation_movingOn() (false)
#define animation_offsetY() (0)

static uint8_t *oledBuffer;
static uint32_t g_xres, g_yres, g_bpp;

inline static void setBuffByte(byte*, byte, byte, byte);//, byte);
inline uint8_t pgm_read_byte (const uint8_t *abc) {return *abc;}


inline static void setBuffByte(byte* buff, byte x, byte y, byte val)//, byte colour)
{
	uint32_t pos = x + (y / 8) * g_xres;
	buff[pos] |= val;
}

inline static byte readPixels(const byte* loc, bool invert)
{
	//byte pixels = ((int)loc & 0x8000 ? eeprom_read_byte((int)loc & ~0x8000) : pgm_read_byte(loc));
	byte pixels = pgm_read_byte(loc);  //d读取flash里面的数据到ram
	if(invert)
		pixels = ~pixels;
	return pixels;
}

/* 把newval中从bit0开始的num_bit个数据位,
 * 设置进oldval里(从start_bit开始)
 * 比如:
 * oldval的bit[start_bit]设置为newval的bit[0]
 * oldval的bit[start_bit+1]设置为newval的bit[1]
 * 保留oldval中其他位不变
 */
static uint8_t set_bits(uint8_t oldval, uint8_t newval, uint8_t start_bit, uint8_t num_bit)
{
    uint8_t bitMsk = (1<<num_bit) - 1;
    newval &= bitMsk;

    oldval &= ~(bitMsk << start_bit);
    oldval |= (newval << start_bit);
    return oldval;
}

void draw_string_P(const char* string, bool invert, byte x, byte y)
{
	byte len = strlen(string);
	char buff[len+1];
	strcpy(buff, string);
	draw_string(buff, invert, x, y);
}


void draw_string(char* string, bool invert, byte x, byte y)
{
	byte charCount = 0;
	while(*string)
	{
		char c = *string - 0x20;
		byte xx = x + (charCount*7);
		draw_bitmap(xx, y, smallFont[(byte)c], SMALLFONT_WIDTH, SMALLFONT_HEIGHT, invert, 0);
        draw_flushArea(xx,y, SMALLFONT_WIDTH, SMALLFONT_HEIGHT);
		if(invert)
		{
			if(xx > 0){
				setBuffByte(oledBuffer, xx-1, y, 0xFF);//, WHITE);
                draw_flushArea(xx-1, y, 1, 8);
			}
			if(xx < g_xres - 5){
				setBuffByte(oledBuffer, xx+5, y, 0xFF);//, WHITE);
                draw_flushArea(xx+5, y, 1, 8);
			}
		}
		string++;
		charCount++;
	}
}


// Ultra fast bitmap drawing
// Only downside is that height must be a multiple of 8, otherwise it gets rounded down to the nearest multiple of 8
// Drawing bitmaps that are completely on-screen and have a Y co-ordinate that is a multiple of 8 results in best performance
// PS - Sorry about the poorly named variables ;_;
void draw_bitmap(byte x, byte yy, const byte* bitmap, byte w, byte h, bool invert, byte offsetY)
{
	// Apply animation offset
	yy += animation_offsetY();

	// 
	byte y = yy - offsetY;

	// 
	byte h2 = (h + 7) / 8;
	
	// 
	byte pixelOffset = (y % 8);

	byte thing3 = (yy+h);
	
	// 
	LOOP(h2, hh)
	{
		// 
		byte hhh = (hh * 8) + y; // Current Y pos (every 8 pixels)
		byte hhhh = hhh + 8; // Y pos at end of pixel column (8 pixels)

		// 
		if(offsetY && (hhhh < yy || hhhh > g_yres || hhh > thing3))
			continue;

		// 
		byte offsetMask = 0xFF;
		if(offsetY)
		{
			if(hhh < yy)
				offsetMask = (0xFF<<(yy-hhh));
			else if(hhhh > thing3)
				offsetMask = (0xFF>>(hhhh-thing3));
		}

		uint32_t aa = ((hhh / 8) * g_xres);
		
		const byte* b = bitmap + (hh*w);
				
		// If() outside of loop makes it faster (doesn't have to keep re-evaluating it)
		// Downside is code duplication
		if(!pixelOffset && hhh < g_yres)
		{
			byte num_bit = 8 - pixelOffset;
			if (num_bit > h)
				num_bit = h;
			// 
			LOOP(w, ww)
			{
				// Workout X co-ordinate in frame buffer to place next 8 pixels
				byte xx = ww + x;
			
				// Stop if X co-ordinate is outside the frame
				if(xx >= g_xres)
					continue;

				// Read pixels
				byte pixels = readPixels(b + ww, invert) & offsetMask;

                /* 把pixels中的bits位设置进oledBuffer */
                oledBuffer[xx + aa] = set_bits(oledBuffer[xx + aa], pixels, 0, num_bit);
			}
			h -= num_bit;	
		}
		else
		{
			uint32_t aaa = ((hhhh / 8) * g_xres);
			byte tmp_h = h;
			
			// 
			LOOP(w, ww)
			{
				tmp_h = h;
				byte num_bit = 8 - pixelOffset;
				if (num_bit > tmp_h)
					num_bit = tmp_h;
				
				// Workout X co-ordinate in frame buffer to place next 8 pixels
				byte xx = ww + x;
		
				// Stop if X co-ordinate is outside the frame
				if(xx >= g_xres)
					continue;

				// Read pixels
				byte pixels = readPixels(b + ww, invert) & offsetMask;
				// 
				if(hhh < g_yres)
				{
                    oledBuffer[xx + aa] = set_bits(oledBuffer[xx + aa], pixels, pixelOffset, num_bit);
					tmp_h -= num_bit;
				}

				// 
				if(tmp_h && (hhhh < g_yres))
				{
					num_bit = 8 - num_bit;
					if (num_bit > tmp_h)
						num_bit = tmp_h;
                    
					pixels >>= (8 - pixelOffset);
                    oledBuffer[xx + aaa] = set_bits(oledBuffer[xx + aaa], pixels, 0, num_bit);
					tmp_h -= num_bit;	
				}
			}

			h = tmp_h;
		}
	}
}

volatile int bInUsed = 0;
void draw_flushArea(byte x, byte y, byte w, byte h){  
    while (bInUsed);
    bInUsed = 1;
    OLED_FlushRegion(x, y, w, h);
    bInUsed = 0;
}

void draw_clearArea(byte x, byte y, byte w){
    uint32_t pos = x + (y / 8) * g_xres;
    memset(&oledBuffer[pos], 0x00, w);
}


void draw_end(void){
    OLED_Flush();
}

void draw_init(void){
    oledBuffer = OLED_GetFrameBuffer(&g_xres, &g_yres, &g_bpp);
}
