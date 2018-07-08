#include "dialog.h"
#include <QApplication>

#include <QDebug>
#include "main.h"

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

    CTxDB txdb("cr+");
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
        /*
        if(mempool.addUnchecked(txNew.GetHash(), txNew))
        {
             qDebug() << "mempool.addUnchecked(txNew.GetHash(), txNew) success!" << endl;
        }
        else
        {
             qDebug() << "mempool.addUnchecked(txNew.GetHash(), txNew) fail!" << endl;
        }
        */

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

        //genesis block to UpdateTxIndex


        unsigned int nTxPos;
        nTxPos = nBlockPos + ::GetSerializeSize(CBlock(), SER_DISK, CLIENT_VERSION) - 1 + GetSizeOfCompactSize(block.vtx.size());

        map<uint256, CTxIndex> mapQueuedChanges;

        BOOST_FOREACH(CTransaction& tx, block.vtx)
        {
            uint256 hashTx = tx.GetHash();

            CDiskTxPos posThisTx(nFile, nBlockPos, nTxPos);

            mapQueuedChanges[hashTx] = CTxIndex(posThisTx, tx.vout.size());

            qDebug() << "posThisTx is : " << posThisTx.ToString().c_str() << endl;

            //nTxPos += ::GetSerializeSize(tx, SER_DISK, CLIENT_VERSION);
        }

        // Write queued txindex changes
        for (map<uint256, CTxIndex>::iterator mi = mapQueuedChanges.begin(); mi != mapQueuedChanges.end(); ++mi)
        {
            if (txdb.UpdateTxIndex((*mi).first, (*mi).second))
            {
                qDebug() << "txdb.UpdateTxIndex success!" << endl;
            }
            else
            {
                qDebug() << "txdb.UpdateTxIndex fail!" << endl;
            }
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

    //mempool read tx
    for (unsigned int i = 0; i < wtx.vin.size(); i++)
    {
        COutPoint prevout = wtx.vin[i].prevout;

        qDebug() << "preout.hash is : " << prevout.hash.ToString().c_str() << endl;

        // Read txindex
        CTxDB txdb("r");
        CTxIndex txindex;
        bool fFound;

        fFound = txdb.ReadTxIndex(prevout.hash, txindex);

        if (fFound)
        {
            qDebug() << "fFound true! pass! " << endl;
            qDebug() << "txindex.pos is : " << txindex.pos.ToString().c_str() << endl;
        }
        else
        {
            qDebug() << "fFound false! reject! " << endl;
        }

        // Read txPrev
        CTransaction txPrev;

        if (txPrev.ReadFromDisk(txindex.pos))
        {
            qDebug() << "txPrev.ReadFromDisk(txindex.pos) true! pass! " << endl;
            qDebug() << "txPrev is : " << endl << txPrev.ToString().c_str() << endl;

        }
        else
        {
            qDebug() << "txPrev.ReadFromDisk(txindex.pos) false! reject! " << endl;
        }

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

    //bitcoinminer

    qDebug("BitcoinMiner started\n");


    // Each thread has its own key and counter
    CReserveKey reservekey(pwalletMain);
    unsigned int nExtraNonce = 0;

    //
    // Create new block
    //
    unsigned int nTransactionsUpdatedLast = nTransactionsUpdated;
    CBlockIndex* pindexPrev = pindexBest;

    auto_ptr<CBlock> pblock(CreateNewBlock(reservekey));
    if (pblock.get())
    {
        qDebug() << "pblock.get() true! pass! " << endl;
        qDebug() << "pblock.get() is : " << endl;
        (pblock.get())->print1();
    }
    else
    {
        qDebug() << "pblock.get() false! reject! " << endl;
    }
    IncrementExtraNonce(pblock.get(), pindexPrev, nExtraNonce);

    qDebug("Running BitcoinMiner with %"PRIszu" transactions in block (%u bytes)\n", pblock->vtx.size(),
           ::GetSerializeSize(*pblock, SER_NETWORK, PROTOCOL_VERSION));


    //
    // Pre-build hash buffers
    //
    char pmidstatebuf[32+16]; char* pmidstate = alignup<16>(pmidstatebuf);
    char pdatabuf[128+16];    char* pdata     = alignup<16>(pdatabuf);
    char phash1buf[64+16];    char* phash1    = alignup<16>(phash1buf);

    FormatHashBuffers(pblock.get(), pmidstate, pdata, phash1);

    unsigned int& nBlockTime = *(unsigned int*)(pdata + 64 + 4);
    unsigned int& nBlockBits = *(unsigned int*)(pdata + 64 + 8);
    unsigned int& nBlockNonce = *(unsigned int*)(pdata + 64 + 12);

    // Search
    //
    nStart = GetTime();
    uint256 hashTarget = CBigNum().SetCompact(pblock->nBits).getuint256();
    uint256 hashbuf[2];
    uint256& hash = *alignup<16>(hashbuf);
    loop
    {
        unsigned int nHashesDone = 0;
        unsigned int nNonceFound;

        // Crypto++ SHA256
        nNonceFound = ScanHash_CryptoPP(pmidstate, pdata + 64, phash1,
                                        (char*)&hash, nHashesDone);

        // Check if something found
        if (nNonceFound != (unsigned int) -1)
        {
            for (unsigned int i = 0; i < sizeof(hash)/4; i++)
                ((unsigned int*)&hash)[i] = ByteReverse(((unsigned int*)&hash)[i]);

            if (hash <= hashTarget)
            {
                // Found a solution
                pblock->nNonce = ByteReverse(nNonceFound);
                assert(hash == pblock->GetHash());

                qDebug() << "CheckWork will start! " << endl;
                if(CheckWork(pblock.get(), *pwalletMain, reservekey))
                {
                    qDebug() << "CheckWork success! " << endl;
                }
                else
                {
                    qDebug() << "CheckWork fail! " << endl;
                }


                break;
            }
        }

        // Meter hashes/sec
        static int64 nHashCounter;
        if (nHPSTimerStart == 0)
        {
            nHPSTimerStart = GetTimeMillis();
            nHashCounter = 0;
        }
        else
            nHashCounter += nHashesDone;
        if (GetTimeMillis() - nHPSTimerStart > 4000)
        {
            static CCriticalSection cs;
            {
                LOCK(cs);
                if (GetTimeMillis() - nHPSTimerStart > 4000)
                {
                    dHashesPerSec = 1000.0 * nHashCounter / (GetTimeMillis() - nHPSTimerStart);
                    nHPSTimerStart = GetTimeMillis();
                    nHashCounter = 0;
                    static int64 nLogTime;
                    if (GetTime() - nLogTime > 30 * 60)
                    {
                        nLogTime = GetTime();
                        qDebug("hashmeter %3d CPUs %6.0f khash/s\n", vnThreadsRunning[THREAD_MINER], dHashesPerSec/1000.0);
                    }
                }
            }
        }

        // Check for stop or if block needs to be rebuilt


        // Update nTime every few seconds
        pblock->UpdateTime(pindexPrev);
        nBlockTime = ByteReverse(pblock->nTime);

    }

    //mapBlockIndex 2
    qDebug() << "show mapBlockIndex!" << endl;

    BOOST_FOREACH(const PAIRTYPE(uint256, CBlockIndex*)& item, mapBlockIndex)
    {
        qDebug() << (item.first).ToString().c_str() << " : ";
        qDebug() << item.second << endl;
        qDebug() << item.second->ToString().c_str() << endl;

        CBlock tmpblock;
        tmpblock.ReadFromDisk(item.second->nFile, item.second->nBlockPos);
        tmpblock.print1();

    }










    return a.exec();
}
