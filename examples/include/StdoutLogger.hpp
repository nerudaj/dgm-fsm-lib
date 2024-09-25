#pragma once

#include <fsm/logging/LoggerInterface.hpp>

// Testing class that prints to stdout
class StdoutLogger : public fsm::LoggerInterface
{
protected:
    void logImplementation(
        const std::string& currentStateName,
        const std::string& blackboardLog,
        const std::string& message,
        const std::string& targetState) override
    {
        std::println(
            "{}, {}, {}, {}",
            currentStateName,
            blackboardLog,
            message,
            targetState);
    }
};
