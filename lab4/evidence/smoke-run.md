# Playable-build check

Validated on Windows with MinGW GCC 6.3.0.

Command: `g++ -std=c++17 -Wall -Wextra code/game.cpp -o snake.exe`

Observed sequence:

1. The welcome screen showed the existing controls and saved high score.
2. A key started the game; the board rendered a three-cell snake and food.
3. With no further direction input, the snake continued right until it hit the wall.
4. The game-over screen showed final score 0.
5. Pressing Q exited normally with status 0.

This checks the refactored executable's start, movement, wall collision and quit paths.
The deterministic unit tests cover food, scoring and growth.
