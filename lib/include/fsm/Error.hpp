#pragma once

#include <format>
#include <stacktrace>
#include <stdexcept>
#include <string>

namespace fsm
{
    /**
     * Library error type used in exceptions.
     * It contains a message with a serialized stack trace.
     */
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
