/*
    Copyright (C) 2016-2025 zafaco GmbH

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License version 3
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "header.h"
#include "typedef.h"
#include "trace.h"

using namespace std;

class CConnection
{
private:
    int mTls;
    string mTlsSubjectValidation;

    struct sockaddr_in sockinfo_in;
    struct sockaddr_in6 sockinfo_in6;

    SSL_CTX *ctx;
    SSL *ssl;

    void toggleBlocking(bool activate);

    void tlsSetup(bool client);

    static void ssl_ctx_message_handler(
        int write_p, int version, int content_type, const void *buf, size_t len, SSL *ssl,
        void *arg);

    int tlsConnect();

    void tlsPrintError();

public:
    int mSocket;
    static long long mTlsRecordsReceived;

    CConnection();

    virtual ~CConnection();

    int udpSocket(string &interface);

    int udp6Socket(string &interface);

    int udpSocketServer(int &nPort);

    int udpSocketServer(int &nPort, string sIp);

    int udp6SocketServer(int &nPort);

    int udp6SocketServer(int &nPort, string sIp);

    int tcpSocket(string &interface, string &sServer, int &nPort);

    int tcpSocket(string &interface, string &sServer, int &nPort, int nTls, string sTlsSubjectValidation);

    int tcp6Socket(string &interface, string &sServer, int &nPort);

    int tcp6Socket(string &interface, string &sServer, int &nPort, int nTls, string sTlsSubjectValidation);

    int tcpSocketServer(int &nPort);

    int tcpSocketServer(int &nPort, string sIp);

    int tcp6SocketServer(int &nPort);

    int tcp6SocketServer(int &nPort, string sIp);

    void setBlocking();

    void setNonBlocking();

    int tlsServe();

    int send(const void *buf, int num, int flags);

    int receive(void *buf, int num, int flags);

    int close();
};

#endif
