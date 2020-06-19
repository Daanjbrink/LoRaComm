#include "../Library/lcd.h"
#include "../Library/lora.h"
#include "../Library/spi.h"

int main()
{
	spiInit();
	lcdInit();
	loraInit();

	while (1)
	{
		
	}
}