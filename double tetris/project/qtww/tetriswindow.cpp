#include "tetriswindow.h"
#include <QPainter>
#include <QDebug>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTime>
#include <QRandomGenerator>

//----------------- 一、自定义游戏区域控件 --------------------

//（1）初始化游戏板
GameAreaWidget::GameAreaWidget(QWidget *parent) : QWidget(parent)
{
    setFixedSize(BOARD_WIDTH * BLOCK_SIZE + 10, BOARD_HEIGHT * BLOCK_SIZE + 10);
    setStyleSheet("background-color: black; border: 4px solid #555;");

    // 初始化游戏板
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            board[i][j] = 0;
        }
    }
}

//（2）复制游戏板
void GameAreaWidget::setGameState(int b[BOARD_HEIGHT][BOARD_WIDTH],
                                  int curPiece, int curRotation,
                                  int curX, int curY, bool gameOver)
{
    // 复制游戏状态
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            board[i][j] = b[i][j];
        }
    }

    this->curPiece = curPiece;
    this->curRotation = curRotation;
    this->curX = curX;
    this->curY = curY;
    this->gameOver = gameOver;

    update(); // 触发重绘
}

//绘制游戏板与方块
void GameAreaWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制背景
    painter.fillRect(rect(), Qt::black);

    // 绘制网格
    painter.setPen(QColor(50, 50, 50));
    for (int i = 0; i <= BOARD_HEIGHT; i++) {
        painter.drawLine(0, i * BLOCK_SIZE, BOARD_WIDTH * BLOCK_SIZE, i * BLOCK_SIZE);
    }
    for (int j = 0; j <= BOARD_WIDTH; j++) {
        painter.drawLine(j * BLOCK_SIZE, 0, j * BLOCK_SIZE, BOARD_HEIGHT * BLOCK_SIZE);
    }

    // 绘制已固定的方块
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (board[i][j] != 0) {
                int type = board[i][j] - 1;
                painter.fillRect(j * BLOCK_SIZE + 1, i * BLOCK_SIZE + 1,
                                 BLOCK_SIZE - 2, BLOCK_SIZE - 2, colors[type+1]);

                // 高光效果
                painter.setPen(QPen(colors[type+1].lighter(150), 1));
                painter.drawLine(j * BLOCK_SIZE + 1, i * BLOCK_SIZE + 1,
                                 (j+1) * BLOCK_SIZE - 2, i * BLOCK_SIZE + 1);
                painter.drawLine(j * BLOCK_SIZE + 1, i * BLOCK_SIZE + 1,
                                 j * BLOCK_SIZE + 1, (i+1) * BLOCK_SIZE - 2);

                // 阴影效果
                painter.setPen(QPen(colors[type+1].darker(150), 1));
                painter.drawLine((j+1) * BLOCK_SIZE - 1, i * BLOCK_SIZE + 1,
                                 (j+1) * BLOCK_SIZE - 1, (i+1) * BLOCK_SIZE - 1);
                painter.drawLine(j * BLOCK_SIZE + 1, (i+1) * BLOCK_SIZE - 1,
                                 (j+1) * BLOCK_SIZE - 1, (i+1) * BLOCK_SIZE - 1);
            }
        }
    }

    // 绘制当前下落的方块
    if (!gameOver) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (shapes[curPiece][curRotation][i][j] != 0) {
                    int boardX = curX + j;
                    int boardY = curY + i;

                    // 只绘制在游戏区域内的部分
                    if (boardY >= 0) {
                        painter.fillRect(boardX * BLOCK_SIZE + 1, boardY * BLOCK_SIZE + 1,
                                         BLOCK_SIZE - 2, BLOCK_SIZE - 2, colors[curPiece+1]);

                        // 高光效果
                        painter.setPen(QPen(colors[curPiece+1].lighter(150), 1));
                        painter.drawLine(boardX * BLOCK_SIZE + 1, boardY * BLOCK_SIZE + 1,
                                         (boardX+1) * BLOCK_SIZE - 2, boardY * BLOCK_SIZE + 1);
                        painter.drawLine(boardX * BLOCK_SIZE + 1, boardY * BLOCK_SIZE + 1,
                                         boardX * BLOCK_SIZE + 1, (boardY+1) * BLOCK_SIZE - 2);

                        // 阴影效果
                        painter.setPen(QPen(colors[curPiece+1].darker(150), 1));
                        painter.drawLine((boardX+1) * BLOCK_SIZE - 1, boardY * BLOCK_SIZE + 1,
                                         (boardX+1) * BLOCK_SIZE - 1, (boardY+1) * BLOCK_SIZE - 1);
                        painter.drawLine(boardX * BLOCK_SIZE + 1, (boardY+1) * BLOCK_SIZE - 1,
                                         (boardX+1) * BLOCK_SIZE - 1, (boardY+1) * BLOCK_SIZE - 1);
                    }
                }
            }
        }
    }
}

