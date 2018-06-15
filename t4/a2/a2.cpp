#include "dialog.h"
#include <QApplication>

#include <QDebug>

#include "net.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.show();

    qDebug() << "app begin!" << endl;

    CNetAddr cNetAddr("127.0.0.1");
    CService cService(cNetAddr, 18333);
    std::string strError;
    bool fBound = BindListenPort(cService, strError);


    if (fBound)
    {
        qDebug() << "Listen on port success!" << endl;
    }else
    {
        qDebug() << "Listen on port fail!" << endl;
        qDebug() << strError.c_str() << endl;
     }



    return a.exec();
}
