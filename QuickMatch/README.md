# QuickMatch - A Game for Blinks
###### By Jonathan West

| Number of Blinks | Number of Players | Duration of Gameplay | Recommended Ages |
|------------------|:-----------------:|:--------------------:|-----------------:|
|  4            | 2             |  Less than1 minute/round    | 12 & Up          |

QuickMatch is a competitive reactionary game for 2-3 players. You must be faster than your oponents in matching two of the same colors.  The first player to match their color 10 times wins the game!

**Note**: The game is currently in Alpha, which means while it is playable, you are very likely to encounter bugs.  Keep an eye on this repository for updates!

## To-Do
1. **BUG** - Tiles occasionally fail to randomize during a new round.  Seems tied to certain tiles continuously sending "buttonPressed" flag.
2. **BUG** - Tiles ocassionaly fail to update flag from gameRef, resulting in uneven player placement.
3. Code Refactoring.
4. Implement way to recover game if tiles get separated.
5. Prevent getting the same randomized pattern twice in a row.
6. Support for 3rd Player.

## SETUP
Arrange the tiles so that one tile is touching all other tiles.  This tile will act as the "referee", keeping track of button presses and the overall score of the game.  Players pick a color (Red or Blue), and begin the game by pressing down the "referee" tile for 3 seconds.  A countdown will initialize (3 yellow flashes followed by a green flash), and the game will begin!


## GAMEPLAY
Tiles in play will display player colors at random (2 Red, 2 Blue).  Players must be quick and press the two tiles that match their color before their opponent to score a point.  When the game detects that two tiles of the same color have been pressed, the tiles will re-randomize, starting a new round.

## WIN CONDITION
The first player to match their color 10 times wins!  This will be indicated by all tiles in play displaying the winning player's color.