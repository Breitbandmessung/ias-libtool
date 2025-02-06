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

#include "trace.h"

std::function<void(std::string, std::string)> CTrace::logFunction = nullptr;

CTrace::CTrace()
{
}

CTrace &CTrace::getInstance()
{
    static CTrace instance;
    return instance;
}

#ifdef IAS_SERVER

void CTrace::init(string sIniFileName, string sCategory)
{
    mCategory = sCategory;

    log4cpp::PropertyConfigurator::configure(sIniFileName);
}

#endif

void CTrace::logCritical(const string &sMessage)
{
#ifdef IAS_CLIENT
    logToPlatform("CRITICAL", sMessage);
#endif

#ifdef IAS_SERVER

    log4cpp::Category &cat = log4cpp::Category::getInstance(mCategory);

    cat.crit(("#" + CTool::toString(getpid()) + " [" + CTool::toString(pthread_self()) + "] " + sMessage).c_str());
#endif
}

void CTrace::logErr(const string &sMessage)
{
#ifdef IAS_CLIENT
    logToPlatform("ERROR", sMessage);
#endif

#ifdef IAS_SERVER

    log4cpp::Category &cat = log4cpp::Category::getInstance(mCategory);

    cat.error(("#" + CTool::toString(getpid()) + " [" + CTool::toString(pthread_self()) + "] " + sMessage).c_str());
#endif
}

void CTrace::logWarn(const string &sMessage)
{
#ifdef IAS_CLIENT
    logToPlatform("WARN", sMessage);
#endif

#ifdef IAS_SERVER

    log4cpp::Category &cat = log4cpp::Category::getInstance(mCategory);

    cat.warn(("#" + CTool::toString(getpid()) + " [" + CTool::toString(pthread_self()) + "] " + sMessage).c_str());
#endif
}

void CTrace::logInfo(const string &sMessage)
{
#ifdef IAS_CLIENT
    logToPlatform("INFO", sMessage);
#endif

#ifdef IAS_SERVER

    log4cpp::Category &cat = log4cpp::Category::getInstance(mCategory);

    cat.info(("#" + CTool::toString(getpid()) + " [" + CTool::toString(pthread_self()) + "] " + sMessage).c_str());
#endif
}

void CTrace::logDebug(const string &sMessage)
{
#if defined(IAS_CLIENT) && !defined(__ANDROID__) && !defined(__APPLE__)
    if (::_DEBUG_)
    {
        logToPlatform("_DEBUG_", sMessage);
    }
#endif
#if defined(IAS_CLIENT) && defined(IAS_IOS)
    if (::_DEBUG_)
    {
        logToPlatform("_DEBUG_", sMessage);
    }
#endif

#ifdef IAS_SERVER

    log4cpp::Category &cat = log4cpp::Category::getInstance(mCategory);

    cat.debug(("#" + CTool::toString(getpid()) + " [" + CTool::toString(pthread_self()) + "] " + sMessage).c_str());
#endif
}

#ifdef IAS_CLIENT

void CTrace::logToPlatform(const string &category, const string &sMessage)
{
    if (CTrace::logFunction != nullptr)
    {
        CTrace::logFunction(category, sMessage);
    }
}

#endif