// Simple Snake Game - Cross-Platform Terminal Version
// C++

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <vector>
#include <string>
#include <fstream>

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    // POSIX
    #include <termios.h>
    #include <unistd.h>
    #include <sys/ioctl.h>
    #include <sys/select.h>
#endif

using namespace std;

// ANSI colors
#define RED    "\033[31m"
#define GREEN  "\033[32m"
#define YELLOW "\033[33m"
#define CYAN   "\033[36m"
#define RESET  "\033[0m"

// Cross-platform Terminal abstraction
class Terminal {
private:
#ifdef _WIN32
    HANDLE hStdin;
    HANDLE hStdout;
    CONSOLE_CURSOR_INFO originalCursorInfo;
    bool cursorInfoSaved = false;
    // buffer to emit an ESC-style arrow sequence on Windows so game logic doesn't change
    deque<char> pendingChars;
#else
    struct termios original;
#endif

public:
    Terminal() {
#ifdef _WIN32
        hStdin = GetStdHandle(STD_INPUT_HANDLE);
        hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

        // Save original cursor info
        CONSOLE_CURSOR_INFO cinfo;
        if (GetConsoleCursorInfo(hStdout, &cinfo)) {
            originalCursorInfo = cinfo;
            cursorInfoSaved = true;
        }

        // Enable ENABLE_VIRTUAL_TERMINAL_PROCESSING so ANSI escapes work on modern Windows terminals
        DWORD mode = 0;
        if (GetConsoleMode(hStdout, &mode)) {
            SetConsoleMode(hStdout, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
#else
        // Save terminal state and set raw (no echo, non-canonical)
        tcgetattr(STDIN_FILENO, &original);
        struct termios raw = original;
        raw.c_lflag &= ~(ICANON | ECHO);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
#endif
    }

    ~Terminal() {
#ifdef _WIN32
        // Restore cursor visibility
        if (cursorInfoSaved) {
            SetConsoleCursorInfo(hStdout, &originalCursorInfo);
        }
#else
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
        showCursor();
#endif
    }

    void hideCursor() {
#ifdef _WIN32
        CONSOLE_CURSOR_INFO cci;
        if (GetConsoleCursorInfo(hStdout, &cci)) {
            cci.bVisible = FALSE;
            SetConsoleCursorInfo(hStdout, &cci);
        }
#else
        cout << "\033[?25l" << flush;
#endif
    }

    void showCursor() {
#ifdef _WIN32
        CONSOLE_CURSOR_INFO cci;
        if (GetConsoleCursorInfo(hStdout, &cci)) {
            cci.bVisible = TRUE;
            SetConsoleCursorInfo(hStdout, &cci);
        }
#else
        cout << "\033[?25h" << flush;
#endif
    }

    void clearScreen() {
#ifdef _WIN32
        // Use WinAPI to clear screen for smoother behavior
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        DWORD cellCount;
        DWORD count;
        COORD homeCoords = {0, 0};

        if (!GetConsoleScreenBufferInfo(hStdout, &csbi)) {
            // fallback to system("cls")
            system("cls");
            return;
        }

        cellCount = csbi.dwSize.X * csbi.dwSize.Y;
        FillConsoleOutputCharacter(hStdout, ' ', cellCount, homeCoords, &count);
        FillConsoleOutputAttribute(hStdout, csbi.wAttributes, cellCount, homeCoords, &count);
        SetConsoleCursorPosition(hStdout, homeCoords);
#else
        cout << "\033[2J\033[H" << flush;
#endif
    }

    void moveCursor(int x, int y) {
#ifdef _WIN32
        // x = column, y = row (1-indexed expected by your code; we pass same numbers)
        COORD pos;
        // Convert to 0-based coordinates for WinAPI
        pos.X = (SHORT)(max(0, x - 1));
        pos.Y = (SHORT)(max(0, y - 1));
        SetConsoleCursorPosition(hStdout, pos);
#else
        cout << "\033[" << y << ";" << x << "H" << flush;
#endif
    }

    bool kbhit() {
#ifdef _WIN32
        if (!pendingChars.empty()) return true;
        return _kbhit() != 0;
#else
        struct timeval tv = {0, 0};
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        return select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) == 1;
#endif
    }

    char getch() {
#ifdef _WIN32
        // If we preloaded pseudo-ESC arrow bytes, return them first
        if (!pendingChars.empty()) {
            char c = pendingChars.front();
            pendingChars.pop_front();
            return c;
        }

        if (!_kbhit()) return 0;
        int c = _getch();
        if (c == 0 || c == 224) {
            // special key / arrow
            int code = _getch();
            // Map Win arrow codes to ESC [ A/B/C/D sequence to match your existing logic
            if (code == 72) { // up
                pendingChars.push_back(27); pendingChars.push_back('['); pendingChars.push_back('A');
            } else if (code == 80) { // down
                pendingChars.push_back(27); pendingChars.push_back('['); pendingChars.push_back('B');
            } else if (code == 77) { // right
                pendingChars.push_back(27); pendingChars.push_back('['); pendingChars.push_back('C');
            } else if (code == 75) { // left
                pendingChars.push_back(27); pendingChars.push_back('['); pendingChars.push_back('D');
            } else {
                // other function keys -> ignore or emit nothing
            }
            // now return first of the queued bytes
            char ch = pendingChars.front();
            pendingChars.pop_front();
            return ch;
        } else {
            // normal ASCII char
            return (char)c;
        }
#else
        char c = 0;
        ssize_t r = read(STDIN_FILENO, &c, 1);
        if (r <= 0) return 0;
        return c;
#endif
    }

    void getSize(int& width, int& height) {
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(hStdout, &csbi)) {
            int cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
            width = cols;
            height = rows;
            return;
        }
        // fallback
        width = 80;
        height = 25;
#else
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        width = w.ws_col;
        height = w.ws_row;
#endif
    }

