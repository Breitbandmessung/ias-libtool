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

#include "basisthread.h"

CBasisThread::CBasisThread()
{
    m_fStop = false;
    m_hThread = 0;
}

CBasisThread::~CBasisThread()
{
}

int CBasisThread::initInstance()
{
    return 0;
}

int CBasisThread::exitInstance()
{
    return 0;
}

int CBasisThread::run()
{
    return 0;
}

int CBasisThread::waitForEnd()
{
    pthread_join(m_hThread, NULL);

    return 0;
}

int CBasisThread::detachThread()
{
    pthread_detach(m_hThread);

    return 0;
}

bool CBasisThread::isRunning()
{
    return !m_fStop;
}

int CBasisThread::stopThread()
{

    TRC_DEBUG("Thread " + CTool::toString((int)(size_t)m_hThread) + ": asked to stop");

    m_fStop = true;

    return 0;
}

unsigned long CBasisThread::getExitCode()
{
    return m_dwExitCode;
}

int CBasisThread::createThread()
{
    int nError = 0;

    if (m_hThread == 0)
    {
        nError = pthread_create(&m_hThread, NULL, threadStart, this);

        if (0 != nError)
            TRC_ERR("Creating Thread failed with Code: " + CTool::toString(nError));

        TRC_INFO("Thread " + CTool::toString((int)(size_t)m_hThread) + ": created");
    }
    else
        TRC_WARN("Thread " + CTool::toString((int)(size_t)m_hThread) + ": already created");

    return nError;
}

void *threadStart(void *pParam)
{
    CBasisThread *pThread = (CBasisThread *)pParam;

    int nExit = 0;

    nExit = pThread->initInstance();

    if (0 != nExit)
        pthread_exit(NULL);

    nExit = pThread->run();

    if (0 != nExit)
        pthread_exit(NULL);

    nExit = pThread->exitInstance();

    pthread_exit(NULL);
}
