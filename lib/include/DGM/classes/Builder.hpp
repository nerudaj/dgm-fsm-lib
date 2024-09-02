#pragma once

#include <DGM/classes/BuilderContext.hpp>
#include <DGM/classes/Error.hpp>
#include <DGM/classes/Fsm.hpp>
#include <DGM/classes/Helper.hpp>
#include <DGM/classes/Types.hpp>

namespace dgm
{
    namespace fsm
    {
        namespace detail
        {
            constexpr const char* EMPTY_MACHINE_ERROR =
                "Machine name cannot be empty";
            constexpr const char* EMPTY_STATE_ERROR =
                "State name cannot be empty";

            template<BlackboardTypeConcept BbT, bool IsSubmachine>
            class StateBuilder;

            template<BlackboardTypeConcept BbT, bool IsSubmachine>
            class MachineBuilder;

            template<BlackboardTypeConcept BbT>
            class MainBuilder;

            template<BlackboardTypeConcept BbT>
            class [[nodiscard]] FinalBuilder final
            {
            public:
                explicit FinalBuilder(BuilderContext<BbT>&& context)
                    : context(std::move(context))
                {
                }

                FinalBuilder(FinalBuilder&&) = delete;

                FinalBuilder(const FinalBuilder&) = delete;

            public:
                Fsm<BbT> build()
                {
                    return Fsm(std::move(context));
                }

            private:
                BuilderContext<BbT> context;
            };

            template<BlackboardTypeConcept BbT, bool IsSubmachine>
            class [[nodiscard]] DefaultTransitionBuilder final
            {
            public:
                explicit DefaultTransitionBuilder(
                    BuilderContext<BbT>&& context) noexcept
                    : context(std::move(context))
                {
                }

                DefaultTransitionBuilder(DefaultTransitionBuilder&&) = delete;

                DefaultTransitionBuilder(const DefaultTransitionBuilder&) =
                    delete;

            public:
                auto andGoToState(StateId name)
                {
                    if (name.empty()) throw Error(EMPTY_STATE_ERROR);

                    context.getCurrentlyBuiltState().destination.primary =
                        createFullStateName(
                            context.currentlyBuiltMachine, name);
                    return MachineBuilder<BbT, IsSubmachine>(
                        std::move(context));
                }

                // TODO: make this two methods
                auto andGoToMachineThenReturnToState(
                    MachineId machineName, StateId stateName)
                {
                    if (machineName.empty()) throw Error(EMPTY_MACHINE_ERROR);

                    if (stateName.empty()) throw Error(EMPTY_STATE_ERROR);

                    if (machineName == context.currentlyBuiltMachine)
                        throw Error(
                            "When transition to machine, you cannot re-enter "
                            "the current machine");

                    if (!context.machines.contains(machineName))
                        throw Error(std::format(
                            "Trying to go to machine called {} that is not "
                            "defined yet",
                            machineName));

                    auto& state = context.getCurrentlyBuiltState();
                    state.destination.primary = createFullStateName(
                        machineName,
                        context.machines.at(machineName).entryState);
                    state.destination.secondary = createFullStateName(
                        context.currentlyBuiltMachine, stateName);

                    return MachineBuilder<BbT, IsSubmachine>(
                        std::move(context));
                }

                auto andLoop()
                {
                    return andGoToState(
                        context.getCurrentlyBuiltMachine().currentlyBuiltState);
                }

                auto andFinish()
                {
                    return MachineBuilder<BbT, IsSubmachine>(
                        std::move(context));
                }

            private:
                BuilderContext<BbT> context;
            };

            template<BlackboardTypeConcept BbT, bool IsSubmachine>
            class [[nodiscard]] ConditionTransitionBuilder final
            {
            public:
                ConditionTransitionBuilder(
                    BuilderContext<BbT>&& context, Condition<BbT>&& condition)
                    : context(std::move(context))
                    , condition(std::move(condition))
                {
                }

                ConditionTransitionBuilder(ConditionTransitionBuilder&&) =
                    delete;

                ConditionTransitionBuilder(const ConditionTransitionBuilder&) =
                    delete;