//------------------ 二、自定义预览区域控件 --------------------

//（1）初始化预览面板
NextPieceArea::NextPieceArea(QWidget *parent) : QWidget(parent)
{
    setFixedSize(6 * BLOCK_SIZE, 6 * BLOCK_SIZE);
    setStyleSheet("background-color: black; border: 2px solid #555;");
}

//（2）重绘预览面板
void NextPieceArea::setNextPiece(int piece)
{
    nextPiece = piece;
    update(); // 触发重绘
}

//（3）绘制预告方块
void NextPieceArea::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    // 绘制下一个方块预览
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (shapes[nextPiece][0][i][j] != 0) {
                int x = j + 1; // 居中显示
                int y = i + 1;

                painter.fillRect(x * BLOCK_SIZE + 1, y * BLOCK_SIZE + 1,
                                 BLOCK_SIZE - 2, BLOCK_SIZE - 2, colors[nextPiece+1]);

                // 添加方块内部高光效果
                painter.setPen(QPen(colors[nextPiece+1].lighter(150), 1));
                painter.drawLine(x * BLOCK_SIZE + 1, y * BLOCK_SIZE + 1,
                                 (x+1) * BLOCK_SIZE - 2, y * BLOCK_SIZE + 1);
                painter.drawLine(x * BLOCK_SIZE + 1, y * BLOCK_SIZE + 1,
                                 x * BLOCK_SIZE + 1, (y+1) * BLOCK_SIZE - 2);

                // 添加方块阴影效果
                painter.setPen(QPen(colors[nextPiece+1].darker(150), 1));
                painter.drawLine((x+1) * BLOCK_SIZE - 1, y * BLOCK_SIZE + 1,
                                 (x+1) * BLOCK_SIZE - 1, (y+1) * BLOCK_SIZE - 1);
                painter.drawLine(x * BLOCK_SIZE + 1, (y+1) * BLOCK_SIZE - 1,
                                 (x+1) * BLOCK_SIZE - 1, (y+1) * BLOCK_SIZE - 1);
            }
        }
    }
}

//------------------ 三、游戏主窗口实现 ------------------------

