#include "drivers/lcd.h"
#include "drivers/spi.h"
#include "lora/lora.h"

int main()
{
	spiInit();
	lcdInit();
	loraInit();

	while (1)
	{
		
	}
}