#include "Serial.h"

//  "Doubles"
//  A game by jwest.
//  JJ

/***************************
  TODO:
    1. Code Refactor.

    2. Find better solution for resetting a game (long Press?).

    3. Store previous flag placement to ensure you dont get same pattern twice.

    4. Find a way to ensure 3 blinks are connected before starting countdown.

    5. Find a way to allow re-connection if formation breaks.

    6. Remove buttonPress() condition to continue to gameState 3 after PlaceFlags?

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
int refTileFace = NULL;

bool Player1 = false;
bool Player2 = false;
bool playerPressed = false;

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
  //PLAYER3,    //      (Yellow Tiles)
};

byte playerFaces[6];

void setup() 
{
  // Player specific variables.
  Player1Count = 0;
  Player2Count = 0;
  Player1Clicked = 0;
  Player2Clicked = 0;
  Player1Score = 0;
  Player2Score = 0;
  Player1 = false;
  Player2 = false;
  playerPressed = false;

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
        setValueSentOnAllFaces(NEWROUND);
        placeFlags();
        if (Player1Count == 2 && Player2Count == 2)
        {
          // Flags evenly placed, proceed.          
          flagsPlaced = true;
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
        setValueSentOnAllFaces(NEWROUND);
        FOREACH_FACE(f)
        {
          flag = getLastValueReceivedOnFace(f);
          sp.println(F("FLAG RECIEVED!"));
          sp.println(f);
          sp.println(flag);
          switch(flag)
          {                                
            case PLAYER1:
              sp.println(F("I AM PLAYER 1"));
              refTileFace = f;
              setColor(RED);
              Player1 = true;
              Player2 = false;
              buttonPressed();
              gameState = 3;
              break;
            case PLAYER2:
              sp.println(F("I AM PLAYER 2"));
              refTileFace = f;
              setColor(BLUE);
              Player1 = false;
              Player2 = true;
              buttonPressed();
              gameState = 3;
              break;
          }            
        }                      
      }         
    break;

    // ********** Listen for Button Presses **********
    case 3:
      sp.println(F("GameState 3"));
      if (gameRef)
      {        
        //Assign Player Values
        FOREACH_FACE(f)
        {
          //Update Player Faces
          if (playerFaces[f] != 0)
          {          

            setValueSentOnFace(playerFaces[f],f);
            
            //Debug values being sent on faces.
            //sp.println(F("SENT TO FACE"));
            //sp.println(f);
            //sp.println(playerFaces[f]); 
          }

          //Listen for  player presses
          if (didValueOnFaceChange(f))
          {
            flag = getLastValueReceivedOnFace(f);
            sp.println(F("FLAG RECIEVED!"));
            sp.println(flag);   
            switch(flag)
            {                
                //case STANDBY:
                //  setup();
                //  break;
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

        //Listen for REF button presses.
        if (Player1 && !refPressed)
        {
          if (buttonPressed())
          {
            sp.println(F("REF PRESSED!"));
            Player1Clicked++;
            refPressed = true;
          }          
        }
        else if (Player2 && !refPressed)
        {
          if (buttonPressed())
          {
            sp.println(F("REF PRESSED!"));
            Player2Clicked++;
            refPressed = true;          
          }          
        }

        // Check Player Score
        if (Player1Clicked == 2)
        {
          Player1Score++;
          if (Player1Score == 10)
          {
            setColor(RED);
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
            setColor(BLUE);
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
        // Listen for NEWROUND from Ref.
        flag = getLastValueReceivedOnFace(refTileFace);
        sp.println(F("FLAG RECIEVED FROM REF:"));
        sp.println(flag);
        //sp.println(flag);
        switch (flag)
        {            
          //case STANDBY:
          //  setup();
          //  break;
          case NEWROUND:
            //Send player back to State 2 to listen for new color.
            sp.println(F("NEWROUND RECIEVED!"));
            startNewRound();
            break;            
          case PLAYER1WIN:
            //Go to gamestate 4 to announce winner.
            setColor(RED);
            gameState = 4;
            break;
          case PLAYER2WIN:
            setColor(BLUE);
            gameState = 4;
            break;
        }

        // Send button press signal to ref.
        if (!playerPressed && buttonPressed())
        {
          playerPressed = true;
          sp.println(F("BUTTON PRESSED!"));
        }
        // Tell everyone you've been pressed.
        if (playerPressed)
        {
          if (Player1)
          {
            //sp.println(F("SENDING PRESS!"));
            setValueSentOnAllFaces(PLAYER1PRESS);
          }
          else if (Player2)
          {
            //sp.println(F("SENDING PRESS!"));
            setValueSentOnAllFaces(PLAYER2PRESS);
          }
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
    setColor(GREEN);
    if (countdownStep.isExpired())
    {              
      //setColor(OFF);                   
      setValueSentOnAllFaces(NEWROUND);
      //sp.println(F("SENT NEWROUND FLAG"));
      //sp.println(NEWROUND);
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
    flag = getLastValueReceivedOnFace(f);
    sp.println(F("VALUE RECIEVED ON FACE"));
    sp.println(f);
    sp.println(flag);
    //Skip Inactive Face
    if (isValueReceivedOnFaceExpired(f))
        continue;
    // Randomize and send colors and states.      
    if (getLastValueReceivedOnFace(f) == NEWROUND)
    {
      sp.println(F("SENDING PLAYER FLAG"));
      state = random(1);
      if (state == 0 && Player1Count < 2)
      {
        playerFaces[f] = PLAYER1;        
        ++Player1Count;
      }
      else if (state == 1 && Player2Count < 2)
      {
        playerFaces[f] = PLAYER2;        
        ++Player2Count;
      }  
    } 
  }
}

void startNewRound()
{  
  refPressed = false;
  playerPressed = false;
  flagsPlaced = false;
  Player1 = false;
  Player2 = false;
  Player1Count = 0;
  Player2Count = 0;
  Player1Clicked = 0;
  Player2Clicked = 0;
  gameState = 2;
  buttonPressed();
  setValueSentOnAllFaces(NEWROUND);
}
