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

byte flag;


enum Flags
{
  //States of each individual tile while running.
  PLAYER1,    //Red Tiles
  PLAYER2,    //Blue Tiles
  NONPLAYER,   //Random Colors excluding Red/BluE
};

void setup() 
{
  setColor(OFF);
  
  // put your setup code here, to run once:
}

void loop() 
{
  if (buttonDoubleClicked())
  {
    placeFlags();
  }

  FOREACH_FACE(f)
  {
    if(didValueOnFaceChange(f))
    flag = getLastValueReceivedOnFace(f);

   switch(flag)
   {
      case PLAYER1:
        setColor(RED);
        break;
      case PLAYER2:
        setColor(BLUE);
        break;
      case NONPLAYER:
        setColor(YELLOW);
        break;
   }
  }
  
}

void placeFlags()
{
  int state = rand(2);
  if (state == 0 && Player1Count <= 2)
    {
      setColor(RED);
      Player1Count++;
    }
  else if (state == 1 && Player2Count <= 2)
    {
      setColor(BLUE);
      Player2Count++;
    }
  else
      setColor(YELLOW);
  FOREACH_FACE(f)
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
      flag = NONPLAYER;
      
   setValueSentOnFace(flag, f);
  }

  if (Player1Count < 2 && Player2Count < 2)
    placeFlags();
}
