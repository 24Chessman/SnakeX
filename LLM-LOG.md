# LLM Log

## Setup

| Item | Details |
| --- | --- |
| Model(s) | GPT-5 Codex |
| Tool / harness | Codex coding agent |
| IDE / editor | VS Code |
| Assignment context provided? | Yes. The Lab Assignment 3 requirements and relevant project files were provided in the coding session. |

## Prompt and Implementation Summary

| # | Instruction summary | Implementation / result |
| --- | --- | --- |
| 1 | Implement Lab Assignment 3 for the existing Snake game and identify what should be submitted. | Reviewed the project structure and `code/game.cpp`, then updated the single-player assumptions to support two-player gameplay. Player 1 uses arrow keys, Player 2 uses W/A/S/D, both snakes share food, scoring is tracked separately, and collisions with walls, self, and the other snake end the game with a loser message. |
| 2 | Run the project once to verify that the updated game starts successfully. | Compiled and launched the game for a basic runtime check. No source code changes were made during this step. |
| 3 | Improve playability and rendering by slowing the game slightly and fixing unstable border/food redraws. | Updated Windows terminal symbols to use fixed-width two-character cells so the board, food, and snakes remain aligned during redraws. Slowed the starting and restart speed from 140 ms to 180 ms per tick. |
| 4 | Address pull request feedback and clean up the project log. | Increased the starting distance between both snakes and changed their initial directions so they begin facing away from each other. Reworded this log into a professional summary of the actual instructions and implementation work. |

## Final Working Attempt

The final working version is Attempt 4. The earlier implementation added multiplayer and rendered correctly after the fixed-width symbol update, but PR feedback identified a playability issue where both snakes started too close together and faced each other. The final update places the snakes farther apart and starts them facing away from each other.

## Prompt Count

Total instruction prompts used to reach the final PR-ready version: 4.

## Code Context Provided

The LLM session used the assignment requirements, project file list, `README.md`, and the relevant contents of `code/game.cpp`. The implementation changed `code/game.cpp` and added this `LLM-LOG.md`.
