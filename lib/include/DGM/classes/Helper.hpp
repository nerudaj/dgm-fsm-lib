#pragma once

#include <DGM/classes/BuilderContext.hpp>
#include <DGM/classes/CompiledContext.hpp>
#include <DGM/classes/StateIndex.hpp>
#include <ranges>

namespace dgm
{
    namespace fsm
    {
        namespace detail
        {
            [[nodiscard]] std::string createFullStateName(
                const MachineId& machineName, const StateId& stateName);

            [[nodiscard]] std::pair<MachineId, StateId>
            getMachineAndStateNameFromFullName(const std::string& fullName);

            template<BlackboardTypeConcept BbT>
            StateIndex createStateIndexFromBuilderContext(
                const BuilderContext<BbT>& context)
            {
                auto&& index = StateIndex();

                for (auto&& [machineName, machineContext] : context.machines)
                {
                    for (auto&& [stateName, stateContext] :
                         machineContext.states)
                    {
                        std::ignore = stateContext;
                        index.addNameToIndex(
                            createFullStateName(machineName, stateName));
                    }
                }

                return index;
            }

            [[nodiscard]] size_t popTopState(BlackboardBase& bb);

            constexpr static void executeTransition(
                BlackboardBase& bb, const CompiledTransition& transition)
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
                    "__main__", context.machines.at("__main__").entryState));
            }
        } // namespace detail
    }     // namespace fsm
} // namespace dgm