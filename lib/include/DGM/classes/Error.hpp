#pragma once

#include <format>
#include <stacktrace>
#include <stdexcept>
#include <string>

namespace dgm
{
    namespace fsm
    {
        class Error : public std::runtime_error
        {
        public:
            Error(
                const std::string& message,
                std::stacktrace trace = std::stacktrace::current())
                : std::runtime_error(std::format(
                    "Exception: {}\n\nStack trace:\n{}", message, trace))
            {
            }
        };
    } // namespace fsm
} // namespace dgm