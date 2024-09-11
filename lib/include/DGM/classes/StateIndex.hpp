#pragma once

#include <DGM/classes/BuilderContext.hpp>
#include <DGM/classes/Error.hpp>
#include <format>
#include <unordered_map>

namespace fsm::detail
{
    class [[nodiscard]] StateIndex final
    {
    public:
        StateIndex() = default;
        StateIndex(StateIndex&&) = default;
        StateIndex(const StateIndex&) = delete;

    public:
        void addNameToIndex(const std::string& name);

        unsigned getStateIndex(const std::string& name) const;

        [[nodiscard]] inline size_t getSize() const noexcept
        {
            return nameToId.size();
        }

        std::vector<std::string> getIndexedStateNames() const;

    private:
        unsigned cnt = 0;
        std::unordered_map<std::string, unsigned> nameToId;
    };

} // namespace fsm::detail
