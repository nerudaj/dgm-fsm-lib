#pragma once

#include <DGM/classes/BuilderContext.hpp>
#include <DGM/classes/Error.hpp>
#include <DGM/classes/Helper.hpp>
#include <DGM/classes/StateIndex.hpp>
#include <DGM/classes/Types.hpp>
#include <cassert>
#include <format>
#include <iostream>
#include <map>
#include <optional>
#include <ostream>
#include <print>
#include <ranges>
#include <string>
#include <utility>

namespace dgm
{
    namespace fsm
    {
        template<BlackboardTypeConcept BbT>
        class [[nodiscard]] Fsm final
        {
        private:

        public:
            explicit Fsm(BuilderContext<BbT>&& context)
            {
                auto&& index =
                    detail::createStateIndexFromBuilderContext(context);

                states.resize(index.getSize());

                if (context.useGlobalError)
                {
                    assert(context.errorDestination.secondary.empty());
                    globalErrorTransition = Transition {
                        .onConditionHit = std::move(context.errorCondition),
                        .destinations =
                            compileTransition(context.errorDestination, index),
                    };
                }

                for (auto& [machineName, machineContext] : machines)
                {
                    for (auto& [stateName, stateContext] : machine.states)
                    {
                        auto idx =
                            index.getStateIndex(detail::createFullStateName(
                                machineName, stateName));
                        states[idx] = State {
                            .transitions = compileAllConditionalTransitions(
                                stateContext.conditions, index),
                            .executeBehavior = std::move(stateContext.action),
                            .destinations = compileTransition(
                                stateContext.destination, index),
                        };
                    }
                }
            }

            Fsm(Fsm&&) = delete;
            Fsm(const Fsm&) = delete;

        public:
            void tick(Blackboard& blackboard)
            {
                // TODO: pop stateid

                if (globalErrorTransition
                    && globalErrorTransition->onConditionHit(blackboard))
                {
                    // TODO: transition to error
                    return;
                }

                unsigned stateId = 0;
                assert(states.size() > stateId);
                const auto& state = states.at[stateId];

                for (const auto& transition : transitions)
                {
                    if (transition.onConditionHit(blackboard))
                    {
                        // todo: take transition
                        return;
                    }
                }

                state.executeBehavior(blackboard);
                // TODO: take default transition
            }

            // void tickUntilBehaviorExecuted(Blackboard& blackboard);

        private:
            struct State
            {
                std::vector<Transition> transitions;
                Action<BbT> executeBehavior;
                std::vector<unsigned> destinations;
            };

        private:
            std::vector<unsigned, std::string> stateIdToName;
            std::vector<unsigned, State> states;
            std::optional<Transition> globalErrorTransition;
        };
    } // namespace fsm
} // namespace dgm
