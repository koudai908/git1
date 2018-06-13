#include "dialog.h"
#include <QApplication>

#include <QDebug>

#include "net.h"
#include "util.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.show();

    qDebug() << "app begin!" << endl;


    if (NewThread(StartNode, NULL))
    {

        qDebug() << "Start Node successed! " << endl;

    }

    if (!vNodes.empty())
    {

        qDebug() << "vNodes is not empty! " << endl;

    }
    else
    {

        qDebug() << "vNodes is empty! " << endl;

    }





    return a.exec();
}
