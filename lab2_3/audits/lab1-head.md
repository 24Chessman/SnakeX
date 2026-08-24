# Code Smell Audit (lab1-head)

| Smell | File:Line | Severity | Confidence | Suggested Refactoring |
|---|---|---|---|---|
| Magic Numbers | `code/game.cpp:419` | Low | C4 | Extract `140` (speedMs), `4` (apples to speedup), and `8` (speedStep) to named constants. |
| Large Class | `code/game.cpp:404` | Medium | C3 | Extract Class: The `Game` class handles input, rendering orchestration, game state loop, and file I/O for scores. |
| Long Method | `code/game.cpp:535` | Low | C3 | Extract Method: `Game::showGameOver()` mixes rendering the game over screen with a blocking input loop. |
| Duplicated Code | `code/game.cpp:515` | High | C4 | The movement, boundary checking, and food collision blocks in `update()` are copy-pasted for `snake` and `snake2`. Extract to a method or loop over a player array. |
| Divergent Change | `code/game.cpp:404` | Medium | C3 | Adding a new player requires changes to `Game` fields, constructor, input handler, updater, and renderer. |
