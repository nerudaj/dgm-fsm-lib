#pragma once

#include <string>

namespace fsm
{
    class [[nodiscard]] LoggerInterface
    {
    public:
        virtual ~LoggerInterface() = default;

    public:
        virtual void
        log(const std::string& currentStateName,
            const std::string& blackboardLog,
            const std::string& message,
            const std::string& targetState) = 0;
    };
} // namespace fsm