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

    qDebug() << "before genesis block, pindexGenesisBlock is: " << pindexGenesisBlock << endl;


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
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
        CBlock block;
        block.vtx.push_back(txNew);
        block.hashPrevBlock = 0;
        block.hashMerkleRoot = block.BuildMerkleTree();
        block.nVersion = 1;
        block.nTime    = 1231006505;
        block.nBits    = 0x1d00ffff;
        block.nNonce   = 2083236893;

        fPrintToConsole = true;

        // debug print
        qDebug() << "block.GetHash() is: " << block.GetHash().ToString().c_str() << endl;
        qDebug() << "hashGenesisBlock is: " << hashGenesisBlock.ToString().c_str() << endl;
        qDebug() << "block.hashMerkleRoot is: " << block.hashMerkleRoot.ToString().c_str() << endl;


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
    }

    qDebug() << "after genesis block, pindexGenesisBlock is: " << pindexGenesisBlock << endl;


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


    RegisterWallet(pwalletMain);


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










    return a.exec();
}
