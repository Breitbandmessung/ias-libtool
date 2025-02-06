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

#include "http.h"

CHttp::CHttp()
{
}

CHttp::~CHttp()
{
}

CHttp::CHttp(CConfigManager *pConfig, CConnection *nConnection)
{
    mConnection = nConnection;

    mConfig = pConfig;

    mHostname = mConfig->readString("general", "hostname", "");
    if (mHostname != "")
    {
        mHostname = mHostname + " (" + LIBIASTOOL_VERSION + ")";
    }
}

CHttp::CHttp(CConfigManager *pConfig, CConnection *nConnection, string sType)
{
    mConnection = nConnection;

    mConfig = pConfig;
    mType = sType;

    mHttpRequestTime = 0;
    mHttpResponseTime = 0;

    mHostname = mConfig->readString("general", "hostname", "");
    if (mHostname != "")
    {
        mHostname = mHostname + " (" + LIBIASTOOL_VERSION + ")";
    }
}

int CHttp::parseResponse()
{
    bool firstRequest = true;
    int nValue = -1;
    int recv_len;
    char rbuffer[MAXBUFFER];
    string buffer;

    bzero(rbuffer, MAXBUFFER);

    recv_len = mConnection->receive(rbuffer, MAXBUFFER, 0);

    buffer = string(rbuffer);

    TRC_DEBUG("Received: (" + to_string(recv_len) + ") --------------------------------" + "\r\n" + buffer);

    if (buffer.find("HTTP/1.1 200 OK") != string::npos)
    {
        mHttpResponseTime = CTool::get_timestamp();
        nValue = 0;
    }

    if (buffer.find("HTTP/1.1 100 Continue") != string::npos)
    {
        mHttpResponseTime = CTool::get_timestamp();
        nValue = 0;
    }

    if (buffer.find(HTTP_FORBIDDEN) != string::npos)
    {
        nValue = -2;
    }

    if (buffer.find(HTTP_BANDWIDTH_LIMIT_EXCEEDED) != string::npos)
    {
        nValue = -3;
    }

    if (firstRequest && buffer.find(HTTP_GET_DATA) != string::npos)
    {
        mType = "GET";
        nValue = responseOk();
    }

    if (firstRequest && buffer.find(HTTP_POST_DATA) != string::npos)
    {
        mType = "POST";
        nValue = responseOk();
    }

#ifndef IAS_CLIENT
    if (nValue == -1)
    {
        return responseNotFound();
    }
#endif

    return nValue;
}

int CHttp::generateHTML(string &content, string message)
{
    content = "<!DOCTYPE html>\n";
    content += "<html>\n";
    content += "<head>\n";
    content += "<title>" + message + "</title>\n";
    content += "</head>\n";
    content += "<body>\n";
    content += message;
    content += "</body>\n";
    content += "</html>\r\n\n";

    return content.size();
}

int CHttp::getHttpResponseDuration()
{
    if (mHttpResponseTime == 0)
        return 0;

    return mHttpResponseTime - mHttpRequestTime;
}

string CHttp::getHttpServerHostname()
{
    return mHttpServerHostname;
}

int CHttp::responseNotFound()
{

    string send_init = "";
    string content = "";

    send_init += "HTTP/1.1 404 Not Found\r\n";
    send_init += "Server: " + mHostname + "\r\n";
    send_init += "Date: " + CTool::get_timestamp_string() + "\r\n";
    send_init += "Content-Length: " + CTool::toString(generateHTML(content, "404 Not Found")) + "\r\n";
    send_init += "Content-Language: en\r\n";
    send_init += "Content-Type: text/html; charset=utf-8\r\n";
    send_init += "Connection: close\r\n\r\n";
    send_init += content;

    int send_len = mConnection->send(send_init.c_str(), send_init.size(), 0);

    TRC_DEBUG("SEND-Buffer: (" + to_string(send_len) + ") --------------------------------" + "\r\n" + send_init);

    return 404;
}

int CHttp::responseOk()
{
    int ret = -1;

    int time = CTool::get_timestamp_sec();

    string send_init = "";
    string content = "";

    send_init += "HTTP/1.1 200 OK\r\n";
    send_init += "Server: " + mHostname + "\r\n";
    send_init += "Date: " + CTool::get_timestamp_string() + "\r\n";
    send_init += "ETag: " + CTool::toString(time) + "\r\n";

    if (mType == "POST")
    {
        send_init = "";
        send_init += "HTTP/1.1 100 Continue\r\n";
        send_init += "Server: " + mHostname + "\r\n";
        send_init += "Date: " + CTool::get_timestamp_string() + "\r\n";
        send_init += "ETag: " + CTool::toString(time) + "\r\n";

        send_init += "Content-Length: 1024000000\r\n";
        send_init += "Content-Type: application/octet-stream\r\n";
        send_init += "Connection: keep-alive\r\n\r\n";

        ret = 202;
    }
    else if (mType == "GET")
    {
        send_init += "Accept-Ranges: bytes\r\n";
        send_init += "Content-Language: en\r\n";
        send_init += "Content-Type: application/octet-stream\r\n";
        send_init += "Cache-Control: max-age=0, no-cache, no-store\r\n";
        send_init += "Pragma: no-cache\r\n";
        send_init += "X-Rack-Cache: miss\r\n";
        send_init += "Connection: keep-alive\r\n\r\n";

        ret = 201;
    }
    else
    {
        ret = -1;
    }

    int send_len = mConnection->send(send_init.c_str(), send_init.size(), 0);

    TRC_DEBUG("SEND-Buffer: (" + to_string(send_len) + ") --------------------------------" + "\r\n" + send_init);

    return ret;
}

int CHttp::requestToReferenceServer(string uri)
{
    string send_init = "";
    send_init += "" + mType + " " + uri + " HTTP/1.1\r\n";
    if (mHostname != "")
    {
        send_init += "Host: " + mHostname + "\r\n";
    }

    send_init += "Connection: keep-alive\r\n\r\n";

    int send_len = mConnection->send(send_init.c_str(), send_init.size(), 0);

    TRC_DEBUG("SEND-Buffer: (" + to_string(send_len) + ") --------------------------------" + "\r\n" + send_init);

    return parseResponse();
}