//（1）绘制游戏面板（两个游戏板+预览面板）
TetrisWindow::TetrisWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("双人Qt俄罗斯方块");
    setFixedSize(1200, 800);

    // 创建主窗口布局
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // 创建两个游戏区域（两个游戏区并排）
    QHBoxLayout *gameAreasLayout = new QHBoxLayout;
    gameArea1 = new GameAreaWidget;
    gameArea2 = new GameAreaWidget;
    gameAreasLayout->setSpacing(120);  // 添加固定120像素的间隔
    gameAreasLayout->addWidget(gameArea1);
    gameAreasLayout->addWidget(gameArea2);
    gameArea1->setStyleSheet("border: 3px solid blue; margin: 10px");  // 蓝色边框标识玩家1
    gameArea2->setStyleSheet("border: 3px solid red; margin: 10px");   // 红色边框标识玩家2


    // 右侧信息面板
    QWidget *infoPanel = new QWidget;
    infoPanel->setFixedWidth(300);  //加宽信息面板
    QVBoxLayout *infoLayout = new QVBoxLayout(infoPanel);

    // 玩家1信息
    QLabel *player1Label = new QLabel("玩家1 (WASD控制/Q硬降)");
    player1Label->setStyleSheet("color: green; font-size: 18px; font-weight: bold;");
    scoreLabel1 = new QLabel("分数: 0");
    levelLabel1 = new QLabel("等级: 1");
    linesLabel1 = new QLabel("消除行数: 0");


    // 玩家2信息
    QLabel *player2Label = new QLabel("玩家2 (5123控制/Enter硬降)");
    player2Label->setStyleSheet("color: red; font-size: 18px; font-weight: bold;");
    scoreLabel2 = new QLabel("分数: 0");
    levelLabel2 = new QLabel("等级: 1");
    linesLabel2 = new QLabel("消除行数: 0");

    scoreLabel1->setStyleSheet("color: white; font-size: 16px;");
    levelLabel1->setStyleSheet("color: white; font-size: 16px;");
    linesLabel1->setStyleSheet("color: white; font-size: 16px;");

    scoreLabel2->setStyleSheet("color: white; font-size: 16px;");
    levelLabel2->setStyleSheet("color: white; font-size: 16px;");
    linesLabel2->setStyleSheet("color: white; font-size: 16px;");

    // 下一个方块预览区域 (使用自定义控件)
    QLabel *nextLabel1 = new QLabel("玩家1下一个方块:");
    QLabel *nextLabel2 = new QLabel("玩家2下一个方块:");
    nextLabel1->setStyleSheet("color: white; font-size: 16px;");
    nextLabel2->setStyleSheet("color: white; font-size: 16px;");

    nextPieceArea1 = new NextPieceArea;
    nextPieceArea2 = new NextPieceArea;

    QLabel *nextLabel = new QLabel("下一个方块:");
    nextLabel->setStyleSheet("color: white; font-size: 16px;");


    // 游戏状态和按钮
    gameStatusLabel = new QLabel("游戏状态: 准备开始");
    gameStatusLabel->setStyleSheet("color: white; font-size: 18px; font-weight: bold;");
    QPushButton *startButton = new QPushButton("开始游戏");
    QPushButton *pauseButton = new QPushButton("暂停/继续");
    QPushButton *restartButton = new QPushButton("重新开始");
    // 按钮样式
    QString buttonStyle = R"(
        QPushButton {
            font-size: 18px;
            min-width: 120px;
            min-height: 40px;
            padding: 8px 16px;
            background-color: #555;
            color: white;
            border: 1px solid #888;
            border-radius: 4px;
        }
        QPushButton:hover {
            background-color: #666;
        }
    )";
    startButton->setStyleSheet(buttonStyle);
    pauseButton->setStyleSheet(buttonStyle);
    restartButton->setStyleSheet(buttonStyle);

    // 添加控件到信息面板
    infoLayout->addWidget(player1Label);
    infoLayout->addWidget(scoreLabel1);
    infoLayout->addWidget(levelLabel1);
    infoLayout->addWidget(linesLabel1);
    infoLayout->addWidget(nextLabel1);
    infoLayout->addWidget(nextPieceArea1, 0, Qt::AlignCenter);

    infoLayout->addSpacing(20);
    infoLayout->addWidget(player2Label);
    infoLayout->addWidget(scoreLabel2);
    infoLayout->addWidget(levelLabel2);
    infoLayout->addWidget(linesLabel2);
    infoLayout->addWidget(nextLabel2);
    infoLayout->addWidget(nextPieceArea2, 0, Qt::AlignCenter);

    infoLayout->addSpacing(30);
    infoLayout->addWidget(gameStatusLabel);
    infoLayout->addStretch();
    infoLayout->addWidget(startButton);
    infoLayout->addWidget(pauseButton);
    infoLayout->addWidget(restartButton);

    // 将游戏区域和信息面板添加到主布局
    mainLayout->addLayout(gameAreasLayout);
    mainLayout->addWidget(infoPanel);

    // 连接按钮信号
    connect(startButton, &QPushButton::clicked, this,[this]() {
        initGame();
        timer->start(500);
        gameStatusLabel->setText("游戏状态: 运行中");
    });

    connect(pauseButton, &QPushButton::clicked, this,[this]() {
        isPaused = !isPaused;
        if (isPaused) {
            timer->stop();
            gameStatusLabel->setText("游戏状态: 暂停");
        } else {
            int interval1 = 500 - (level1-1)*50;
            int interval2 = 500 - (level2-1)*50;
            //使用两个玩家中较快的一个
            timer->start(qMin(interval1, interval2));
            gameStatusLabel->setText("游戏状态: 运行中");
        }
    });

    connect(restartButton, &QPushButton::clicked, this,[this]() {
        initGame();
        timer->start(500);
        gameStatusLabel->setText("游戏状态: 运行中");
    });

    // 初始化游戏
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TetrisWindow::updateGame);

    //初始化游戏
    initGame();
}

