#include "DemoHead.h"

#if T_LedIndicatorDemo

#include <QApplication>
#include "T_LedIndicatorTest.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    T_LedIndicatorTest demo;
    demo.show();

    return app.exec();
}

#endif
