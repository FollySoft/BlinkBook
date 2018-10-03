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
  PLAYER2,    //Blue Tiles
  PLAYER3,   //Random Colors excluding Red/BluE
};

void setup() 
{
  setValueSentOnAllFaces(STANDBY);
}

void loop() 
{ 
  if (gameState==0)
  {
    Player1Count = 0;
    Player2Count = 0;
    FOREACH_FACE(f)
    {
      if (isValueReceivedOnFaceExpired(f))
        continue;
      if (getLastValueReceivedOnFace(f) == STANDBY)
        setColor(OFF);
    }
    if (buttonSingleClicked())
    {
        placeFlags();
        gameState = 1;
    }
    FOREACH_FACE(f)
    if(didValueOnFaceChange(f))
      gameState = 1;
  }
  
  if (gameState == 1)
  {
    FOREACH_FACE(f)
    {
      if (isValueReceivedOnFaceExpired(f))
        continue;
      if(didValueOnFaceChange(f))
        flag = getLastValueReceivedOnFace(f);
  
     switch(flag)
     {
        case STANDBY:
          setColor(OFF);
          break;
        case PLAYER1:
          setColor(RED);
          break;
        case PLAYER2:
          setColor(BLUE);
          break;
        case PLAYER3:
          setColor(YELLOW);
          break;
     }
    }
    if (buttonDoubleClicked())
    {
      gameState = 0;
      FOREACH_FACE(f)
      {
        setValueSentOnFace(STANDBY,f);
        if (getLastValueReceivedOnFace(f) == STANDBY)
          setColor(OFF);
      }
    }
  }
}

void placeFlags()
{
  int state = rand(2);
  if (state == 0 && Player1Count < 2)
    {
      setColor(RED);
      Player1Count++;
    }
  else if (state == 1 && Player2Count < 2)
    {
      setColor(BLUE);
      Player2Count++;
    }
  else
      setColor(YELLOW);
  FOREACH_FACE(f)
  {
    if (getLastValueReceivedOnFace(f) == STANDBY)
    {
     state = rand(2);
      if (state == 0 && Player1Count < 2)
      {
        flag = PLAYER1;
        Player1Count++;
      }
      else if (state == 1 && Player2Count < 2)
      {
        flag = PLAYER2;
        Player2Count++;
      }
      else
        flag = PLAYER3;
      
      setValueSentOnFace(flag, f); 
    }
  }

  if (Player1Count < 2 && Player2Count < 2)
    placeFlags();
}