//（2）释放内存
TetrisWindow::~TetrisWindow()
{
}

//（3）初始化游戏数据，生成第一个方块
void TetrisWindow::initGame()
{
    // 清空游戏区域
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            board1[i][j] = 0;
            board2[i][j] = 0;
        }
    }

    score1 = 0;
    level1 = 1;
    linesCleared1 = 0;
    gameOver1 = false;

    score2 = 0;
    level2 = 1;
    linesCleared2 = 0;
    gameOver2 = false;

    isPaused = false;

    // 生成第一个方块 - 使用QRandomGenerator
    curPiece1 = QRandomGenerator::global()->bounded(7);
    nextPiece1 = QRandomGenerator::global()->bounded(7);

    curPiece2 = QRandomGenerator::global()->bounded(7);
    nextPiece2 = QRandomGenerator::global()->bounded(7);

    nextPieceArea1->setNextPiece(nextPiece1); // 更新预览
    nextPieceArea2->setNextPiece(nextPiece2); // 更新预览

    // 更新游戏区域状态
    updateGameArea(1);
    updateGameArea(2);
    newPiece(1);
    newPiece(2);

    updateUI();
}

//（4）更新游戏状态
void TetrisWindow::updateGameArea(int player)
{
    // 将当前游戏状态传递给游戏区域控件
    if (player == 1) {
        gameArea1->setGameState(board1, curPiece1, rotation1, curX1, curY1, gameOver1);
    } else {
        gameArea2->setGameState(board2, curPiece2, rotation2, curX2, curY2, gameOver2);
    }
}

//（5）生成预告方块
void TetrisWindow::newPiece(int player)
{
    // 根据玩家选择正确的变量引用
    int &currentPiece = (player == 1) ? curPiece1 : curPiece2;
    int &nextPiece = (player == 1) ? nextPiece1 : nextPiece2;
    int &currentX = (player == 1) ? curX1 : curX2;
    int &currentY = (player == 1) ? curY1 : curY2;
    int &currentRotation = (player == 1) ? rotation1 : rotation2;
    bool &currentGameOver = (player == 1) ? gameOver1 : gameOver2;
    NextPieceArea *nextPieceArea = (player == 1) ? nextPieceArea1 : nextPieceArea2;

    // 设置当前方块为下一个方块
    currentPiece = nextPiece;

    // 生成新的下一个方块
    nextPiece = QRandomGenerator::global()->bounded(7);

    // 更新预览区域
    nextPieceArea->setNextPiece(nextPiece);

    // 设置初始位置 (居中上方)
    currentX = BOARD_WIDTH / 2 - 2;
    currentY = 0;
    currentRotation = 0;

    // 检查游戏是否结束（新方块无法放置）
    if (checkCollision(player, currentX, currentY, currentRotation)) {
        currentGameOver = true;

        // 如果两个玩家都游戏结束，停止计时器
        if (gameOver1 && gameOver2) {
            timer->stop();
            gameStatusLabel->setText("游戏状态: 结束");

            // 显示获胜信息
            QString winnerText;
            if (score1 > score2) {
                winnerText = "玩家1获胜! 分数: " + QString::number(score1);
            } else if (score2 > score1) {
                winnerText = "玩家2获胜! 分数: " + QString::number(score2);
            } else {
                winnerText = "平局! 分数: " + QString::number(score1);
            }

            QMessageBox::information(this, "游戏结束",
                                     QString("%1\n玩家1分数: %2\n玩家2分数: %3")
                                         .arg(winnerText)
                                         .arg(score1)
                                         .arg(score2));
        } else if (player == 1) {
            gameStatusLabel->setText("游戏状态: 玩家1结束");
        } else {
            gameStatusLabel->setText("游戏状态: 玩家2结束");
        }
    }

    // 更新游戏区域
    updateGameArea(player);
}

