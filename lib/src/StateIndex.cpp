#include "DGM/classes/StateIndex.hpp"
#include <algorithm>
#include <ranges>

void dgm::fsm::detail::StateIndex::addNameToIndex(const StateId& name)
{
    if (nameToId.contains(name))
    {
        throw Error(std::format(
            "Precondition error - name {} is already present in "
            "StateIndex",
            name));
    }

    nameToId[name] = cnt++;
}

unsigned dgm::fsm::detail::StateIndex::getStateIndex(const StateId& name) const

{
    if (!nameToId.contains(name))
    {
        throw Error(std::format("Error - state {} has not been defined", name));
    }

    return nameToId.at(name);
}

std::vector<std::string>
dgm::fsm::detail::StateIndex::getIndexedStateNames() const
{

    auto&& indexedStateNames =
        std::views::transform(
            nameToId,
            [](const std::pair<std::string, unsigned>& kv)
                -> std::pair<unsigned, std::string> {
                return { kv.second, kv.first };
            })
        | std::ranges::to<std::vector>();

    std::ranges::sort(
        indexedStateNames,
        [](const std::pair<unsigned, std::string>& a,
           const std::pair<unsigned, std::string>& b) -> bool
        { return a.first < b.first; });

    return indexedStateNames
           | std::views::transform(
               [](const std::pair<unsigned, std::string>& kv) -> std::string
               { return kv.second; })
           | std::ranges::to<std::vector>();
}