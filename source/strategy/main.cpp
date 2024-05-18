#include "frame.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    frame        w;
    w.setWindowState(Qt::WindowMaximized);
    w.show();

    return a.exec();
}
