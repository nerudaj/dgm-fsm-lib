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
        // TODO: move all to detail

        template<class T>
        concept BlackboardTypeConcept = true;

        template<BlackboardTypeConcept BbT>
        using Action = std::function<void(BbT&)>;

        template<BlackboardTypeConcept BbT>
        using Condition = std::function<void(const BbT&)>;
    } // namespace fsm
} // namespace dgm