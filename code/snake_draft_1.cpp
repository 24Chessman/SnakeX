#include <iostream>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <ncurses.h>
#include <unistd.h>
using namespace std;

struct Position { int x, y; };

class Food {
public:
    Position pos;
    void spawn(int W, int H, const deque<Position>& snake) {
        while (true) {
            pos.x = rand() % W;
            pos.y = rand() % H;
            bool onSnake = false;
            for (auto s : snake)
                if (s.x == pos.x && s.y == pos.y) { onSnake = true; break; }
            if (!onSnake) break;
        }
    }
};

class Snake {
public:
    deque<Position> body;
    int dx = 1, dy = 0;
    Snake(int x, int y) {
        body.clear();
        body.push_front({x, y});
        body.push_back({x - 1, y});
        body.push_back({x - 2, y});
    }

    void setDir(int key) {
        switch (key) {
            case KEY_UP: case 'w': if (dy == 0) { dx = 0; dy = -1; } break;
            case KEY_DOWN: case 's': if (dy == 0) { dx = 0; dy = 1; } break;
            case KEY_LEFT: case 'a': if (dx == 0) { dx = -1; dy = 0; } break;
            case KEY_RIGHT: case 'd': if (dx == 0) { dx = 1; dy = 0; } break;
        }
    }

    Position move(bool grow = false) {
        Position head = body.front();
        head.x += dx; head.y += dy;
        body.push_front(head);
        if (!grow) body.pop_back();
        return head;
    }

    bool collided(int W, int H) {
        const Position head = body.front();
        if (head.x < 0 || head.x >= W || head.y < 0 || head.y >= H) return true;
        for (size_t i = 1; i < body.size(); ++i)
            if (body[i].x == head.x && body[i].y == head.y) return true;
        return false;
    }
};

class Game {
    int W, H;
    Snake snake;
    Food food;
    int score = 0;
    bool gameOver = false;
    bool waiting = true;

public:
    Game(int w, int h) : W(w), H(h), snake(w / 4, h / 2) {
        food.spawn(W, H, snake.body);
    }

    void run() {
        initscr();
        noecho();
        curs_set(0);
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);
        start_color(); use_default_colors();
        init_pair(1, COLOR_GREEN, -1);
        init_pair(2, COLOR_RED, -1);
        init_pair(3, COLOR_YELLOW, -1);
        init_pair(4, COLOR_CYAN, -1);
        init_pair(5, COLOR_WHITE, -1);

        timeout(100);

        while (true) {
            clear();
            draw();
            int key = getch();
            if (key == 'q' || key == 27) break;

            if (!gameOver) {
                if (waiting) {
                    if (key != ERR) { snake.setDir(key); waiting = false; }
                } else {
                    snake.setDir(key);
                    update();
                }
            } else {
                drawGameOver();
                nodelay(stdscr, FALSE);
                key = getch();
                if (key == 'r' || key == 'R') {
                    endwin();          // close ncurses window cleanly
                    restart();         // relaunch a fresh game
                    return;            // exit current loop entirely
                }
                else if (key == 'q' || key == 'Q' || key == 27) {
                    endwin();
                    return;
                }
                nodelay(stdscr, TRUE);
            }
        }

        endwin();
    }

private:
    void drawBorder() {
        attron(COLOR_PAIR(4) | A_BOLD);
        for (int x = 0; x < W + 2; ++x) { mvprintw(0, x, "#"); mvprintw(H + 1, x, "#"); }
        for (int y = 0; y < H + 2; ++y) { mvprintw(y, 0, "#"); mvprintw(y, W + 1, "#"); }
        attroff(COLOR_PAIR(4) | A_BOLD);
    }

    void draw() {
        drawBorder();

        attron(COLOR_PAIR(2) | A_BOLD);
        mvprintw(food.pos.y + 1, food.pos.x + 1, "*");
        attroff(COLOR_PAIR(2) | A_BOLD);

        bool head = true;
        for (auto s : snake.body) {
            attron(COLOR_PAIR(head ? 5 : 1) | A_BOLD);
            mvprintw(s.y + 1, s.x + 1, head ? "@" : "o");
            attroff(COLOR_PAIR(head ? 5 : 1) | A_BOLD);
            head = false;
        }

        attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(H + 3, 2, "Score: %d", score);
        if (waiting) mvprintw(H + 4, 2, "Press a direction key to start");
        attroff(COLOR_PAIR(3) | A_BOLD);
        refresh();
    }

    void update() {
        Position head = snake.move();
        if (head.x == food.pos.x && head.y == food.pos.y) {
            snake.move(true);
            score++;
            food.spawn(W, H, snake.body);
        }
        if (snake.collided(W, H)) gameOver = true;
    }

    void drawGameOver() {
        clear();
        drawBorder();
        int cx = W / 2 - 8, cy = H / 2;
        attron(COLOR_PAIR(2) | A_BOLD);
        mvprintw(cy, cx, "=== GAME OVER ===");
        attroff(COLOR_PAIR(2) | A_BOLD);
        attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(cy + 1, cx - 2, "Final Score: %d", score);
        mvprintw(cy + 3, cx - 10, "Press [R] to Restart or [Q] to Quit");
        attroff(COLOR_PAIR(3) | A_BOLD);
        refresh();
    }

    void restart() {
        // Relaunch a fresh new Game instance cleanly
        system("clear");
        Game newGame(W, H);
        newGame.run();
    }
};

int main() {
    srand((unsigned)time(nullptr));
    Game g(40, 20);
    g.run();
    return 0;
}
