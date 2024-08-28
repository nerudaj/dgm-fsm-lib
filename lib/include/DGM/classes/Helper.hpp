#pragma once

#include <DGM/classes/BuilderContext.hpp>
#include <DGM/classes/CompiledContext.hpp>
#include <DGM/classes/StateIndex.hpp>
#include <ranges>

namespace dgm
{
    namespace fsm
    {
        namespace detail
        {
            [[nodiscard]] std::string createFullStateName(
                const MachineId& machineName, const StateId& stateName);

            template<BlackboardTypeConcept BbT>
            StateIndex createStateIndexFromBuilderContext(
                const BuilderContext<BbT>& context)
            {
                auto&& index = StateIndex();

                for (auto&& [machineName, machineContext] : context.machines)
                {
                    for (auto&& [stateName, stateContext] :
                         machineContext.states)
                    {
                        std::ignore = stateContext;
                        index.addNameToIndex(
                            createFullStateName(machineName, stateName));
                    }
                }

                return index;
            }

            CompiledTransition compileTransition(
                const TransitionContext& destination, const StateIndex& index);

            template<BlackboardTypeConcept BbT>
            std::vector<CompiledConditionalTransition<BbT>>
            compileAllConditionalTransitions(
                std::vector<ConditionalTransitionContext<BbT>>& transitions,
                const StateIndex& index)
            {
                return transitions
                       | std::views::transform(
                           [&](ConditionalTransitionContext& transition)
                           {
                               return CompiledConditionalTransition {
                                   .onConditionHit =
                                       std::move(transition.condition),
                                   .transition = compileTransition(
                                       transition.destination, index),
                               };
                           });
            }

            template<BlackboardTypeConcept BbT>
            CompiledState<BbT> compileState(
                StateBuilderContext<BbT>& state, const StateIndex& index)
            {
                return CompiledState<BbT> {
                    .conditionalTransitions = compileAllConditionalTransitions(
                        state.conditions, index),
                    .executeBehavior = std::move(state.action),
                    .defaultTransition =
                        compileTransition(state.destination, index),
                };
            }

        } // namespace detail
    }     // namespace fsm
} // namespace dgm