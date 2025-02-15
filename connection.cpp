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

#include "connection.h"

long long CConnection::mTlsRecordsReceived = 0;

CConnection::CConnection()
{
    mSocket = 0;
    mTls = 0;
    mTlsSubjectValidation = "";
}

CConnection::~CConnection()
{
    close();
}

int CConnection::udpSocket(string &interface)
{

    mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (mSocket == -1)
    {
        TRC_ERR("Error [udpSocket]: Could not create socket");
        return -1;
    }

    memset(&sockinfo_in, 0, sizeof(sockinfo_in));
    sockinfo_in.sin_family = AF_INET;
    sockinfo_in.sin_addr.s_addr = inet_addr(interface.c_str());

    if (::bind(mSocket, (struct sockaddr *)&sockinfo_in, sizeof(sockinfo_in)) == -1)
    {
        TRC_ERR("Error [udpSocket]: Could not bind socket to interface");
        return -1;
    }

    return mSocket;
}

int CConnection::udp6Socket(string &interface)
{
    int no = 0;

    mSocket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (mSocket == -1)
    {
        TRC_ERR("Error [udp6Socket]: Could not create socket");
        return -1;
    }

    memset(&sockinfo_in6, 0, sizeof(sockinfo_in6));
    sockinfo_in6.sin6_flowinfo = 0;
    sockinfo_in6.sin6_family = AF_INET6;

    setsockopt(mSocket, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&no, sizeof(no));

    (void)inet_pton(AF_INET6, interface.c_str(), sockinfo_in6.sin6_addr.s6_addr);

    if (::bind(mSocket, (struct sockaddr *)&sockinfo_in6, sizeof(sockinfo_in6)) == -1)
    {
        TRC_ERR("Error [udp6Socket]: Could not bind socket to interface");
        return -1;
    }

    return mSocket;
}

int CConnection::udpSocketServer(int &nPort)
{
    return udpSocketServer(nPort, "");
}

int CConnection::udpSocketServer(int &nPort, string sIp)
{

    mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (mSocket == -1)
    {
        TRC_ERR("Error [udpSocket]: Could not create socket");
        return -1;
    }

    memset(&sockinfo_in, 0, sizeof(sockinfo_in));
    sockinfo_in.sin_family = AF_INET;
    sockinfo_in.sin_addr.s_addr = htonl(INADDR_ANY);

    if (nPort != 0)
    {
        sockinfo_in.sin_port = htons(nPort);
    }
    if (sIp.compare("") != 0)
    {
        sockinfo_in.sin_addr.s_addr = inet_addr(sIp.c_str());
    }

    if (::bind(mSocket, (struct sockaddr *)&sockinfo_in, sizeof(sockinfo_in)) == -1)
    {
        TRC_ERR("Error [udpSocket]: Could not bind socket to interface");
        return -1;
    }

    return mSocket;
}

int CConnection::udp6SocketServer(int &nPort)
{
    return udp6SocketServer(nPort, "");
}

int CConnection::udp6SocketServer(int &nPort, string sIp)
{
    int no = 0;

    mSocket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (mSocket == -1)
    {
        TRC_ERR("Error [udp6Socket]: Could not create socket");
        return -1;
    }

    memset(&sockinfo_in6, 0, sizeof(sockinfo_in6));
    sockinfo_in6.sin6_flowinfo = 0;
    sockinfo_in6.sin6_family = AF_INET6;
    sockinfo_in6.sin6_port = htons(nPort);

    if (sIp.compare("") != 0)
    {
        (void)inet_pton(AF_INET6, sIp.c_str(), sockinfo_in6.sin6_addr.s6_addr);
    }

    setsockopt(mSocket, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&no, sizeof(no));

    if (::bind(mSocket, (struct sockaddr *)&sockinfo_in6, sizeof(sockinfo_in6)) == -1)
    {
        TRC_ERR("Error [udp6Socket]: Could not bind socket to interface");
        return -1;
    }

    return mSocket;
}

int CConnection::tcpSocket(string &interface, string &sServer, int &nPort)
{
    return tcpSocket(interface, sServer, nPort, 0, "");
}

