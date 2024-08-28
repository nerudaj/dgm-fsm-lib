#include <DGM/classes/Helper.hpp>

dgm::fsm::detail::CompiledTransition dgm::fsm::detail::compileTransition(
    const TransitionContext& destination, const StateIndex& index)
{
    if (destination.secondary.empty())
        if (destination.primary.empty())
            return {};
        else
            return { index.getStateIndex(destination.primary) };
    return { index.getStateIndex(destination.primary),
             index.getStateIndex(destination.secondary) };
}

std::string dgm::fsm::detail::createFullStateName(
    const MachineId& machineName, const StateId& stateName)
{
    return machineName + ":" + stateName;
}
