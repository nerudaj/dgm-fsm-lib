#pragma once

#include <fsm/detail/BuilderContext.hpp>
#include <fsm/detail/CompiledContext.hpp>
#include <fsm/detail/Constants.hpp>
#include <fsm/detail/StateIndex.hpp>
#include <ranges>

namespace fsm::detail
{
    [[nodiscard]] std::string createFullStateName(
        const std::string& machineName, const std::string& stateName);

    [[nodiscard]] std::pair<std::string, std::string>
    getMachineAndStateNameFromFullName(const std::string& fullName);

    template<BlackboardTypeConcept BbT>
    void updateIndexWithMachineContext(
        StateIndex& index,
        const std::string& machineName,
        const MachineBuilderContext<BbT>& context)
    {
        for (auto&& [stateName, stateContext] : context.states)
        {
            std::ignore = stateContext;
            index.addNameToIndex(createFullStateName(machineName, stateName));
        }
    }

    template<BlackboardTypeConcept BbT>
    StateIndex
    createStateIndexFromBuilderContext(const BuilderContext<BbT>& context)
    {
        auto&& index = StateIndex();

        // Make sure the error machine is processed first so the implementation
        // of Fsm::isErrored is trivial
        if (context.machines.contains(ERROR_MACHINE_NAME))
            updateIndexWithMachineContext(
                index,
                ERROR_MACHINE_NAME,
                context.machines.at(ERROR_MACHINE_NAME));

        for (auto&& [machineName, machineContext] : context.machines)
        {
            if (machineName == ERROR_MACHINE_NAME) continue;

            updateIndexWithMachineContext(index, machineName, machineContext);
        }

        return index;
    }

    [[nodiscard]] size_t popTopState(BlackboardBase& bb);

    constexpr static void
    executeTransition(BlackboardBase& bb, const CompiledTransition& transition)
    {
        auto reverseTransition = std::ranges::reverse_view(transition);
        bb.__stateIdxs.insert(
            bb.__stateIdxs.end(),
            reverseTransition.begin(),
            reverseTransition.end());
    }

    template<BlackboardTypeConcept BbT>
    [[nodiscard]] size_t getEntryStateIdx(
        const BuilderContext<BbT>& context, const StateIndex& index)
    {
        return index.getStateIndex(createFullStateName(
            MAIN_MACHINE_NAME,
            context.machines.at(MAIN_MACHINE_NAME).entryState));
    }

    template<BlackboardTypeConcept BbT>
    [[nodiscard]] size_t getErrorStatesCount(const BuilderContext<BbT>& context)
    {
        return context.machines.contains(ERROR_MACHINE_NAME)
                   ? context.machines.at(ERROR_MACHINE_NAME).states.size()
                   : size_t {};
    }
} // namespace fsm::detail
