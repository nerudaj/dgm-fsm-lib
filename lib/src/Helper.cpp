#include <DGM/classes/Helper.hpp>

std::vector<unsigned> dgm::fsm::compileDestinations(
    const Destination& destination, const StateIndex& index)
{
    if (destination.primary.empty()) return {};
    if (destination.secondary.empty())
        return { index.getStateIndex(destination.primary) };
    return { index.getStateIndex(destination.primary),
             index.getStateIndex(destination.secondary) };
}