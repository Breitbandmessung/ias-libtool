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

#include "tool.h"

CTool::CTool()
{
}

CTool::~CTool()
{
}

int CTool::toInt(string s)
{
    int i;
    stringstream sstr(s);
    sstr >> i;
    return i;
}

unsigned int CTool::toUInt(string s)
{
    unsigned int b;
    stringstream sstr(s);
    sstr >> b;
    return b;
}

unsigned long long CTool::toULL(string s)
{
    unsigned long long b;
    stringstream sstr(s);
    sstr >> b;
    return b;
}

long long CTool::toLL(string s)
{
    long long b;
    stringstream sstr(s);
    sstr >> b;
    return b;
}

float CTool::toFloat(string s)
{
    float b;
    stringstream sstr(s);
    sstr >> b;
    return b;
}

void CTool::toLower(string &sText)
{
    for (unsigned int n = 0; n < sText.size(); n++)
    {
        sText.at(n) = tolower(sText.at(n));
    }
}

void CTool::toUpper(string &sText)
{
    for (unsigned int n = 0; n < sText.size(); n++)
    {
        sText.at(n) = toupper(sText.at(n));
    }
}

string CTool::get_ip_str(const struct sockaddr *sa)
{
    string ip;
    switch (sa->sa_family)
    {
    case AF_INET:
    {
        char s1[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr), s1, INET_ADDRSTRLEN);
        ip = string(s1);
        break;
    }
    case AF_INET6:
    {
        char s2[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr), s2, INET6_ADDRSTRLEN);
        ip = string(s2);
        break;
    }
    default:
    {
        ip = "0.0.0.0";
    }
    }

    return ip;
}

unsigned long long CTool::calculateResultsMin(map<int, unsigned long long> dmap)
{
    unsigned long long min = dmap.begin()->second;

    map<int, unsigned long long>::iterator AI;

    for (AI = dmap.begin(); AI != dmap.end(); ++AI)
    {
        if ((*AI).second < min)
            min = (*AI).second;
    }

    return min;
}

unsigned long long CTool::calculateResultsAvg(map<int, unsigned long long> dmap)
{
    int count = 0;

    unsigned long long avg = 0;

    map<int, unsigned long long>::iterator AI;

    for (AI = dmap.begin(); AI != dmap.end(); ++AI)
    {
        avg += (*AI).second;
        count++;
    }

    if (count != 0)
    {
        avg = avg / count;
    }

    return avg;
}

int CTool::calculateResults(struct measurement_data &sMeasurement)
{
    return CTool::calculateResults(sMeasurement, 1, 0, 0);
}

int CTool::calculateResults(
    struct measurement_data &sMeasurement, double increment, int ai_offset,
    unsigned long long duration)
{
    double count = 0;

    unsigned long long min = std::next(sMeasurement.results.begin(), ai_offset)->second;
    unsigned long long avg = 0;
    unsigned long long max = 0;
    unsigned long long sum = 0;
    unsigned long long sumSq = 0;
    double avgDouble = 0;

    vector<double> medianVector;

    map<int, unsigned long long>::iterator AI;

    for (AI = std::next(sMeasurement.results.begin(), ai_offset);
         AI != sMeasurement.results.end(); ++AI)
    {
        if ((*AI).second < min)
            min = (*AI).second;

        if ((*AI).second > max)
            max = (*AI).second;

        sum += (*AI).second;

        medianVector.push_back((*AI).second);
        sumSq += (*AI).second * (*AI).second;

        count += increment;
    }

    if (count != 0)
    {
        if (duration != 0)
        {
            avg = avgDouble = (double)sum / ((double)duration / 1e6);
        }
        else
        {
            avg = avgDouble = (double)sum / (double)count;
        }

        sMeasurement.min = min;
        sMeasurement.avg = avg;
        sMeasurement.max = max;

        if (duration != 0)
        {
            sMeasurement.duration_ns = duration * 1000;
        }
        else
        {
            sMeasurement.duration_ns = count * 1e9;
        }

        sMeasurement.interim_values = medianVector;

        sort(medianVector.begin(), medianVector.end());
        size_t samples = medianVector.size();

        if (samples % 2 == 0)
        {
            sMeasurement.median_ns =
                ((medianVector[samples / 2 - 1] + medianVector[samples / 2]) / 2) * 1000;
        }
        else
        {
            sMeasurement.median_ns = medianVector[samples / 2] * 1000;
        }

        double variancePopulation = (double)sumSq / (double)count - avgDouble * avgDouble;
        sMeasurement.standard_deviation_ns = sqrt(variancePopulation) * 1000;
    }
    else
    {
        sMeasurement.min = 0;
        sMeasurement.avg = 0;
        sMeasurement.max = 0;
        sMeasurement.median_ns = 0;
        sMeasurement.standard_deviation_ns = 0;
    }

    return 0;
}

