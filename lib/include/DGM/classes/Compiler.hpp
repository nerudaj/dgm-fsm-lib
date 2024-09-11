#pragma once

#include <DGM/classes/BuilderContext.hpp>
#include <DGM/classes/CompiledContext.hpp>
#include <DGM/classes/Helper.hpp>
#include <DGM/classes/StateIndex.hpp>
#include <DGM/classes/Types.hpp>
#include <ranges>

namespace fsm::detail
{
    class Compiler final
    {
    public:
        static [[nodiscard]] CompiledTransition compileTransition(
            const TransitionContext& destination, const StateIndex& index);

        template<BlackboardTypeConcept BbT>
        static [[nodiscard]] std::vector<CompiledConditionalTransition<BbT>>
        compileAllConditionalTransitions(
            std::vector<ConditionalTransitionContext<BbT>>& transitions,
            const StateIndex& index)
        {
            return transitions
                   | std::views::transform(
                       [&](ConditionalTransitionContext<BbT>& transition)
                       {
                           return CompiledConditionalTransition {
                               .onConditionHit =
                                   std::move(transition.condition),
                               .transition = compileTransition(
                                   transition.destination, index),
                           };
                       })
                   | std::ranges::to<std::vector>();
        }

        template<BlackboardTypeConcept BbT>
        static [[nodiscard]] CompiledState<BbT>
        compileState(StateBuilderContext<BbT>& state, const StateIndex& index)
        {
            return CompiledState<BbT> {
                .conditionalTransitions =
                    compileAllConditionalTransitions(state.conditions, index),
                .executeBehavior = std::move(state.action),
                .defaultTransition =
                    compileTransition(state.destination, index),
            };
        }

        template<BlackboardTypeConcept BbT>
        static [[nodiscard]] std::vector<CompiledState<BbT>>
        compileMachine(BuilderContext<BbT>& context, const StateIndex& index)
        {
            return index.getIndexedStateNames()
                   | std::views::transform(
                       [](const std::string& fullName)
                       { return getMachineAndStateNameFromFullName(fullName); })
                   | std::views::transform(
                       [&context,
                        &index](const std::pair<MachineId, StateId>& namePair)
                       {
                           return compileState(
                               context.machines[namePair.first]
                                   .states[namePair.second],
                               index);
                       })
                   | std::ranges::to<std::vector>();
        }
    };
} // namespace fsm::detail
