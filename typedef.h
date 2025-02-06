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

#include "header.h"

#ifndef TYPEDEF_H_
#define TYPEDEF_H_

using namespace std;

struct measurement_data
{
public:
    unsigned long long min;
    unsigned long long avg;
    unsigned long long max;
    unsigned long long duration_ns;
    unsigned long long median_ns;
    unsigned long long standard_deviation_ns;
    vector<double> interim_values;

    unsigned long long packetsize;
    unsigned long long datasize;
    unsigned long long datasize_total;
    unsigned long long datasize_slow_start;
    string client;
    string server;
    string servername;
    string serverhostname;

    unsigned long long requests;
    unsigned long long replies;
    unsigned long long missing;
    unsigned long long errors;
    unsigned long long hops;

    unsigned long long totime;
    unsigned long long starttime;
    unsigned long long endtime;
    unsigned long long totaltime;

    map<int, unsigned long long> httpresponse;
    map<int, unsigned long long> results;
    map<int, long long> results_timestamp;
    map<int, int> values_int;
    map<int, double> values_double;
    map<int, string> values_string;

    float measurement_phase_progress;
};

struct conf_data
{
public:
    string sTestName;
    string sProvider;

    int nTestCase;
    int instances;
    string config_filename;
    string ntservice_filename;
    string xml_filename;
    string tmp_filename;
    string log_filename;

    string sHostName;
    string sConfigPath;
    string sLogPath;
    string sTmpPath;
};

#endif
