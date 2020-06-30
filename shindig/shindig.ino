#include "Serial.h"

//  "shindig" - A game by jwest.
//	Mercy is the mark of great men.  
//	I guess we're all right.
//  JJ

int gameState = 0;

bool damagedFace[5] = {false, false, false, false, false};

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
					setValueSentOnFace(EMPTY,f);
					setColorOnFace(OFF, f);
				}
				else
				{
					setValueSentOnFace(KNIFE, f);
					setColorOnFace(BLUE, f);
				}
			}
			gameState = 1;
		/********* Play State *********/
		case 1:
			// Listen for knife signal
			FOREACH_FACE(f)
			{
				if (f != 0)
				{
					if (getLastValueReceivedOnFace(f) == KNIFE && 
						!isValueReceivedOnFaceExpired(f))
					{
						damagedFace[f - 1] = true;
					}
					if (damagedFace[f - 1] == true)
					{
						setColorOnFace(RED,f);
					}
				}
			}
			// Reset Game
			if (buttonDoubleClicked())
			{
				for (int i = 0; i <= sizeof(damagedFace); i++)
				{
					damagedFace[i] = false;
				}
				gameState = 0;
			}
		/********* Dead State *********/
	}
}
