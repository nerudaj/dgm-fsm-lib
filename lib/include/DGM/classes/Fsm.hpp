#pragma once

#include <DGM/classes/BuilderContext.hpp>
#include <DGM/classes/Compiler.hpp>
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
            explicit Fsm(detail::BuilderContext<BbT>&& context)
            {
                auto&& index =
                    detail::createStateIndexFromBuilderContext(context);

                entryStateIdx = detail::getEntryStateIdx(context, index);
                states = detail::Compiler::compileMachine(context, index);
                stateIdToName = index.getIndexedStateNames();
            }

            Fsm(Fsm&&) = delete;
            Fsm(const Fsm&) = delete;

        public:
            /**
             *  Sets up blackboard so the current state is the entry
             *  state of the main machine
             */
            void start(BbT& blackboard)
            {
                blackboard.__stateIdxs = { entryStateIdx };
            }

            void tick(BbT& blackboard)
            {
                if (blackboard.__stateIdxs.empty()) return;

                auto currentStateIdx = detail::popTopState(blackboard);

                assert(currentStateIdx < states.size());
                auto& state = states[currentStateIdx];

                for (const auto& condition : state.conditionalTransitions)
                {
                    if (condition.onConditionHit(blackboard))
                    {
                        std::println(
                            "{},Condition {} hit,->{}",
                            stateIdToName[currentStateIdx],
                            condition.onConditionHit.target_type().name(),
                            getTransitionLog(condition.transition));

                        detail::executeTransition(
                            blackboard, condition.transition);
                        return;
                    }
                }

                state.executeBehavior(blackboard);
                std::println(
                    "{},Behavior {} executed,->{}",
                    stateIdToName[currentStateIdx],
                    state.executeBehavior.target_type().name(),
                    getTransitionLog(state.defaultTransition));
                detail::executeTransition(blackboard, state.defaultTransition);
            }

            [[nodiscard]] constexpr bool
            isFinished(const Blackboard& blackboard) const noexcept
            {
                return blackboard.__stateIdxs.empty();
            }

            // void tickUntilBehaviorExecuted(Blackboard& blackboard);

        private:
            std::string
            getTransitionLog(const detail::CompiledTransition& transition)
            {
                if (transition.isEmpty())
                    return "Finishing";
                else if (transition.getSize() == 1u)
                    return stateIdToName[transition[0]];
                return stateIdToName[transition[0]] + "/"
                       + stateIdToName[transition[1]];
            }

        private:
            std::vector<std::string> stateIdToName;
            std::vector<detail::CompiledState<BbT>> states;
            size_t entryStateIdx;
        };
    } // namespace fsm
} // namespace dgm