int CConnection::tcpSocket(
    string &interface, string &sServer, int &nPort, int nTls, string sTlsSubjectValidation)
{
    int flags = 0;

    timeval tv;
    tv.tv_sec = 15;
    tv.tv_usec = 0;

    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket == -1)
    {
        TRC_ERR("Error [tcpSocket]: Could not create socket");
        return -1;
    }

    mTls = nTls;
    mTlsSubjectValidation = sTlsSubjectValidation;

    memset(&sockinfo_in, 0, sizeof(sockinfo_in));
    sockinfo_in.sin_family = AF_INET;
    sockinfo_in.sin_addr.s_addr = inet_addr(interface.c_str());

    if (::bind(mSocket, (struct sockaddr *)&sockinfo_in, sizeof(sockinfo_in)) == -1)
    {
        TRC_ERR("Error [tcpSocket]: Could not bind socket to interface");
        return -1;
    }

    memset(&sockinfo_in, 0, sizeof(sockinfo_in));
    sockinfo_in.sin_family = AF_INET;
    sockinfo_in.sin_addr.s_addr = inet_addr(sServer.c_str());
    sockinfo_in.sin_port = htons(nPort);

    fcntl(mSocket, F_SETFL, flags | O_NONBLOCK);

    connect(mSocket, (sockaddr *)&sockinfo_in, sizeof(sockinfo_in));

    TRC_DEBUG(("Connection from: " + interface + " to: " + sServer + ":" + CTool::toString(nPort)).c_str());

    fcntl(mSocket, F_SETFL, flags & (~O_NONBLOCK));

    fd_set set;
    FD_ZERO(&set);
    FD_SET(mSocket, &set);

    if (select(mSocket + 1, NULL, &set, NULL, &tv) == -1)
    {
        TRC_ERR("Error [tcpSocket]: Creating socket failed - Could not connect to Server");
        return -1;
    }

    setsockopt(mSocket, SOL_SOCKET, SO_SNDTIMEO, (timeval *)&tv, sizeof(timeval));
    setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, (timeval *)&tv, sizeof(timeval));

    if (mTls && tlsConnect() != 0)
    {
        return -1;
    }

    return mSocket;
}

int CConnection::tcp6Socket(string &interface, string &sServer, int &nPort)
{
    return tcp6Socket(interface, sServer, nPort, 0, "");
}

int CConnection::tcp6Socket(
    string &interface, string &sServer, int &nPort, int nTls, string sTlsSubjectValidation)
{
    int on = 1;
    int no = 0;
    int flags = 0;

    timeval tv;
    tv.tv_sec = 15;
    tv.tv_usec = 0;

    mSocket = socket(AF_INET6, SOCK_STREAM, 0);
    if (mSocket == -1)
    {
        TRC_ERR("Error [tcp6Socket]: Could not create socket");
        return -1;
    }

    mTls = nTls;
    mTlsSubjectValidation = sTlsSubjectValidation;

    memset(&sockinfo_in6, 0, sizeof(sockinfo_in6));
    sockinfo_in6.sin6_flowinfo = 0;
    sockinfo_in6.sin6_family = AF_INET6;
    (void)inet_pton(AF_INET6, interface.c_str(), sockinfo_in6.sin6_addr.s6_addr);

    setsockopt(mSocket, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&no, sizeof(no));
    setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on));

    if (::bind(mSocket, (struct sockaddr *)&sockinfo_in6, sizeof(sockinfo_in6)) == -1)
    {
        TRC_ERR("Error [tcp6Socket]: Could not bind socket to interface");
        return -1;
    }

    memset(&sockinfo_in6, 0, sizeof(sockinfo_in6));
    sockinfo_in6.sin6_family = AF_INET6;
    sockinfo_in6.sin6_port = htons(nPort);
    (void)inet_pton(AF_INET6, sServer.c_str(), sockinfo_in6.sin6_addr.s6_addr);

    fcntl(mSocket, F_SETFL, flags | O_NONBLOCK);

    connect(mSocket, (sockaddr *)&sockinfo_in6, sizeof(sockinfo_in6));

    TRC_DEBUG(("Connection from: " + interface + " to: " + sServer + ":" + CTool::toString(nPort)).c_str());

    fcntl(mSocket, F_SETFL, flags & (~O_NONBLOCK));

    fd_set set;
    FD_ZERO(&set);
    FD_SET(mSocket, &set);

    if (select(mSocket + 1, NULL, &set, NULL, &tv) == -1)
    {
        TRC_ERR("Error [tcp6Socket]: Creating socket failed - Could not connect to Server");
        return -1;
    }

    setsockopt(mSocket, SOL_SOCKET, SO_SNDTIMEO, (timeval *)&tv, sizeof(timeval));
    setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, (timeval *)&tv, sizeof(timeval));

    if (mTls && tlsConnect() != 0)
    {
        return -1;
    }

    return mSocket;
}

