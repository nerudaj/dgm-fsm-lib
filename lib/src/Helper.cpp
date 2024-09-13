#include <fsm/Error.hpp>
#include <fsm/detail/Helper.hpp>

std::string fsm::detail::createFullStateName(
    const std::string& machineName, const std::string& stateName)
{
    return std::format("{}:{}", machineName, stateName);
}

std::pair<std::string, std::string>
fsm::detail::getMachineAndStateNameFromFullName(const std::string& fullName)
{
    auto separatorIdx = fullName.find(':');
    if (separatorIdx == std::string::npos)
        throw Error(std::format("{} is not a valid full state name", fullName));

    return { fullName.substr(0, separatorIdx),
             fullName.substr(separatorIdx + 1) };
}

size_t fsm::detail::popTopState(BlackboardBase& bb)
{
    auto idx = bb.__stateIdxs.back();
    bb.__stateIdxs.pop_back();
    return idx;
}
