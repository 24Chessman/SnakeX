// snake_with_emojis.cpp
// Compile: g++ -std=c++11 -o snake_with_emojis snake_with_emojis.cpp

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <vector>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/select.h>

using namespace std;

// ANSI colors
#define RED    "\033[31m"
#define GREEN  "\033[32m"
#define YELLOW "\033[33m"
#define CYAN   "\033[36m"
#define RESET  "\033[0m"

class Terminal {
private:
    struct termios original;
public:
    Terminal() {
        tcgetattr(STDIN_FILENO, &original);
        struct termios raw = original;
        raw.c_lflag &= ~(ICANON | ECHO);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    }
    ~Terminal() {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
        showCursor();
    }
    void hideCursor() { cout << "\033[?25l" << flush; }
    void showCursor() { cout << "\033[?25h" << flush; }
    void clearScreen() { cout << "\033[2J\033[H" << flush; }
    void moveCursor(int x, int y) { cout << "\033[" << y << ";" << x << "H" << flush; }

    bool kbhit() {
        struct timeval tv = {0, 0};
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        return select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) == 1;
    }

    char getch() {
        char c = 0;
        ssize_t r = read(STDIN_FILENO, &c, 1);
        if (r <= 0) return 0;
        return c;
    }

    void getSize(int& width, int& height) {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        width = w.ws_col;
        height = w.ws_row;
    }

    void sleep(int ms) { usleep(ms * 1000); }
};

// Helper: detect multibyte UTF-8 (heuristic: any byte >= 0x80 means multibyte)
bool isUTF8MultiByte(const string &s) {
    for (unsigned char c : s) if (c >= 0x80) return true;
    return false;
}

struct Position {
    int x, y;
    Position(int _x = 0, int _y = 0) : x(_x), y(_y) {}
    bool operator==(const Position& o) const { return x == o.x && y == o.y; }
};

enum Direction { UP, DOWN, LEFT, RIGHT, NONE };

// Emoji / symbol choices
const string EMOJI_FOOD = "🍎";     // apple
const string EMOJI_SNAKE_HEAD = "🐍"; // snake head (same emoji fine)
const string EMOJI_SNAKE_BODY = "🟩"; // green square for body
const string BORDER_CELL = "##";    // border (ASCII double char)
const string EMPTY_CELL = "  ";     // two spaces to fill a 2-column cell

class Food {
private:
    Position pos;
    string symbol;
public:
    Food() : symbol(EMOJI_FOOD) {}
    Position getPosition() const { return pos; }
    string getSymbol() const { return symbol; }

    void spawn(int maxX, int maxY, const deque<Position>& snakeBody) {
        bool valid = false;
        while (!valid) {
            pos.x = rand() % (maxX - 2) + 1;
            pos.y = rand() % (maxY - 2) + 1;
            valid = true;
            for (const auto &s : snakeBody) {
                if (pos == s) { valid = false; break; }
            }
        }
    }
};

class Snake {
private:
    deque<Position> body;
    Direction current, next;
    bool growing;
    string headSym;
    string bodySym;
public:
    Snake(int startX, int startY)
        : current(RIGHT), next(RIGHT), growing(false),
          headSym(EMOJI_SNAKE_HEAD), bodySym(EMOJI_SNAKE_BODY) {
        // horizontal snake facing right
        body.push_back(Position(startX, startY));
        body.push_back(Position(startX - 1, startY));
        body.push_back(Position(startX - 2, startY));
    }
    const deque<Position>& getBody() const { return body; }
    Position getHead() const { return body.front(); }
    string getHeadSymbol() const { return headSym; }
    string getBodySymbol() const { return bodySym; }
    Direction getCurrentDirection() const { return current; }

    void setDirection(Direction d) {
        if ((d == UP && current == DOWN) ||
            (d == DOWN && current == UP) ||
            (d == LEFT && current == RIGHT) ||
            (d == RIGHT && current == LEFT)) {
            return;
        }
        next = d;
    }

