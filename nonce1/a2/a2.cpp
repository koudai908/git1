#include "dialog.h"
#include <QApplication>

#include <QDebug>

#include "db.h"
#include "walletdb.h"
#include "bitcoinrpc.h"
#include "net.h"
#include "init.h"
#include "util.h"
#include "main.h"

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

CBigNum bnProofOfWorkLimit1(~uint256(0) >> 32);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.show();

    qDebug() << "app begin!" << endl;
/*

    //block.nBits    = 0x1d00ffff;
    //unsigned int nBits = 0x1d00ffff;
    unsigned int nBits = 0x1d00ffff;

    CBigNum bnTarget;
    bnTarget.SetCompact(nBits);

    qDebug() << "bnTarget is : " << bnTarget.GetHex().c_str() << endl;
    qDebug() << "bnProofOfWorkLimit is : " << bnProofOfWorkLimit1.GetHex().c_str() << endl;

    // Check range
    if (bnTarget <= 0 || bnTarget > bnProofOfWorkLimit1)
    {
        qDebug("CheckProofOfWork() : nBits below minimum work");
        qDebug() << endl;
        qDebug() << "nBits not pass!" << endl;
        return error("CheckProofOfWork() : nBits below minimum work");
    }
    else
    {
         qDebug() << "nBits pass!" << endl;
    }

*/

    //load wallet 1

    qDebug() << "load wallet 1 start!" << endl;

    bool fFirstRun = true;

    pwalletMain = new CWallet("wallet.dat");
    DBErrors nLoadWalletRet = pwalletMain->LoadWallet(fFirstRun);

    if (nLoadWalletRet == DB_LOAD_OK)
    {
        qDebug() << "pwalletMain->LoadWallet success!" << endl;
    }


    //

    qDebug() << "Genesis block start!" << endl;

    // Genesis block
    const char* pszTimestamp = "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks";
    CTransaction txNew;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CBigNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));

    txNew.vout[0].nValue = 50 * COIN;
    txNew.vout[0].scriptPubKey = CScript() << pwalletMain->vchDefaultKey << OP_CHECKSIG;

    qDebug() << "pwalletMain->vchDefaultKey is : " << endl;
    std::vector<unsigned char> raw = pwalletMain->vchDefaultKey.Raw();
    for(std::vector<unsigned char>::iterator m = raw.begin(); m != raw.end(); m++)
    {
        qDebug() << *m;
    }
    qDebug() << endl;

    qDebug() << "txNew.vout[0].scriptPubKey is : " << txNew.vout[0].scriptPubKey.ToString().c_str() << endl;

    //txNew.vout[0].scriptPubKey is :  045fd294bec35f0c388f2194021517360dcdcd0f2ac2a71aa96f961f95ef2b0f55dc52eba8cd5cc28a29b5f8391d720f8398692c688d50efc0fc8edd60318c433e OP_CHECKSIG

    CScript testscriptPubKey = CScript() << ParseHex("045fd294bec35f0c388f2194021517360dcdcd0f2ac2a71aa96f961f95ef2b0f55dc52eba8cd5cc28a29b5f8391d720f8398692c688d50efc0fc8edd60318c433e") << OP_CHECKSIG;

    qDebug() << "testscriptPubKey is : " << testscriptPubKey.ToString().c_str() << endl;




    CBlock block;
    block.vtx.push_back(txNew);

    block.hashPrevBlock = 0;
    block.hashMerkleRoot = block.BuildMerkleTree();
    block.nVersion = 1;
    block.nTime    = 1231006505;
    block.nBits    = 0x1d00ffff;
    //block.nBits    = 0x1fffffff;
    block.nNonce   = 0;
    //block.nNonce   = 2083236893;


    // Pre-build hash buffers

    qDebug() << "Pre-build hash start!" << endl;

    char pmidstatebuf[32+16]; char* pmidstate = alignup<16>(pmidstatebuf);
    char pdatabuf[128+16];    char* pdata     = alignup<16>(pdatabuf);
    char phash1buf[64+16];    char* phash1    = alignup<16>(phash1buf);

    qDebug() << "FormatHashBuffers start!" << endl;

    FormatHashBuffers(&block, pmidstate, pdata, phash1);

    unsigned int& nBlockTime = *(unsigned int*)(pdata + 64 + 4);
    unsigned int& nBlockBits = *(unsigned int*)(pdata + 64 + 8);
    unsigned int& nBlockNonce = *(unsigned int*)(pdata + 64 + 12);

    // Search
    int64 nStart = GetTime();
    uint256 hashTarget = CBigNum().SetCompact(block.nBits).getuint256();

    qDebug() << "hashTarget is: " << hashTarget.ToString().c_str() << endl;

    uint256 hashbuf[2];
    uint256& hash = *alignup<16>(hashbuf);

    nHPSTimerStart = 0;

    loop
    {
        unsigned int nHashesDone = 0;
        unsigned int nNonceFound;

        // Crypto++ SHA256
        nNonceFound = ScanHash_CryptoPP(pmidstate, pdata + 64, phash1, (char*)&hash, nHashesDone);

        //qDebug() << "after ScanHash_CryptoPP, nNonceFound is: " << nNonceFound << endl;
        //qDebug() << "after ScanHash_CryptoPP, hash is: " << hash.ToString().c_str() << endl;

        // Check if something found

        if (nNonceFound != (unsigned int) -1)
        {
            for (unsigned int i = 0; i < sizeof(hash)/4; i++)
                ((unsigned int*)&hash)[i] = ByteReverse(((unsigned int*)&hash)[i]);

            //qDebug() << "after ByteReverse, hash is: " << hash.ToString().c_str() << endl;


            if (hash <= hashTarget)
            {
                // Found a solution

                qDebug() << "hash <= hashTarget " << endl;

                block.nNonce = ByteReverse(nNonceFound);
                qDebug() << "finally, nNonceFound is: " << nNonceFound << endl;
                qDebug() << "finally, block.nNonce is: " << block.nNonce << endl;

                qDebug() << "finally, hash is: " << hash.ToString().c_str() << endl;
                qDebug() << "finally, block.GetHash() is: " << block.GetHash().ToString().c_str() << endl;

                //assert(hash == pblock->GetHash());
                break;

            }
            else
            {
                //qDebug() << "hash is not <= hashTarget " << endl;
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

    }

    qDebug() << "finally, loop finish! " << endl;



    // debug print
    qDebug() << "new block.GetHash() is: " << block.GetHash().ToString().c_str() << endl;
    qDebug() << "old hashGenesisBlock is: " << hashGenesisBlock.ToString().c_str() << endl;
    qDebug() << "new block.hashMerkleRoot is: " << block.hashMerkleRoot.ToString().c_str() << endl;

    //

    block.print1();
    qDebug() << endl;

    //block.nBits    = 0x1fffffff;
    //hashTarget is:  007fffff00000000000000000000000000000000000000000000000000000000
    //finally, block.nNonce is:  3476488448
    //finally, hash is:  00000ffb87710d7f55b1693236955421c49963731349f51fd278ae5b7946c273
    //finally, block.GetHash() is:  00000ffb87710d7f55b1693236955421c49963731349f51fd278ae5b7946c273
    //new block.GetHash() is:  00000ffb87710d7f55b1693236955421c49963731349f51fd278ae5b7946c273




    return a.exec();
}
