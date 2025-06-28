#ifndef ANIMEPREDICTOR_LOGMANAGER_H
#define ANIMEPREDICTOR_LOGMANAGER_H

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <mutex>

#define __LOGERROR__ (std::string(__FILE__ ":") + std::to_string(__LINE__))

class LogManager {

private:
    std::ofstream _logFile;

    std::map<std::string,std::vector<std::string>> _customLogBuffer;

    std::vector<std::string> _infoLogBuffer;
    std::vector<std::string> _warningLogBuffer;
    std::vector<std::string> _errorLogBuffer;
    std::vector<std::string> _criticalLogBuffer;

    std::mutex _mutex;

private:
    LogManager();

    std::string GetLogFileName() const;
    std::string GetCurrentTime() const;
    std::string GetCurrentDate() const;

    void WriteToFile(const std::string &message);

public:
    ~LogManager();
    LogManager(const LogManager&) = delete;
    LogManager(LogManager&&) = delete;
    LogManager& operator=(const LogManager&) = delete;
    LogManager& operator=(LogManager&&) = delete;

    static LogManager& GetInstance();

    const std::vector<std::string> &GetCustomLogBuffer(const std::string& bufferName);

    const std::vector<std::string> &GetInfoLogBuffer();
    const std::vector<std::string> &GetWarningLogBuffer();
    const std::vector<std::string> &GetErrorLogBuffer();
    const std::vector<std::string> &GetCriticalLogBuffer();

    template<typename... Data>
    static void LogCustom(bool isAddTime, const std::string& bufferName, const Data &... data) {
        LogManager& logger = LogManager::GetInstance();

        std::ostringstream messageStream;
        if (isAddTime) {
            messageStream << "[" << logger.GetCurrentTime() << "] ";
        }

        ((messageStream << " " << data), ...);
        const std::string rawMessage = messageStream.str();
        const std::string message = "\033[36m" + rawMessage + "\033[0m";

        const std::lock_guard guard(logger._mutex);
        logger._logFile.open(logger.GetLogFileName(), std::ios::app);

        logger.WriteToFile(messageStream.str());
        logger._customLogBuffer[bufferName].emplace_back(rawMessage);
        std::cout << message << std::endl;
        logger._logFile.close();
    }

    template<typename... Data>
    static void LogInfo(const Data &... data) {
        LogManager& logger = LogManager::GetInstance();

        std::ostringstream messageStream;
        messageStream << "[" << logger.GetCurrentTime() << "] INFO:";
        ((messageStream << " " << data), ...);
        const std::string rawMessage = messageStream.str();
        const std::string message = "\033[36m" + rawMessage + "\033[0m";

        const std::lock_guard guard(logger._mutex);
        logger._logFile.open(logger.GetLogFileName(), std::ios::app);

        logger.WriteToFile(messageStream.str());
        logger._infoLogBuffer.emplace_back(rawMessage);
        std::cout << message << std::endl;
        logger._logFile.close();
    }

    template<typename... Data>
    static void LogWarning(const Data &... data) {
        LogManager& logger = LogManager::GetInstance();

        std::ostringstream messageStream;
        messageStream << "[" << logger.GetCurrentTime() << "] WARNING:";
        ((messageStream << " " << data), ...);
        const std::string rawMessage = messageStream.str();
        const std::string message = "\033[33m" + rawMessage + "\033[0m";

        const std::lock_guard guard(logger._mutex);
        logger._logFile.open(logger.GetLogFileName(), std::ios::app);

        logger.WriteToFile(messageStream.str());
        logger._warningLogBuffer.emplace_back(rawMessage);
        std::cout << message << std::endl;
        logger._logFile.close();
    }

    template<typename... Data>
    static void LogError(const Data &... data) {
        LogManager& logger = LogManager::GetInstance();

        std::ostringstream messageStream;
        messageStream << "[" << logger.GetCurrentTime() << "] ERROR:";
        ((messageStream << " " << data), ...);
        const std::string rawMessage = messageStream.str();
        const std::string message = "\033[31m" + rawMessage + "\033[0m";

        const std::lock_guard guard(logger._mutex);
        logger._logFile.open(logger.GetLogFileName(), std::ios::app);

        logger.WriteToFile(messageStream.str());
        logger._errorLogBuffer.emplace_back(rawMessage);
        std::cout << message << std::endl;
        logger._logFile.close();
    }

    template<typename... Data>
    static void LogCritical(const Data &... data) {
        LogManager& logger = LogManager::GetInstance();

        std::ostringstream messageStream;
        messageStream << "[" << logger.GetCurrentTime() << "] CRITICAL:";
        ((messageStream << " " << data), ...);
        const std::string rawMessage = messageStream.str();
        const std::string message = "\033[1;31m" + rawMessage + "\033[0m";

        const std::lock_guard guard(logger._mutex);
        logger._logFile.open(logger.GetLogFileName(), std::ios::app);

        logger.WriteToFile(messageStream.str());
        logger._criticalLogBuffer.emplace_back(rawMessage);
        std::cout << message << std::endl;
        logger._logFile.close();

        abort();
    }
};

#endif //ANIMEPREDICTOR_LOGMANAGER_H
