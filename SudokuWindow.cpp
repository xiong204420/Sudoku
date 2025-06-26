#include "SudokuWindow.h"
#include "ui_sudokuwindow.h"
#include <QPushButton>
#include <QGridLayout>
#include <QRegularExpressionValidator>
#include <QRegularExpression>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFont>
#include <QPalette>

SudokuWindow::SudokuWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SudokuWindow)
{
    ui->setupUi(this);
    setupBoard();
    updateBoard();
    showStatus("欢迎使用数独游戏！");

    // connect(ui->btnNewGame, &QPushButton::clicked, this, &SudokuWindow::on_btnNewGame_clicked);
    // connect(ui->btnRandomGame, &QPushButton::clicked, this, &SudokuWindow::on_btnRandomGame_clicked);
    // connect(ui->btnOpen, &QPushButton::clicked, this, &SudokuWindow::on_btnOpen_clicked);
    // connect(ui->btnSave, &QPushButton::clicked, this, &SudokuWindow::on_btnSave_clicked);
    // connect(ui->btnSolve, &QPushButton::clicked, this, &SudokuWindow::on_btnSolve_clicked);
    // connect(ui->btnCheck, &QPushButton::clicked, this, &SudokuWindow::on_btnCheck_clicked);
}

SudokuWindow::~SudokuWindow()
{
    delete ui;
}

// 初始化9x9 QLineEdit数组
void SudokuWindow::setupBoard()
{
    QGridLayout *grid = qobject_cast<QGridLayout*>(ui->boardFrame->layout());
    m_cells.resize(9);
    QFont cellFont;
    cellFont.setPointSize(14);
    for (int i = 0; i < 9; ++i) {
        m_cells[i].resize(9);
        for (int j = 0; j < 9; ++j) {
            QLineEdit *edit = new QLineEdit(this);
            edit->setAlignment(Qt::AlignCenter);
            edit->setMaxLength(1);
            edit->setFont(cellFont);
            edit->setFixedSize(36, 36);
            edit->setValidator(new QRegularExpressionValidator(QRegularExpression("[1-9]?"), edit));
            // 设置格子边框以区分宫
            QString style = "border: 1px solid gray;";
            if (i % 3 == 0) style += "border-top: 2px solid black;";
            if (j % 3 == 0) style += "border-left: 2px solid black;";
            if (i == 8)     style += "border-bottom: 2px solid black;";
            if (j == 8)     style += "border-right: 2px solid black;";
            edit->setStyleSheet(style);
            grid->addWidget(edit, i, j);
            m_cells[i][j] = edit;
            connect(edit, &QLineEdit::textChanged, this, &SudokuWindow::cellTextChanged);
        }
    }
}

// 根据 m_sudoku 更新界面格子
void SudokuWindow::updateBoard()
{
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
        {
            int v = m_sudoku.getValue(i, j);
            QString str = v > 0 ? QString::number(v) : "";
            m_cells[i][j]->blockSignals(true);
            m_cells[i][j]->setText(str);
            m_cells[i][j]->blockSignals(false);
            m_cells[i][j]->setReadOnly(false);
            m_cells[i][j]->setStyleSheet(m_cells[i][j]->styleSheet() + "color:black;background:white;");
        }
    clearHighlights();
}

// 空白盘
void SudokuWindow::on_btnNewGame_clicked()
{
    m_sudoku.clear();
    updateBoard();
    showStatus("已新建空白棋盘。");
}

// 新游戏：随机有解盘
void SudokuWindow::on_btnRandomGame_clicked()
{
    // 可以让用户自定义数量，这里固定30
    m_sudoku.generateRandom(30);
    updateBoard();
    showStatus("已生成随机有解棋盘。");
}

