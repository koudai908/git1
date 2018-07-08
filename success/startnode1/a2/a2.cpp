#include "dialog.h"
#include <QApplication>

#include <QDebug>

#include "net.h"

static const char *strDNSSeed0[][2] = {
    {"127.0.0.1", "127.0.0.1"},
    {"bitcoin.sipa.be", "seed.bitcoin.sipa.be"},
    {"bluematt.me", "dnsseed.bluematt.me"},
    {"dashjr.org", "dnsseed.bitcoin.dashjr.org"},
    {"xf2.org", "bitseed.xf2.org"},
};

unsigned int pnSeed0[] =
{
    0x959bd347, 0xf8de42b2, 0x73bc0518, 0xea6edc50, 0x21b00a4d, 0xc725b43d, 0xd665464d, 0x1a2a770e,
    0x27c93946, 0x65b2fa46, 0xb80ae255, 0x66b3b446, 0xb1877a3e, 0x6ee89e3e, 0xc3175b40, 0x2a01a83c,
    0x95b1363a, 0xa079ad3d, 0xe6ca801f, 0x027f4f4a, 0x34f7f03a, 0xf790f04a, 0x16ca801f, 0x2f4d5e40,
    0x3a4d5e40, 0xc43a322e, 0xc8159753, 0x14d4724c, 0x7919a118, 0xe0bdb34e, 0x68a16b2e, 0xff64b44d,
    0x6099115b, 0x9b57b05b, 0x7bd1b4ad, 0xdf95944f, 0x29d2b73d, 0xafa8db79, 0xe247ba41, 0x24078348,
    0xf722f03c, 0x33567ebc, 0xace64ed4, 0x984d3932, 0xb5f34e55, 0x27b7024d, 0x94579247, 0x8894042e,
    0x9357d34c, 0x1063c24b, 0xcaa228b1, 0xa3c5a8b2, 0x5dc64857, 0xa2c23643, 0xa8369a54, 0x31203077,
    0x00707c5c, 0x09fc0b3a, 0x272e9e2e, 0xf80f043e, 0x9449ca3e, 0x5512c33e, 0xd106b555, 0xe8024157,
    0xe288ec29, 0xc79c5461, 0xafb63932, 0xdb02ab4b, 0x0e512777, 0x8a145a4c, 0xb201ff4f, 0x5e09314b,
    0xcd9bfbcd, 0x1c023765, 0x4394e75c, 0xa728bd4d, 0x65331552, 0xa98420b1, 0x89ecf559, 0x6e80801f,
    0xf404f118, 0xefd62b51, 0x05918346, 0x9b186d5f, 0xacabab46, 0xf912e255, 0xc188ea62, 0xcc55734e,
    0xc668064d, 0xd77a4558, 0x46201c55, 0xf17dfc80, 0xf7142f2e, 0x87bfb718, 0x8aa54fb2, 0xc451d518,
    0xc4ae8831, 0x8dd44d55, 0x5bbd206c, 0x64536b5d, 0x5c667e60, 0x3b064242, 0xfe963a42, 0xa28e6dc8,
    0xe8a9604a, 0xc989464e, 0xd124a659, 0x50065140, 0xa44dfe5e, 0x1079e655, 0x3fb986d5, 0x47895b18,
    0x7d3ce4ad, 0x4561ba50, 0x296eec62, 0x255b41ad, 0xaed35ec9, 0x55556f12, 0xc7d3154d, 0x3297b65d,
    0x8930121f, 0xabf42e4e, 0x4a29e044, 0x1212685d, 0x676c1e40, 0xce009744, 0x383a8948, 0xa2dbd0ad,
    0xecc2564d, 0x07dbc252, 0x887ee24b, 0x5171644c, 0x6bb798c1, 0x847f495d, 0x4cbb7145, 0x3bb81c32,
    0x45eb262e, 0xc8015a4e, 0x250a361b, 0xf694f946, 0xd64a183e, 0xd4f1dd59, 0x8f20ffd4, 0x51d9e55c,
    0x09521763, 0x5e02002e, 0x32c8074d, 0xe685762e, 0x8290b0bc, 0x762a922e, 0xfc5ee754, 0x83a24829,
    0x775b224d, 0x6295bb4d, 0x38ec0555, 0xbffbba50, 0xe5560260, 0x86b16a7c, 0xd372234e, 0x49a3c24b,
    0x2f6a171f, 0x4d75ed60, 0xae94115b, 0xcb543744, 0x63080c59, 0x3f9c724c, 0xc977ce18, 0x532efb18,
    0x69dc3b2e, 0x5f94d929, 0x1732bb4d, 0x9c814b4d, 0xe6b3762e, 0xc024f662, 0x8face35b, 0x6b5b044d,
    0x798c7b57, 0x79a6b44c, 0x067d3057, 0xf9e94e5f, 0x91cbe15b, 0x71405eb2, 0x2662234e, 0xcbcc4a6d,
    0xbf69d54b, 0xa79b4e55, 0xec6d3e51, 0x7c0b3c02, 0x60f83653, 0x24c1e15c, 0x1110b62e, 0x10350f59,
    0xa56f1d55, 0x3509e7a9, 0xeb128354, 0x14268e2e, 0x934e28bc, 0x8e32692e, 0x8331a21f, 0x3e633932,
    0xc812b12e, 0xc684bf2e, 0x80112d2e, 0xe0ddc96c, 0xc630ca4a, 0x5c09b3b2, 0x0b580518, 0xc8e9d54b,
    0xd169aa43, 0x17d0d655, 0x1d029963, 0x7ff87559, 0xcb701f1f, 0x6fa3e85d, 0xe45e9a54, 0xf05d1802,
    0x44d03b2e, 0x837b692e, 0xccd4354e, 0x3d6da13c, 0x3423084d, 0xf707c34a, 0x55f6db3a, 0xad26e442,
    0x6233a21f, 0x09e80e59, 0x8caeb54d, 0xbe870941, 0xb407d20e, 0x20b51018, 0x56fb152e, 0x460d2a4e,
    0xbb9a2946, 0x560eb12e, 0xed83dd29, 0xd6724f53, 0xa50aafb8, 0x451346d9, 0x88348e2e, 0x7312fead,
    0x8ecaf96f, 0x1bda4e5f, 0xf1671e40, 0x3c8c3e3b, 0x4716324d, 0xdde24ede, 0xf98cd17d, 0xa91d4644,
    0x28124eb2, 0x147d5129, 0xd022042e, 0x61733d3b, 0xad0d5e02, 0x8ce2932e, 0xe5c18502, 0x549c1e32,
    0x9685801f, 0x86e217ad, 0xd948214b, 0x4110f462, 0x3a2e894e, 0xbd35492e, 0x87e0d558, 0x64b8ef7d,
    0x7c3eb962, 0x72a84b3e, 0x7cd667c9, 0x28370a2e, 0x4bc60e7b, 0x6fc1ec60, 0x14a6983f, 0x86739a4b,
    0x46954e5f, 0x32e2e15c, 0x2e9326cf, 0xe5801c5e, 0x379607b2, 0x32151145, 0xf0e39744, 0xacb54c55,
    0xa37dfb60, 0x83b55cc9, 0x388f7ca5, 0x15034f5f, 0x3e94965b, 0x68e0ffad, 0x35280f59, 0x8fe190cf,
    0x7c6ba5b2, 0xa5e9db43, 0x4ee1fc60, 0xd9d94e5f, 0x04040677, 0x0ea9b35e, 0x5961f14f, 0x67fda063,
    0xa48a5a31, 0xc6524e55, 0x283d325e, 0x3f37515f, 0x96b94b3e, 0xacce620e, 0x6481cc5b, 0xa4a06d4b,
    0x9e95d2d9, 0xe40c03d5, 0xc2f4514b, 0xb79aad44, 0xf64be843, 0xb2064070, 0xfca00455, 0x429dfa4e,
    0x2323f173, 0xeda4185e, 0xabd5227d, 0x9efd4d58, 0xb1104758, 0x4811e955, 0xbd9ab355, 0xe921f44b,
    0x9f166dce, 0x09e279b2, 0xe0c9ac7b, 0x7901a5ad, 0xa145d4b0, 0x79104671, 0xec31e35a, 0x4fe0b555,
    0xc7d9cbad, 0xad057f55, 0xe94cc759, 0x7fe0b043, 0xe4529f2e, 0x0d4dd4b2, 0x9f11a54d, 0x031e2e4e,
    0xe6014f5f, 0x11d1ca6c, 0x26bd7f61, 0xeb86854f, 0x4d347b57, 0x116bbe2e, 0xdba7234e, 0x7bcbfd2e,
    0x174dd4b2, 0x6686762e, 0xb089ba50, 0xc6258246, 0x087e767b, 0xc4a8cb4a, 0x595dba50, 0x7f0ae502,
    0x7b1dbd5a, 0xa0603492, 0x57d1af4b, 0x9e21ffd4, 0x6393064d, 0x7407376e, 0xe484762e, 0x122a4e53,
    0x4a37aa43, 0x3888a6be, 0xee77864e, 0x039c8dd5, 0x688d89af, 0x0e988f62, 0x08218246, 0xfc2f8246,
    0xd1d97040, 0xd64cd4b2, 0x5ae4a6b8, 0x7d0de9bc, 0x8d304d61, 0x06c5c672, 0xa4c8bd4d, 0xe0fd373b,
    0x575ebe4d, 0x72d26277, 0x55570f55, 0x77b154d9, 0xe214293a, 0xfc740f4b, 0xfe3f6a57, 0xa9c55f02,
    0xae4054db, 0x2394d918, 0xb511b24a, 0xb8741ab2, 0x0758e65e, 0xc7b5795b, 0xb0a30a4c, 0xaf7f170c,
    0xf3b4762e, 0x8179576d, 0x738a1581, 0x4b95b64c, 0x9829b618, 0x1bea932e, 0x7bdeaa4b, 0xcb5e0281,
    0x65618f54, 0x0658474b, 0x27066acf, 0x40556d65, 0x7d204d53, 0xf28bc244, 0xdce23455, 0xadc0ff54,
    0x3863c948, 0xcee34e5f, 0xdeb85e02, 0x2ed17a61, 0x6a7b094d, 0x7f0cfc40, 0x59603f54, 0x3220afbc,
    0xb5dfd962, 0x125d21c0, 0x13f8d243, 0xacfefb4e, 0x86c2c147, 0x3d8bbd59, 0xbd02a21f, 0x2593042e,
    0xc6a17a7c, 0x28925861, 0xb487ed44, 0xb5f4fd6d, 0x90c28a45, 0x5a14f74d, 0x43d71b4c, 0x728ebb5d,
    0x885bf950, 0x08134dd0, 0x38ec046e, 0xc575684b, 0x50082d2e, 0xa2f47757, 0x270f86ae, 0xf3ff6462,
    0x10ed3f4e, 0x4b58d462, 0xe01ce23e, 0x8c5b092e, 0x63e52f4e, 0x22c1e85d, 0xa908f54e, 0x8591624f,
    0x2c0fb94e, 0xa280ba3c, 0xb6f41b4c, 0x24f9aa47, 0x27201647, 0x3a3ea6dc, 0xa14fc3be, 0x3c34bdd5,
    0x5b8d4f5b, 0xaadeaf4b, 0xc71cab50, 0x15697a4c, 0x9a1a734c, 0x2a037d81, 0x2590bd59, 0x48ec2741,
    0x53489c5b, 0x7f00314b, 0x2170d362, 0xf2e92542, 0x42c10b44, 0x98f0f118, 0x883a3456, 0x099a932e,
    0xea38f7bc, 0x644e9247, 0xbb61b62e, 0x30e0863d, 0x5f51be54, 0x207215c7, 0x5f306c45, 0xaa7f3932,
    0x98da7d45, 0x4e339b59, 0x2e411581, 0xa808f618, 0xad2c0c59, 0x54476741, 0x09e99fd1, 0x5db8f752,
    0xc16df8bd, 0x1dd4b44f, 0x106edf2e, 0x9e15c180, 0x2ad6b56f, 0x633a5332, 0xff33787c, 0x077cb545,
    0x6610be6d, 0x75aad2c4, 0x72fb4d5b, 0xe81e0f59, 0x576f6332, 0x47333373, 0x351ed783, 0x2d90fb50,
    0x8d5e0f6c, 0x5b27a552, 0xdb293ebb, 0xe55ef950, 0x4b133ad8, 0x75df975a, 0x7b6a8740, 0xa899464b,
    0xfab15161, 0x10f8b64d, 0xd055ea4d, 0xee8e146b, 0x4b14afb8, 0x4bc1c44a, 0x9b961dcc, 0xd111ff43,
    0xfca0b745, 0xc800e412, 0x0afad9d1, 0xf751c350, 0xf9f0cccf, 0xa290a545, 0x8ef13763, 0x7ec70d59,
    0x2b066acf, 0x65496c45, 0xade02c1b, 0xae6eb077, 0x92c1e65b, 0xc064e6a9, 0xc649e56d, 0x5287a243,
    0x36de4f5b, 0x5b1df6ad, 0x65c39a59, 0xdba805b2, 0x20067aa8, 0x6457e56d, 0x3cee26cf, 0xfd3ff26d,
    0x04f86d4a, 0x06b8e048, 0xa93bcd5c, 0x91135852, 0xbe90a643, 0x8fa0094d, 0x06d8215f, 0x2677094d,
    0xd735685c, 0x164a00c9, 0x5209ac5f, 0xa9564c5c, 0x3b504f5f, 0xcc826bd0, 0x4615042e, 0x5fe13b4a,
    0x8c81b86d, 0x879ab68c, 0x1de564b8, 0x434487d8, 0x2dcb1b63, 0x82ab524a, 0xb0676abb, 0xa13d9c62,
    0xdbb5b86d, 0x5b7f4b59, 0xaddfb44d, 0xad773532, 0x3997054c, 0x72cebd89, 0xb194544c, 0xc5b8046e,
    0x6e1adeb2, 0xaa5abb51, 0xefb54b44, 0x15efc54f, 0xe9f1bc4d, 0x5f401b6c, 0x97f018ad, 0xc82f9252,
    0x2cdc762e, 0x8e52e56d, 0x1827175e, 0x9b7d7d80, 0xb2ad6845, 0x51065140, 0x71180a18, 0x5b27006c,
    0x0621e255, 0x721cbe58, 0x670c0cb8, 0xf8bd715d, 0xe0bdc5d9, 0xed843501, 0x4b84554d, 0x7f1a18bc,
    0x53bcaf47, 0x5729d35f, 0xf0dda246, 0x22382bd0, 0x4d641fb0, 0x316afcde, 0x50a22f1f, 0x73608046,
    0xc461d84a, 0xb2dbe247,
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.show();

    qDebug() << "app begin!" << endl;

    /*
    ThreadOpenConnections2(NULL);

    // Connect to a point
    {
        string strAddr0 = "127.0.0.1";
        CAddress addr0;
        if(OpenNetworkConnection(addr0, NULL, strAddr0.c_str()))
        {
            qDebug() << "Connect to a point success!" << endl;
        }
        else
        {
            qDebug() << "Connect to a point fail!" << endl;
        }

    }

    // Add seed nodes if IRC isn't working
    // Add seed nodes from pnSeed
    {
        qDebug() << "Add seed nodes from pnSeed start!" << endl;
        std::vector<CAddress> vAdd;
        for (unsigned int i = 0; i < ARRAYLEN(pnSeed0); i++)
        {
            // It'll only connect to one or two seed nodes because once it connects,
            // it'll get a pile of addresses with newer timestamps.
            // Seed nodes are given a random 'last seen time' of between one and two
            // weeks ago.
            const int64 nOneWeek = 7*24*60*60;
            struct in_addr ip;
            memcpy(&ip, &pnSeed0[i], sizeof(ip));
            CAddress addr(CService(ip, GetDefaultPort()));

            qDebug() << "CAddress is : " << addr.ToString().c_str() << endl;

            addr.nTime = GetTime()-GetRand(nOneWeek)-nOneWeek;
            vAdd.push_back(addr);
        }

        qDebug() << "from pnSeed, vAdd is : " << endl;
        BOOST_FOREACH(CAddress& addr, vAdd)
        {
            qDebug() << "CAddress is : " << addr.ToString().c_str() << endl;
        }
        qDebug() << "from pnSeed, vAdd.size() is : " << vAdd.size() << endl;

        //addrman.Add(vAdd, CNetAddr("127.0.0.1"));
    }


    //ThreadDNSAddressSeed2
    {
        qDebug("Loading addresses from DNS seeds (could take a while)\n");

        std::vector<CNetAddr> vaddr;
        std::vector<CAddress> vAdd;
        if (LookupHost(strDNSSeed0[0][1], vaddr))
        {
            BOOST_FOREACH(CNetAddr& ip, vaddr)
            {
                int nOneDay = 24*3600;
                CAddress addr = CAddress(CService(ip, GetDefaultPort()));

                qDebug() << "CAddress is : " << addr.ToString().c_str() << endl;

                addr.nTime = GetTime() - 3*nOneDay - GetRand(4*nOneDay); // use a random age between 3 and 7 days old
                vAdd.push_back(addr);

            }

            qDebug() << "from pseed.bitcoin.sipa.be, vAdd is : " << endl;
            BOOST_FOREACH(CAddress& addr, vAdd)
            {
                qDebug() << "CAddress is : " << addr.ToString().c_str() << endl;
            }
            qDebug() << "from seed.bitcoin.sipa.be, vAdd.size() is : " << vAdd.size() << endl;

        }


        if(addrman.Add(vAdd, CNetAddr(strDNSSeed0[0][0], true)))
        {
            qDebug() << "addrman.Add success! " << endl;
        }
        else
        {
            qDebug() << "addrman.Add fail! " << endl;
        }

    }
    */

    //addrman add a point
    {
        qDebug("Loading addresses from DNS seeds (could take a while)\n");

        std::vector<CNetAddr> vaddr;
        std::vector<CAddress> vAdd;
        if (LookupHost(strDNSSeed0[0][1], vaddr))
        {
            qDebug() << "LookupHost success! " << endl;

            qDebug() << "from 127.0.0.1, vAddr is : " << endl;
            BOOST_FOREACH(CNetAddr& ip, vaddr)
            {

                qDebug() << "CNetAddr is : " << ip.ToString().c_str() << endl;
            }


            BOOST_FOREACH(CNetAddr& ip, vaddr)
            {
                int nOneDay = 24*3600;
                CAddress addr = CAddress(CService(ip, GetDefaultPort()));

                qDebug() << "CAddress is : " << addr.ToString().c_str() << endl;

                if (addr.IsRoutable())
                {
                    qDebug() << "addr.IsRoutable() true!" << endl;
                }
                else
                {
                    qDebug() << "addr.IsRoutable() false!"  << endl;
                }



                addr.nTime = GetTime() - 3*nOneDay - GetRand(4*nOneDay); // use a random age between 3 and 7 days old
                vAdd.push_back(addr);

            }

        }

        qDebug() << "from 127.0.0.1, vAdd is : " << endl;
        BOOST_FOREACH(CAddress& addr, vAdd)
        {
            qDebug() << "CAddress is : " << addr.ToString().c_str() << endl;
        }
        qDebug() << "127.0.0.1, vAdd.size() is : " << vAdd.size() << endl;




        if(addrman.Add(vAdd, CNetAddr(strDNSSeed0[0][0], true)))
        {
            qDebug() << "addrman.Add success! " << endl;
        }
        else
        {
            qDebug() << "addrman.Add fail! " << endl;
        }


    }



    return a.exec();
}

