// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Synaptics Incorporated

#include <algorithm>
#include <chrono>

#include "astra_console.hpp"
#include "astra_log.hpp"

AstraConsole::AstraConsole(std::string deviceName, std::string logPath)
{
    ASTRA_LOG;

    std::string logFile = logPath + "/console.log";
    m_consoleLog = std::ofstream(logFile, std::ios::out | std::ios::trunc);
}

AstraConsole::~AstraConsole()
{
    ASTRA_LOG;
}

void AstraConsole::Append(const std::string &data)
{
    ASTRA_LOG;

    std::string trimmedData = data;
    trimmedData.erase(trimmedData.find_last_not_of(" \t\n\r\f\v") + 1);

    if (trimmedData.size() >= m_uBootPrompt.size() &&
        trimmedData.rfind(m_uBootPrompt) == (trimmedData.size() - m_uBootPrompt.size()))
    {
        log(ASTRA_LOG_LEVEL_DEBUG) << "U-Boot prompt detected." << endLog;
        {
            std::lock_guard<std::mutex> lock(m_promptMutex);
            m_promptDetected = true;
        }
        m_promptCV.notify_one();
    }

    m_consoleData += data;
    m_consoleLog << data;
    m_consoleLog.flush();
}

std::string &AstraConsole::Get()
{
    ASTRA_LOG;

    return m_consoleData;
}

bool AstraConsole::WaitForPrompt(std::chrono::milliseconds timeout)
{
    ASTRA_LOG;

    std::unique_lock<std::mutex> lock(m_promptMutex);

    bool hasPrompt = false;
    if (timeout == std::chrono::milliseconds::zero()) {
        m_promptCV.wait(lock, [this] {
            return m_promptDetected || m_shutdown.load();
        });
        hasPrompt = m_promptDetected;
    } else {
        hasPrompt = m_promptCV.wait_for(lock, timeout, [this] {
            return m_promptDetected || m_shutdown.load();
        }) && m_promptDetected;
    }

    if (m_shutdown.load()) {
        return false;
    }

    // Consume one prompt detection so callers can wait for the next prompt.
    if (hasPrompt) {
        m_promptDetected = false;
    }

    return hasPrompt;
}

void AstraConsole::Shutdown()
{
    ASTRA_LOG;

    m_shutdown.store(true);
    m_promptCV.notify_all();
    m_consoleLog.close();
}