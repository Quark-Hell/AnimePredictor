#include "LogManager.h"

LogManager::LogManager() {
    _logFile.open(GetLogFileName(), std::ios::app);
    _logFile.close();
}

LogManager::~LogManager() {
    if (_logFile.is_open()) {
        _logFile.close();
    }
}

std::string LogManager::GetCurrentDate() const {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);
    std::ostringstream current_time;
    current_time << std::put_time(std::localtime(&time), "%Y-%m-%d");
    return current_time.str();
}

std::string LogManager::GetCurrentTime() const {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);
    std::ostringstream current_time;
    current_time << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return current_time.str();
}

std::string LogManager::GetLogFileName() const {
#ifdef WINDOWS
    return std::filesystem::current_path().string()+"\\Logs\\" + GetCurrentDate() + ".log";
#else
    return std::filesystem::current_path().string() + "/Logs/" + GetCurrentDate() + ".log";
#endif
}


void LogManager::WriteToFile(const std::string& message) {
    if (_logFile.is_open()) {
        _logFile << message << std::endl;
    }
}

LogManager& LogManager::GetInstance() {
    std::filesystem::path logsPath = std::filesystem::current_path() / "Logs";
    if (!std::filesystem::exists(logsPath)) {
        std::filesystem::create_directory(logsPath);
    }

    static LogManager logger;
    return logger;
}

const std::vector<std::string>& LogManager::GetCustomLogBuffer(const std::string& bufferName) {
    auto it = _customLogBuffer.find(bufferName);
    if (it != _customLogBuffer.end()) {
        return it->second;
    }

    static const std::vector<std::string> emptyBuffer;
    return emptyBuffer;
}

const std::vector<std::string>& LogManager::GetInfoLogBuffer(){
    return _infoLogBuffer;
}
const std::vector<std::string>& LogManager::GetWarningLogBuffer(){
    return _warningLogBuffer;
}
const std::vector<std::string>& LogManager::GetErrorLogBuffer(){
    return _errorLogBuffer;
}
const std::vector<std::string>& LogManager::GetCriticalLogBuffer(){
    return _criticalLogBuffer;
}