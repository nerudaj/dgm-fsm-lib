#pragma once

#include <fsm/logging/LoggerInterface.hpp>

namespace fsm
{
    class [[nodiscard]] NullLogger final : public LoggerInterface
    {
    protected:
        void logImplementation(const Log&) override {}
    };
} // namespace fsm