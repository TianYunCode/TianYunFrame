#include "DemoHead.h"

#if T_BorderlessStretchWindowDemo

#include "T_BorderlessStretchWindowTest.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    T_BorderlessStretchWindowTest window;
    window.resize(500, 400);
    window.show();

    return app.exec();
}

#endif
