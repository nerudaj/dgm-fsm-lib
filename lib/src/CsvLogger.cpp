#include <fsm/logging/CsvLogger.hpp>
#include <print>

fsm::CsvLogger::CsvLogger(const std::filesystem::path& logFilePath)
    : outstream(logFilePath)
{
    log("CurrentState", "Blackboard", "Message", "TargetState");
}

void fsm::CsvLogger::log(
    const std::string& currentStateName,
    const std::string& blackboardLog,
    const std::string& message,
    const std::string& targetState)
{
    std::println(
        outstream,
        "{},{},{},{}",
        currentStateName,
        blackboardLog,
        message,
        targetState);
}
