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

    //load wallet 1

    qDebug() << "load wallet 1 start!" << endl;

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

        pwalletMain->SetDefaultKey(newDefaultKey);

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


    //load block chain

    qDebug() << "load block chain start!" << endl;

    CTxDB txdb("cr");
    if (txdb.LoadBlockIndex())
    {
        qDebug() << "txdb.LoadBlockIndex() success!" << endl;
    }
    else
    {
        qDebug() << "txdb.LoadBlockIndex() fail!" << endl;
    }

    qDebug() << "before create genesis block, pindexGenesisBlock is: " << pindexGenesisBlock << endl;


    //create genesis block
    if (mapBlockIndex.empty())
    {
        qDebug() << "mapBlockIndex is empty!" << endl;

        qDebug() << "Genesis block start!" << endl;

        // Genesis block
        const char* pszTimestamp = "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks";
        CTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CBigNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));

        txNew.vout[0].nValue = 50 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << pwalletMain->vchDefaultKey << OP_CHECKSIG;

        CBlock block;
        block.vtx.push_back(txNew);

        //
        if(mempool.addUnchecked(txNew.GetHash(), txNew))
        {
             qDebug() << "mempool.addUnchecked(txNew.GetHash(), txNew) success!" << endl;
        }
        else
        {
             qDebug() << "mempool.addUnchecked(txNew.GetHash(), txNew) fail!" << endl;
        }

        block.hashPrevBlock = 0;
        block.hashMerkleRoot = block.BuildMerkleTree();
        block.nVersion = 1;
        block.nTime    = 1231006505;
        //block.nBits    = 0x1d00ffff;
        block.nBits    = 0x1fffffff;
        //block.nNonce   = 2083236893;
        block.nNonce   = 3476488448;

        fPrintToConsole = true;

        // debug print
        qDebug() << "new block.GetHash() is: " << block.GetHash().ToString().c_str() << endl;
        qDebug() << "old hashGenesisBlock is: " << hashGenesisBlock.ToString().c_str() << endl;
        qDebug() << "new block.hashMerkleRoot is: " << block.hashMerkleRoot.ToString().c_str() << endl;

        //hash is:  00000ffb87710d7f55b1693236955421c49963731349f51fd278ae5b7946c273

        //assert(block.hashMerkleRoot == uint256("0x4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"));

        block.print1();
        qDebug() << endl;

        //assert(block.GetHash() == hashGenesisBlock);

        // Start new block file
        unsigned int nFile;
        unsigned int nBlockPos;

        if (block.WriteToDisk(nFile, nBlockPos))
        {
            qDebug() << "block.WriteToDisk success!" << endl;
        }
        else
        {
            qDebug() << "LoadBlockIndex() : writing genesis block to disk failed" << endl;
        }

        if (block.AddToBlockIndex(nFile, nBlockPos))
        {
            qDebug() << "block.AddToBlockIndex success!" << endl;
        }
        else
        {
            qDebug() << "LoadBlockIndex() : genesis block not accepted" << endl;
        }

    }

    else
    {
        qDebug() << "mapBlockIndex is not empty!" << endl;

        BOOST_FOREACH(const PAIRTYPE(uint256, CBlockIndex*)& item, mapBlockIndex)
        {
            qDebug() << (item.first).ToString().c_str() << " : ";
            qDebug() << item.second << endl;
        }

        //36a6c67d57885e3030b0e162bd3b8f6666261d549b9372cd3ddae1da138308f0  :
        //0xf13c80
    }

    qDebug() << "after genesis block, pindexGenesisBlock is: " << pindexGenesisBlock << endl;


    //load wallet 2

    qDebug() << "load wallet 2 start!" << endl;

    CBlockIndex *pindexRescan = pindexBest;

    CWalletDB walletdb("wallet.dat");
    CBlockLocator locator;
    if (walletdb.ReadBestBlock(locator))
    {
        pindexRescan = locator.GetBlockIndex();
    }

    qDebug() << "pindexBest is: " << pindexBest << endl;
    qDebug() << "pindexRescan is: " << pindexRescan << endl;


    if (pindexBest != pindexRescan)
    {
        qDebug("Rescanning last %i blocks (from block %i)...\n", pindexBest->nHeight - pindexRescan->nHeight, pindexRescan->nHeight);
        nStart = GetTimeMillis();
        pwalletMain->ScanForWalletTransactions(pindexRescan, true);
        qDebug(" rescan      %15"PRI64d"ms\n", GetTimeMillis() - nStart);
    }

    //show wallet
    qDebug() << "show wallet start!" << endl;

    //mapwallet std::map<uint256, CWalletTx> mapWallet
    qDebug() << "show mapwallet : " << endl;
    BOOST_FOREACH(const PAIRTYPE(uint256, CWalletTx)& item, pwalletMain->mapWallet)
    {
        qDebug() << (item.first).ToString().c_str() << " : " << endl;
        qDebug() << (item.second).ToString().c_str() << endl;
    }

    //is mine
    qDebug() << "is mine start! " << endl;
    for(map<uint256, CWalletTx>::const_iterator it = pwalletMain->mapWallet.begin(); it != pwalletMain->mapWallet.end(); it++ )
    {
        const CWalletTx* pwtx = &(*it).second;
        qDebug() << "(pwtx->vout).size() is : " << (pwtx->vout).size() << endl;

        BOOST_FOREACH(const CTxOut& txout, pwtx->vout)
        {
            if(pwalletMain->IsMine(txout))
            {
                qDebug() << "txout is mine : " << endl;
                qDebug() << "txout.nvalue is : " << txout.nValue << endl;
            }
            else
            {
                qDebug() << "txout is not mine : " << endl;
            }
        }
    }

    //balance
    qDebug() << "show balance : " << endl;

    qDebug() << "pwalletMain->GetBalance(): " << pwalletMain->GetBalance() << endl;

    int64 nTotal = 0;
    for(map<uint256, CWalletTx>::const_iterator it = pwalletMain->mapWallet.begin(); it != pwalletMain->mapWallet.end(); it++ )
    {
        const CWalletTx* pcoin = &(*it).second;

        if (pcoin->IsFinal())
        {
            qDebug() << "pcoin->IsFinal() true! " << endl;
        }
        else
        {
            qDebug() << "pcoin->IsFinal() false! " << endl;
        }

        if (pcoin->IsConfirmed())
        {
            qDebug() << "pcoin->IsConfirmed() true! " << endl;
        }
        else
        {
            qDebug() << "pcoin->IsConfirmed() false! " << endl;
        }

        nTotal += pcoin->GetAvailableCredit();
    }
    qDebug() << "nTotal is : " << nTotal << endl;


    //walletMain->CreateTransactio
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


    //wtx.AcceptToMemoryPool()
    if (wtx.AcceptToMemoryPool())
    {
        qDebug() << "wtx.AcceptToMemoryPool() success!" << endl;

        //show mempool
        BOOST_FOREACH(const PAIRTYPE(uint256, CTransaction)& item, mempool.mapTx)
        {
            qDebug() << (item.first).ToString().c_str() << " : " << endl;
            qDebug() << (item.second).ToString().c_str() << endl;
        }

    }
    else
    {
        qDebug() << "wtx.AcceptToMemoryPool() fail!" << endl;
    }









    return a.exec();
}
