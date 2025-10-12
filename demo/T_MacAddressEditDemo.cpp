#include "DemoHead.h"

#if T_MacAddressEditDemo

#include <QApplication>
#include "T_MacAddressEditTest.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    T_MacAddressEditTest demo;
    demo.show();

    return app.exec();
}

#endif
