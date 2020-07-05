#include "Serial.h"

//  "shindig" - A game by jwest.
//	Mercy is the mark of great men.  
//	I guess we're all right.
//  JJ

/********************
* Name Ideas:
*	- Brute Force
*	- Scuffle
*	- Frenzy
********************/

int gameState = 0;

int damage = 0;
bool damageTaken = false;
bool damagedFace[5] = {false, false, false, false, false};

Timer healthFlashTimer;
bool healthLEDOn = false;
int healthLEDSpeed;

Color knifeColor = makeColorHSB(210, 20, 255);

enum Flags
{
	EMPTY,
	KNIFE
};

void setup()
{		
	gameState = 0;
}

void loop()
{		
	switch (gameState)
	{
		/********* Setup State *********/
		case 0:
			FOREACH_FACE(f)
			{
				if (f != 0)
				{
					setColorOnFace(OFF, f);
				}
				else
				{
					setValueSentOnFace(KNIFE, f);
					setColorOnFace(knifeColor, f);
				}
			}
			if (isAlone())
			{
				gameState = 1;
			}
		break;
		/********* Play State *********/
		case 1:
			// Listen for knife signal
			FOREACH_FACE(f)
			{
				if (f != 0)
				{
					// Take damage ONCE when knife face detected.
					if (getLastValueReceivedOnFace(f) == KNIFE && 
						!isValueReceivedOnFaceExpired(f) &&
						damagedFace[f-1] == false)
					{
						damagedFace[f - 1] = true;
						damage++;
					}
					//  Allow face to be damaged again after knife expires.
					if (damagedFace[f - 1] == true &&
						isValueReceivedOnFaceExpired(f))
					{
						damagedFace[f - 1] = false;
					}
				}

				/*
				if (f != 0)
				{
					// Add to damage counter if recently stabbed.
					if (getLastValueReceivedOnFace(f) == KNIFE && 
						!isValueReceivedOnFaceExpired(f) &&
						!damageTaken)
					{
						damageTaken = true;
						damage++;
					}
					// Allow for additional stabs if face empty for 200ms.
					else if (getLastValueReceivedOnFace(f) == KNIFE &&
							isValueReceivedOnFaceExpired(f) &&
							damageTaken)
					{
						damageTaken = false;
					}
				}
				*/
			}

			if (damage > 0)
			{
				healthFlash(damage);
			}
				
			// Jump to Dead State
			if (damage == 3)
			{
				setColor(RED);
				buttonDoubleClicked();
				gameState = 2;
			}
		break;
		/********* Dead State *********/
		case 2:
			// Reset Game
			if (buttonDoubleClicked())
			{
				damage = 0;
				damageTaken = false;
				FOREACH_FACE(f)
				{
					setColorOnFace(OFF,f);
				}
				gameState = 0;
			}
		break;
	}
}

void healthFlash(int damage)
{
	switch (damage)
	{
		case 0:
			// Do nothing.
		break;

		case 1:
			// Taken 1 hit.
			healthLEDSpeed = 800; // Flash every 800 ms
		break;
		
		case 2:
			// Taken 2 Hits
			healthLEDSpeed = 400; // Flash every 400 ms
		break;
		
		default:
		break;
	}
	
	if (healthFlashTimer.isExpired()) 
	{
		healthLEDOn = !healthLEDOn;
		if (healthLEDOn) 
		{ 
			FOREACH_FACE(f)
			{
				if (f != 0) { setColorOnFace(RED, f); }
			}
		}
		else
		{
			FOREACH_FACE(f)
			{
				if (f != 0) { setColorOnFace(OFF, f); }
			}
		}
		healthFlashTimer.set(healthLEDSpeed); 
	} 

}