string CTool::getSystemInfoOS()
{
    struct utsname unameData;

    if (uname(&unameData) != 0)
        return "-";

    return unameData.sysname;
}

string CTool::getSystemInfoOSVersion()
{
    struct utsname unameData;

    if (uname(&unameData) != 0)
        return "-";

    return unameData.release;
}

int CTool::validateIp(const string &ipAddress)
{
    struct sockaddr_in sa4;
    struct sockaddr_in6 sa6;

    return (inet_pton(AF_INET6, ipAddress.c_str(), &(sa6.sin6_addr)) != 0) ? 6 : (inet_pton(AF_INET, ipAddress.c_str(), &(sa4.sin_addr)) != 0) ? 4 : 0;
}

int CTool::get_timestamp_usec()
{
    struct timeval tp;

    gettimeofday(&tp, NULL);

    return tp.tv_usec;
}

int CTool::get_timestamp_sec()
{
    struct timeval tp;

    gettimeofday(&tp, NULL);

    return tp.tv_sec;
}

unsigned long long CTool::get_timestamp()
{
    struct timeval tp;

    gettimeofday(&tp, NULL);

    return (((unsigned long long)(tp.tv_sec) * 1000000) + (unsigned long long)(tp.tv_usec));
}

int CTool::get_timestamp_offset()
{
    struct tm *lt;
    time_t t = time(NULL);
    lt = localtime(&t);

    long int offset = lt->tm_gmtoff;

    return offset;
}

string CTool::get_timestamp_string()
{
    string date;

    time_t now = time(0);

    char *dt = ctime(&now);

    date = string(dt);

    date.erase(remove(date.begin(), date.end(), '\r'), date.end());
    date.erase(remove(date.begin(), date.end(), '\n'), date.end());

    return date;
}

void CTool::replaceStringInPlace(string &subject, const string &search, const string &replace)
{
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != string::npos)
    {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
}

void CTool::tokenize(string &str, vector<string> &tokens, string &delimiters)
{

    string::size_type lastPos = str.find_first_not_of(delimiters, 0);

    string::size_type pos = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {

        tokens.push_back(str.substr(lastPos, pos - lastPos));

        lastPos = str.find_first_not_of(delimiters, pos);

        pos = str.find_first_of(delimiters, lastPos);
    }
}

pid_t CTool::spawn_process(char *args1[])
{
    pid_t pid = fork();

    if (pid == -1)
    {
        TRC_ERR("Error: Could not fork Process");
        return -1;
    }

    if (pid == 0)
    {
        execv(args1[0], args1);
        abort();
    }
    else
    {
        return pid;
    }
}

string CTool::getDownloadSpeed(string sString)
{
    string s;

    size_t first;
    size_t last;

    const char seperator = '_';

    last = sString.rfind(seperator);
    first = sString.rfind(seperator, last - 1);

    s = sString.substr(first + 1, last - first - 1);
    return s;
}

string CTool::getIpFromHostname(string sString)
{
    return getIpFromHostname(sString, 6);
}

