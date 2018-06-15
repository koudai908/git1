#include "dialog.h"
#include <QApplication>

#include <QDebug>

#include "net.h"

CNode* ConnectNode1(CAddress addrConnect, const char *strDest = NULL, int64 nTimeout=0);


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.show();

    qDebug() << "app begin!" << endl;

    CService addrConnect("127.0.0.1", 18333);


    CAddress cAddr(addrConnect, NODE_NETWORK);

    CNode* pnode = ConnectNode1(cAddr);



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

CNode* ConnectNode1(CAddress addrConnect, const char *pszDest, int64 nTimeout)
{
    
    // Connect
    SOCKET hSocket;
    if (ConnectSocket(addrConnect, hSocket))
    {

        // debug print
        qDebug() << "connected " << addrConnect.ToString().c_str() << endl;


        // Add node
        CNode* pnode = new CNode(hSocket, addrConnect, "", false);
        if (nTimeout != 0)
            pnode->AddRef(nTimeout);
        else
            pnode->AddRef();

        {
            LOCK(cs_vNodes);
            vNodes.push_back(pnode);
        }

        pnode->nTimeConnected = GetTime();
        return pnode;
    }
    else
    {
        qDebug() << "connected fail!" << endl;
        return NULL;
    }
}

