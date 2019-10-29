#include "Serial.h"

//  "HoldOut"
//  A game by jwest.
//  JJ

int playerCount = 0;
bool playerFound = false;

int brightness = 0;
int gameState = 0;

Color playerColors[6] = {RED, ORANGE, YELLOW, GREEN, BLUE, MAGENTA};
Color playerColor;

void setup()
{	
	playerColor = playerColors[random(5)];
	setColor(playerColor);
	brightness = 255;
	gameState = 0;
}

void loop()
{		
	switch (gameState)
	{
		case 0:
			if (buttonDoubleClicked())
			{
				gameState = 1;
			}
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
				setColor(dim(playerColor, brightness));
			}	
			break;
		case 2:
		if (buttonDoubleClicked())
		{
			setup();
		}
	}
}