    void move() {
        current = next;
        Position head = getHead();
        Position newHead = head;
        switch (current) {
            case UP: newHead.y--; break;
            case DOWN: newHead.y++; break;
            case LEFT: newHead.x--; break;
            case RIGHT: newHead.x++; break;
            case NONE: return;
        }
        body.push_front(newHead);
        if (!growing) body.pop_back();
        else growing = false;
    }

    void grow() { growing = true; }

    bool checkSelfCollision() const {
        Position h = getHead();
        for (size_t i = 1; i < body.size(); ++i) if (h == body[i]) return true;
        return false;
    }
};

class GameBoard {
private:
    int width;   // in "cells"
    int height;  // in "cells"
    vector<vector<string>> grid;
public:
    GameBoard(int w, int h) : width(w), height(h) {
        grid.assign(height, vector<string>(width, EMPTY_CELL));
        clear();
    }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    void clear() {
        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x)
                grid[y][x] = (y == 0 || y == height-1 || x == 0 || x == width-1) ? BORDER_CELL : EMPTY_CELL;
    }
    void place(int x, int y, const string &sym) {
        if (x >= 0 && x < width && y >= 0 && y < height) grid[y][x] = sym;
    }

    bool isInsideBoundaries(const Position& p) const {
        return p.x > 0 && p.x < width - 1 && p.y > 0 && p.y < height - 1;
    }

    // Render: we print each cell; if a cell is single-byte ASCII-only we append a space to make 2 cols.
    void render(Terminal& term, int score, int highScore, int prevScore) const {
        term.clearScreen();
        term.moveCursor(1, 1);
        cout << CYAN << "SNAKE GAME  " << RESET
             << " | Score: " << GREEN << score << RESET
             << " | Prev: " << YELLOW << prevScore << RESET
             << " | High: " << GREEN << highScore << RESET;
        term.moveCursor(1, 2);
        cout << "Controls: W/A/S/D or ARROW KEYS | Q = Quit" << flush;

        for (int y = 0; y < height; ++y) {
            term.moveCursor(1, y + 3);
            for (int x = 0; x < width; ++x) {
                const string &cell = grid[y][x];
                // choose color
                if (cell == EMOJI_FOOD) {
                    cout << RED << cell << RESET;
                } else if (cell == EMOJI_SNAKE_HEAD) {
                    cout << GREEN << cell << RESET;
                } else if (cell == EMOJI_SNAKE_BODY) {
                    cout << GREEN << cell << RESET;
                } else if (cell == BORDER_CELL) {
                    cout << YELLOW << cell << RESET;
                } else {
                    cout << cell;
                }
                // append extra space if this appears ASCII-only (so each cell occupies ~2 columns)
                if (!isUTF8MultiByte(cell)) cout << ' ';
            }
        }
        cout << flush;
    }
};

class Game {
private:
    Terminal term;
    GameBoard* board;
    Snake* snake;
    Food* food;
    int score;
    int highScore;
    int previousScore;
    bool gameOver;
    bool running;
    int speedMs; // ms per tick

public:
    Game(int boardSize)
        : score(0), highScore(0), previousScore(0), gameOver(false), running(true), speedMs(120) {
        term.hideCursor();
        board = new GameBoard(boardSize, boardSize);
        int sx = boardSize / 2;
        int sy = boardSize / 2;
        snake = new Snake(sx, sy);
        food = new Food();
        food->spawn(board->getWidth(), board->getHeight(), snake->getBody());
    }
    ~Game() {
        delete board;
        delete snake;
        delete food;
        term.showCursor();
    }

    void handleInput() {
        if (!term.kbhit()) return;
        char k = term.getch();
        if (k == 0) return;

        if (k == 27) { // ESC sequence for arrows
            // try to read two bytes
            if (term.kbhit()) {
                char b1 = term.getch();
                if (b1 == '[' && term.kbhit()) {
                    char b2 = term.getch();
                    if (b2 == 'A') snake->setDirection(UP);
                    else if (b2 == 'B') snake->setDirection(DOWN);
                    else if (b2 == 'C') snake->setDirection(RIGHT);
                    else if (b2 == 'D') snake->setDirection(LEFT);
                }
            }
        } else {
            k = toupper(k);
            if (k == 'W') snake->setDirection(UP);
            else if (k == 'S') snake->setDirection(DOWN);
            else if (k == 'A') snake->setDirection(LEFT);
            else if (k == 'D') snake->setDirection(RIGHT);
            else if (k == 'Q') running = false;
        }
    }

