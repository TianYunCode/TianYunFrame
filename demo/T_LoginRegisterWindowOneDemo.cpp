#include "DemoHead.h"

#if LoginRegisterWindowOneDemo

#include <QApplication>
#include "LoginRegisterWindowOne.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    LoginRegisterWindowOne window(nullptr, "蓝牙车灯产测工具");
    window.show();

    return app.exec();
}

#endif