string CTool::getIpFromHostname(string sString, int nType)
{
    int error = 0;

    char host[NI_MAXHOST];
    struct addrinfo query, *result;

    memset(&query, 0, sizeof query);

    if (nType == 4)
        query.ai_family = AF_INET;
    if (nType == 6)
        query.ai_family = AF_INET6;

    query.ai_socktype = SOCK_DGRAM;

    if ((error = getaddrinfo(sString.c_str(), NULL, &query, &result)) != 0)
    {
        query.ai_family = AF_INET;

        if ((error = getaddrinfo(sString.c_str(), NULL, &query, &result)) != 0)
        {
            TRC_ERR("Could not Request DNS - DNS ERROR");

            return "";
        }
    }

    getnameinfo(result->ai_addr, result->ai_addrlen, host, sizeof host, NULL, 0, NI_NUMERICHOST);

    freeaddrinfo(result);

    return string(host);
}

struct addrinfo *CTool::getIpsFromHostname(string sString, bool bReachable)
{
    int error = 0;

    struct addrinfo query, *ips;

    memset(&query, 0, sizeof query);
    query.ai_family = AF_UNSPEC;

    if (bReachable)
    {
        query.ai_flags = (AI_V4MAPPED | AI_ADDRCONFIG);
    }
    else
    {
        query.ai_flags = AI_ADDRCONFIG;
    }

    if ((error = getaddrinfo(sString.c_str(), NULL, &query, &ips)) != 0)
    {
        TRC_ERR("Could not Request DNS - DNS ERROR");
    }

    return ips;
}

string CTool::getHostname()
{
    char hostname[1024];
    gethostname(hostname, 1024);
    return string(hostname);
}

int CTool::randomData()
{
    return (255 * (rand() / (RAND_MAX + 1.0)));
}

int CTool::randomData(char *sbuffer, int size)
{
    srand(CTool::get_timestamp());

    for (int counter = 0; counter < size; counter++)
    {
        sbuffer[counter] = (char)randomData();
    }

    return 0;
}

int CTool::randomData(vector<char> &vVector, int size)
{
    srand(CTool::get_timestamp());

    vVector.clear();
    for (int i = 0; i < size; i++)
        vVector.push_back((char)randomData());

    return 0;
}

void CTool::print_stacktrace()
{

#ifndef __ANDROID__

    char chTrace[1024];
    TRC_DEBUG("stack trace:");

    void *addrlist[64];

    int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void *));

    if (addrlen == 0)
    {
        TRC_DEBUG("  <empty, possibly corrupt>");
        return;
    }

    char **symbollist = backtrace_symbols(addrlist, addrlen);

    size_t funcnamesize = 256;
    char *funcname = (char *)malloc(funcnamesize);

    for (int i = 1; i < addrlen; i++)
    {
        char *begin_name = 0, *begin_offset = 0, *end_offset = 0;

        for (char *p = symbollist[i]; *p; ++p)
        {
            if (*p == '(')
                begin_name = p;
            else if (*p == '+')
                begin_offset = p;
            else if (*p == ')' && begin_offset)
            {
                end_offset = p;
                break;
            }
        }

        if (begin_name && begin_offset && end_offset && begin_name < begin_offset)
        {
            *begin_name++ = '\0';
            *begin_offset++ = '\0';
            *end_offset = '\0';

            int status;
            char *ret = abi::__cxa_demangle(begin_name, funcname, &funcnamesize, &status);
            if (status == 0)
            {
                funcname = ret;
                snprintf(chTrace, sizeof(chTrace), "  %s : %s+%s", symbollist[i], funcname, begin_offset);
                TRC_DEBUG(chTrace);
            }
            else
            {
                snprintf(chTrace, sizeof(chTrace), "  %s : %s()+%s", symbollist[i], begin_name, begin_offset);
                TRC_DEBUG(chTrace);
            }
        }
        else
        {
            snprintf(chTrace, sizeof(chTrace), "  %s", symbollist[i]);
            TRC_DEBUG(chTrace);
        }
    }

    free(funcname);
    free(symbollist);

#endif
}

string CTool::to_string_precision(double value, const int precision)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

string CTool::sha256(string str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}