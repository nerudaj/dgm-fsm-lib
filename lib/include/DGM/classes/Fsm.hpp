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
            explicit Fsm(detail::BuilderContext<BbT>&& context)
            {
                auto&& index =
                    detail::createStateIndexFromBuilderContext(context);

                states = detail::compileMachine(context, index);
                stateIdToName = index.getIndexedStateNames();
            }

            Fsm(Fsm&&) = delete;
            Fsm(const Fsm&) = delete;

        public:
            void tick(BbT& blackboard)
            {
                /*
                // TODO: pop stateid

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
                */
            }

            // void tickUntilBehaviorExecuted(Blackboard& blackboard);

        private:
            std::vector<std::string> stateIdToName;
            std::vector<detail::CompiledState<BbT>> states;
        };
    } // namespace fsm
} // namespace dgm
