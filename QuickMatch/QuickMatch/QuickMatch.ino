#include "Serial.h"

/***************************

  TODO:
    - Seperate Color Update from gameState 2 to ensure no pre-mature colors display.
    - Remove buttonPress() condition to continue to gameState 3 after PlaceFlags.

****************************/    
Timer countdownStep;
int countDown = 8;
bool countDownLED = false;

// Number of Tiles on Board
// Should Max at 2
int Player1Count = 0;
int Player2Count = 0;
bool flagsPlaced = false;

// Number of Tiles have been pressed.
// Should Max at 2
int Player1Clicked = 0;
int Player2Clicked = 0;

//Score Keeper
int Player1Score = 0;
int Player2Score = 0;

// Tile State Info
bool gameStart = false;
int gameState = 0;
bool gameRef = false; //First tile that is clicked
int refFace = 0;      //Which face is the ref connected to.

bool Player1 = false;
bool Player2 = false;

byte flag;
ServicePortSerial sp;


enum Flags
{
  //States of each individual tile while running.
  STANDBY,
  COUNTDOWN,
  PLAYER1,      //Red Tiles
  PLAYER2,      //Blue Tiles
  PLAYER1PRESS,      //Red Tiles
  PLAYER2PRESS,      //Blue Tiles
  PLAYER1WIN,      //Red Tiles
  PLAYER2WIN,      //Blue Tiles
  NEWROUND,
  FLAGSPLACED,
  //PLAYER3,    //Random Colors excluding Red/BluE
};

void setup() 
{
  // Player specific variables.
  Player1Count = 0;
  Player2Count = 0;
  Player1Clicked = 0;
  Player2Clicked = 0;
  Player1 = false;
  Player2 = false;

  // GameRef Variables
  gameRef = false;
  flagsPlaced = false;
  
  // Default State
  setColor(OFF);
  gameState = 0;
  setValueSentOnAllFaces(STANDBY);

  // Countdown Variables
  countDownLED = true;
  countDown = 8;

  // Start debug
  sp.begin();
}

