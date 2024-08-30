#pragma once

#include <DGM/classes/Types.hpp>
#include <cassert>
#include <vector>

namespace dgm
{
    namespace fsm
    {
        namespace detail
        {
            class [[nodiscard]] CompiledTransition final
            {
            public:
                constexpr CompiledTransition(
                    std::initializer_list<size_t> items) noexcept
                {
                    assert(items.size() <= 2u);
                    std::copy(items.begin(), items.end(), std::begin(data));
                    size = items.size();
                }

                CompiledTransition(CompiledTransition&&) = default;
                CompiledTransition(const CompiledTransition&&) = delete;

            public:
                constexpr [[nodiscard]] auto begin(this auto&& self) noexcept
                {
                    return std::begin(self.data);
                }

                constexpr [[nodiscard]] auto end(this auto&& self) noexcept
                {
                    return std::end(self.data);
                }

                constexpr [[nodiscard]] size_t getSize() const noexcept
                {
                    return size;
                }

                constexpr [[nodiscard]] bool isEmpty() const noexcept
                {
                    return size == 0;
                }

                constexpr [[nodiscard]] auto&&
                operator[](this auto&& self, size_t index) noexcept
                {
                    assert(index < self.size);
                    return self.data[index];
                }

            private:
                size_t data[2] = { 0u, 0u };
                size_t size = 0;
            };

            template<BlackboardTypeConcept BbT>
            struct [[nodiscard]] CompiledConditionalTransition final
            {
                Condition<BbT> onConditionHit;
                CompiledTransition transition;
            };

            template<BlackboardTypeConcept BbT>
            struct [[nodiscard]] CompiledState final
            {
                std::vector<CompiledConditionalTransition<BbT>>
                    conditionalTransitions;
                Action<BbT> executeBehavior;
                CompiledTransition defaultTransition;
            };
        } // namespace detail
    }     // namespace fsm
} // namespace dgm