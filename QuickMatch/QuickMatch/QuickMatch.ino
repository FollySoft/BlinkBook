#include "Serial.h"

//  "Doubles"
//  A game by jwest.
//  JJ

/***************************

  TODO:
    - Seperate Color Update from gameState 2 to ensure no pre-mature colors display.
      - Store flags in array when finished, signal FLAGSPLACED, place flags from array?
    - Remove buttonPress() condition to continue to gameState 3 after PlaceFlags.

****************************/    
Timer countdownStep;
int countDown = 8;
bool countDownLED = false;

// Number of Tiles on Board
// Should Max at 2
int Player1Count = 0;
int Player2Count = 0;
int playerReady = 0;
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
bool gameRef = false;     //First tile that is clicked
bool refPressed = false;  //Keep track of ref button press

bool Player1 = false;
bool Player2 = false;

byte flag;
ServicePortSerial sp;


enum Flags
{
  //States of each individual tile while running.
  STANDBY,      // -> 0
  COUNTDOWN,    // -> 1
  PLAYER1,      // -> 2 (Red Tiles)
  PLAYER2,      // -> 3 (Blue Tiles)
  PLAYER1PRESS, // -> 4 
  PLAYER2PRESS, // -> 5
  PLAYER1WIN,   // -> 6
  PLAYER2WIN,   // -> 7
  NEWROUND,     // -> 8
  FLAGSPLACED,  // -> 9
  READY,        // -> 10
  //PLAYER3,    //Random Colors excluding Red/BluE
};

byte playerFaces[6];

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
  refPressed = false;
  
  // Default State
  setColor(OFF);
  gameState = 0;
  setValueSentOnAllFaces(STANDBY);

  // Countdown Variables
  countDownLED = false;
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
          //Force buttonPress check.
          //Ref gets automatic press on gameState 3 otherwise.
          buttonPressed();
          FOREACH_FACE(f)
          /*{
            setValueSentOnFace(f, FLAGSPLACED);
          }*/
          setValueSentOnAllFaces(FLAGSPLACED);
          sp.println(F("SENT FLAGSPLACED"));

          // Reset Player Count to re-use in next state.
          //Player1Count = 0;
          //Player2Count = 0;
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
            switch(flag)
            {                
                case FLAGSPLACED:
                case PLAYER1:
                case PLAYER2:
                  gameState = 3;
                  break;
            }            
          }              
        }
      }         
    break;

    case 3:
      sp.println(F("GameState 3"));
      if (gameRef)        
      {
        //Assign Player Values
        FOREACH_FACE(f)
        {
          if (playerFaces[f] != 0)
          {
            setValueSentOnFace(f, playerFaces[f]);
            sp.println(F("SENT TO FACE"));
            sp.println(f);
            sp.println(playerFaces[f]); 
          }
          //Listen for presses
          if (isValueReceivedOnFaceExpired(f))
          {
            continue;
          }
          if (didValueOnFaceChange(f))
          {
            flag = getLastValueReceivedOnFace(f);
            sp.println(F("FLAG RECIEVED!"));
            sp.println(flag);   
            switch(flag)
            {                
                /*case STANDBY:
                  setup();
                  break;*/
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

        //Listen for ref button presses.
        if (buttonPressed() && Player1 && !refPressed)
        {
          Player1Clicked++;
          refPressed = true;
        }
        else if (buttonPressed() && Player2 && !refPressed)
        {
          Player2Clicked++;
          refPressed = true;          
        }

        if (Player1Clicked == 2)
        {
          Player1Score++;
          if (Player1Score == 10)
          {
            setValueSentOnAllFaces(PLAYER1WIN);
            gameState = 4;
          }
          else
          {
            startNewRound();
          }          
        }
        else if (Player2Clicked == 2)
        {
          Player2Score++;
          if (Player2Score == 10)
          {
            setValueSentOnAllFaces(PLAYER2WIN);
            gameState = 4;
          }
          else
          {
            startNewRound();
          }          
        }
      }

      else if (!gameRef)
      {
        FOREACH_FACE(f)
        {
          if (isValueReceivedOnFaceExpired(f))
          {
            continue;
          }
          else
          {
            flag = getLastValueReceivedOnFace(f);
            sp.println(F("FLAG RECIEVED!"));
            sp.println(flag);   
            switch(flag)
            {                
                /*case STANDBY:
                  setup();
                  break;*/
                case PLAYER1:
                  setColor(RED);
                  Player1 = true;
                  Player2 = false;
                  break;
                case PLAYER1WIN:
                  setColor(RED);
                  gameState = 4;
                  break;
                case PLAYER2:
                  setColor(BLUE);
                  Player1 = false;
                  Player2 = true;
                  break;
                case PLAYER2WIN:
                  setColor(BLUE);
                  gameState = 4;
                  break;
                case NEWROUND:
                  startNewRound();
                  break; 
            }   
          }
        }

        // Send button press signal to ref.
        if (buttonPressed() && Player1)
        {
          setValueSentOnAllFaces(PLAYER1PRESS);          
        }
        else if (buttonPressed() && Player2)
        {
          setValueSentOnAllFaces(PLAYER2PRESS);
        }
      }
    break;

    // ********** Update Colors, wait for presses **********
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
  if (countDown > 1)
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
      setValueSentOnAllFaces(NEWROUND);
      sp.println(F("SENT NEWROUND FLAG"));
      sp.println(NEWROUND);
      gameState = 2;
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
        playerFaces[f] = PLAYER1;
        setValueSentOnFace(PLAYER1, f);
        ++Player1Count;
      }
      else if (state == 1 && Player2Count < 2)
      {
        playerFaces[f] = PLAYER2;
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
