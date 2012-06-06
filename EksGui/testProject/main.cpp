#include <QtGui/QApplication>
#include "XVector3DWidget"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    XVector3DWidget widg( XVector3D( 0.0, 1.0, 2.0 ), QStringList() << "a" << "b" << "c" << "d" );

    widg.show();

    return a.exec();
}
