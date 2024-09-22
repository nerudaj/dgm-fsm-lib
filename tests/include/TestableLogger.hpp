#pragma once

#include <fsm/logging/LoggerInterface.hpp>

class TestableLogger final : public fsm::LoggerInterface
{
public:
    std::string lastLogCurrentState = "";
    std::string lastLogBlackboard = "";
    std::string lastLogMessage = "";
    std::string lastLogTargetState = "";

protected:
    void logImplementation(
        const std::string& currentState,
        const std::string& blackboardLog,
        const std::string& message,
        const std::string& targetState)
    {
        lastLogCurrentState = currentState;
        lastLogBlackboard = blackboardLog;
        lastLogMessage = message;
        lastLogTargetState = targetState;
    }
};
