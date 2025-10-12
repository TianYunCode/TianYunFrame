#include "DemoHead.h"

#if T_MaskWidgetDemo

#include <qapplication.h>
#include <qtimer.h>
#include "T_MaskWidgetTest.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    T_MaskWidgetTest window;
    window.show();

    return app.exec();
}

#endif
