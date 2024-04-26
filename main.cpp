#include "widget.h"
#include <QApplication>
#include <QWidget>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle("Dynamic contours");
    w.resize(800, 750);

    // QMenu *editMenu = menuBar->addMenu("Правка");
    // QAction *copyAction = editMenu->addAction("Копировать");

    w.show();
    return a.exec();
}
