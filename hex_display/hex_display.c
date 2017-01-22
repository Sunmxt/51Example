#include <reg52.h>

unsigned char code display_code[] = 
	{0xC0, 0xF9, 0xA4, 0xB0, 0xD9, 0xD2, 0x72, 0xF8, 0x00, 0x90};

void main()
{
	int i, j;
	while(1)
	{
		for(i = 0 ; i < 4 ; i++)
		{
			P2 = (~(1 << i)) & 0xF;
			P0 = display_code[i];
			for(j = 0 ; j<110 ;j++);
			P2 = 0xFF;
		}
	}
}