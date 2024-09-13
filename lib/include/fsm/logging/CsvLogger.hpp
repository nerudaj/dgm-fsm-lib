#pragma once

#include <filesystem>
#include <fsm/logging/LoggerInterface.hpp>
#include <fstream>

namespace fsm
{
    class [[nodiscard]] CsvLogger final : public LoggerInterface
    {
    public:
        CsvLogger(const std::filesystem::path& logFilePath);

    public:
        void
        log(const std::string& currentStateName,
            const std::string& blackboardLog,
            const std::string& message,
            const std::string& targetState) override;

    private:
        std::ofstream outstream;
    };
} // namespace fsm