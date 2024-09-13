#pragma once

#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

namespace fsm
{
    /**
     * \brief Base class for blackboard
     *
     * Each blackboard class needs to hold some context
     * specific for the FSM model. This context is stored
     * in the base class, you just need to inherit it publicly
     * for your Blackboard class.
     */
    struct [[nodiscard]] BlackboardBase
    {
        std::vector<size_t> __stateIdxs;
    };

    /**
     * \brief Constraint that checks if a class was derived from BlackboardBase
     */
    template<class T>
    concept BlackboardTypeConcept = std::derived_from<T, BlackboardBase>;

    namespace detail
    {
        template<BlackboardTypeConcept BbT>
        using Action = std::function<void(BbT&)>;

        template<BlackboardTypeConcept BbT>
        using Condition = std::function<bool(const BbT&)>;
    } // namespace detail
} // namespace fsm