            public:
                auto goToState(StateId name)
                {
                    if (name.empty()) throw Error(EMPTY_STATE_ERROR);

                    context.getCurrentlyBuiltState().conditions.push_back(
                        ConditionalTransitionContext {
                            .condition = condition,
                            .destination = TransitionContext {
                                .primary = createFullStateName(
                                    context.currentlyBuiltMachine, name) } });
                    return StateBuilder<BbT, IsSubmachine>(std::move(context));
                }

                auto goToMachineAndThenToState(
                    MachineId machineName, StateId stateName)
                {
                    if (machineName.empty()) throw Error(EMPTY_MACHINE_ERROR);

                    if (stateName.empty()) throw Error(EMPTY_STATE_ERROR);

                    if (machineName == context.currentlyBuiltMachine)
                        throw Error(
                            "When transition to machine, you cannot re-enter "
                            "the current machine");

                    if (!context.machines.contains(machineName))
                        throw Error(std::format(
                            "Trying to go to machine called {} that is not "
                            "defined yet",
                            machineName));

                    context.getCurrentlyBuiltState().conditions.push_back(
                        ConditionalTransitionContext {
                            .condition = std::move(condition),
                            .destination =
                                TransitionContext {
                                    .primary = createFullStateName(
                                        machineName,
                                        context.machines.at(machineName)
                                            .entryState),
                                    .secondary = createFullStateName(
                                        context.currentlyBuiltMachine,
                                        stateName) },
                        });
                    return StateBuilder<BbT, IsSubmachine>(std::move(context));
                }

                auto finish()
                {
                    context.getCurrentlyBuiltState().conditions.push_back(
                        ConditionalTransitionContext {
                            .condition = std::move(condition),
                        });
                    return StateBuilder<BbT, IsSubmachine>(std::move(context));
                }

            private:
                BuilderContext<BbT> context;
                Condition<BbT> condition;
            };

            template<BlackboardTypeConcept BbT, bool IsSubmachine>
            class [[nodiscard]] StateBuilderBeforePickingAnything final
            {
            public:
                explicit StateBuilderBeforePickingAnything(
                    BuilderContext<BbT>&& context) noexcept
                    : context(std::move(context))
                {
                }

                StateBuilderBeforePickingAnything(
                    StateBuilderBeforePickingAnything&&) = delete;

                StateBuilderBeforePickingAnything(
                    const StateBuilderBeforePickingAnything&) = delete;

            public:
                auto when(Condition<BbT>&& condition)
                {
                    return ConditionTransitionBuilder<BbT, IsSubmachine>(
                        std::move(context), std::move(condition));
                }

                auto exec(Action<BbT>&& action)
                {
                    context.getCurrentlyBuiltState().action = std::move(action);
                    return DefaultTransitionBuilder<BbT, IsSubmachine>(
                        std::move(context));
                }

            private:
                BuilderContext<BbT> context;
            };

            template<BlackboardTypeConcept BbT, bool IsSubmachine>
            class [[nodiscard]] StateBuilder final
            {
            public:
                explicit StateBuilder(BuilderContext<BbT>&& context) noexcept
                    : context(std::move(context))
                {
                }

                StateBuilder(StateBuilder&&) = delete;

                StateBuilder(const StateBuilder&) = delete;

            public:
                auto orWhen(Condition<BbT>&& condition)
                {
                    return ConditionTransitionBuilder<BbT, IsSubmachine>(
                        std::move(context), std::move(condition));
                }

                auto otherwiseExec(Action<BbT>&& action)
                {
                    context.getCurrentlyBuiltState().action = std::move(action);
                    return DefaultTransitionBuilder<BbT, IsSubmachine>(
                        std::move(context));
                }

            private:
                BuilderContext<BbT> context;
            };

            template<BlackboardTypeConcept BbT, bool IsSubmachine>
            class [[nodiscard]] MachineBuilderPreEntryPoint final
            {
            public:
                explicit MachineBuilderPreEntryPoint(
                    BuilderContext<BbT>&& context) noexcept
                    : context(std::move(context))
                {
                }

                MachineBuilderPreEntryPoint(MachineBuilderPreEntryPoint&&) =
                    delete;

                MachineBuilderPreEntryPoint(
                    const MachineBuilderPreEntryPoint&) = delete;