//（6）检测是否碰撞
bool TetrisWindow::checkCollision(int player, int newX, int newY, int newRotation)
{
    // 检查新位置是否碰撞
    int (*currentBoard)[BOARD_WIDTH] = (player == 1) ? board1 : board2;
    int currentPiece = (player == 1) ? curPiece1 : curPiece2;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            // 跳过空块
            if (shapes[currentPiece][newRotation][i][j] == 0)
                continue;

            int boardX = newX + j;
            int boardY = newY + i;

            // 检查是否超出边界
            if (boardX < 0 || boardX >= BOARD_WIDTH || boardY >= BOARD_HEIGHT) {
                return true;
            }

            // 检查是否与已有方块重叠（注意boardY>=0）
            if (boardY >= 0 && currentBoard[boardY][boardX] != 0) {
                return true;
            }
        }
    }
    return false;
}

//（7）实现消行与分数更新
void TetrisWindow::pieceDropped(int player)
{
    // 根据玩家选择正确的变量
    int (*currentBoard)[BOARD_WIDTH] = (player == 1) ? board1 : board2;
    int &currentPiece = (player == 1) ? curPiece1 : curPiece2;
    int &currentX = (player == 1) ? curX1 : curX2;
    int &currentY = (player == 1) ? curY1 : curY2;
    int &currentRotation = (player == 1) ? rotation1 : rotation2;
    bool &currentGameOver = (player == 1) ? gameOver1 : gameOver2;
    int &currentScore = (player == 1) ? score1 : score2;
    int &currentLinesCleared = (player == 1) ? linesCleared1 : linesCleared2;

    // 将当前方块固定到游戏区域
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (shapes[currentPiece][currentRotation][i][j] != 0) {
                int boardX = currentX + j;
                int boardY = currentY + i;
                if (boardY >= 0) { // 只记录在游戏区域内的部分
                    currentBoard[boardY][boardX] = currentPiece + 1; // 颜色索引从1开始
                }
            }
        }
    }

    // 消除满行
    int linesRemoved = 0;
    for (int i = BOARD_HEIGHT - 1; i >= 0; i--) {
        bool full = true;
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (currentBoard[i][j] == 0) {
                full = false;
                break;
            }
        }

        if (full) {
            // 移除该行，上面的行下移
            for (int k = i; k > 0; k--) {
                for (int j = 0; j < BOARD_WIDTH; j++) {
                    currentBoard[k][j] = currentBoard[k-1][j];
                }
            }
            // 最上面一行清空
            for (int j = 0; j < BOARD_WIDTH; j++) {
                currentBoard[0][j] = 0;
            }
            i++; // 因为下移了，所以重新检查当前行
            linesRemoved++;
        }
    }

    // 更新分数
    if (linesRemoved > 0) {
        currentLinesCleared += linesRemoved;
        currentScore += linesRemoved * 100 * linesRemoved;
    }

    // 生成新方块
    currentPiece = (player == 1) ? nextPiece1 : nextPiece2;
    if (player == 1) {
        nextPiece1 = QRandomGenerator::global()->bounded(7);
    } else {
        nextPiece2 = QRandomGenerator::global()->bounded(7);
    }
    currentX = BOARD_WIDTH / 2 - 2;
    currentY = 0;
    currentRotation = 0;

    // 检查游戏是否结束
    if (checkCollision(player, currentX, currentY, currentRotation)) {
        currentGameOver = true;
    }

    // 更新游戏区域状态
    updateGameArea(player);

}

