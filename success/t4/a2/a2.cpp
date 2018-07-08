#include "dialog.h"
#include <QApplication>

#include "irc.h"
#include "db.h"
#include "net.h"
#include "init.h"
#include "strlcpy.h"
#include "addrman.h"
#include "ui_interface.h"

#ifdef WIN32
#include <string.h>
#endif

#ifdef USE_UPNP
#include <miniupnpc/miniwget.h>Listen on port success
#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>
#include <miniupnpc/upnperrors.h>
#endif

#include <QDebug>

using namespace std;
using namespace boost;

SOCKET hListenSocket;

bool BindListenPort1(const CService &addrBind, string& strError)
{
    strError = "";
    int nOne = 1;

#ifdef WIN32
    // Initialize Windows Sockets
    WSADATA wsadata;
    int ret = WSAStartup(MAKEWORD(2,2), &wsadata);
    if (ret != NO_ERROR)
    {
        qDebug() << "WSAStartup fail!" << endl;
        strError = strprintf("Error: TCP/IP socket library failed to start (WSAStartup returned error %d)", ret);
        printf("%s\n", strError.c_str());
        return false;
    }
    else
    {
        qDebug() << "WSAStartup success!" << endl;
    }

#endif

    // Create socket for listening for incoming connections
#ifdef USE_IPV6
    struct sockaddr_storage sockaddr;
#else
    struct sockaddr sockaddr;
#endif
    socklen_t len = sizeof(sockaddr);
    if (!addrBind.GetSockAddr((struct sockaddr*)&sockaddr, &len))
    {
        strError = strprintf("Error: bind address family for %s not supported", addrBind.ToString().c_str());
        printf("%s\n", strError.c_str());
        return false;
    }

    hListenSocket = socket(((struct sockaddr*)&sockaddr)->sa_family, SOCK_STREAM, IPPROTO_TCP);

    if (hListenSocket == INVALID_SOCKET)
    {
        qDebug() << "hListenSocket is invalid!" << endl;
        strError = strprintf("Error: Couldn't open socket for incoming connections (socket returned error %d)", WSAGetLastError());
        printf("%s\n", strError.c_str());
        return false;
    }

    if (hListenSocket != INVALID_SOCKET)
    {
        qDebug() << "hListenSocket in bindlistenport is valid!" << endl;
        qDebug() << "construct socket(hListenSocket) success!" << endl;
    }

#ifdef SO_NOSIGPIPE
    // Different way of disabling SIGPIPE on BSD
    setsockopt(hListenSocket, SOL_SOCKET, SO_NOSIGPIPE, (void*)&nOne, sizeof(int));
#endif

#ifndef WIN32
    // Allow binding if the port is still in TIME_WAIT state after
    // the program was closed and restarted.  Not an issue on windows.
    setsockopt(hListenSocket, SOL_SOCKET, SO_REUSEADDR, (void*)&nOne, sizeof(int));
#endif


#ifdef WIN32
    // Set to non-blocking, incoming connections will also inherit this
    if (ioctlsocket(hListenSocket, FIONBIO, (u_long*)&nOne) == SOCKET_ERROR)
#else
    if (fcntl(hListenSocket, F_SETFL, O_NONBLOCK) == SOCKET_ERROR)
#endif
    {
        strError = strprintf("Error: Couldn't set properties on socket for incoming connections (error %d)", WSAGetLastError());
        printf("%s\n", strError.c_str());
        return false;
    }
    else
    {
        qDebug() << "ioctlsocket or fcntl success!" << endl;
    }

#ifdef USE_IPV6
    // some systems don't have IPV6_V6ONLY but are always v6only; others do have the option
    // and enable it by default or not. Try to enable it, if possible.
    if (addrBind.IsIPv6()) {
#ifdef IPV6_V6ONLY
        setsockopt(hListenSocket, IPPROTO_IPV6, IPV6_V6ONLY, (void*)&nOne, sizeof(int));
#endif
#ifdef WIN32
        int nProtLevel = 10 /* PROTECTION_LEVEL_UNRESTRICTED */;
        int nParameterId = 23 /* IPV6_PROTECTION_LEVEl */;
        // this call is allowed to fail
        setsockopt(hListenSocket, IPPROTO_IPV6, nParameterId, (const char*)&nProtLevel, sizeof(int));
#endif
    }
#endif

    if (::bind(hListenSocket, (struct sockaddr*)&sockaddr, len) == SOCKET_ERROR)
    {
        int nErr = WSAGetLastError();
        if (nErr == WSAEADDRINUSE)
            strError = strprintf(_("Unable to bind to %s on this computer. Bitcoin is probably already running."), addrBind.ToString().c_str());
        else
            strError = strprintf(_("Unable to bind to %s on this computer (bind returned error %d, %s)"), addrBind.ToString().c_str(), nErr, strerror(nErr));
        printf("%s\n", strError.c_str());
        return false;
    }
    else
    {
        qDebug() << "bind success!" << endl;
        qDebug() << "Bind to " << addrBind.ToString().c_str() << endl;
    }

    printf("Bound to %s\n", addrBind.ToString().c_str());

    // Listen for incoming connections
    if (listen(hListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        strError = strprintf("Error: Listening for incoming connections failed (listen returned error %d)", WSAGetLastError());
        printf("%s\n", strError.c_str());
        return false;
    }
    else
    {
        qDebug() << "listen success!" << endl;

    }

    /*
    vhListenSocket.push_back(hListenSocket);

    if (!vhListenSocket.empty())
    {
        qDebug() << "vhLisentenSocket is not empty! " << endl;
    }else
    {
        qDebug() << "vhLisentenSocket is empty! " << endl;
     }

    if (addrBind.IsRoutable() && fDiscover)
        AddLocal(addrBind, LOCAL_BIND);

     */

    return true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.show();

    qDebug() << "app begin!" << endl;

    CNetAddr cNetAddr("127.0.0.1");
    //CService cService(cNetAddr, 18333);
    CService cService(cNetAddr, 8333);

    std::string strError;
    bool fBound = BindListenPort1(cService, strError);


    if (fBound)
    {
        qDebug() << "Listen on port success!" << endl;
    }else
    {
        qDebug() << "Listen on port fail!" << endl;
        qDebug() << strError.c_str() << endl;
     }



    if (hListenSocket != INVALID_SOCKET)
    {
        qDebug() << "hListenSocket in main is valid!" << endl;
    }

    while(true)
    {
        qDebug() << "server waiting for connect..." << endl;

        struct sockaddr sockaddr;
        socklen_t len = sizeof(sockaddr);
        SOCKET hSocket = accept(hListenSocket, (struct sockaddr*)&sockaddr, &len);

        CAddress addr;
        CNode* pnode;

        if (hSocket == INVALID_SOCKET)
        {
            qDebug() << "hsocket is invalid!" << endl;
            int nErr = WSAGetLastError();
            if (nErr != WSAEWOULDBLOCK)
                printf("socket error accept failed: %d\n", nErr);


            sleep(10);
            continue;

        }


        if (hSocket != INVALID_SOCKET)
        {
            qDebug() << "hsocket is valid" << endl;
            if (addr.SetSockAddr((const struct sockaddr*)&sockaddr))
            {
                qDebug() << "addr.SetSockAddr success!" << endl;
                qDebug() << "addr is " << addr.ToString().c_str() << endl;

                pnode = new CNode(hSocket, addr, "", true);
                vNodes.push_back(pnode);

            }

        }


        char pchBuf[0x10000];
        int nBytes = recv(pnode->hSocket, pchBuf, sizeof(pchBuf), MSG_DONTWAIT);
        if (nBytes > 0)
        {
            qDebug() << "server receive message success!" << endl;
            qDebug() << "server receive: " << pchBuf << endl;
        }

        break;

    }



    return a.exec();
}
