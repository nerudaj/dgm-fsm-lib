#pragma once

#include <fsm/logging/LoggerInterface.hpp>

class TestableLogger final : public fsm::LoggerInterface
{
public:
    std::string lastLogMachineId = "";
    std::string lastLogBlackboardId = "";
    std::string lastLogCurrentState = "";
    std::string lastLogBlackboard = "";
    std::string lastLogMessage = "";
    std::string lastLogTargetState = "";

protected:
    void logImplementation(const Log& log)
    {
        lastLogMachineId = log.machineId;
        lastLogBlackboardId = log.blackboardId;
        lastLogCurrentState = log.currentStateName;
        lastLogBlackboard = log.blackboardLog;
        lastLogMessage = log.message;
        lastLogTargetState = log.targetStateName;
    }
};
