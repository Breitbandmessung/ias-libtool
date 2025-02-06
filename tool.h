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
#ifndef TOOL_H
#define TOOL_H

#include "header.h"
#include "typedef.h"
#include "configmanager.h"
#include "trace.h"
#include "sha1.hpp"
#include "json11.hpp"

using namespace std;

class CTool
{
public:
    CTool();

    virtual ~CTool();

    static int toInt(string s);

    static unsigned int toUInt(string s);

    static unsigned long long toULL(string s);

    static long long toLL(string s);

    static float toFloat(string s);

    static void toLower(string &sText);

    static void toUpper(string &sText);

    static string get_ip_str(const struct sockaddr *sa);

    static unsigned long long calculateResultsMin(map<int, unsigned long long> dmap);

    static unsigned long long calculateResultsAvg(map<int, unsigned long long> dmap);

    static int calculateResults(struct measurement_data &sMeasurement);

    static int calculateResults(struct measurement_data &sMeasurement, double increment, int ai_offset, unsigned long long duration);

    static string getSystemInfoOS();

    static string getSystemInfoOSVersion();

    static int validateIp(const string &ipAddress);

    static int get_timestamp_usec();

    static int get_timestamp_sec();

    static unsigned long long get_timestamp();

    static int get_timestamp_offset();

    static string get_timestamp_string();

    static void replaceStringInPlace(string &subject, const string &search, const string &replace);

    static void tokenize(string &str, vector<string> &tokens, string &delimiters);

    static pid_t spawn_process(char *args1[]);

    static string getDownloadSpeed(string sString);

    static string getIpFromHostname(string sString);

    static string getIpFromHostname(string sString, int nType);

    static struct addrinfo *getIpsFromHostname(string sString, bool bReachable);

    static string getHostname();

    static int randomData();

    static int randomData(char *sbuffer, int size);

    static int randomData(vector<char> &vVector, int size);

    static void print_stacktrace();

    static string to_string_precision(double value, const int precision);

    static string sha256(string str);

    template <typename T>
    static string toString(T i)
    {
        stringstream s;
        s << i;
        return s.str();
    }

    template <typename T>
    static T toNumber(string s)
    {
        T value;
        stringstream sstr(s);
        sstr >> value;
        return value;
    }

    static void getPid(uint64_t &pid)
    {
#ifdef __APPLE__
        pthread_threadid_np(NULL, &pid);
#else
        pid = syscall(SYS_gettid);
#endif
    }
};

#endif
