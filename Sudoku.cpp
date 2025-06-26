#include "Sudoku.h"

Sudoku::Sudoku(QObject *parent)
    : QObject(parent),
    m_board(9, QVector<int>(9, 0))
{
}

void Sudoku::setValue(int row, int col, int val)
{
    if (row < 0 || row > 8 || col < 0 || col > 8 || val < 0 || val > 9)
        return;
    m_board[row][col] = val;
    emit boardChanged();
}

int Sudoku::getValue(int row, int col) const
{
    if (row < 0 || row > 8 || col < 0 || col > 8)
        return 0;
    return m_board[row][col];
}

void Sudoku::setBoard(const Board &board)
{
    if (board.size() != 9)
        return;
    for (const auto &row : board)
        if (row.size() != 9)
            return;
    m_board = board;
    emit boardChanged();
}

Sudoku::Board Sudoku::getBoard() const
{
    return m_board;
}

void Sudoku::clear()
{
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            m_board[i][j] = 0;
    emit boardChanged();
}

bool Sudoku::isCellValid(int row, int col, int val) const
{
    if (val < 1 || val > 9)
        return false;
    // 检查行
    for (int j = 0; j < 9; ++j)
        if (j != col && m_board[row][j] == val)
            return false;
    // 检查列
    for (int i = 0; i < 9; ++i)
        if (i != row && m_board[i][col] == val)
            return false;
    // 检查3x3宫
    int boxRow = (row / 3) * 3;
    int boxCol = (col / 3) * 3;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if ((boxRow + i != row || boxCol + j != col) && m_board[boxRow + i][boxCol + j] == val)
                return false;
    return true;
}

bool Sudoku::isValid() const
{
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
        {
            int val = m_board[i][j];
            if (val == 0)
                continue;
            // 用临时变量校验，避免修改成员变量
            Board boardCopy = m_board;
            boardCopy[i][j] = 0;
            // 临时用一个Sudoku对象校验
            Sudoku temp;
            temp.setBoard(boardCopy);
            if (!temp.isCellValid(i, j, val))
                return false;
        }
    return true;
}

bool Sudoku::isFull() const
{
    for (const auto &row : m_board)
        for (int num : row)
            if (num == 0)
                return false;
    return true;
}

QVector<int> Sudoku::getCandidates(int row, int col) const
{
    QVector<int> candidates;
    if (m_board[row][col] != 0)
        return candidates;
    for (int val = 1; val <= 9; ++val)
    {
        if (isCellValid(row, col, val))
            candidates.append(val);
    }
    return candidates;
}

QVector<QPair<int, int>> Sudoku::getEmptyCells() const
{
    QVector<QPair<int, int>> empty;
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            if (m_board[i][j] == 0)
                empty.append(qMakePair(i, j));
    return empty;
}

bool Sudoku::solve(Board &solution) const
{
    solution = m_board;
    return solveRecursive(solution);
}

bool Sudoku::solveRecursive(Board &board) const
{
    for (int row = 0; row < 9; ++row)
    {
        for (int col = 0; col < 9; ++col)
        {
            if (board[row][col] == 0)
            {
                for (int val = 1; val <= 9; ++val)
                {
                    // 用isCellValid需要用临时棋盘
                    bool valid = true;
                    // 检查行
                    for (int j = 0; j < 9; ++j)
                        if (board[row][j] == val)
                            valid = false;
                    // 检查列
                    for (int i = 0; i < 9; ++i)
                        if (board[i][col] == val)
                            valid = false;
                    // 检查宫
                    int boxRow = (row / 3) * 3;
                    int boxCol = (col / 3) * 3;
                    for (int i = 0; i < 3; ++i)
                        for (int j = 0; j < 3; ++j)
                            if (board[boxRow + i][boxCol + j] == val)
                                valid = false;
                    if (valid)
                    {
                        board[row][col] = val;
                        if (solveRecursive(board))
                            return true;
                        board[row][col] = 0;
                    }
                }
                return false; // 无解
            }
        }
    }
    return true; // 全部填满
}

// 随机打乱1~9
static QVector<int> shuffledDigits() {
    QVector<int> nums;
    for (int i = 1; i <= 9; ++i) nums.append(i);
    for (int i = 8; i > 0; --i) {
        int j = QRandomGenerator::global()->bounded(i + 1);
        nums.swapItemsAt(i, j);
    }
    return nums;
}

// 用递归填充一个完整的随机解
bool Sudoku::fillBoardRandom(Board &board) const
{
    for (int row = 0; row < 9; ++row)
    {
        for (int col = 0; col < 9; ++col)
        {
            if (board[row][col] == 0)
            {
                QVector<int> nums = shuffledDigits();
                for (int val : nums)
                {
                    bool valid = true;
                    for (int j = 0; j < 9; ++j)
                        if (board[row][j] == val)
                            valid = false;
                    for (int i = 0; i < 9; ++i)
                        if (board[i][col] == val)
                            valid = false;
                    int boxRow = (row / 3) * 3;
                    int boxCol = (col / 3) * 3;
                    for (int i = 0; i < 3; ++i)
                        for (int j = 0; j < 3; ++j)
                            if (board[boxRow + i][boxCol + j] == val)
                                valid = false;
                    if (valid)
                    {
                        board[row][col] = val;
                        if (fillBoardRandom(board))
                            return true;
                        board[row][col] = 0;
                    }
                }
                return false;
            }
        }
    }
    return true;
}

// 随机生成一个部分填充且有解的棋盘
void Sudoku::generateRandom(int filledCount)
{
    // 1. 生成完整解
    Board full(9, QVector<int>(9, 0));
    fillBoardRandom(full);

    // 2. 随机保留filledCount个格子，其余清零
    QVector<QPair<int, int>> cells;
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            cells.append(qMakePair(i, j));
    // 打乱cells
    for (int i = cells.size() - 1; i > 0; --i) {
        int j = QRandomGenerator::global()->bounded(i + 1);
        cells.swapItemsAt(i, j);
    }

    Board puzzle = full;
    for (int idx = filledCount; idx < 81; ++idx)
        puzzle[cells[idx].first][cells[idx].second] = 0;

    m_board = puzzle;
    emit boardChanged();
}
