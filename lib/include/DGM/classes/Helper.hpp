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

            [[nodiscard]] std::pair<MachineId, StateId>
            getMachineAndStateNameFromFullName(const std::string& fullName);

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

            template<BlackboardTypeConcept BbT>
            std::vector<CompiledState<BbT>> compileMachine(
                BuilderContext<BbT>& context, const StateIndex& index)
            {
                return index.getIndexedStateNames()
                       | std::views::transform(
                           [](const std::string& fullName) {
                               return getMachineAndStateNameFromFullName(
                                   fullName);
                           })
                       | std::views::transform(
                           [&context, &index](
                               const std::pair<MachineId, StateId>& namePair)
                           {
                               return compileState(
                                   context.machines[namePair.first]
                                       .states[namePair.second],
                                   index);
                           })
                       | std::ranges::to<std::vector>();
            }

        } // namespace detail
    }     // namespace fsm
} // namespace dgm