void loop() 
{  
  if (buttonDoubleClicked())
  {    
    setup();
  }

  switch (gameState)
  {
    // ********** Waiting for Game to Start **********
    case 0:
      sp.println(F("GameState 0"));
      // Check if ref tile's button was pressed, trigger countdown.
      standbyLoop();      
    break;
    
    // ********** Countdown **********
    case 1:
      sp.println(F("GameState 1"));      
      // Countdown Display
      countDownLoop();
    break;

    // ********** Place Flags, Update Colors **********
    case 2:
      sp.println(F("GameState 2"));
      // All other tiles update color.
      if (gameRef && !flagsPlaced)
      {
        placeFlags();
        if (Player1Count == 2 && Player2Count == 2)
        {
          // Flags evenly placed, proceed.          
          flagsPlaced = true;
          sp.println(F("SENT FLAGS"));
          //Force buttonPress check.
          //Ref gets automatic press on gameState 3 otherwise.
          buttonPressed();
          gameState = 3;     
        }
        else
        {
          // Reset player counter, try to randomize again.
          Player1Count = 0;
          Player2Count = 0;
        }
      }
      else if (!gameRef)
      {
        FOREACH_FACE(f)
        {  
          if(didValueOnFaceChange(f))
          {            
            flag = getLastValueReceivedOnFace(f);
            sp.println(F("FLAG RECIEVED!"));
            sp.println(flag);
            switch(flag)
             {
                case STANDBY:
                  setup();
                  break;
                case PLAYER1:
                  //setColor(RED);
                  Player1 = true;
                  Player2 = false;
                  gameState = 3;           
                  break;
                case PLAYER2:
                  //setColor(BLUE);
                  Player1 = false;
                  Player2 = true;                  
                  gameState = 3;
                  break;
              }            
          }              
        }
      } 
        // GameRef automatically goes to game state 3.    
    break;

    // ********** Update Colors, wait for presses **********
    case 3:
      sp.println(F("GameState 3"));
      sp.println(F("Player1Clicked:")), 
      sp.println(Player1Clicked);
      sp.println(F("Player2Clicked:"));
      sp.println(Player2Clicked);
      //Player1Clicked = 0;
      //Player2Clicked = 0;
      if (Player1)
      {
        setColor(RED);
        if (buttonPressed())
        {        
          if (gameRef)
          {
            sp.println(F("REF BUTTON PRESSED!"));
            Player1Clicked++;
          }
          else
          {            
            setValueSentOnAllFaces(PLAYER1PRESS);
            //gameState = 4;
          }
        }
      }      
      
      else if (Player2)
      {
        setColor(BLUE);
        if (buttonPressed())
        {
           if (gameRef)
          {
            sp.println(F("REF BUTTON PRESSED!"));
            Player2Clicked++;
          }
          else
          {            
            setValueSentOnAllFaces(PLAYER2PRESS);
            //gameState = 4;
          }

        }         
      }      

      // Victory checking for Ref Tile
      if (gameRef)
      {
        if (Player1Clicked == 2)
        {
          /*setColor(RED);
          setValueSentOnAllFaces(PLAYER1WIN);
          gameState = 4;*/
          Player1Score += 1;
          startNewRound();
        }
        else if (Player2Clicked == 2)
        {
          /*setColor(BLUE);
          setValueSentOnAllFaces(PLAYER2WIN);
          gameState = 4;*/
          Player2Score += 1;
          startNewRound();
        }

        FOREACH_FACE(f)
        {
          if (isValueReceivedOnFaceExpired(f))
          {
            continue;
          }
          if(didValueOnFaceChange(f))
          {
            flag = getLastValueReceivedOnFace(f);          
            switch(flag)
            {
                case PLAYER1PRESS:
                  sp.println(F("OTHER BUTTON PRESSED!"));
                  Player1Clicked++;
                  break;
                case PLAYER2PRESS:
                  sp.println(F("OTHER BUTTON PRESSED!"));
                  Player2Clicked++;
                  break;
            }
          }          
        }                          
      }
      // Victory checking for all other tiles.
      else
      {
        FOREACH_FACE(f)
        {
          if (isValueReceivedOnFaceExpired(f))
            continue;
          if (didValueOnFaceChange(f))
            flag = getLastValueReceivedOnFace(f);          
            switch(flag)
            {
                case NEWROUND:
                  gameState = 2;
                  startNewRound();
                  break;
                case STANDBY:
                  setup();
                  break;
                case PLAYER1WIN:
                  setColor(RED);
                  gameState = 4;
                  break;
                case PLAYER2WIN:
                  setColor(BLUE);
                  gameState = 4;
                  break;
                default:
                  break;
            }            
        }
      }
    break;

    case 4:
      sp.println("GameState 4");
      if (!gameRef)
      {
        FOREACH_FACE(f)
        {
          if(didValueOnFaceChange(f))
          {
             flag = getLastValueReceivedOnFace(f);
             switch(flag)
             {
                case STANDBY:
                  setup();
                  break;
                default:
                  break;
              }
          }
        }
      }
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
      else if (getLastValueReceivedOnFace(f) == COUNTDOWN)
      {
        sp.println(F("RECIEVED COUNTDOWN"));
        gameState = 1;
      }
    }
  }
  // Button was pressed, place flags and start game.  
  if (buttonLongPressed())
  {
      sp.println(F("BUTTON LONG PRESSED"));
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
          setValueSentOnFace(COUNTDOWN, f);
          sp.println(F("SENT COUNTDOWN")); 
        } 
      }
      gameState = 1;
  }
}

void countDownLoop()
{
  // Countdown Yellow Flash
  if (countDown > 0)
  {
    if (countDownLED) { setColor(YELLOW); }
    else { setColor(OFF); }
    
    if (countdownStep.isExpired())
    {            
      //setColor(OFF);
      sp.println(countDown);
      countDown--;
      countDownLED = !countDownLED;          
      countdownStep.set(500);
    }
  }   
  // Green "GO!" Flash     
  else    
  {
    //if (countDownLED) { setColor(GREEN); }
    //else { setColor(OFF); }
    setColor(GREEN);
    if (countdownStep.isExpired())
    {              
      //setColor(OFF);             
      gameState = 2;
      setValueSentOnAllFaces(NEWROUND);
      sp.println(F("SENT NEWROUND FLAG"));
      sp.println(NEWROUND);
    }       
  }        
}

void placeFlags()
{
  //int state = rand(2);
  int state = random(1);
  if (state == 0)
    {
      setColor(RED);
      Player1 = true;
      Player2 = false;
      ++Player1Count;
    }
  else if (state == 1)
    {
      setColor(BLUE);
      Player1 = false;
      Player2 = true;
      ++Player2Count;
    }
  FOREACH_FACE(f)
  {
    //Skip Inactive Face
    if (isValueReceivedOnFaceExpired(f))
        continue;
    // Randomize and send colors and states. 
    else if (getLastValueReceivedOnFace(f) == NEWROUND)
    {
      sp.println(F("SENDING PLAYER FLAG"));
      state = random(1);
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
}

void startNewRound()
{  
  flagsPlaced = false;
  Player1 = false;
  Player2 = false;
  Player1Count = 0;
  Player2Count = 0;
  Player1Clicked = 0;
  Player2Clicked = 0;
  gameState = 2;  
  setValueSentOnAllFaces(NEWROUND);
}
