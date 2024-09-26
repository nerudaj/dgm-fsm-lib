#include <fsm/logging/CsvLogger.hpp>
#include <print>

fsm::CsvLogger::CsvLogger(const std::filesystem::path& logFilePath)
    : fileStream(logFilePath), outstream(fileStream)
{
    logHeaders();
}

fsm::CsvLogger::CsvLogger(std::ostream& stream) : outstream(stream)
{
    logHeaders();
}

void fsm::CsvLogger::logHeaders()
{
    std::println(
        outstream,
        "MachineId,BlackboardId,BlackboardLog,Message,CurrentStateName,"
        "TargetStateName");
}

void fsm::CsvLogger::logImplementation(const Log& log)
{
    std::println(
        outstream,
        "{},{},{},{},{},{}",
        log.machineId,
        log.blackboardId,
        log.blackboardLog,
        log.message,
        log.currentStateName,
        log.targetStateName);
}
