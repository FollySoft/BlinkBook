#include "Serial.h"

//  "HoldOut"
//  A game by jwest.
//  JJ

int brightness = 0;
int gameState = 0;

void setup()
{
	setColor(BLUE);
	brightness = 255;

	gameState = 1;
}

void loop()
{		
	switch (gameState)
	{
		case 0:
			break;
		case 1:
			if (buttonPressed())
			{
				brightness = 255;
			}
			if (brightness == 0)
			{
				setColor(OFF);
				gameState = 2;
			}
			else
			{
				brightness = brightness - 1;
				setColor(dim(BLUE, brightness));
			}	
			break;
		case 2:
		if (buttonDoubleClicked())
		{
			setup();
		}
	}
}