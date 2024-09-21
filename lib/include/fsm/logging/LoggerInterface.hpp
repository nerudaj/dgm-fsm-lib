#pragma once

#include <format>
#include <fsm/Types.hpp>
#include <string>

namespace fsm
{
    class [[nodiscard]] LoggerInterface
    {
    public:
        virtual ~LoggerInterface() = default;

    public:
        template<BlackboardTypeConcept BlackboardType>
        void
        log(const std::string& currentStateName,
            const BlackboardType& blackboard,
            const std::string& message,
            const std::string& targetState)
        {
            if constexpr (IsFormatterSpecializedForBlackboard<
                              BlackboardType,
                              char>::value)
            {
                logImplementation(
                    currentStateName,
                    std::format("{}", blackboard),
                    message,
                    targetState);
            }
            else
            {
                logImplementation(
                    currentStateName,
                    std::format(
                        "Blackboard at address {:#x}",
                        static_cast<std::uintptr_t>(&blackboard)),
                    message,
                    targetState);
            }
        }

    protected:
        virtual void logImplementation(
            const std::string& currentStateName,
            const std::string& blackboardLog,
            const std::string& message,
            const std::string& targetState) = 0;
    };
} // namespace fsm