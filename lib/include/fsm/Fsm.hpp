#pragma once

#include <cassert>
#include <format>
#include <fsm/Error.hpp>
#include <fsm/Types.hpp>
#include <fsm/detail/BuilderContext.hpp>
#include <fsm/detail/Compiler.hpp>
#include <fsm/detail/Helper.hpp>
#include <fsm/detail/StateIndex.hpp>
#include <iostream>
#include <map>
#include <optional>
#include <ostream>
#include <print>
#include <ranges>
#include <string>
#include <utility>

namespace fsm
{
    template<BlackboardTypeConcept BbT>
    class [[nodiscard]] Fsm final
    {
    private:

    public:
        explicit Fsm(
            const detail::StateIndex& index,
            detail::BuilderContext<BbT>&& context)
            : stateIdToName(index.getIndexedStateNames())
            , states(detail::Compiler::compileMachine(context, index))
            , entryStateIdx(detail::getEntryStateIdx(context, index))
            , errorStateCount(detail::getErrorStatesCount(context))
            , globalErrorTransition(
                  detail::Compiler::compileGlobalErrorTransition<BbT>(
                      context, index))
        {
        }

        Fsm(Fsm&&) = delete;
        Fsm(const Fsm&) = delete;

    public:
        /**
         *  Sets up blackboard so the current state is the entry
         *  state of the main machine
         */
        void initBlackboard(BbT& blackboard)
        {
            blackboard.__stateIdxs = { entryStateIdx };
        }

        void tick(BbT& blackboard)
        {
            if (blackboard.__stateIdxs.empty()) return;

            auto currentStateIdx = detail::popTopState(blackboard);

            if (!isErrorStateIdx(currentStateIdx)
                && globalErrorTransition.onConditionHit(blackboard))
            {
                blackboard.__stateIdxs.clear();
                detail::executeTransition(
                    blackboard, globalErrorTransition.transition);
                return;
            }

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

                    if (isErrorTransition(condition.transition))
                    {
                        blackboard.__stateIdxs.clear();
                    }

                    detail::executeTransition(blackboard, condition.transition);
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

        [[nodiscard]] constexpr bool
        isErrored(const Blackboard& blackboard) const noexcept
        {
            return !blackboard.__stateIdxs.empty()
                   && isErrorStateIdx(blackboard.__stateIdxs.back());
        }

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

        [[nodiscard]] constexpr bool isErrorStateIdx(size_t idx) const noexcept
        {
            return idx < errorStateCount;
        }

        [[nodiscard]] constexpr bool isErrorTransition(
            const detail::CompiledTransition& transition) const noexcept
        {
            return transition.getSize() == 1u && isErrorStateIdx(transition[0]);
        }

    private:
        std::vector<std::string> stateIdToName;
        std::vector<detail::CompiledState<BbT>> states;
        size_t entryStateIdx = 0;
        size_t errorStateCount = 0;
        detail::CompiledConditionalTransition<BbT> globalErrorTransition;
    };
} // namespace fsm