    void update() {
        snake->move();
        Position head = snake->getHead();

        if (!board->isInsideBoundaries(head)) { gameOver = true; return; }
        if (snake->checkSelfCollision()) { gameOver = true; return; }

        if (head == food->getPosition()) {
            snake->grow();
            score++;
            if (score > highScore) highScore = score;
            food->spawn(board->getWidth(), board->getHeight(), snake->getBody());
            if (speedMs > 40) speedMs -= 3;
        }
    }

    void render() {
        board->clear();
        // Place food and snake on board
        Position fpos = food->getPosition();
        board->place(fpos.x, fpos.y, food->getSymbol());

        const deque<Position>& body = snake->getBody();
        for (size_t i = 0; i < body.size(); ++i) {
            const Position &p = body[i];
            board->place(p.x, p.y, (i == 0) ? snake->getHeadSymbol() : snake->getBodySymbol());
        }

        board->render(term, score, highScore, previousScore);
    }

    void showGameOver() {
        previousScore = score;
        term.clearScreen();
        term.moveCursor(1, 5);
        cout << "\n\n";
        cout << "\t ================================\n";
        cout << "\t         GAME OVER!\n";
        cout << "\t ================================\n\n";
        cout << "\t   Final Score: " << score << endl;
        cout << "\t   High Score: " << highScore << endl;
        cout << "\t   Previous Score: " << previousScore << endl;
        cout << "\n\t ================================\n\n";
        cout << "\t Press R to Restart or Q to Quit\n\n";
        cout << flush;

        while (true) {
            if (term.kbhit()) {
                char k = term.getch();
                if (k == 0) continue;
                if (k == 27) { // drain arrows if any
                    if (term.kbhit()) term.getch();
                    if (term.kbhit()) term.getch();
                    continue;
                }
                k = toupper(k);
                if (k == 'R') { restart(); return; }
                else if (k == 'Q') { running = false; return; }
            }
            term.sleep(50);
        }
    }

    void restart() {
        delete snake;
        delete food;
        score = 0;
        gameOver = false;
        speedMs = 120;
        int sx = board->getWidth() / 2;
        int sy = board->getHeight() / 2;
        snake = new Snake(sx, sy);
        food = new Food();
        food->spawn(board->getWidth(), board->getHeight(), snake->getBody());
    }

    void run() {
        while (running) {
            if (!gameOver) {
                handleInput();
                update();
                render();
                term.sleep(speedMs);
            } else {
                showGameOver();
            }
        }
    }
};

int main() {
    srand((unsigned)time(0));
    Terminal term;

    int consoleW, consoleH;
    term.getSize(consoleW, consoleH);

    // Each game "cell" will occupy ~2 terminal columns (we use emojis + ASCII).
    // So calculate available columns in cells by dividing by 2.
    int availableColsInCells = (consoleW - 2) / 2;
    int availableRows = consoleH - 6; // header + padding

    int gameSize = min(availableColsInCells, availableRows);
    if (gameSize < 10) gameSize = 10; // minimum playable size

    term.clearScreen();
    term.hideCursor();
    cout << "===================================\n";
    cout << "       WELCOME TO SNAKE (EMOJI)\n";
    cout << "===================================\n\n";
    cout << "Instructions:\n";
    cout << "  - Use W/A/S/D or ARROW KEYS to control the snake\n";
    cout << "  - Eat " << EMOJI_FOOD << " to grow and score\n";
    cout << "  - Avoid walls and yourself\n";
    cout << "  - Press Q to quit anytime\n\n";
    cout << "Press any key to start...\n";
    cout << flush;
    // wait for any key
    while (!term.kbhit()) term.sleep(50);
    term.getch();

    Game game(gameSize);
    game.run();

    term.clearScreen();
    term.showCursor();
    cout << "\nThanks for playing — gg!\n";
    return 0;
}