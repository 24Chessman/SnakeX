# 🐍 SnakeX — IT603 Final Project (C++)

The **SnakeX Game** is a C++-based arcade project created as part of **IT603: Introduction to Programming (Autumn 2025-26)**.  
In this game, the player controls a snake that grows longer after eating food. The game ends when the snake collides with itself or the game boundary.

---

## 🎮 Game Features

### 🧱 Game Setup
- Grid-based play area (e.g., NxN cells).
- Snake starts with a length of 3 cells.
- Food spawns randomly on the grid (never on the snake’s body).

### 🎮 Gameplay Mechanics
- The snake moves continuously in the current direction.
- Player controls direction using `W / A / S / D` or arrow keys.
- Eating food:
  - Increases snake’s length by one cell.
  - Increases the player’s score.
- Real-time score display during gameplay.
- **Game Over** occurs when:
  - Snake collides with walls.
  - Snake collides with itself.
- Displays **final score** and **highest score** on the game-over screen.
- Option to **restart** or **exit** after game over.

---

## ⚙️ Technical Details

### 🧑‍💻 Programming Concepts Used
- **Object-Oriented Programming (OOP):**
  - Classes: `Game`,`Snake`, `Food`, `GameBoard`
  - **Encapsulation** for managing internal game logic.

- **Data Structures:**
  - Queue → to represent the snake’s body.
  - 2D Array → to represent the game grid.

- **Basic Concepts:**
  - Loops, conditionals, and modular functions for clean design.

---

## 🧩 Non-Functional Requirements

- **Ease of Use:**  
  - Simple and beginner-friendly console interface.  
  - Clear instructions before the game starts.  

- **Performance:**  
  - Smooth gameplay with no noticeable lag.  

- **Extensibility:**  
  - Modular design allows future enhancements like:
    - Obstacles
    - Multiple levels
    - Special food items

---

## 🧾 Acceptance Criteria

✅ Game starts and runs without crashes or runtime errors.  
✅ Snake moves correctly according to user input.  
✅ Food never spawns inside the snake.  
✅ Score updates accurately after each food consumption.  
✅ Proper detection of wall/self-collisions.  
✅ Final score and highest score displayed on Game Over screen.  
✅ Option provided to restart or exit.

---

## 🧠 Evaluation Criteria (As per IT603 Guidelines)

| Category | Description | Weight |
|-----------|--------------|--------|
| **Project Completion** | Game meets all acceptance criteria | 30% |
| **Viva (Make it Efficient)** | Explain logic, data structures, and tradeoffs | 30% |
| **UI/UX (Make it Elegant)** | Smooth user experience, readable interface | 20% |
| **Extensibility** | Code supports new features easily | 15% |
| **Continuous Development** | Regular commits & progress | 5% |

---

## 🚀 How to Run

### 1️⃣ Clone the Repository
```bash
git clone https://github.com/24Chessman/SnakeX.git
```
### 2️⃣ Navigate to the Folder
```
cd SnakeX
```
### 3️⃣ Compile the Program
- If you’re on Windows (using MinGW):
```
g++ snake.cpp -o snake.exe
```
- If you’re on Linux/macOS:
```
g++ snake.cpp -o snake.out
```
### 4️⃣ Run the Game
```
./snake.exe   # Windows
./snake.out   # Linux/macOS
```

---

## 💡 Future Enhancements

- Multiple difficulty levels
- Obstacles on the board
- Sound effects and color graphics using `ncurses` or `graphics.h`
- Save and load high scores to file

---

## 🧑‍🏫 Credits

Developed by **Naitik Sutariya (24Chessman), Harshal Prajapati (HarshalPrajapati), Gaurang Rahani (GaurangRahani), Jay Trivedi (JayTrivedi18)**  
Under **IT603: Introduction to Programming (Autumn 2025-26)**  
Guided by **Prof. Ankush Chander & TAs, Dhirubhai Ambani University**
