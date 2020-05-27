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

int revivingFace = 0;

Color playerColors[6] = {RED, ORANGE, YELLOW, GREEN, BLUE, MAGENTA};
Color playerColor;

enum Flags
{
  //States of each individual tile while running.
  READY,		// -> 0
  ALIVE,		// -> 1
  DEAD,			// -> 2
};

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
		/********* Standby State *********/
		
		case 0:
			playerColor = playerColors[random(5)];			
			setColor(playerColor);			
			gameState = 1;
			break;
		
		/********* Ready State *********/
		
		case 1:
			setValueSentOnAllFaces(READY);
			readyFlashFunc();
			if (buttonSingleClicked())
			{
				setColor(playerColor);	
				gameState = 2;
			}	
			break;
		
		/********* Play State *********/
		
		case 2:
			setValueSentOnAllFaces(ALIVE);
			if (millis() % 3 == 0)
			{
				brightness -= 5;
				setColor(dim(playerColor, brightness));
			}
			if (buttonPressed()) { brightness = 255; }
			if (brightness == 0)
			{
				setColor(OFF);
				gameState = 3;
			}
			break;
		
		/********* Dead State *********/
		
		case 3:			
			setValueSentOnAllFaces(DEAD);
			setColor(OFF);
			brightness = 0;
			// Jump to revival state if re-attached to a live tile.
			FOREACH_FACE(f)
			{
				if (didValueOnFaceChange(f) &&
					!isValueReceivedOnFaceExpired(f) &&
					getLastValueReceivedOnFace(f) == ALIVE)
				{
					revivingFace = f;	
					gameState = 4;
				}
			}

			// Hard reset to ready state.
			if (buttonDoubleClicked())
			{
				playerColor = playerColors[random(5)];
				setColor(playerColor);
				readyFlash.set(500);
				gameState = 1;
			}
			break;
		
		/********* Revive State *********/
		
		case 4:
			//If connection is broken before ready, return to dead state.
			
			if (isAlone() || isValueReceivedOnFaceExpired(revivingFace)) 
			{ 
				gameState = 3; 
			}
			if (didValueOnFaceChange(revivingFace) && 
				getLastValueReceivedOnFace(revivingFace) != ALIVE)
			{
				gameState = 3;
			}

			// Fade light in until full, return to Play state.
			if (millis() % 3 == 0)
			{
				if (brightness < 255)
				{
					brightness += 3;
					setColor(dim(playerColor, brightness));
				}
				else if (brightness == 255)
				{
					//Blink when full?
					readyFlashFunc();
					if (isValueReceivedOnFaceExpired(revivingFace) ||
						isAlone())
						{
							setColor(playerColor);
							gameState = 2;
						}
				}
			}
			break;

	}
}

void readyFlashFunc()
{
	if (readyFlash.isExpired()) 
	{
		readyLEDOn = !readyLEDOn;
		if (readyLEDOn) { setColor(playerColor); }
		else { setColor(OFF); }      
		readyFlash.set(500);   // Flash again in 500 millseconds  
	} 
}
