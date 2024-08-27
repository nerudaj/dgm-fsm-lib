#pragma once

#include <DGM/classes/BuilderContext.hpp>
#include <DGM/classes/Types.hpp>

namespace dgm
{
    namespace fsm
    {
        namespace detail
        {
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
                BuilderContext<BbT> build()
                {
                    return std::move(context);
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
                    // TODO: exceptions
                    assert(machineName != context.currentlyBuiltMachine);
                    assert(context.machines.contains(machineName));

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
                    context.getCurrentlyBuiltState().conditions.push_back(
                        ConditionalTransition {
                            .condition = condition,
                            .destination = Destination {
                                .primary = createFullStateName(
                                    context.currentlyBuiltMachine, name) } });
                    return StateBuilder<BbT, IsSubmachine>(std::move(context));
                }

                auto goToMachineAndThenToState(
                    MachineId machineName, StateId stateName)
                {
                    context.getCurrentlyBuiltState().conditions.push_back(
                        ConditionalTransition {
                            .condition = std::move(condition),
                            .destination =
                                Destination {
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
                        ConditionalTransition {
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
                    auto& machine = context.getCurrentlyBuiltMachine();

                    assert(!machine.states.contains(name));

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
                    assert(!context.machines.contains(name));
                    assert(!name.empty());

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
                    assert(!name.empty());
                    return MainBuilder<BbT>(BuilderContext<BbT> {
                        .errorCondition = std::move(condition),
                        .errorDestination =
                            Destination {
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
                return detail::MainBuilder<BbT>(BuilderContext<BbT> {});
            }

            auto withGlobalErrorCondition(Condition<BbT>&& condition)
            {
                return detail::GlobalErrorConditionBuilder(
                    std::move(condition));
            }
        };
    } // namespace fsm
} // namespace dgm
