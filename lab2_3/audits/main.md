# Code Smell Audit (main)

| Smell | File:Line | Severity | Confidence | Suggested Refactoring |
|---|---|---|---|---|
| Magic Numbers | `code/game.cpp:419` | Low | C4 | Extract `140` (speedMs), `4` (apples to speedup), and `8` (speedStep) to named constants. |
| Large Class | `code/game.cpp:404` | Medium | C3 | Extract Class: The `Game` class handles input, rendering orchestration, game state loop, and file I/O for scores. Split file I/O into a `ScoreManager`. |
| Long Method | `code/game.cpp:535` | Low | C3 | Extract Method: `Game::showGameOver()` mixes rendering the game over screen with a blocking input loop for restarting/quitting. |
| Inappropriate Intimacy | `code/game.cpp:511` | Medium | C4 | The `Game` class reaches deep into `Food` and `Snake` to handle food collision, manually incrementing apples and score. Delegate collision handling or growth rules. |
| Hardcoded UI Strings | `code/game.cpp:379` | Low | C2 | Output streams in `GameBoard::render` hardcode strings like "SNAKE GAME". Extract to configuration or constants. |
| Primitive Obsession | `code/game.cpp:410` | Low | C2 | `score`, `highScore`, `previousScore` are loosely managed ints. Group into a `ScoreState` struct. |
| Data Clump | `code/game.cpp:525` | Low | C1 | Passes multiple integers continuously. |
