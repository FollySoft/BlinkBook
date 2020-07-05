//  "Dynamo" - A game by jwest.
//	Inspired by the Pokemon Stadium mini-game "Thundering Dynamo".
//  JJ

int gameState = 0;
bool gameRef = false;

byte flag;
enum Flags
{
  //States of each individual tile while running.
  STANDBY,      // -> 0
  SETUP,    // -> 1
  COUNTDOWN,      // -> 2 (Red Tiles)
  EVEN_FACE,      // -> 3 (Blue Tiles)
  ODD_FACE, // -> 4 
  PLAYER2PRESS, // -> 5
  PLAYER1WIN,   // -> 6
  PLAYER2WIN,   // -> 7
  NEWROUND,     // -> 8
  FLAGSPLACED,  // -> 9
  READY,        // -> 10
  //PLAYER3,    //      (Yellow Tiles)
};

void setup()
{
	gameRef = false;
	setValueSentOnAllFaces(STANDBY);
}

void loop()
{
	switch (gameState)
	{
		// ********** Standby **********
		case 0:
			standbyLoop();
			break;

    	// ********** Setup **********
	    case 1:
			// Countdown Display
			setupLoop();
			break;
	}
}

void standbyLoop()
{
  // Wait for signal from "Ref" tile.
  if (!gameRef)
  {
    FOREACH_FACE(f)
    {
      if (isValueReceivedOnFaceExpired(f) || getLastValueReceivedOnFace(f) == STANDBY)
        continue;
      else if (getLastValueReceivedOnFace(f) == SETUP)
      {
        gameState = 1;
      }
    }
  }
  // Button was pressed, place flags and start game.  
  if (buttonLongPressed())
  {    
      // Declare this tile as "Ref"
      gameRef = true;
      FOREACH_FACE(f)
      {
        //Skip Inactive Face
        if (isValueReceivedOnFaceExpired(f))
            continue;
        // Begin Countdown
        else
        {
          setValueSentOnFace(SETUP, f);
        } 
      }
      gameState = 1;
  }
}

void setupLoop()
{
  // Countdown Yellow Flash
  if (gameRef)
  {
  	setColor(GREEN);
  	FOREACH_FACE(f)
  	{
  		if (f %2 != 0)
  		{
  			setValueSentOnFace(f, EVEN_FACE);
  		}
  		else
  		{
  			setValueSentOnFace(f, ODD_FACE);	
  		}
  	}
  }
  else
  {
  	FOREACH_FACE(f)
  	{
  		flag = getLastValueReceivedOnFace(f);
  		switch (flag)
  		{
  			case EVEN_FACE:
  				setColor(YELLOW);
  				break;
			case ODD_FACE:
				setColor(BLUE);
				break;
		}
  	}
  }
}