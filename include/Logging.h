//
// Created by Guy on 10/07/2021.
//

#ifndef GLMODELVIEWER_LOGGING_H
#define GLMODELVIEWER_LOGGING_H

#include <string>
#include <fstream>

#define LOG(msg) Logging::Log(std::string(__FILE__) + std::string("@") + std::to_string(__LINE__) + std::string("::") + std::string(__FUNCTION__) + std::string(" >> ") + msg);
#define LOG_LN(msg) Logging::Log(std::string(__FILE__) + std::string("@") + std::to_string(__LINE__) + std::string("::") + std::string(__FUNCTION__) + std::string(" >> ") + msg + "\n");
#define LOG_RUN(func) LOG(std::string(#func) + "\n"); func

namespace Logging {
//    void Log(const char* message, const char* function, const char* file, const char* line);
    void Log(std::wstring message);
    void Log(std::string message);
    std::ofstream& Get();
    void Init();
};

class Logger {
public:

    ~Logger();

    void Initialise();
    void LogLine(std::wstring message, bool flush = true);
    void LogLine(std::string message, bool flush = true);

    std::ofstream& Get();

protected:
    std::ofstream output;
    std::wofstream woutput;
    bool ready = false;


};

#endif//GLMODELVIEWER_LOGGING_H
