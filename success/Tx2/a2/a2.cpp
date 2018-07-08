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


    //load wallet

    qDebug() << "load wallet start!" << endl;

    int64 nStart;
    nStart = GetTimeMillis();
    bool fFirstRun = true;

    pwalletMain = new CWallet("wallet.dat");
    DBErrors nLoadWalletRet = pwalletMain->LoadWallet(fFirstRun);

    if (nLoadWalletRet != DB_LOAD_OK)
    {
        if (nLoadWalletRet == DB_CORRUPT)
        {
            qDebug() << "Error loading wallet.dat: Wallet corrupted" << endl;
        }
        else if (nLoadWalletRet == DB_NONCRITICAL_ERROR)
        {
            qDebug() << "DB_NONCRITICAL_ERROR" << endl;
        }

        else if (nLoadWalletRet == DB_TOO_NEW)
            qDebug() << "Error loading wallet.dat: Wallet requires newer version of Bitcoin" << endl;
        else if (nLoadWalletRet == DB_NEED_REWRITE)
        {
            qDebug() << "Wallet needed to be rewritten: restart Bitcoin to complete" << endl;

        }
        else
            qDebug() << "Error loading wallet.dat" << endl;
    }
    else
    {
        qDebug() << "Load wallet.dat success!" << endl;
    }

    if (fFirstRun)
    {
        // Create new keyUser and set as default key
        RandAddSeedPerfmon();

        CPubKey newDefaultKey;
        if (pwalletMain->GetKeyFromPool(newDefaultKey, false))
        {
            qDebug() << "pwalletMain->GetKeyFromPool success!" << endl;


        }
        else
        {
            qDebug()<< "Cannot initialize keypool" << endl;
        }

        if(pwalletMain->SetDefaultKey(newDefaultKey))
        {
            qDebug() << "pwalletMain->SetDefaultKey success!" << endl;
        }
        else
        {
            qDebug()<< "pwalletMain->SetDefaultKey fail!" << endl;
        }



        if (pwalletMain->SetAddressBookName(pwalletMain->vchDefaultKey.GetID(), ""))
        {
            qDebug() << "pwalletMain->SetAddressBookName success!" << endl;
        }
        else
        {
            qDebug() << "Cannot write default address!" << endl;
        }

    }
    else
    {
        qDebug() << "It is not first run!" << endl;

        //pwalletMain->vchDefaultKey.vchPubKey
        std::vector<unsigned char> vor = pwalletMain->vchDefaultKey.vchPubKey;
        for (std::vector<unsigned char>::iterator m = vor.begin(); m != vor.end(); m++)
        {
            qDebug() << *m << " ";
        }

        qDebug() << endl;

        //std::map<CTxDestination, std::string> mapAddressBook;
        //pwalletMain->mapAddressBook

        BOOST_FOREACH(const PAIRTYPE(CTxDestination, std::string)& item, pwalletMain->mapAddressBook)
        {
            qDebug() << CBitcoinAddress(item.first).ToString().c_str() << " : ";
            qDebug() << item.second.c_str() << endl;
        }

        // 1NbS1K5KkMGxFo9LcN5WVxqAAboymX355d

    }

    RegisterWallet(pwalletMain);

    QString address = "1NbS1K5KkMGxFo9LcN5WVxqAAboymX355d";
    qint64 amount = 50;

    //SendCoinsRecipient rcp;
    //rcp.address = "1NbS1K5KkMGxFo9LcN5WVxqAAboymX355d";
    //rcp.amount = amount;

    std::vector<std::pair<CScript, int64> > vecSend;
    CScript scriptPubKey;
    scriptPubKey.SetDestination(CBitcoinAddress(address.toStdString()).Get());
    vecSend.push_back(make_pair(scriptPubKey, amount));

    CWalletTx wtx;
    CReserveKey keyChange(pwalletMain);
    int64 nFeeRequired = 0;

    bool fCreated = pwalletMain->CreateTransaction(scriptPubKey, amount, wtx, keyChange, nFeeRequired);

    if(fCreated)
    {
        qDebug() << "pwalletMain->CreateTransaction success!" << endl;

        qDebug() << wtx.ToString().c_str() << endl;
    }
    else
    {
        qDebug()<< "pwalletMain->CreateTransaction fail!" << endl;
    }






    return a.exec();
}
