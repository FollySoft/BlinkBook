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
bool reviveReady = false;

Color playerColors[6] = {RED, ORANGE, YELLOW, GREEN, BLUE, MAGENTA};
Color playerColor;

ServicePortSerial sp;

enum Flags
{
  //States of each individual tile while running.
  READY,		// -> 0
  ALIVE,		// -> 1
  DEAD,			// -> 2
  REVIVE,		// -> 3
};

void setup()
{		
	brightness = 255;
	gameState = 0;
	randomize();
	sp.begin();
}

void loop()
{		
	switch (gameState)
	{
		/********* Standby State *********/
		
		case 0:
			sp.println(F("GameState STANDBY"));
			playerColor = playerColors[random(5)];			
			setColor(playerColor);			
			gameState = 1;
			break;
		
		/********* Ready State *********/
		
		case 1:
			sp.println(F("GameState READY"));
			readyFlashFunc();
			if (buttonPressed())
			//if (isAlone() || buttonSingleClicked())
			{
				setColor(playerColor);	
				gameState = 2;
			}	
			// Check if any neighbors need reviving.
			FOREACH_FACE(f)
			{
				if (getLastValueReceivedOnFace(f) == DEAD && 
					!isValueReceivedOnFaceExpired(f))
				{
					setValueSentOnFace(REVIVE, f);
				}
				else
				{
					setValueSentOnFace(READY, f);
				}
			}
			break;
		
		/********* Play State *********/
		
		case 2:
			sp.println(F("GameState PLAY"));
			// Dim over time
			//setValueSentOnAllFaces(ALIVE);
			if (millis() % 3 == 0)
			{
				brightness -= 5;
				setColor(dim(playerColor, brightness));
			}
			// Reset brightness
			if (buttonPressed()) { brightness = 255; }
			// Go to dead state
			if (brightness == 0)
			{
				setColor(OFF);
				gameState = 3;
			}

			// Check if any neighbors need reviving.
			FOREACH_FACE(f)
			{
				if (getLastValueReceivedOnFace(f) == DEAD && 
					!isValueReceivedOnFaceExpired(f))
				{
					setValueSentOnFace(REVIVE, f);
				}
				else
				{
					setValueSentOnFace(ALIVE, f);
				}
			}
			break;
		
		/********* Dead State *********/
		
		case 3:			
			sp.println(F("GameState DEAD"));
			setValueSentOnAllFaces(DEAD);
			setColor(OFF);
			brightness = 0;
			// Jump to revival state if re-attached to a live tile.
			if (isAlone()) { reviveReady = true; }
			FOREACH_FACE(f)
			{
				// Revive only if ready and a fresh Revive message is detected.
				if (reviveReady &&
					!isValueReceivedOnFaceExpired(f) &&
					getLastValueReceivedOnFace(f) == REVIVE)
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
				buttonPressed();
			}
			break;
		
		/********* Revive State *********/
		
		case 4:
			sp.println(F("GameState REVIVE"));
			//If connection is broken before ready, return to dead state.
			if (reviveReady)
			{
				if (isAlone() || isValueReceivedOnFaceExpired(revivingFace)) 
				{ 
					gameState = 3; 
				}
				if (didValueOnFaceChange(revivingFace) && 
					getLastValueReceivedOnFace(revivingFace) != REVIVE)
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
						// Return to ready state.
						reviveReady = false;
						gameState = 1;
						buttonPressed();
					}
				}
				break;
			}
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
