#include "dialog.h"
#include <QApplication>

#include <QDebug>

#include "db.h"
#include "walletdb.h"
#include "bitcoinrpc.h"
#include "net.h"
#include "init.h"
#include "util.h"
#include "ui_interface.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <openssl/crypto.h>

#ifndef WIN32
#include <signal.h>
#endif

using namespace std;
using namespace boost;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.show();

    qDebug() << "app begin!" << endl;

    CTxDB txdb("cr");
    if (txdb.LoadBlockIndex())
    {
        qDebug() << "txdb.LoadBlockIndex() success!" << endl;
    }
    else
    {
        qDebug() << "txdb.LoadBlockIndex() fail!" << endl;
    }

    fPrintToConsole = true;
    PrintBlockTree();
    qDebug() << endl;

    txdb.Close();


    if (mapBlockIndex.empty())
    {
        qDebug() << "mapBlockIndex is empty!" << endl;
    }
    else
    {
        qDebug() << "mapBlockIndex is not empty!" << endl;
    }


    return a.exec();
}
