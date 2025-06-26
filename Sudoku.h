#ifndef SUDOKU_H
#define SUDOKU_H

#include <QObject>
#include <QVector>
#include <QPair>
#include <QRandomGenerator>

/**
 * @brief The Sudoku class
 * 负责存储和操作数独棋盘，包含基本操作与求解功能
 */
class Sudoku : public QObject
{
    Q_OBJECT

public:
    explicit Sudoku(QObject *parent = nullptr);

    // 9x9棋盘，0表示空格
    typedef QVector<QVector<int>> Board;

    // 设置指定格子的值（0-9），0为清空
    void setValue(int row, int col, int val);

    // 取得指定格子的值
    int getValue(int row, int col) const;

    // 设置整个棋盘
    void setBoard(const Board &board);

    // 取得整个棋盘
    Board getBoard() const;

    // 清空棋盘
    void clear();

    // 检查棋盘当前状态是否有效（无冲突，不保证可解）
    bool isValid() const;

    // 检查一个格子的填写是否合法
    bool isCellValid(int row, int col, int val) const;

    // 检查是否已全部填写
    bool isFull() const;

    // 尝试求解当前棋盘，返回是否有解，解写入board
    bool solve(Board &solution) const;

    // 返回所有候选数字
    QVector<int> getCandidates(int row, int col) const;

    // 返回所有空格位置
    QVector<QPair<int, int>> getEmptyCells() const;

    // 随机生成一个有解的数独棋盘，参数filledCount为初始已填数字个数（一般20~35）
    void generateRandom(int filledCount = 30);

signals:
    void boardChanged();

private:
    Board m_board;

    // 内部递归求解
    bool solveRecursive(Board &board) const;

    // 递归生成完整解
    bool fillBoardRandom(Board &board) const;
};

#endif // SUDOKU_H
