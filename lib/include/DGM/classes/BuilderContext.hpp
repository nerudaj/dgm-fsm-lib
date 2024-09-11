#pragma once

#include <DGM/classes/NonEmptyString.hpp>
#include <DGM/classes/Types.hpp>
#include <format>
#include <map>
#include <string>
#include <vector>

namespace fsm::detail
{
    using StateId = NonEmptyString<char>;
    using MachineId = NonEmptyString<char>;

    struct TransitionContext
    {
        std::string primary = "";
        std::string secondary = "";
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
        std::string entryState;
        std::string currentlyBuiltState;
        std::map<std::string, StateBuilderContext<BbT>> states;
    };

    template<BlackboardTypeConcept BbT>
    struct BuilderContext
    {
        std::string currentlyBuiltMachine;
        std::map<std::string, MachineBuilderContext<BbT>> machines;
        Condition<BbT> errorCondition;
        TransitionContext errorDestination;
        bool useGlobalError = false;
    };
} // namespace fsm::detail
