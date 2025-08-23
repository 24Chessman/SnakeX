# 🐍 Snake Game

The **Snake Game** is a classic arcade project built as part of **IT603: Introduction to Programming (Autumn 2025-26)**.  
In this game, the player controls a snake that grows longer each time it consumes food. The game ends when the snake collides with itself or the boundaries of the grid.

---

## 🎮 Features

- Grid-based game area (NxN cells).
- Snake starts with length of 3 cells.
- Food spawns randomly on the grid (never on the snake).
- Continuous snake movement with keyboard controls (`W/A/S/D` or arrow keys).
- Snake grows longer and score increases with each food consumed.
- Game Over conditions:
  - Collision with boundaries
  - Collision with itself
- Displays current score during gameplay and final score on game-over screen.
- Option to restart or exit after game over.

---

## 🧑‍💻 Technical Details

- **Programming Concepts**
  - **OOP**:  
    - `Snake`, `Food`, and `GameBoard` represented as classes.  
    - Encapsulation of game logic.  
    - (Optional) Inheritance & Polymorphism for extended functionality.  
  - **Data Structures**:  
    - Queue / Linked List → Snake body  
    - 2D Array → Game grid  
  - **Basics**: Loops, conditionals, functions, modular design.

---

## 🚀 How to Run

1. Clone this repository:
   ```bash
   git clone https://github.com/your-username/snake-game.git

2. Navigate into the project folder:
   ```bash
   cd snake-game

3. Run the program:
   ```bash
   python snake.py

