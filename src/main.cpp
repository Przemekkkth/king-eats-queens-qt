#include <QApplication>
#include "utils.h"
#include "view.h"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    View v;
    v.setWindowTitle(CONF::TITLE);
    v.show();
    a.exec();
}
