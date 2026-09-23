// Part D test: Rule 3 — "Eating food increases the snake's length by exactly one cell."
//
// This test exercises the BEHAVIORAL rule through Game::update().
// It does NOT call snake->grow() directly.
//
// Seam used: object seam introduced in Commit 1.
// Game(boardSize, customFood) accepts a caller-owned Food*.
// The StubFood is stack-allocated; Game must NOT delete it (ownsFood == false).

#define main snake_game_main
#include "../../code/game.cpp"
#undef main

#include <cassert>
#include <iostream>

using namespace std;

// ---------------------------------------------------------------------------
// StubFood: a Food test double with a predetermined position.
//
// getPosition() returns the fixed position set in the constructor.
// spawn() is overridden to do nothing so that after consumption the food
// stays at a safe position (0,0 is off the inner board) and is never
// consumed a second time during the test.
// ---------------------------------------------------------------------------
class StubFood : public Food {
    Position fixedPos;
public:
    explicit StubFood(int x, int y) : fixedPos(x, y) {}

    Position getPosition() const override { return fixedPos; }

    void spawn(int /*maxX*/, int /*maxY*/,
               const deque<Position>& /*snakeBody*/) override {
        // After consumption Game calls spawn() to reposition food.
        // Move it off the playfield so it is never accidentally re-eaten.
        fixedPos = Position(0, 0);
    }
};

// ---------------------------------------------------------------------------
// testFoodConsumptionGrowth
//
// Verifies that when the snake's head reaches the food position inside
// Game::update(), the snake's body grows by exactly one cell.
//
// Board: 20x20
// Snake start: sx = sy = 20/2 = 10, head at (10,10), facing RIGHT.
// After the first update() the snake moves to (11,10).
// StubFood is placed at (11,10) so the head lands on it immediately.
// Game::update() detects the collision, calls snake->grow() and score++.
// After the second update() the snake advances to (12,10) and the pending
// growth is incorporated, so body.size() == initialLength + 1.
// ---------------------------------------------------------------------------
void testFoodConsumptionGrowth() {
    // Board 20x20 → snake starts at (10,10), facing RIGHT.
    // First move: head will step to (11,10).
    StubFood stubFood(11, 10);  // caller owns this

    Game game(20, &stubFood);   // ownsFood == false; Game will NOT delete stubFood

    size_t initialLength = game.getSnake()->getBody().size();

    // update() 1: snake moves to (11,10), matches food, grow() is called.
    game.update();

    // update() 2: snake moves to (12,10); growing flag is cleared and the tail
    // is kept, so the body has grown by one.
    game.update();

    size_t finalLength = game.getSnake()->getBody().size();

    assert(finalLength == initialLength + 1);

    cout << "Rule 3 test passed: food consumption grows the snake by exactly one cell.\n";
}

int main() {
    testFoodConsumptionGrowth();
    cout << "Part D test passed.\n";
    return 0;
}
