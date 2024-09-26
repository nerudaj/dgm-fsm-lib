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
                logImplementation(
                    std::format("{:#x}", fsmId),
                    currentStateName,
                    std::format("{}", blackboard),
                    message,
                    targetStateName);
            }
            else
            {
                logImplementation(
                    std::format("{:#x}", fsmId),
                    currentStateName,
                    std::format(
                        "Blackboard @ {:#x}",
                        reinterpret_cast<std::uintptr_t>(&blackboard)),
                    message,
                    targetStateName);
            }
        }

    protected:
        virtual void logImplementation(
            const std::string& fsmId,
            const std::string& currentStateName,
            const std::string& blackboardLog,
            const std::string& message,
            const std::string& targetState) = 0;
    };
} // namespace fsm