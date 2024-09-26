#include <fsm/logging/CsvLogger.hpp>
#include <print>

fsm::CsvLogger::CsvLogger(const std::filesystem::path& logFilePath)
    : fileStream(logFilePath), outstream(fileStream)
{
    logImplementation(
        "MachineId", "CurrentState", "Blackboard", "Message", "TargetState");
}

void fsm::CsvLogger::logImplementation(
    const std::string& fsmId,
    const std::string& currentStateName,
    const std::string& blackboardLog,
    const std::string& message,
    const std::string& targetState)
{
    std::println(
        outstream,
        "{},{},{},{},{}",
        fsmId,
        currentStateName,
        blackboardLog,
        message,
        targetState);
}
