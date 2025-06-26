#include <QApplication>
#include "SudokuWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SudokuWindow w;
    w.show();

    return a.exec();
}
