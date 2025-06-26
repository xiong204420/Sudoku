#ifndef SUDOKUWINDOW_H
#define SUDOKUWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QRegularExpressionValidator>
#include "Sudoku.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SudokuWindow; }
QT_END_NAMESPACE

class SudokuWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SudokuWindow(QWidget *parent = nullptr);
    ~SudokuWindow();

private slots:
    void on_btnNewGame_clicked();        // 空白盘
    void on_btnRandomGame_clicked();     // 随机有解盘
    void on_btnOpen_clicked();
    void on_btnSave_clicked();
    void on_btnSolve_clicked();
    void on_btnCheck_clicked();
    void cellTextChanged();

    // void on_SudokuWindow_iconSizeChanged(const QSize &iconSize);

private:
    Ui::SudokuWindow *ui;
    Sudoku m_sudoku;
    QVector<QVector<QLineEdit*>> m_cells;

    void setupBoard();
    void updateBoard();
    void setBoardEditable(bool editable);
    void highlightErrors();
    void loadBoardToSudoku();
    void showStatus(const QString &msg, bool error = false);
    void clearHighlights();
};

#endif // SUDOKUWINDOW_H
