//
// Created by Guy on 10/07/2021.
//

#include <Logging.h>

#include <utility>

Logger logInstance;
extern std::string assetRoot;

void Logging::Log(std::wstring message) {
    logInstance.LogLine(std::move(message), true);
}
void Logging::Log(std::string message) {
    logInstance.LogLine(std::move(message), true);
}

std::ofstream& Logging::Get() {
    return logInstance.Get();
}

void Logging::Init() {
    logInstance.Initialise();
}

Logger::~Logger() {
    output.flush();
    output.close();

    woutput.flush();
    woutput.close();
}

void Logger::LogLine(std::wstring message, bool flush) {
    if(!ready)
        return;
    woutput << message;
    if(flush)
        woutput.flush();
}

void Logger::LogLine(std::string message, bool flush) {
    if(!ready)
        return;
    output << message;
    if(flush)
        output.flush();
}
std::ofstream& Logger::Get() {
    return output;
}

void Logger::Initialise() {
    output = std::ofstream("log.txt");
    woutput = std::wofstream("wlog.txt");
    ready = true;
}
