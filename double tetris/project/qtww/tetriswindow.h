#ifndef TETRISWINDOW_H
#define TETRISWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QKeyEvent>
#include <QLabel>  // 添加 QLabel 头文件
#include <QWidget> // 添加 QWidget 头文件

// 方块定义
const int BOARD_WIDTH = 10;
const int BOARD_HEIGHT = 20;
const int BLOCK_SIZE = 36;

// 方块形状定义
const int shapes[7][4][4][4] = {
    // I
    {{{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}},
     {{0,0,1,0}, {0,0,1,0}, {0,0,1,0}, {0,0,1,0}},
     {{0,0,0,0}, {0,0,0,0}, {1,1,1,1}, {0,0,0,0}},
     {{0,1,0,0}, {0,1,0,0}, {0,1,0,0}, {0,1,0,0}}},

    // J
    {{{1,0,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}},
     {{0,1,1,0}, {0,1,0,0}, {0,1,0,0}, {0,0,0,0}},
     {{0,0,0,0}, {1,1,1,0}, {0,0,1,0}, {0,0,0,0}},
     {{0,1,0,0}, {0,1,0,0}, {1,1,0,0}, {0,0,0,0}}},

    // L
    {{{0,0,1,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}},
     {{0,1,0,0}, {0,1,0,0}, {0,1,1,0}, {0,0,0,0}},
     {{0,0,0,0}, {1,1,1,0}, {1,0,0,0}, {0,0,0,0}},
     {{1,1,0,0}, {0,1,0,0}, {0,1,0,0}, {0,0,0,0}}},

    // O
    {{{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}},
     {{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}},
     {{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}},
     {{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}}},

    // S
    {{{0,1,1,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0}},
     {{0,1,0,0}, {0,1,1,0}, {0,0,1,0}, {0,0,0,0}},
     {{0,0,0,0}, {0,1,1,0}, {1,1,0,0}, {0,0,0,0}},
     {{1,0,0,0}, {1,1,0,0}, {0,1,0,0}, {0,0,0,0}}},

    // T
    {{{0,1,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}},
     {{0,1,0,0}, {0,1,1,0}, {0,1,0,0}, {0,0,0,0}},
     {{0,0,0,0}, {1,1,1,0}, {0,1,0,0}, {0,0,0,0}},
     {{0,1,0,0}, {1,1,0,0}, {0,1,0,0}, {0,0,0,0}}},

    // Z
    {{{1,1,0,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}},
     {{0,0,1,0}, {0,1,1,0}, {0,1,0,0}, {0,0,0,0}},
     {{0,0,0,0}, {1,1,0,0}, {0,1,1,0}, {0,0,0,0}},
     {{0,1,0,0}, {1,1,0,0}, {1,0,0,0}, {0,0,0,0}}}
};

// 方块颜色定义
const QColor colors[8] = {
    Qt::black,
    Qt::cyan,    // I
    Qt::blue,    // J
    QColor(255, 165, 0), // Orange for L
    Qt::yellow,  // O
    Qt::green,   // S
    Qt::magenta, // T
    Qt::red      // Z
};

// 前置声明
class GameAreaWidget;
class NextPieceArea;

// 自定义游戏区域控件
class GameAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameAreaWidget(QWidget *parent = nullptr);
    void setGameState(int b[BOARD_HEIGHT][BOARD_WIDTH],
                      int curPiece, int curRotation,
                      int curX, int curY, bool gameOver);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int board[BOARD_HEIGHT][BOARD_WIDTH];
    int curPiece;
    int curRotation;
    int curX;
    int curY;
    bool gameOver;
};

// 自定义预览区域控件
class NextPieceArea : public QWidget
{
    Q_OBJECT
public:
    explicit NextPieceArea(QWidget *parent = nullptr);
    void setNextPiece(int piece);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int nextPiece;
};

class TetrisWindow : public QMainWindow
{
    Q_OBJECT

public:
    TetrisWindow(QWidget *parent = nullptr);
    ~TetrisWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    bool event(QEvent *event) override; // 添加事件处理

private slots:
    void updateGame();

private:
    void initGame();
    void newPiece(int player);
    bool checkCollision(int player, int newX, int newY, int newRotation);
    void pieceDropped(int player);
    void removeFullLines(int player);
    void updateUI();
    void updateGameArea(int player);

    // 玩家1状态
    GameAreaWidget *gameArea1;
    GameAreaWidget *gameArea2;
    NextPieceArea *nextPieceArea1;
    NextPieceArea *nextPieceArea2;

    int board1[BOARD_HEIGHT][BOARD_WIDTH];
    int curPiece1;
    int nextPiece1;
    int curX1;
    int curY1;
    int rotation1;
    int score1;
    int level1;
    int linesCleared1;
    bool gameOver1;

    //玩家2状态
    int board2[BOARD_HEIGHT][BOARD_WIDTH];
    int curPiece2;
    int nextPiece2;
    int curX2;
    int curY2;
    int rotation2;
    int score2;
    int level2;
    int linesCleared2;
    bool gameOver2;

    // 共享状态
    QLabel *scoreLabel1;
    QLabel *scoreLabel2;
    QLabel *levelLabel1;
    QLabel *linesLabel1;
    QLabel *levelLabel2;
    QLabel *linesLabel2;

    QLabel *gameStatusLabel;

    QTimer *timer;

    bool isPaused;
};

#endif // TETRISWINDOW_H
