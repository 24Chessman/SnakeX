# Ubiquitous Language: SnakeX

| Term | Definition | Aliases to Avoid |
|---|---|---|
| Snake | The player character entity that moves around the board, consisting of a head and body segments. | worm, player |
| Food | The consumable item that the snake eats to increase its length and score. | apple, fruit, pill |
| GameBoard | The bounded grid area where the gameplay occurs and entities are rendered. | grid, arena, map |
| Position | A specific (x, y) coordinate pair on the GameBoard. | location, coordinates |
| Score | The numerical value representing the player's progress, increased by eating Food. | points, scoreCount |
| Collision | When the snake's head intersects with the GameBoard boundary or its own body. | crash, hit |
| Tick / Speed | The delay between game frames, controlling how fast the snake moves. | frameRate, sleepDelay |

## Flagged Ambiguities
- "apple" vs "food": The code class is `Food`, but the variable tracking quantity is `appleCount`. We should standardize on `Food`.
- "speedMs" vs "tick": The frame delay is called `speedMs` in code, but conceptually acts as a "tick" rate.
