#include "Serial.h"

//  "shindig" - A game by jwest.
//
//	Mercy is the mark of great men.  
//	I guess we're just good men.
//
//	...well, we're alright.
//
//  JJ

/********************
* Name Ideas:
*	- Brute Force
*	- Scuffle
*	- Frenzy
*	- (Challenge) To Arms
********************/

int gameState = 0;

enum gameWeapons
{
	SWORD,		// Face 0
	AXE,		// Faces 0 & 1
	CHAKRAM		// Faces 0, 2, & 4
};
int gameWeapon = 0;

int swordFaces[1] = {0};
int axeFaces[2] = {0, 1};
int chakramFaces[3] = {0, 2, 4};

int damage = 0;
bool damageTaken = false;
bool damagedFace[5] = {false, false, false, false, false};

Timer healthFlashTimer;
bool healthLEDOn = false;
int healthLEDSpeed;

Color bladeColor = makeColorHSB(210, 20, 220);

enum Flags
{
	EMPTY,
	BLADE
};

ServicePortSerial sp;

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
			sp.println("STATE 0");
			// Display current weapon
			switch (gameWeapon)
			{
				case SWORD:
					weaponDisplay(swordFaces, 1);					
				break;
				case AXE:
					weaponDisplay(axeFaces, 2);
				break;
				case CHAKRAM:
					weaponDisplay(chakramFaces, 3);
				break;
			}
			// Cycle through weapons
			if (buttonSingleClicked())
			{
				gameWeapon++;
				if (gameWeapon > 2) { gameWeapon = 0; }
			}
			// Start game
			if (buttonDoubleClicked())
			{
				if (isAlone())
				{
					gameState = 1;
				}
			}
		break;
		/********* Play State *********/
		case 1:
			// Listen for blade signal
			switch (gameWeapon)
			{
				case SWORD:
					weaponDetect(swordFaces, 1);
					if (damage > 0)
					{
						healthFlash(damage, swordFaces, 1);
					}
				break;
				
				case AXE:
					weaponDetect(axeFaces, 2);
					if (damage > 0)
					{
						healthFlash(damage, axeFaces, 2);
					}
				break;

				case CHAKRAM:
					weaponDetect(chakramFaces, 3);
					if (damage > 0)
					{
						healthFlash(damage, chakramFaces, 3);
					}
				break;
			}
			
			/*if (damage > 0)
			{
				healthFlash(damage);
			}*/
				
			//Jump to Dead State
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

void weaponDisplay(int weaponFaces[], int size)
{
	byte currentBrightness = max(225, random(255));
	byte currentHue = map(max(20, random(40)), 0, 360, 0, 255);
	
	FOREACH_FACE(f)
	{
		for (int i = 0; i < size; i++)
		{
			// If face matches face in weapon array, set blade
			if ((f - 1) == weaponFaces[i])
			{
				setValueSentOnFace(BLADE, f);
				setColorOnFace(bladeColor, f);
				goto cont;
			}
			else
			{
				setValueSentOnFace(EMPTY, f);
				setColorOnFace(makeColorHSB(currentHue, 255, currentBrightness), f);
				//setColorOnFace(OFF, f);
			}
		}
		cont:; // Continue to next face in loop
	}
}


void weaponDetect(int weaponFaces[], int size)
{
	FOREACH_FACE(f)
	{
		if (!compareFaces(f, weaponFaces, size))
		{
			// Take damage ONCE when blade face detected.
			if (getLastValueReceivedOnFace(f) == BLADE && 
				!isValueReceivedOnFaceExpired(f) &&
				damagedFace[f - 1] == false)
			{
				damagedFace[f - 1] = true;
				setColorOnFace(RED, f);
				damage++;
			}
			//  Allow face to be damaged again after blade expires.
			if (damagedFace[f - 1] == true &&
				isValueReceivedOnFaceExpired(f))
			{
				damagedFace[f - 1] = false;
			}
		}
		else
		{
			continue;
		}
	}
}


bool compareFaces(int blinkFace, int weaponFaces[], int size)
{
	bool matchFound = false;
	for (int i = 0; i < size; i++)
	{
		if ((blinkFace - 1) == weaponFaces[i])
		{
			matchFound = true;
			return matchFound;
		}
		else { continue; }
	}
	if (matchFound == false) { return matchFound; }
}

void healthFlash(int damage, int weaponFaces[], int size)
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
				if (!compareFaces(f, weaponFaces, size)) 
				{ 
					setColorOnFace(RED, f); 
				}
			}
		}
		else
		{
			FOREACH_FACE(f)
			{
				if (!compareFaces(f, weaponFaces, size))  
				{ 
					setColorOnFace(OFF, f); 
				}
			}
		}
		healthFlashTimer.set(healthLEDSpeed); 
	}	

}