    void sleep(int ms) {
#ifdef _WIN32
        ::Sleep(ms);
#else
        usleep(ms * 1000);
#endif
    }
};

// Simple persistent score storage
struct ScoreData { int previousScore; int highScore; };

ScoreData loadScores(const string &filename = "scores.txt") {
    ScoreData s = {0, 0};
    ifstream in(filename);
    if (in.is_open()) {
        in >> s.previousScore >> s.highScore;
        in.close();
    }
    return s;
}

void saveScores(const ScoreData &s, const string &filename = "scores.txt") {
    ofstream out(filename, ios::trunc);
    if (out.is_open()) {
        out << s.previousScore << " " << s.highScore << "\n";
        out.close();
    }
}

struct Position {
    int x, y;
    Position(int _x = 0, int _y = 0) : x(_x), y(_y) {}
    bool operator==(const Position& o) const { return x == o.x && y == o.y; }
};

enum Direction { UP, DOWN, LEFT, RIGHT, NONE };

// Emojis and cell decorations
const string EMOJI_FOOD = "🍎";
const string EMOJI_SNAKE_HEAD = "🐍";
const string EMOJI_SNAKE_BODY = "🟩";
const string BORDER_CELL = "██"; // uses two-block characters
const string EMPTY_CELL = "  ";  // two spaces to occupy 2 columns

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
public:
    Snake(int startX, int startY)
        : current(RIGHT), next(RIGHT), growing(false) {
        // horizontal initial snake facing right
        body.push_back(Position(startX, startY));
        body.push_back(Position(startX - 1, startY));
        body.push_back(Position(startX - 2, startY));
    }
    const deque<Position>& getBody() const { return body; }
    Position getHead() const { return body.front(); }
    string getHeadSymbol() const { return EMOJI_SNAKE_HEAD; }
    string getBodySymbol() const { return EMOJI_SNAKE_BODY; }
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
    int width;   // cells
    int height;  // cells
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

    // Render — anchored per cell to avoid drift
    void render(Terminal& term, int score, int highScore, int prevScore) const {
        term.clearScreen();
        term.moveCursor(1, 1);
        cout << CYAN << "SNAKE GAME  " << RESET
             << " | Score: " << GREEN << score << RESET
             << " | Prev: " << YELLOW << prevScore << RESET
             << " | High: " << GREEN << highScore << RESET;
        term.moveCursor(1, 2);
        cout << "Controls: W/A/S/D or ARROW KEYS | Q = Quit" << flush;

        // every cell is two terminal columns wide; place at col = 1 + x*2
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int col = 1 + x * 2;
                int row = y + 3;
                term.moveCursor(col, row);
                const string &cell = grid[y][x];
                if (cell == EMOJI_FOOD) cout << RED << cell << RESET;
                else if (cell == EMOJI_SNAKE_HEAD) cout << GREEN << cell << RESET;
                else if (cell == EMOJI_SNAKE_BODY) cout << GREEN << cell << RESET;
                else if (cell == BORDER_CELL) cout << YELLOW << cell << RESET;
                else cout << cell;
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
    int speedMs; // ms tick
    const int speedStep = 8;   // reduce ms by this per apple
    const int minSpeedMs = 30; // fastest (smallest ms)

    // Track apples eaten for speed increase
    int appleCount;

public:
    Game(int boardSize)
        : score(0), gameOver(false), running(true), speedMs(140), previousScore(0), highScore(0), appleCount(0) {
        term.hideCursor();

        // load scores from file
        ScoreData loaded = loadScores();
        previousScore = loaded.previousScore;
        highScore = loaded.highScore;

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

        if (k == 27) { // ESC starting arrow sequence
            // attempt to read next two bytes (non-blocking)
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

            // Increase apple counter
            appleCount++;

            // Increase speed after every 4 apples
            if (appleCount >= 4) {
                speedMs -= speedStep;
                if (speedMs < minSpeedMs) speedMs = minSpeedMs;
                appleCount = 0; // Reset apple count after speed increase
            }

            if (score > highScore) highScore = score;
            food->spawn(board->getWidth(), board->getHeight(), snake->getBody());
        }
    }

    void render() {
        board->clear();
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
        if (score > highScore) highScore = score;

        // persist scores
        ScoreData saveData = { previousScore, highScore };
        saveScores(saveData);

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
                if (k == 27) { // drain possible arrow bytes
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
        speedMs = 140; // reset speed for new run
        appleCount = 0; // reset apple count
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

    // Get terminal size dynamically and calculate the largest square grid
    int consoleW, consoleH;
    term.getSize(consoleW, consoleH);

    // Dynamic grid size based on terminal
    int gameSize = min((consoleW - 2) / 2, consoleH - 6); // leave some space for UI
    if (gameSize < 10) gameSize = 10; // ensure at least 10x10 grid

    term.clearScreen();
    term.hideCursor();
    cout << "===================================\n";
    cout << "       WELCOME TO SNAKE GAME\n";
    cout << "===================================\n\n";
    cout << "Saved High Score: " << loadScores().highScore << "\n\n";
    cout << "Instructions:\n";
    cout << "  - Use W/A/S/D or ARROW KEYS to control the snake\n";
    cout << "  - Eat " << EMOJI_FOOD << " to grow and score\n";
    cout << "  - Speed increases after every 4 apples eaten\n";
    cout << "  - Avoid walls and yourself\n";
    cout << "  - Press Q to quit anytime\n\n";
    cout << "Press any key to start...\n";
    cout << flush;

    while (!term.kbhit()) term.sleep(50);
    term.getch();

    Game game(gameSize);
    game.run();

    term.clearScreen();
    term.showCursor();
    cout << "\nThank you for playing! Your scores are saved to scores.txt\n";
    return 0;
}
