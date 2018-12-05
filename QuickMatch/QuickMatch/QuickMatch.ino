// Number of Tiles on Board
// Should Max at 2
int Player1Count = 0;
int Player2Count = 0;

// Number of Tiles have been pressed.
// Should Max at 2
int Player1Clicked = 0;
int Player2Clicked = 0;

//Score Keeper
int Player1Score = 0;
int Player2Score = 0;

int gameState = 0;

byte flag;


enum Flags
{
  //States of each individual tile while running.
  STANDBY,
  PLAYER1,    //Red Tiles
  PLAYER2    //Blue Tiles
  //PLAYER3,   //Random Colors excluding Red/BluE
};

void setup() 
{
  gameState = 0;
  setValueSentOnAllFaces(STANDBY);
}

void loop() 
{ 
  if (gameState==0)
  {
    Player1Count = 0;
    Player2Count = 0;
    setColor(OFF);
    FOREACH_FACE(f)
    {
      if (isValueReceivedOnFaceExpired(f))
        continue;
      if (getLastValueReceivedOnFace(f) == STANDBY)
      {
        Player1Count = 0;
        Player2Count = 0;
        setColor(OFF);
      }
      else
      {
        gameState = 1;
      }
    }
    if (buttonSingleClicked())
    {
        placeFlags();
        gameState = 1;
    }
    /*FOREACH_FACE(f)
    {
      if(didValueOnFaceChange(f))
        gameState = 1;
    }*/
  }
  
  else if (gameState == 1)
  {
    FOREACH_FACE(f)
    {
      if(didValueOnFaceChange(f))
      {
         flag = getLastValueReceivedOnFace(f);
         switch(flag)
         {
            case STANDBY:
              gameState = 0;
              setColor(OFF);
              break;
            case PLAYER1:
              setColor(RED);
              break;
            case PLAYER2:
              setColor(BLUE);
              break;
          }
      }
    }
    if (buttonDoubleClicked())
    {    
      FOREACH_FACE(f)
      {
        setValueSentOnFace(STANDBY,f);
        if (getLastValueReceivedOnFace(f) == STANDBY)
          setColor(OFF);
      }
      Player1Count = 0;
      Player2Count = 0;
      gameState = 0;
    }
  }
}

void placeFlags()
{
  //int state = rand(2);
  int state = rand(1);
  if (state == 0 && Player1Count < 2)
    {
      setColor(RED);
      ++Player1Count;
    }
  else if (state == 1 && Player2Count < 2)
    {
      setColor(BLUE);
      ++Player2Count;
    }
  FOREACH_FACE(f)
  {
    //Skip Inactive Face
    if (getLastValueReceivedOnFace(f) != STANDBY)
        continue;
    // Randomize and send colors and states. 
    else if (getLastValueReceivedOnFace(f) == STANDBY)
    {
     state = rand(1);
      if (state == 0 && Player1Count < 2)
      {
        setValueSentOnFace(PLAYER1, f);
        ++Player1Count;
      }
      else if (state == 1 && Player2Count < 2)
      {
        setValueSentOnFace(PLAYER2, f);
        ++Player2Count;
      }    
    } 
  }

      //Ensure that only 2 of each piece is sent.
    if (Player1Count != 2 && Player2Count != 2)
    {
      Player1Count = 0;
      Player2Count = 0;
      placeFlags(); 
    }
}