//（8）更新玩家数据
void TetrisWindow::updateGame()
{
    if (isPaused) return;

    // 更新玩家1
    if (!gameOver1) {
        if (!checkCollision(1, curX1, curY1 + 1, rotation1)) {
            curY1++;
        } else {
            pieceDropped(1);
        }
        updateGameArea(1);
    }

    // 更新玩家2
    if (!gameOver2) {
        if (!checkCollision(2, curX2, curY2 + 1, rotation2)) {
            curY2++;
        } else {
            pieceDropped(2);
        }
        updateGameArea(2);
    }

    updateUI();
}

//（9）按键交互
void TetrisWindow::keyPressEvent(QKeyEvent *event)
{
    if (isPaused && event->key() != Qt::Key_P) {
        // 游戏暂停时只响应P键
        return;
    }

    switch (event->key()) {
    // ============= 玩家1控制 (WASD) =============
    case Qt::Key_A:  //左移
        if (!gameOver1 && !checkCollision(1, curX1-1, curY1, rotation1)) {
            curX1--;
            updateGameArea(1);
        }
        break;
    case Qt::Key_D:  //右移
        if (!gameOver1 && !checkCollision(1, curX1+1, curY1, rotation1)) {
            curX1++;
            updateGameArea(1);
        }
        break;
    case Qt::Key_S:  //加速下落
        if (!gameOver1 && !checkCollision(1, curX1, curY1+1, rotation1)) {
            curY1++;
            updateGameArea(1);
        }
        break;
    case Qt::Key_W:  //旋转
        if (!gameOver1) {
            int newRotation = (rotation1 + 1) % 4;
            if (!checkCollision(1, curX1, curY1, newRotation)) {
                rotation1 = newRotation;
            } else {
                //碰撞检测：尝试左右微调
                if (!checkCollision(1, curX1-1, curY1, newRotation)) {
                    curX1--;
                    rotation1 = newRotation;
                } else if (!checkCollision(1, curX1+1, curY1, newRotation)) {
                    curX1++;
                    rotation1 = newRotation;
                }
            }
            updateGameArea(1);
        }
        break;
    case Qt::Key_Q:  //硬降（直接落到底部）
        if(!gameOver1){
            while (!checkCollision(1, curX1, curY1+1, rotation1)) {
                curY1++;
            }
            pieceDropped(1); // 立即固定
        }
        break;

    // ============= 玩家2控制 (5123键) =============
    case Qt::Key_1:  //左移
        if (!gameOver2 && !checkCollision(2, curX2-1, curY2, rotation2)){
            curX2--;
            updateGameArea(2);
        }
        break;
    case Qt::Key_3:  //右移
        if (!gameOver2 && !checkCollision(2, curX2+1, curY2, rotation2)){
            curX2++;
            updateGameArea(2);
        }
        break;
    case Qt::Key_2:  //加速下落
        if (!gameOver2 && !checkCollision(2, curX2, curY2+1, rotation2)) {
            curY2++;
            updateGameArea(2);
        }
        break;
    case Qt::Key_5:  //旋转
        if (!gameOver2) {
            int newRotation = (rotation2 + 1) % 4;
            if (!checkCollision(2, curX2, curY2, newRotation)) {
                rotation2 = newRotation;
            } else {
                // 碰撞检测：尝试左右微调
                if (!checkCollision(2, curX2-1, curY2, newRotation)) {
                    curX2--;
                    rotation2 = newRotation;
                } else if (!checkCollision(2, curX2+1, curY2, newRotation)) {
                    curX2++;
                    rotation2 = newRotation;
                }
            }
            updateGameArea(2);
        }
        break;
    case Qt::Key_Enter:  //硬降
        if (!gameOver2) {
            while (!checkCollision(2, curX2, curY2+1, rotation2)) {
                curY2++;
            }
            pieceDropped(2);
        }
        break;

    // ============= 通用控制 =============
    case Qt::Key_P:  // 暂停/继续
        isPaused = !isPaused;
        if (isPaused) {
            timer->stop();
            gameStatusLabel->setText("游戏状态: 暂停");
        } else {
            // 根据两个玩家的等级重新计算速度
            int interval1 = 500 - (level1-1)*50;
            int interval2 = 500 - (level2-1)*50;
            timer->start(qMin(interval1, interval2));
            gameStatusLabel->setText("游戏状态: 运行中");
        }
        break;
    case Qt::Key_R:  // 重新开始
        initGame();
        timer->start(500);
        gameStatusLabel->setText("游戏状态: 运行中");
        break;
    case Qt::Key_Escape:  // ESC退出全屏
        showNormal();
        break;
    case Qt::Key_F11:  // F11切换全屏
        isFullScreen() ? showNormal() : showFullScreen();
        break;
    default:
        QWidget::keyPressEvent(event);
        return;
    }

    // 更新游戏区域状态
    updateGameArea(1);
    updateGameArea(2);
}

