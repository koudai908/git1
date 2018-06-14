#include "dialog.h"
#include <QApplication>

#include <QDebug>

#include "irc.h"
#include "net.h"
#include "strlcpy.h"
#include "base58.h"

using namespace std;
using namespace boost;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.show();

    qDebug() << "app begin!" << endl;


    CService addrConnect("92.243.23.21", 6667); // irc.lfnet.org

    CService addrIRC("irc.lfnet.org", 6667, true);
    if (addrIRC.IsValid())
        addrConnect = addrIRC;

    SOCKET hSocket;
    if (ConnectSocket(addrConnect, hSocket))
    {

        qDebug() << "IRC connect success!" << endl;

    }

    string strLine;
    if (RecvLine(hSocket,strLine))
    {

        qDebug() << strLine.c_str() << endl;

    }




    return a.exec();
}
