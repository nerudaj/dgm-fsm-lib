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
        /**
         *  Emit log from the FSM
         *
         *  \param fsmId  Id of the FSM in form of its address
         *  \param currentStateName  Name of the FSM state that is currently
         * being ticked \param blackboard  Blackboard that is being updated
         *  \param message  Verbose message about what is happening
         *  \param targetStateName  Name of the state the FSM will transition to
         */
        template<BlackboardTypeConcept BlackboardType>
        void
        log(const std::uintptr_t fsmId,
            const std::string& currentStateName,
            const BlackboardType& blackboard,
            const std::string& message,
            const std::string& targetStateName)
        {
            if constexpr (IsFormatterSpecializedForBlackboard<
                              BlackboardType,
                              char>::value)
            {
                logImplementation(Log {
                    .machineId = std::format("{:#x}", fsmId),
                    .blackboardId = std::format(
                        "{:#x}", reinterpret_cast<std::uintptr_t>(&blackboard)),
                    .blackboardLog = std::format("{}", blackboard),
                    .message = message,
                    .currentStateName = currentStateName,
                    .targetStateName = targetStateName,
                });
            }
            else
            {
                logImplementation(Log {
                    .machineId = std::format("{:#x}", fsmId),
                    .blackboardId = std::format(
                        "{:#x}", reinterpret_cast<std::uintptr_t>(&blackboard)),
                    .blackboardLog = "",
                    .message = message,
                    .currentStateName = currentStateName,
                    .targetStateName = targetStateName,
                });
            }
        }

    protected:
        struct Log
        {
            std::string machineId;
            std::string blackboardId;
            std::string blackboardLog;
            std::string message;
            std::string currentStateName;
            std::string targetStateName;
        };

        virtual void logImplementation(const Log& log) = 0;
    };
} // namespace fsm