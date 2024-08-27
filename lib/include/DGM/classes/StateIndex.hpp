#pragma once

#include <DGM/classes/BuilderContext.hpp>
#include <DGM/classes/Error.hpp>
#include <format>
#include <unordered_map>

namespace dgm
{
    namespace fsm
    {
        class [[nodiscard]] StateIndex final
        {
        public:
            StateIndex() = default;
            StateIndex(StateIndex&&) = delete;
            StateIndex(const StateIndex&) = delete;

        public:
            void addNameToIndex(const StateId& name);

            unsigned getStateIndex(const StateId& name) const;

            [[nodiscard]] inline size_t getSize() const noexcept
            {
                return nameToId.size();
            }

        private:
            unsigned cnt = 0;
            std::unordered_map<StateId, unsigned> nameToId;
        };
    } // namespace fsm
} // namespace dgm