#pragma once

#include <filesystem>
#include <fsm/logging/LoggerInterface.hpp>
#include <fstream>
#include <iostream>

namespace fsm
{
    class [[nodiscard]] CsvLogger final : public LoggerInterface
    {
    public:
        /**
         *  Open a logger that logs into file at given path
         *
         *  \param logFilePath Path to output log
         */
        explicit CsvLogger(const std::filesystem::path& logFilePath);

        /**
         *  Open a logger that logs into a provided ostream
         *
         *  \param stream  Any output stream (std::cout by default)
         */
        explicit CsvLogger(std::ostream& stream = std::cout) noexcept
            : outstream(stream)
        {
        }

        CsvLogger(const CsvLogger&) = delete;
        CsvLogger(CsvLogger&&) = default;

    protected:
        void logImplementation(
            const std::string& fsmId,
            const std::string& currentStateName,
            const std::string& blackboardLog,
            const std::string& message,
            const std::string& targetState) override;

    private:
        std::ofstream fileStream;
        std::ostream& outstream;
    };
} // namespace fsm