int CConnection::tcpSocketServer(int &nPort)
{
    return tcpSocketServer(nPort, "");
}

int CConnection::tcpSocketServer(int &nPort, string sIp)
{
    int on = 1;

    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket == -1)
    {
        TRC_ERR("Error [tcpSocketServer]: Could not create socket");
        return -1;
    }

    memset(&sockinfo_in, 0, sizeof(sockinfo_in));
    sockinfo_in.sin_family = AF_INET;
    sockinfo_in.sin_addr.s_addr = htonl(INADDR_ANY);
    sockinfo_in.sin_port = htons(nPort);

    if (sIp.compare("") != 0)
    {
        sockinfo_in.sin_addr.s_addr = inet_addr(sIp.c_str());
    }

    setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on));

    if (::bind(mSocket, (struct sockaddr *)&sockinfo_in, sizeof(sockinfo_in)) == -1)
    {
        TRC_ERR("Error [tcpSocketServer]: Could not bind socket to interface");
        return -1;
    }

    listen(mSocket, -1);

    return mSocket;
}

int CConnection::tcp6SocketServer(int &nPort)
{
    return tcp6SocketServer(nPort, "");
}

int CConnection::tcp6SocketServer(int &nPort, string sIp)
{
    int on = 1;
    int no = 0;

    mSocket = socket(AF_INET6, SOCK_STREAM, 0);
    if (mSocket == -1)
    {
        TRC_ERR("Error [tcp6SocketServer]: Could not create socket");
        return -1;
    }

    memset(&sockinfo_in6, 0, sizeof(sockinfo_in6));
    sockinfo_in6.sin6_flowinfo = 0;
    sockinfo_in6.sin6_family = AF_INET6;
    sockinfo_in6.sin6_port = htons(nPort);

    if (sIp.compare("") != 0)
    {
        (void)inet_pton(AF_INET6, sIp.c_str(), sockinfo_in6.sin6_addr.s6_addr);
    }

    setsockopt(mSocket, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&no, sizeof(no));

    setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on));

    if (::bind(mSocket, (struct sockaddr *)&sockinfo_in6, sizeof(sockinfo_in6)) == -1)
    {
        TRC_ERR("Error [tcp6SocketServer]: Could not bind socket to interface");
        return -1;
    }

    listen(mSocket, -1);

    return mSocket;
}

void CConnection::setBlocking()
{
    toggleBlocking(true);
}

void CConnection::setNonBlocking()
{
    toggleBlocking(false);
}

void CConnection::toggleBlocking(bool activate)
{
    const int flags = fcntl(mSocket, F_GETFL, 0);
    fcntl(mSocket, F_SETFL, activate ? flags ^ O_NONBLOCK : flags | O_NONBLOCK);
}

int CConnection::tlsServe()
{
    mTls = 1;

    tlsSetup(false);

    if (!ctx)
    {
        TRC_ERR("TLS Error while creating context: ");
        tlsPrintError();
        return -1;
    }

    SSL_CTX_set_ecdh_auto(ctx, 1);

    string certDir = "/var/opt/ias-server/certs/";
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(certDir.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            string dirName = ent->d_name;
            string certFile;
            string keyFile;

            if (!(dirName.compare(".") == 0) && !(dirName.compare("..") == 0))
            {
                certFile = certDir + dirName + "/" + dirName + ".crt";
                keyFile = certDir + dirName + "/" + dirName + ".key";

                if (SSL_CTX_use_certificate_chain_file(ctx, certFile.c_str()) <= 0)
                {
                    TRC_ERR("TLS Error while setting certificate file: ");
                    tlsPrintError();
                    return -1;
                }

                if (SSL_CTX_use_PrivateKey_file(ctx, keyFile.c_str(), SSL_FILETYPE_PEM) <= 0)
                {
                    TRC_ERR("TLS Error while setting key file: ");
                    tlsPrintError();
                    return -1;
                }
            }
        }
        closedir(dir);
    }
    else
    {
        TRC_CRIT("TLS error: failed to open TLS certificate Directory: " + certDir);
        return -1;
    }

    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, mSocket);

    if (SSL_accept(ssl) <= 0)
    {
        TRC_ERR("TLS Error while accepting TLS connection: ");
        tlsPrintError();
        return -1;
    }
    else
    {
        TRC_INFO("TLS Connection established");
    }

    return 0;
}

