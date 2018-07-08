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
    if (txdb.LoadBlockIndexGuts())
    {
        qDebug() << "txdb.LoadBlockIndexGuts() success!" << endl;
    }
    else
    {
        qDebug() << "txdb.LoadBlockIndexGuts() fail!" << endl;
    } 


    if (mapBlockIndex.empty())
    {
        qDebug() << "mapBlockIndex is empty!" << endl;
    }
    else
    {
        qDebug() << "mapBlockIndex is not empty!" << endl;
    }

    BOOST_FOREACH(const PAIRTYPE(uint256, CBlockIndex*)& item, mapBlockIndex)
    {
        qDebug() << (item.first).ToString().c_str() << " : ";
        qDebug() << item.second << endl;
    }


    return a.exec();
}
