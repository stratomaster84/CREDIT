#include "mytabs.h"

// ======================================================================
int main(int argc, char *argv[])
{
    MyTabs *_main;
    QApplication app(argc, argv);
    _main = new MyTabs;
    _main->show();
    app.exec();
    delete _main;
    return 0;
}
