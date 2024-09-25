#pragma once

#include <filesystem>
#include <fsm/Types.hpp>
#include <fsm/detail/BuilderContext.hpp>
#include <fsm/detail/Constants.hpp>
#include <fstream>
#include <print>

namespace fsm
{
    template<class T>
    concept OstreamType =
        std::same_as<T, std::ostream> || std::convertible_to<T, std::ostream>;

    /**
     *  Exports the diagram in plaintext format worded as Mermaid Flowchart
     * diagram You can paste the data here: https://mermaid.live/edit and get a
     * nice looking diagram for your machine.
     *
     *  It does not contain self loops nor names transitions.
     */
    class [[nodiscard]] MermaidExporter final
    {
    public:
        explicit MermaidExporter(std::ostream& stream) : save(stream) {}

        MermaidExporter(const MermaidExporter&) = delete;
        MermaidExporter(MermaidExporter&&) = default;

    public:
        template<BlackboardTypeConcept BbT>
        void exportDiagram(const detail::BuilderContext<BbT>& context)
        {
            auto getFullEntryName =
                [&](const std::string& machineName) -> std::string
            {
                return detail::createFullStateName(
                    machineName, context.machines.at(machineName).entryState);
            };

            auto printTransition =
                [&](const std::string& source,
                    const detail::TransitionContext& destination)
            {
                if (destination.secondary.empty())
                {
                    if (destination.primary.empty())
                    {
                        std::println(
                            save, "  {} -->|finish| {}", source, "___( )");
                    }
                    else if (source != destination.primary)
                    {
                        std::println(
                            save, "  {} --> {}", source, destination.primary);
                    }
                }
                else
                {
                    std::println(
                        save,
                        "  {} -->|{}| {}",
                        source,
                        destination.primary,
                        destination.secondary);
                }
            };

            std::println(save, "flowchart TD");
            std::println(
                save,
                "  _( ) -->|entry| {}",
                getFullEntryName(detail::MAIN_MACHINE_NAME));

            if (context.useGlobalError)
            {
                std::println(
                    save,
                    "  __( ) -->|error: with global entry condition| {}",
                    getFullEntryName(detail::ERROR_MACHINE_NAME));
            }
            else if (context.machines.contains(detail::ERROR_MACHINE_NAME))
            {
                std::println(
                    save,
                    "  __( ) -->|error: no global entry condition| {}",
                    getFullEntryName(detail::ERROR_MACHINE_NAME));
            }

            for (auto&& [machineName, machineContext] : context.machines)
            {
                for (auto&& [stateName, stateContext] : machineContext.states)
                {
                    const auto currentState =
                        detail::createFullStateName(machineName, stateName);

                    for (auto&& condition : stateContext.conditions)
                    {
                        printTransition(currentState, condition.destination);
                    }

                    printTransition(currentState, stateContext.destination);
                }
            }
        }

    private:
        std::ostream& save;
    };
} // namespace fsm