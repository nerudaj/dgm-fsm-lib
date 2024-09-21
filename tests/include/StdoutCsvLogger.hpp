#pragma once

#include <fsm/logging/LoggerInterface.hpp>
#include <print>

class StdoutCsvLogger final : public fsm::LoggerInterface
{
protected:
    void logImplementation(
        const std::string& currentStateName,
        const std::string& blackboardLog,
        const std::string& message,
        const std::string& targetState)
    {
        std::println(
            "{}, {}, {}, {}",
            currentStateName,
            blackboardLog,
            message,
            targetState);
    }
};