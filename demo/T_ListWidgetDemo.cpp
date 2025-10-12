#include "DemoHead.h"

#if T_ListWidgetDemo

#include "T_ListWidgetTest.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置默认样式
    app.setStyle("Fusion");

    T_ListWidgetTest window;
    window.show();

    return app.exec();
}

#endif
