#pragma once

#include <DGM/classes/Types.hpp>
#include <format>
#include <map>
#include <string>
#include <vector>

namespace dgm
{
    namespace fsm
    {
        namespace detail
        {
            using StateId = std::string;
            using MachineId = std::string;

            struct TransitionContext
            {
                StateId primary = "";
                StateId secondary = "";
            };

            template<BlackboardTypeConcept BbT>
            struct ConditionalTransitionContext
            {
                Condition<BbT> condition;
                TransitionContext destination;
            };

            template<BlackboardTypeConcept BbT>
            struct StateBuilderContext
            {
                std::vector<ConditionalTransitionContext<BbT>> conditions;
                Action<BbT> action;
                TransitionContext destination;
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
                TransitionContext errorDestination;
                bool useGlobalError = false;
            };
        } // namespace detail
    }     // namespace fsm
} // namespace dgm