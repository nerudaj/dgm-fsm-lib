#include <DGM/classes/Error.hpp>
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

std::pair<dgm::fsm::detail::MachineId, dgm::fsm::detail::StateId>
dgm::fsm::detail::getMachineAndStateNameFromFullName(
    const std::string& fullName)
{
    auto separatorIdx = fullName.find(':');
    if (separatorIdx == std::string::npos)
        throw Error(std::format("{} is not a valid full state name", fullName));

    return { fullName.substr(0, separatorIdx),
             fullName.substr(separatorIdx + 1) };
}
