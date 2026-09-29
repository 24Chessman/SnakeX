# Lab 4 — Group SnakeX

| | |
|---|---|
| Repository | https://github.com/24Chessman/SnakeX |
| Base tag | `lab4-base` at `5fc6d9df7e17d42d2478e28dfa41a42380602521` |
| Pull request | `unadkat7:lab4` → `24Chessman:main` |

---

## 1. Five rules — [5]

The original Part A record was made from the repository README before opening the implementation and is preserved in `lab4/PART-A.md`.

| # | Rule |
|---|---|
| 1 | A new game starts with a snake three cells long. |
| 2 | The snake continues moving in its current direction. |
| 3 | Eating food grows the snake by one cell and increases the score. |
| 4 | Hitting a wall or the snake's own body ends the game. |
| 5 | Food never spawns on a cell occupied by the snake. |

Later inspection clarified rule 3: the score changes on the eating tick and the scheduled extra body cell appears on the following move. The seam preserves that existing order.

---

## 2. What you could test, and what stopped you — [10]

No production source was changed for this part. Every citation below exists in `lab4-base`.

| # | Rule | Test written? | Blocking dependency (`file:line` + what it is) |
|---|---|---|---|
| 1 | Initial length is three | Yes: `initialSnakeHasThreeCells`. | `code/game.cpp:301`–`code/game.cpp:305` constructs three cells and `code/game.cpp:307` exposes the body. Full `Game` startup is excluded because `code/game.cpp:406` embeds a terminal and `code/game.cpp:422` reads the score file. |
| 2 | Movement continues | Yes: `movementContinuesAndRejectsReversal` checks two moves without new input and checks reversal rejection. | `code/game.cpp:319` exposes `move()` and `code/game.cpp:308` exposes the head. The interactive path polls input at `code/game.cpp:441` and sleeps at `code/game.cpp:596`. |
| 3 | Eating grows and scores | No complete test. Growth runs only while arranging a collision test. | The decision is trapped inside `Game::update` at `code/game.cpp:511`–`code/game.cpp:520`; `code/game.cpp:409` fixes the collaborator to concrete `Food*`, `code/game.cpp:286`–`code/game.cpp:287` calls `rand()`, and `code/game.cpp:408`/`code/game.cpp:410` hide the snake and score. There is no terminal-free tick boundary with supplied food and observable progress. |
| 4 | Collision ends the game | Partial only: boundary and self-collision predicates are checked, not the `gameOver` transition. | Predicates are available at `code/game.cpp:369` and `code/game.cpp:337`, but `code/game.cpp:507` assigns private state declared at `code/game.cpp:411`. The visible game-over loop polls input at `code/game.cpp:553` and sleeps at `code/game.cpp:565`. |
| 5 | Food avoids the snake | Yes: `foodSamplesAvoidOccupiedCells` checks 100 generated positions against each occupied cell. | `code/game.cpp:283` accepts the body and `code/game.cpp:280` returns the position. Direct RNG at `code/game.cpp:286`–`code/game.cpp:287` prevents scripting a particular retry, so this test samples the invariant rather than proving every RNG sequence. |

> **Rules testable without modifying the source: 3 / 5.**

There are five baseline test functions, but only rules 1, 2 and 5 have direct rule assertions. The collision tests exercise useful predicates without claiming that the private game-over transition was verified. `lab4/run-tests.ps1` obtains the exact tagged source using `git show lab4-base:code/game.cpp`; the tests rename `main` locally and do not edit that generated source.

---

## 3. Coverage, and what it missed — [6]

Coverage was measured with MinGW GCC/gcov 6.3.0 after clearing previous `.gcda` counters.

| | |
|---|---|
| Line coverage | **18.50%** of 346 production lines (64 executed) |
| Branch coverage | **16.67%** of 480 branches executed (80); **13.13%** taken at least once (63 outcomes) |
| Command used | `powershell -ExecutionPolicy Bypass -File lab4/run-tests.ps1 -Suite baseline` |
| Compiler / coverage commands | `g++ -std=c++17 --coverage -O0 -g -Wall -Wextra ...`; `./tests.exe`; `gcov -b -c tests.gcno` |

