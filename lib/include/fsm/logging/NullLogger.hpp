#pragma once

#include <fsm/logging/LoggerInterface.hpp>

namespace fsm
{
    class [[nodiscard]] NullLogger final : public LoggerInterface
    {
    protected:
        void logImplementation(
            const std::string&,
            const std::string&,
            const std::string&,
            const std::string&) override
        {
        }
    };
} // namespace fsm