// 打开棋盘
void SudokuWindow::on_btnOpen_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "打开棋盘文件", "", "文本文件 (*.txt *.csv)");
    if (fileName.isEmpty()) return;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件！");
        return;
    }
    QVector<QVector<int>> board(9, QVector<int>(9, 0));
    QTextStream in(&file);
    int row = 0;
    while (!in.atEnd() && row < 9) {
        QString line = in.readLine();
        int col = 0;
        for (QChar c : line) {
            if ((c.isDigit() && c != '0') || c == '.') {
                board[row][col] = (c == '.') ? 0 : c.digitValue();
                ++col;
                if (col >= 9) break;
            }
        }
        if (col == 9) ++row;
    }
    file.close();
    if (row != 9) {
        QMessageBox::warning(this, "错误", "棋盘文件格式有误！");
        return;
    }
    m_sudoku.setBoard(board);
    updateBoard();
    showStatus("棋盘已载入。");
}

// 保存棋盘
void SudokuWindow::on_btnSave_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "保存棋盘文件", "", "文本文件 (*.txt *.csv)");
    if (fileName.isEmpty()) return;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法保存文件！");
        return;
    }
    loadBoardToSudoku();
    QTextStream out(&file);
    auto board = m_sudoku.getBoard();
    for (int i = 0; i < 9; ++i) {
        QString line;
        for (int j = 0; j < 9; ++j)
            line += board[i][j] == 0 ? '.' : QChar('0' + board[i][j]);
        out << line << '\n';
    }
    file.close();
    showStatus("棋盘已保存。");
}

// 求解棋盘
void SudokuWindow::on_btnSolve_clicked()
{
    loadBoardToSudoku();
    Sudoku::Board solution;
    if (m_sudoku.solve(solution)) {
        m_sudoku.setBoard(solution);
        updateBoard();
        showStatus("已自动求解。");
    } else {
        showStatus("该棋盘无解！", true);
    }
}

// 检查填写是否有效
void SudokuWindow::on_btnCheck_clicked()
{
    loadBoardToSudoku();
    if (!m_sudoku.isValid()) {
        highlightErrors();
        showStatus("存在冲突或错误，请检查红色格子！", true);
    } else if (m_sudoku.isFull()) {
        showStatus("恭喜，数独已全部正确填写！");
    } else {
        showStatus("当前填写无冲突，请继续。");
    }
}

// 从界面同步数据到m_sudoku
void SudokuWindow::loadBoardToSudoku()
{
    QVector<QVector<int>> board(9, QVector<int>(9, 0));
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j) {
            QString text = m_cells[i][j]->text();
            if (text.isEmpty() || text == "0")
                board[i][j] = 0;
            else
                board[i][j] = text.toInt();
        }
    m_sudoku.setBoard(board);
}

// 响应单元格内容变化，自动校验
void SudokuWindow::cellTextChanged()
{
    loadBoardToSudoku();
    clearHighlights();
    highlightErrors();
}

// 高亮冲突格子
void SudokuWindow::highlightErrors()
{
    clearHighlights();
    auto board = m_sudoku.getBoard();
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j) {
            int val = board[i][j];
            if (val == 0) continue;
            m_cells[i][j]->setStyleSheet(m_cells[i][j]->styleSheet() + "color:black;background:white;");
            board[i][j] = 0;
            Sudoku temp;
            temp.setBoard(board);
            if (!temp.isCellValid(i, j, val)) {
                m_cells[i][j]->setStyleSheet(m_cells[i][j]->styleSheet() + "color:white;background:#d32f2f;");
            }
            board[i][j] = val;
        }
}

// 清除所有高亮
void SudokuWindow::clearHighlights()
{
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            m_cells[i][j]->setStyleSheet(m_cells[i][j]->styleSheet() + "color:black;background:white;");
}

// 显示状态栏信息
void SudokuWindow::showStatus(const QString &msg, bool error)
{
    ui->statusLabel->setText(msg);
    QPalette pal = ui->statusLabel->palette();
    pal.setColor(QPalette::WindowText, error ? Qt::red : Qt::black);
    ui->statusLabel->setPalette(pal);
}

