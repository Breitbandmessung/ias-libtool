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

#define LIBIASTOOL_VERSION "2.4"

#ifndef HEADER_H_
#define HEADER_H_

#ifdef IAS_SERVER

#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#endif

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/utsname.h>

#include <syslog.h>
#include <signal.h>
#include <sched.h>
#include <unistd.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#include <net/if_dl.h>

#ifdef TARGET_OS_IPHONE
#elif TARGET_IPHONE_SIMULATOR
#elif TARGET_OS_MAC
#include <netinet/if_ether.h>
#endif
#else
#include <netinet/ether.h>
#include <netpacket/packet.h>
#endif

#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <netdb.h>
#include <resolv.h>
#include <ifaddrs.h>

#include <cmath>
#include <dirent.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <exception>

#include <pthread.h>
#include <fcntl.h>
#include <map>
#include <list>
#include <vector>
#include <getopt.h>
#include <getopt.h>
#include <errno.h>
#include <cxxabi.h>

#ifndef __ANDROID__

#include <execinfo.h>

#endif

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/sha.h>

#define MAX_PACKET_SIZE 1500
#define MAXBUFFER 1580

#define TRC_DEBUG(s) CTrace::getInstance().logDebug(s)
#define TRC_INFO(s) CTrace::getInstance().logInfo(s)
#define TRC_WARN(s) CTrace::getInstance().logWarn(s)
#define TRC_ERR(s) CTrace::getInstance().logErr(s)
#define TRC_CRIT(s) CTrace::getInstance().logCritical(s)

#define HTTP_CONTINUE "HTTP/1.1 100 Continue\r\n\r\n"
#define HTTP_OK "HTTP/1.1 200 OK\r\n\r\n"
#define HTTP_BAD_REQUEST "HTTP/1.1 400 Bad Request\r\n\r\n"
#define HTTP_FORBIDDEN "HTTP/1.1 403 Forbidden\r\n\r\n"
#define HTTP_BANDWIDTH_LIMIT_EXCEEDED "HTTP/1.1 509 Bandwidth Limit Exceeded\r\n\r\n"

#define HTTP_GET "GET / HTTP/1.1"
#define HTTP_GET_DATA "GET /data.img"
#define HTTP_POST "POST / HTTP/1.1"
#define HTTP_POST_DATA "POST /data.img"
#define HTTP_OPTIONS "OPTIONS / HTTP/1.1"
#define HTTP_CONTENT_LENGTH "Content-Length:"
#define HTTP_HOST "Host:"
#define HTTP_ACCESS_METHOD "Access-Control-Request-Method:"
#define HTTP_ACCESS_HEADERS "Access-Control-Request-Headers:"
#define HTTP_ORIGIN "Origin:"
#define HTTP_CONNECTION "Connection:"
#define HTTP_DATA "/data.img"
#define HTTP_IP "/ip"
#define HTTP_USER_AGENT "User-Agent"

extern bool _DEBUG_;
extern bool RUNNING;
extern const char *PLATFORM;
extern const char *CLIENT_OS;

extern bool TIMER_ACTIVE;
extern bool TIMER_RUNNING;
extern bool TIMER_STOPPED;

extern int TIMER_INDEX;
extern int TIMER_DURATION;
extern unsigned long long MEASUREMENT_DURATION;

extern int INSTANCE;

extern volatile bool g_fExit;

#endif

#if __WORDSIZE == 64
#define IS64BIT
#else
#define IS32BIT
#endif
