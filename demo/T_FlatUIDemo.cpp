#include "DemoHead.h"

#if T_FlatUIDemo

#include <QApplication>
#include "T_FlatUITest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    T_FlatUITest window;

    window.show();

    return a.exec();
}


#endif
