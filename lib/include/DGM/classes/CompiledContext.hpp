#pragma once

#include <DGM/classes/Types.hpp>
#include <vector>

namespace dgm
{
    namespace fsm
    {
        namespace detail
        {
            using CompiledTransition = std::vector<size_t>;

            template<BlackboardTypeConcept BbT>
            struct CompiledConditionalTransition
            {
                Condition<BbT> onConditionHit;
                CompiledTransition transition;
            };

            template<BlackboardTypeConcept BbT>
            struct CompiledState
            {
                std::vector<CompiledConditionalTransition<BbT>>
                    conditionalTransitions;
                Action<BbT> executeBehavior;
                CompiledTransition defaultTransition;
            };
        } // namespace detail
    }     // namespace fsm
} // namespace dgm