            public:
                auto withEntryState(StateId name)
                {
                    if (name.empty()) throw Error(EMPTY_STATE_ERROR);

                    auto& machine = context.getCurrentlyBuiltMachine();
                    machine.entryState = name;
                    machine.currentlyBuiltState = name;
                    machine.states[name] = {};
                    return StateBuilderBeforePickingAnything<BbT, IsSubmachine>(
                        std::move(context));
                }

            private:
                BuilderContext<BbT> context;
            };

            template<BlackboardTypeConcept BbT, bool IsSubmachine>
            class [[nodiscard]] MachineBuilder final
            {
            public:
                explicit MachineBuilder(BuilderContext<BbT>&& context) noexcept
                    : context(std::move(context))
                {
                }

                MachineBuilder(MachineBuilder&&) = delete;
                MachineBuilder(const MachineBuilder&) = delete;

            public:
                auto withState(StateId name)
                {
                    if (name.empty()) throw Error(EMPTY_STATE_ERROR);

                    auto& machine = context.getCurrentlyBuiltMachine();

                    if (machine.states.contains(name))
                        throw Error(std::format(
                            "Trying to redeclare state with name {} in machine "
                            "{}",
                            name,
                            context.currentlyBuiltMachine));

                    machine.currentlyBuiltState = name;
                    machine.states[name] = {};
                    return StateBuilderBeforePickingAnything<BbT, IsSubmachine>(
                        std::move(context));
                }

                auto done()
                {
                    if constexpr (IsSubmachine)
                    {
                        return MainBuilder<BbT>(std::move(context));
                    }
                    else
                    {
                        return FinalBuilder<BbT>(std::move(context));
                    }
                }

            protected:
                BuilderContext<BbT> context;
            };

            template<BlackboardTypeConcept BbT>
            class [[nodiscard]] MainBuilder final
            {
            public:
                explicit MainBuilder(BuilderContext<BbT>&& context)
                    : context(std::move(context))
                {
                }

                MainBuilder(MainBuilder&&) = delete;
                MainBuilder(const MainBuilder&) = delete;

            public:
                template<bool IsSubmachine = true>
                auto withSubmachine(MachineId name)
                {
                    if (name.empty()) throw Error(EMPTY_MACHINE_ERROR);

                    if (context.machines.contains(name))
                        throw Error(std::format(
                            "Trying to redeclare machine with name {}", name));

                    context.currentlyBuiltMachine = name;
                    context.machines[name] = {};
                    return MachineBuilderPreEntryPoint<BbT, IsSubmachine>(
                        std::move(context));
                }

                auto withMainMachine()
                {
                    return withSubmachine<false>("__main__");
                }

            private:
                BuilderContext<BbT> context;
            };

            template<BlackboardTypeConcept BbT>
            class [[nodiscard]] GlobalErrorConditionBuilder final
            {
            public:
                explicit GlobalErrorConditionBuilder(
                    Condition<BbT>&& condition) noexcept
                    : condition(std::move(condition))
                {
                }

                GlobalErrorConditionBuilder(GlobalErrorConditionBuilder&&) =
                    delete;
                GlobalErrorConditionBuilder(
                    const GlobalErrorConditionBuilder&) = delete;

            public:
                auto goToState(StateId name)
                {
                    if (name.empty()) throw Error(EMPTY_STATE_ERROR);

                    return MainBuilder<BbT>(BuilderContext<BbT> {
                        .errorCondition = std::move(condition),
                        .errorDestination =
                            TransitionContext {
                                .primary =
                                    createFullStateName("__main__", name),
                            },
                        .useGlobalError = true,
                    });
                }

            private:
                Condition<BbT> condition;
            };
        } // namespace detail

        template<BlackboardTypeConcept BbT>
        class [[nodiscard]] Builder final
        {
        public:
            Builder() = default;
            Builder(Builder&&) = delete;
            Builder(const Builder&) = delete;

        public:
            auto withNoGlobalErrorCondition()
            {
                return detail::MainBuilder<BbT>(detail::BuilderContext<BbT> {});
            }

            /*auto withGlobalErrorCondition(detail::Condition<BbT>&& condition)
            {
                return detail::GlobalErrorConditionBuilder(
                    std::move(condition));
            }*/
        };
    } // namespace fsm
} // namespace dgm
