#include "Serial.h"

//  "HoldOut" - A game by jwest.
//	Inspired by the game "Chicanery", by Anna Anthropy.
//  JJ

Timer readyFlash;
bool readyLEDOn = false;

int playerCount = 0;
bool playerFound = false;

int brightness = 0;

int gameState = 0;

Color playerColors[6] = {RED, ORANGE, YELLOW, GREEN, BLUE, MAGENTA};
Color playerColor;

void setup()
{		
	brightness = 255;
	gameState = 0;
	randomize();
}

void loop()
{		
	switch (gameState)
	{
		/********* Setup State *********/
		case 0:
			playerColor = playerColors[random(5)];
			setColor(playerColor);
			buttonPressed();
			gameState = 1;
			break;
		/********* Ready State *********/
		case 1:
			if (readyFlash.isExpired()) 
			{
				readyLEDOn = !readyLEDOn;
				if (readyLEDOn) { setColor(playerColor); }
				else { setColor(OFF); }      
				readyFlash.set(500);   // Flash again in 500 millseconds  
			} 
			if (buttonPressed())
			{
				setColor(playerColor);	
				gameState = 2;
			}	
			break;
		/********* Play State *********/
		case 2:		
			if (millis() % 3 == 0)
			{
				brightness -= 5;
				setColor(dim(playerColor, brightness));
			}
			
			if (buttonPressed()) { brightness = 255; }
			if (brightness == 0)
			{
				buttonDoubleClicked(); //Avoid skipping dead state.
				setColor(OFF);
				gameState = 3;
			}
			break;
		/********* Dead State *********/
		case 3:
			if (buttonDoubleClicked())
			{
				//Reset Variables, return to Ready State
				playerColor = playerColors[random(5)];
				setColor(playerColor);
				buttonPressed();
				brightness = 255;
				readyFlash.set(500);
				gameState = 0;
			}
			break;
	}
}