void CConnection::tlsPrintError()
{
    BIO *bio = BIO_new(BIO_s_mem());
    ERR_print_errors(bio);
    char *rbuffer;
    size_t len = BIO_get_mem_data(bio, &rbuffer);
    string ret(rbuffer, len);
    BIO_free(bio);

    if (ret.empty())
    {
        ret = "No Error available in TLS BIO";
    }

    TRC_ERR(ret);
}

int CConnection::send(const void *buf, int num, int flags)
{
    if (mTls == 0)
    {
        return ::send(mSocket, buf, num, flags);
    }
    else if (mTls == 1)
    {
        return SSL_write(ssl, buf, num);
    }

    return -1;
}

int CConnection::receive(void *buf, int num, int flags)
{
    if (mTls == 0)
    {
        return ::recv(mSocket, buf, num, flags);
    }
    else if (mTls == 1)
    {
        return SSL_read(ssl, buf, num);
    }

    return -1;
}

int CConnection::close()
{
    int close = ::close(mSocket);

    if (mTls)
    {
        ERR_free_strings();
        EVP_cleanup();
        SSL_shutdown(ssl);
        SSL_CTX_free(ctx);
    }

    return close;
}

static int tlsVerifyCertificateCallback(int ok, X509_STORE_CTX *store_ctx)
{

    int cert_error = X509_STORE_CTX_get_error(store_ctx);

    switch (cert_error)
    {
    case X509_V_ERR_CERT_NOT_YET_VALID:
    case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD:
    case X509_V_ERR_CERT_HAS_EXPIRED:
    case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD:
    {
        TRC_ERR("TLS Certificate Validation failed with error: " + to_string(cert_error));
        return -1;
        break;
    }
    }

    return (ok);
}

void CConnection::tlsSetup(bool client)
{
    SSL_library_init();

    OpenSSL_add_all_algorithms();

    SSL_load_error_strings();
    ERR_load_crypto_strings();

    const SSL_METHOD *method;
    if (client)
    {
        method = TLS_client_method();
    }
    else
    {
        method = TLS_server_method();
    }

    ctx = SSL_CTX_new(method);

    SSL_CTX_set_msg_callback(ctx, ssl_ctx_message_handler);
}

void CConnection::ssl_ctx_message_handler(
    int write_p, int version, int content_type, const void *buf, size_t len, SSL *ssl,
    void *arg)
{

    if (write_p == 0 && content_type == SSL3_RT_HEADER)
    {
        CConnection::mTlsRecordsReceived++;
    }
}

int CConnection::tlsConnect()
{
    tlsSetup(true);

    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, mSocket);

    if (SSL_connect(ssl) <= 0)
    {
        TRC_ERR("SSL Error while negotiating TLS connection: ");
        tlsPrintError();
        return -1;
    }
    else
    {
        X509 *server_cert = SSL_get_peer_certificate(ssl);

        X509_STORE *store;
        X509_STORE_CTX *store_ctx;

        store = X509_STORE_new();
        X509_STORE_set_verify_cb(store, tlsVerifyCertificateCallback);
        X509_STORE_add_cert(store, server_cert);

        store_ctx = X509_STORE_CTX_new();
        X509_STORE_CTX_init(store_ctx, store, server_cert, NULL);

        if (X509_verify_cert(store_ctx) != 0)
        {
            TRC_ERR("TLS Certificate Chain Verifiy failed");
            return -1;
        }

        string subject_name = "";
        subject_name = X509_NAME_oneline(X509_get_subject_name(server_cert), 0, 0);

        TRC_DEBUG("TLS Certificate Subject name: " + subject_name);
        TRC_DEBUG("TLS Subject validation: " + mTlsSubjectValidation);

        string wildcard = "/CN=*";
        std::size_t index = subject_name.find(wildcard);
        if (index != std::string::npos)
        {
            TRC_DEBUG("TLS Certificate Subject is a wildcard");

            subject_name.erase(index, wildcard.length());
            if (mTlsSubjectValidation.compare("") != 0 && mTlsSubjectValidation.find(subject_name) == string::npos)
            {
                TRC_ERR("TLS Certificate Subject name validation failed");
                return -1;
            }
        }
        else
        {
            if (mTlsSubjectValidation.compare("") != 0 && subject_name.find(mTlsSubjectValidation) == string::npos)
            {
                TRC_ERR("TLS Certificate Subject name validation failed");
                return -1;
            }
        }

        TRC_INFO("TLS Connection established");
    }

    return 0;
}
