# 基于Qt实现tetris游戏（双人版）

## 1.游戏整体架构

采用经典的MVC模式：

- **Model**：游戏逻辑（`TetrisWindow`中的核心算法）
- **View**：自定义控件（`GameAreaWidget`和`NextPieceArea`）
- **Controller**：键盘事件和定时器控制（“`WSAD`”实现方块左右/下落/旋转，定时器`QTimer`驱动方块自动下落）

游戏界面展示：

![](./project/png/game interface.png)

### **2. 核心数据结构**

#### （1）**方块形状表示**

```
const int shapes[7][4][4][4]; // [方块类型][旋转状态][4x4矩阵]
```

- **四维数组**表示**7种方块**（I,J,L,O,S,T,Z）的4种旋转状态

- 每个方块用**4x4矩阵**表示，1表示有方块，0表示空白


![](E:\double tetris\project\png\block type.png)

#### **(2) 颜色定义**

```
const QColor colors[8]; // [0]为背景色，[1-7]对应7种方块颜色
```

- **一维数组**记录不同类型方块的颜色

#### **(3) 游戏板**

```
int board[BOARD_HEIGHT][BOARD_WIDTH]; // 20行x10列
```

- **二维数组**记录每个格子的状态

### **3. 关键模块实现**

#### **(1) 游戏主窗口 (`TetrisWindow`)**

**功能实现**：

- 整合游戏区域、预览区域和控制面板
- 管理游戏状态（分数、等级和消除行数）
- 处理用户输入（按键交互）和游戏逻辑

主要代码：

```
GameAreaWidget::GameAreaWidget(QWidget *parent) : QWidget(parent)    //二维数组初始化游戏板
```

```
void GameAreaWidget::setGameState(int b[BOARD_HEIGHT][BOARD_WIDTH],int curPiece, int curX, int curY, bool gameOver)  //更新游戏板状态
```

```
void TetrisWindow::initGame()  //初始化游戏数据，如清空游戏版，充值分数，随机生成方块
```

```
void GameAreaWidget::paintEvent(QPaintEvent *event)  
//绘制游戏区域，如
// 1. 绘制网格线
// 2. 绘制已固定的方块（带立体效果）
// 3. 绘制当前下落方块
// 4. 游戏结束状态提示
```

#### **(2) 下一个方块预览 (`NextPieceArea`)**

**功能**：

- 显示下一个将出现的方块
- 居中显示在6x6的区域内

```
NextPieceArea::NextPieceArea(QWidget *parent) : QWidget(parent)
//设置下一个方块类型
```

```
// 更新游戏区域显示
void TetrisWindow::updateGameArea()
```

```
// 创建新方块
void TetrisWindow::newPiece()
```

```
void NextPieceArea::paintEvent(QPaintEvent *event)
//绘制预览区域
```

### **4. 核心算法**

#### **(1) 方块旋转算法**

- 通过预定义的`shapes`数组实现，直接切换旋转状态索引
- 旋转时进行碰撞检测，若失败尝试左右微调（"**wall kick**"）

#### **(2) 碰撞检测算法**

- 检查预告方块是否有效，即检查预告方块是否超出边界或与已有方块重叠

```
bool checkCollision(int newX, int newY, int newRotation) {
    for (每个方块单元) {
        if (超出边界 || 与已有方块重叠) 
            return true;
    }
    return false;
}
```

#### **(3) 消行算法**

- **从底部向上扫描**从底部开始处理可以避免覆盖未检查的行。

  ```
  for (int i = BOARD_HEIGHT - 1; i >= 0; i--)
  ```

- **满行检测**：当一行中的所有格子非空（`board[i][j] != 0`）时进行上一行数据下移。

  ```
  if (board[i][j] == 0) {
      lineFull = false;
      break;
  }
  ```

- **下移操作**：将当前行上方的所有行数据向下复制一格。

  ```
  for (int k = i; k > 0; k--) {
      for (int j = 0; j < BOARD_WIDTH; j++) {
          board[k][j] = board[k-1][j];
      }
  }
  ```

- **顶部清零**：最上方行移走后需清空。

  ```
  for (int j = 0; j < BOARD_WIDTH; j++) {
      board[0][j] = 0;
  }
  i++;  //实现多行消除
  ```

### **5. 关键Qt技术应用**

1. **自定义控件**：通过继承`QWidget`实现`GameAreaWidget`和`NextPieceArea`
2. **绘图系统**：使用`QPainter`实现高性能绘制
3. **事件处理**：重写`keyPressEvent`处理键盘控制
4. **定时器**：`QTimer`驱动方块自动下落
5. **样式定制**：通过`QPalette`实现深色主题

### **6. 控制逻辑**

**键盘映射**：

玩家1：

- W：旋转
- A/D：左右移动
- S：加速下落
- Q：硬降到底

玩家2：

- 5：旋转
- 1/3：左右移动
- 2：加速下落
- Enter：硬降到底

通用按键：

- P：暂停/继续
- R：重新开始
- Esc：退出全屏/退出游戏
- Ctrl+F11：进入全屏

### **8. 可能的改进方向**

1. **实现存档功能**：`QSettings`保存最高分
2. **添加音效系统**：使用`QSoundEffect`
3. **实现幽灵方块与等级计算**：增加游戏可玩性
4. **动态难度调整**：可调速度等级
5. **动态调整面板**：目前全屏退出后会出现面板错乱的情况