//（10）实现全屏控制
bool TetrisWindow::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Escape) {
            // ESC键退出全屏
            showNormal();
            return true;
        } else if (keyEvent->key() == Qt::Key_F11) {
            // F11键切换全屏
            if (isFullScreen()) {
                showNormal();
            } else {
                showFullScreen();
            }
            return true;
        }
    }
    return QMainWindow::event(event);
}

//（11）游戏结束检测
void TetrisWindow::updateUI()
{
    // 更新玩家1信息
    scoreLabel1->setText(QString("玩家1分数: %1").arg(score1));
    levelLabel1->setText(QString("玩家1等级: %1").arg(level1));
    linesLabel1->setText(QString("玩家1消除行: %1").arg(linesCleared1));

    // 更新玩家2信息
    scoreLabel2->setText(QString("玩家2分数: %2").arg(score2));
    levelLabel2->setText(QString("玩家2等级: %2").arg(level2));
    linesLabel2->setText(QString("玩家2消除行: %2").arg(linesCleared2));


    // 更新游戏状态显示
    if (gameOver1 && gameOver2) {
        gameStatusLabel->setText("游戏结束 - 双方失败");
        gameStatusLabel->setStyleSheet("color: red; font-size: 18px;");
    }
    else if (gameOver1) {
        gameStatusLabel->setText("游戏状态: 玩家2领先");
        gameStatusLabel->setStyleSheet("color: red; font-size: 18px;");
    }
    else if (gameOver2) {
        gameStatusLabel->setText("游戏状态: 玩家1领先");
        gameStatusLabel->setStyleSheet("color: blue; font-size: 18px;");
    }
    else if (isPaused) {
        gameStatusLabel->setText("游戏状态: 暂停");
        gameStatusLabel->setStyleSheet("color: yellow; font-size: 18px;");
    }
    else {
        // 实时比较分数
        if (score1 > score2) {
            gameStatusLabel->setText("游戏进行中 - 玩家1领先");
            gameStatusLabel->setStyleSheet("color: blue; font-size: 18px;");
        }
        else if (score2 > score1) {
            gameStatusLabel->setText("游戏进行中 - 玩家2领先");
            gameStatusLabel->setStyleSheet("color: red; font-size: 18px;");
        }
        else {
            gameStatusLabel->setText("游戏进行中 - 势均力敌");
            gameStatusLabel->setStyleSheet("color: white; font-size: 18px;");
        }
    }

    // 更新预览区域
    nextPieceArea1->setNextPiece(nextPiece1);
    nextPieceArea2->setNextPiece(nextPiece2);

    // 如果游戏结束，显示最终结果
    // 只在第一次检测到游戏结束时显示
    static bool showedMessage = false;
    if (gameOver1 || gameOver2) {
        QString status;
        if (gameOver1 && gameOver2) {
            if (score1 > score2) {
                status = "玩家1获胜!";
            } else if (score2 > score1) {
                status = "玩家2获胜!";
            } else {
                status = "平局!";
            }
        } else if (gameOver1) {
            status = "玩家2获胜!";
        } else {
            status = "玩家1获胜!";
        }

        if (!showedMessage && (gameOver1 || gameOver2)) {
            showedMessage = true;
            QMessageBox::information(this, "游戏结束",
                                QString("%1\n\n玩家1得分: %2\n玩家2得分: %3")
                                        .arg(status)
                                        .arg(score1)
                                        .arg(score2));
        }
    } else {
        showedMessage = false;
    }
}

