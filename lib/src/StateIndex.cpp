#include "DGM/classes/StateIndex.hpp"

void dgm::fsm::StateIndex::addNameToIndex(const StateId& name)
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

unsigned dgm::fsm::StateIndex::getStateIndex(const StateId& name) const

{
    if (!nameToId.contains(name))
    {
        throw Error(std::format("Error - state {} has not been defined", name));
    }

    return nameToId.at(name);
}