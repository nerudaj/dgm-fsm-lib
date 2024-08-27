#pragma once

#include <DGM/classes/Types.hpp>
#include <map>
#include <string>
#include <vector>

namespace dgm
{
    namespace fsm
    {
        using StateId = std::string;
        using MachineId = std::string;

        namespace detail
        {
            [[nodiscard]] static inline std::string createFullStateName(
                const MachineId& machineName, const StateId& stateName)
            {
                return machineName + ":" + stateName;
            }
        } // namespace detail

        struct Destination
        {
            StateId primary = "";
            StateId secondary = "";
        };

        template<BlackboardTypeConcept BbT>
        struct ConditionalTransition
        {
            Condition<BbT> condition;
            Destination destination;
        };

        template<BlackboardTypeConcept BbT>
        struct StateBuilderContext
        {
            std::vector<ConditionalTransition<BbT>> conditions;
            Action<BbT> action;
            Destination destination;
        };

        template<BlackboardTypeConcept BbT>
        struct MachineBuilderContext
        {
            StateId entryState;
            StateId currentlyBuiltState;
            std::map<StateId, StateBuilderContext<BbT>> states;
        };

        template<BlackboardTypeConcept BbT>
        struct BuilderContext
        {
            MachineId currentlyBuiltMachine;
            std::map<MachineId, MachineBuilderContext<BbT>> machines;
            Condition<BbT> errorCondition;
            Destination errorDestination;
            bool useGlobalError = false;

            MachineBuilderContext<BbT>& getCurrentlyBuiltMachine()
            {
                return machines.at(currentlyBuiltMachine);
            }

            StateBuilderContext<BbT>& getCurrentlyBuiltState()
            {
                auto& machine = getCurrentlyBuiltMachine();
                return machine.states.at(machine.currentlyBuiltState);
            }
        };
    } // namespace fsm
} // namespace dgm