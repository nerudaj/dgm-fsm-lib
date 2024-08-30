#pragma once

#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

namespace dgm
{
    namespace fsm
    {
        struct [[nodiscard]] BlackboardBase
        {
            std::vector<size_t> __stateIdxs;
        };

        template<class T>
        concept BlackboardTypeConcept = std::derived_from<T, BlackboardBase>;

        namespace detail
        {
            template<BlackboardTypeConcept BbT>
            using Action = std::function<void(BbT&)>;

            template<BlackboardTypeConcept BbT>
            using Condition = std::function<void(const BbT&)>;
        } // namespace detail
    }     // namespace fsm
} // namespace dgm