**One rule that is executed by the suite but not verified by it:**

| | |
|---|---|
| Rule | Rule 3, specifically the exact one-cell growth behavior. |
| Line that runs | `code/game.cpp:335` (`grow`) and `code/game.cpp:332` (consume the growth flag while retaining the tail) each execute three times in the collision arrangement. |
| The assertion that is missing | Save the body length before `grow(); move();` and assert `lengthAfter == lengthBefore + 1`; the full rule also needs food at the next head and `scoreAfter == scoreBefore + 1`. |

The baseline collision test uses growth only to construct an overlapping body, then asserts collision. Coverage therefore shows that growth ran, while the rule's exact outcome remains unchecked. The seam suite adds explicit length and score assertions. Raw results are in `lab4/evidence/`.

For context, the post-seam suite compiles the complete current source and reaches 19.94% of 351 lines, 18.78% of 490 branches, and 13.88% of branch outcomes. These figures use a changed denominator and are not presented as a coverage improvement claim.

---

## 4. The seam — [10]

| | |
|---|---|
| Rule made testable | Rule 3: eating increases score by exactly one and schedules exactly one extra body cell on the following move. |
| Commit 1 (seam) | `16bf4e5` — `refactor: expose game tick through a food collaborator seam` |
| Commit 2 (test) | `HEAD` (the second commit after `lab4-base`) — `test: verify eating behavior and document lab 4` |
| Seam kind | **Object seam** (`FoodSource&`). |
| Enabling point | `code/game.cpp:419`: callers supply a `FoodSource&` to `advanceGame`; production calls it with `Food` at `code/game.cpp:539`, while tests supply `ScriptedFood`. |
| What production code gave up | Exclusive ownership of tick orchestration and private progress representation. `advanceGame` and `GameProgress` are now callable production APIs, and `Food` pays a small virtual-dispatch/ABI cost. Callers must supply coherent state and a valid food collaborator. |

The extracted operation preserves the original order: move, reject collision, check food, schedule growth, increment score and apple count, adjust delay after four apples with a 30 ms floor, update high score, and respawn. Defaults remain score 0, apple count 0, game-over false and delay 140 ms. Restart resets the same fields and retains high score.

The normal game builds warning-free with `g++ -std=c++17 -Wall -Wextra code/game.cpp`. It was run in a Windows terminal through start, continuous movement, wall collision and quit, returning status 0. The 10 seam tests cover exact scoring, delayed one-cell growth, a non-eating tick, both high-score branches, apple threshold timing, the delay floor, and wall/self collision before scoring.

---

## 5. The double — [4]

| | |
|---|---|
| What you passed through the seam | **Stub**: `ScriptedFood`, `lab4/tests/seam.cpp:6`. |
| The method under test | `advanceGame`, the operation called by `Game::update`. |

The collaborator is asked “where is the food?” through `getPosition()`, and it returns positions supplied by the test; `spawn()` advances to the next canned answer instead of simulating randomness. Tests assert score, length and game-over state rather than recorded calls or an expected interaction protocol, so the double is a stub rather than a spy or mock.

---

## 6. Two smells in your own tests — [5]

| | Smell | `file:line` | One-line fix |
|---|---|---|---|
| 1 | Eager test | `lab4/tests/baseline.cpp:12` | Split continuous movement and reversal rejection into separate tests, each with one act. |
| 2 | Assertion roulette | `lab4/tests/baseline.cpp:33` | Add the sample number, generated position and occupied cell to the loop assertion's failure diagnostics. |

These findings follow `lab4/SMELLS.md`. The shared assertion helper reports test name, expression and line, but the loop failure still would not identify which of 100 samples and three occupied cells failed. Both are genuine limitations; neither test is an assertion-free placeholder.
