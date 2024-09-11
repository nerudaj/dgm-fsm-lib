#pragma once

#include <DGM/classes/BuilderContext.hpp>
#include <DGM/classes/BuilderContextHelper.hpp>
#include <DGM/classes/Error.hpp>
#include <DGM/classes/Fsm.hpp>
#include <DGM/classes/Helper.hpp>
#include <DGM/classes/Types.hpp>

namespace fsm::detail
{
    template<BlackboardTypeConcept BbT, bool IsSubmachine, bool IsErrorMachine>
    class StateBuilder;

    template<BlackboardTypeConcept BbT, bool IsSubmachine, bool IsErrorMachine>
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
            for (auto&& [_, machineContext] : context.machines)
            {
                for (auto&& [__, state] : machineContext.states)
                {
                    for (auto&& condition : state.conditions)
                    {
                        if (isRestartTransition(condition.destination))
                            setPrimaryTransitionDestinationToMainEntryPoint(
                                condition.destination);
                    }

                    if (isRestartTransition(state.destination))
                        setPrimaryTransitionDestinationToMainEntryPoint(
                            state.destination);
                }
            }

            return Fsm(std::move(context));
        }

    private:
        void setPrimaryTransitionDestinationToMainEntryPoint(
            TransitionContext& destination)
        {
            destination.primary = createFullStateName(
                "__main__", context.machines.at("__main__").entryState);
        }

        [[nodiscard]] constexpr bool
        isRestartTransition(const TransitionContext& transition) const noexcept
        {
            return transition.primary == "__error__:__restart__";
        }

    private:
        BuilderContext<BbT> context;
    };

    template<
        BlackboardTypeConcept BbT,
        bool IsSubmachine,
        bool BuildDefaultTransition = true>
    class [[nodiscard]] MachineBackTransitionBuilder final
    {
    public:
        MachineBackTransitionBuilder(
            BuilderContext<BbT>&& context, MachineId targetMachineName) noexcept
            : context(std::move(context)), targetMachineName(targetMachineName)
        {
            assert(BuildDefaultTransition);
        }

        MachineBackTransitionBuilder(
            BuilderContext<BbT>&& context,
            MachineId targetMachineName,
            Condition<BbT>&& condition) noexcept
            : context(std::move(context))
            , targetMachineName(targetMachineName)
            , condition(std::move(condition))
        {
            assert(!BuildDefaultTransition);
        }

        MachineBackTransitionBuilder(MachineBackTransitionBuilder&&) = default;

        MachineBackTransitionBuilder(const MachineBackTransitionBuilder&) =
            delete;

    public:
        auto thenGoToState(StateId stateName)
        {
            auto&& destination = TransitionContext {
                .primary = createFullStateName(
                    targetMachineName,
                    context.machines.at(targetMachineName).entryState),
                .secondary = createFullStateName(
                    context.currentlyBuiltMachine, stateName),
            };

            if constexpr (BuildDefaultTransition)
            {
                getCurrentlyBuiltState(context).destination =
                    std::move(destination);

                return MachineBuilder<BbT, IsSubmachine>(std::move(context));
            }
            else
            {
                getCurrentlyBuiltState(context).conditions.push_back(
                    ConditionalTransitionContext {
                        .condition = std::move(condition),
                        .destination = std::move(destination),
                    });
                return StateBuilder<BbT, IsSubmachine>(std::move(context));
            }
        }

    private:
        BuilderContext<BbT> context;
        MachineId targetMachineName;
        Condition<BbT> condition;
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

        DefaultTransitionBuilder(const DefaultTransitionBuilder&) = delete;

    public:
        auto andGoToState(StateId name)
        {
            return andGoToStateInternal(name);
        }

        auto andGoToMachine(MachineId machineName)
        {
            if (machineName.get() == context.currentlyBuiltMachine)
                throw Error(
                    "When transition to machine, you cannot re-enter "
                    "the current machine");

            if (!context.machines.contains(machineName))
                throw Error(std::format(
                    "Trying to go to machine called {} that is not "
                    "defined yet",
                    machineName.get()));

            return MachineBackTransitionBuilder<BbT, IsSubmachine>(
                std::move(context), machineName);
        }

        auto andLoop()
        {
            return andGoToStateInternal(
                getCurrentlyBuiltMachine(context).currentlyBuiltState);
        }

        auto andFinish()
        {
            return MachineBuilder<BbT, IsSubmachine>(std::move(context));
        }

    private:
        auto andGoToStateInternal(const std::string& stateName)
        {
            getCurrentlyBuiltState(context).destination.primary =
                createFullStateName(context.currentlyBuiltMachine, stateName);
            return MachineBuilder<BbT, IsSubmachine>(std::move(context));
        }

    private:
        BuilderContext<BbT> context;
    };

    template<BlackboardTypeConcept BbT>
    class [[nodiscard]] DefaultTransitionErrorBuilder final
    {
    public:
        explicit DefaultTransitionErrorBuilder(
            BuilderContext<BbT>&& context) noexcept
            : context(std::move(context))
        {
        }

        DefaultTransitionErrorBuilder(DefaultTransitionErrorBuilder&&) = delete;

        DefaultTransitionErrorBuilder(const DefaultTransitionErrorBuilder&) =
            delete;

    public:
        auto andGoToState(StateId name)
        {
            getCurrentlyBuiltState(context).destination.primary =
                createFullStateName(context.currentlyBuiltMachine, name);
            return MachineBuilder<BbT, false, true>(std::move(context));
        }

        auto andLoop()
        {
            return andGoToState(
                getCurrentlyBuiltMachine(context).currentlyBuiltState);
        }

        auto andRestart()
        {
            return andGoToState("__restart__");
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
            : context(std::move(context)), condition(std::move(condition))
        {
        }

        ConditionTransitionBuilder(ConditionTransitionBuilder&&) = delete;

        ConditionTransitionBuilder(const ConditionTransitionBuilder&) = delete;

    public:
        auto goToState(StateId name)
        {
            addConditionalTransitionToStateInCurrentMachine(
                std::move(condition), name, context);
            return StateBuilder<BbT, IsSubmachine>(std::move(context));
        }

        auto goToMachine(MachineId machineName)
        {
            if (machineName.get() == context.currentlyBuiltMachine)
                throw Error(
                    "When transition to machine, you cannot re-enter "
                    "the current machine");

            if (!context.machines.contains(machineName))
                throw Error(std::format(
                    "Trying to go to machine called {} that is not "
                    "defined yet",
                    machineName.get()));

            return MachineBackTransitionBuilder<BbT, IsSubmachine, false>(
                std::move(context), machineName, std::move(condition));
        }

        auto finish()
        {
            getCurrentlyBuiltState(context).conditions.push_back(
                ConditionalTransitionContext {
                    .condition = std::move(condition),
                });
            return StateBuilder<BbT, IsSubmachine>(std::move(context));
        }

        auto error()
        {
            if (!context.machines.contains("__error__"))
            {
                throw new Error(
                    "You cannot call error() when no error machine was "
                    "defined");
            }

            addConditionalErrorTransition(std::move(condition), context);
            return StateBuilder<BbT, IsSubmachine>(std::move(context));
        }

    private:
        BuilderContext<BbT> context;
        Condition<BbT> condition;
    };

    template<BlackboardTypeConcept BbT>
    class [[nodiscard]] ConditionTransitionErrorBuilder final
    {
    public:
        ConditionTransitionErrorBuilder(
            BuilderContext<BbT>&& context, Condition<BbT>&& condition)
            : context(std::move(context)), condition(std::move(condition))
        {
        }

        ConditionTransitionErrorBuilder(ConditionTransitionErrorBuilder&&) =
            delete;

        ConditionTransitionErrorBuilder(
            const ConditionTransitionErrorBuilder&) = delete;

    public:
        auto goToState(StateId name)
        {
            addConditionalTransitionToStateInCurrentMachine(
                std::move(condition), name, context);
            return StateBuilder<BbT, false, true>(std::move(context));
        }

        auto restart()
        {
            return goToState("__restart__");
        }

    private:
        BuilderContext<BbT> context;
        Condition<BbT> condition;
    };

    template<BlackboardTypeConcept BbT, bool IsSubmachine, bool IsErrorMachine>
    class [[nodiscard]] StateBuilderBase
    {
    public:
        StateBuilderBase(BuilderContext<BbT>&& context)
            : context(std::move(context))
        {
        }

        StateBuilderBase(StateBuilderBase&&) = default;

        StateBuilderBase(const StateBuilderBase&) = delete;

    public:
        auto whenBaseImpl(Condition<BbT>&& condition)
        {
            if constexpr (IsErrorMachine)
            {
                return ConditionTransitionErrorBuilder<BbT>(
                    std::move(context), std::move(condition));
            }
            else
            {
                return ConditionTransitionBuilder<BbT, IsSubmachine>(
                    std::move(context), std::move(condition));
            }
        }

        auto execBaseImpl(Action<BbT>&& action)
        {
            getCurrentlyBuiltState(context).action = std::move(action);

            if constexpr (IsErrorMachine)
            {
                return DefaultTransitionErrorBuilder<BbT>(std::move(context));
            }
            else
            {
                return DefaultTransitionBuilder<BbT, IsSubmachine>(
                    std::move(context));
            }
        }

    private:
        BuilderContext<BbT> context;
    };

    template<
        BlackboardTypeConcept BbT,
        bool IsSubmachine,
        bool IsErrorMachine = false>
    class [[nodiscard]] StateBuilderBeforePickingAnything final
        : public StateBuilderBase<BbT, IsSubmachine, IsErrorMachine>
    {
    public:
        explicit StateBuilderBeforePickingAnything(
            BuilderContext<BbT>&& context) noexcept
            : StateBuilderBase<BbT, IsSubmachine, IsErrorMachine>(
                std::move(context))
        {
        }

        StateBuilderBeforePickingAnything(StateBuilderBeforePickingAnything&&) =
            delete;

        StateBuilderBeforePickingAnything(
            const StateBuilderBeforePickingAnything&) = delete;

    public:
        auto when(Condition<BbT>&& condition)
        {
            return StateBuilderBase<BbT, IsSubmachine, IsErrorMachine>::
                whenBaseImpl(std::move(condition));
        }

        auto exec(Action<BbT>&& action)
        {
            return StateBuilderBase<BbT, IsSubmachine, IsErrorMachine>::
                execBaseImpl(std::move(action));
        }
    };

    template<
        BlackboardTypeConcept BbT,
        bool IsSubmachine,
        bool IsErrorMachine = false>
    class [[nodiscard]] StateBuilder final
        : public StateBuilderBase<BbT, IsSubmachine, IsErrorMachine>
    {
    public:
        explicit StateBuilder(BuilderContext<BbT>&& context) noexcept
            : StateBuilderBase<BbT, IsSubmachine, IsErrorMachine>(
                std::move(context))
        {
        }

        StateBuilder(StateBuilder&&) = delete;

        StateBuilder(const StateBuilder&) = delete;

    public:
        auto orWhen(Condition<BbT>&& condition)
        {
            return StateBuilderBase<BbT, IsSubmachine, IsErrorMachine>::
                whenBaseImpl(std::move(condition));
        }

        auto otherwiseExec(Action<BbT>&& action)
        {
            return StateBuilderBase<BbT, IsSubmachine, IsErrorMachine>::
                execBaseImpl(std::move(action));
        }
    };

    template<
        BlackboardTypeConcept BbT,
        bool IsSubmachine,
        bool IsErrorMachine = false>
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
            insertNewStateIntoContext(name, context);

            return StateBuilderBeforePickingAnything<BbT, IsSubmachine>(
                std::move(context));
        }

        auto done()
        {
            if constexpr (IsSubmachine || IsErrorMachine)
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

    template<
        BlackboardTypeConcept BbT,
        bool IsSubmachine,
        bool IsErrorMachine = false>
    class [[nodiscard]] MachineBuilderPreEntryPoint final
    {
    public:
        explicit MachineBuilderPreEntryPoint(
            BuilderContext<BbT>&& context) noexcept
            : context(std::move(context))
        {
        }

        MachineBuilderPreEntryPoint(MachineBuilderPreEntryPoint&&) = delete;

        MachineBuilderPreEntryPoint(const MachineBuilderPreEntryPoint&) =
            delete;

    public:
        auto withEntryState(StateId name)
        {
            getCurrentlyBuiltMachine(context).entryState = name;
            insertNewStateIntoContext(name, context);

            if constexpr (IsErrorMachine)
            {
                context.errorDestination.primary =
                    createFullStateName("__error__", name);
            }

            return StateBuilderBeforePickingAnything<
                BbT,
                IsSubmachine,
                IsErrorMachine>(std::move(context));
        }

    private:
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
            if (context.machines.contains(name))
                throw Error(std::format(
                    "Trying to redeclare machine with name {}", name.get()));

            insertNewMachineIntoContext(name, context);
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
            BuilderContext<BbT>&& context) noexcept
            : context(std::move(context))
        {
        }

        GlobalErrorConditionBuilder(GlobalErrorConditionBuilder&&) = delete;
        GlobalErrorConditionBuilder(const GlobalErrorConditionBuilder&) =
            delete;

    public:
        auto noGlobalEntryCondition()
        {
            return MachineBuilderPreEntryPoint<BbT, false, true>(
                std::move(context));
        }

        auto useGlobalEntryCondition(Condition<BbT>&& condition)
        {
            context.useGlobalError = true;
            context.errorCondition = condition;
            return MachineBuilderPreEntryPoint<BbT, false, true>(
                std::move(context));
        }

    private:
        BuilderContext<BbT> context;
    };
} // namespace fsm::detail

namespace fsm
{
    template<BlackboardTypeConcept BbT>
    class [[nodiscard]] Builder final
    {
    public:
        Builder() = default;
        Builder(Builder&&) = delete;
        Builder(const Builder&) = delete;

    public:
        auto withNoErrorMachine()
        {
            return detail::MainBuilder<BbT>(detail::BuilderContext<BbT> {});
        }

        auto withErrorMachine()
        {
            auto&& context = detail::BuilderContext<BbT> {};
            detail::insertNewMachineIntoContext("__error__", context);
            return detail::GlobalErrorConditionBuilder<BbT>(std::move(context));
        }
    };
} // namespace fsm
