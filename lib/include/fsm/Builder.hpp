#pragma once

#include <fsm/Error.hpp>
#include <fsm/Fsm.hpp>
#include <fsm/Types.hpp>
#include <fsm/detail/BuilderContext.hpp>
#include <fsm/detail/BuilderContextHelper.hpp>
#include <fsm/detail/Constants.hpp>
#include <fsm/detail/Helper.hpp>

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
        explicit constexpr FinalBuilder(BuilderContext<BbT>&& context) noexcept
            : context(std::move(context))
        {
        }

        FinalBuilder(FinalBuilder&&) = delete;

        FinalBuilder(const FinalBuilder&) = delete;

    public:
        /**
         * Construct the FSM model from builder definitions.
         */
        Fsm<BbT> build()
        {
            for (auto&& [_, machineContext] : context.machines)
            {
                for (auto&& [__, stateContext] : machineContext.states)
                {
                    replacePlaceholderTransitionsWithCorrectOnes(stateContext);
                }
            }

            auto&& index = detail::createStateIndexFromBuilderContext(context);
            return Fsm(index, std::move(context));
        }

    private:
        void replacePlaceholderTransitionsWithCorrectOnes(
            StateBuilderContext<BbT>& state)
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

    private:
        void setPrimaryTransitionDestinationToMainEntryPoint(
            TransitionContext& destination)
        {
            destination.primary = createFullStateName(
                MAIN_MACHINE_NAME,
                context.machines.at(MAIN_MACHINE_NAME).entryState);
        }

        [[nodiscard]] constexpr bool
        isRestartTransition(const TransitionContext& transition) const noexcept
        {
            return transition.primary == RESTART_METASTATE_TRANSITION;
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
        /**
         * When invoked submachine finishes, the control flow returns to
         * the specified state in the currently-defined machine.
         */
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
        /**
         * Transition into a state in the currently-defined machine.
         */
        auto andGoToState(StateId name)
        {
            return andGoToStateInternal(name);
        }

        /**
         * Transition into an entry state of a given submachine.
         */
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

        /**
         * Loop the current state. Equivalent of going to currently-defined
         * state.
         */
        auto andLoop()
        {
            return andGoToStateInternal(
                getCurrentlyBuiltMachine(context).currentlyBuiltState);
        }

        /**
         * Finish the execution of the machine, or 'accept' in university terms.
         *
         * If the current machine is a submachine, finishing returns the
         * control flow to the machine that invoked this submachine.
         */
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
        /**
         * Go to a state in the error machine.
         */
        auto andGoToState(StateId name)
        {
            return andGoToStateImpl(name);
        }

        /**
         * Loop the current state. Equivalent of going to currently-defined
         * state.
         */
        auto andLoop()
        {
            return andGoToStateImpl(
                getCurrentlyBuiltMachine(context).currentlyBuiltState);
        }

        /**
         * Restart the FSM. This will transition into the entry state
         * of the main machine.
         */
        auto andRestart()
        {
            return andGoToStateImpl(RESTART_METASTATE_NAME);
        }

    private:
        auto andGoToStateImpl(const std::string& name)
        {
            getCurrentlyBuiltState(context).destination.primary =
                createFullStateName(context.currentlyBuiltMachine, name);
            return MachineBuilder<BbT, false, true>(std::move(context));
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
        /**
         * Transition into a state in the currently-defined machine.
         */
        auto goToState(StateId name)
        {
            addConditionalTransitionToStateInCurrentMachine(
                std::move(condition), name, context);
            return StateBuilder<BbT, IsSubmachine>(std::move(context));
        }

        /**
         * Transition into an entry state of a given submachine.
         */
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

        /**
         * Finish the execution of the machine, or 'accept' in university terms.
         *
         * If the current machine is a submachine, finishing returns the
         * control flow to the machine that invoked this submachine.
         */
        auto finish()
        {
            getCurrentlyBuiltState(context).conditions.push_back(
                ConditionalTransitionContext {
                    .condition = std::move(condition),
                });
            return StateBuilder<BbT, IsSubmachine>(std::move(context));
        }

        /**
         * Transition into entry state of the error machine.
         */
        auto error()
        {
            if (!context.machines.contains(ERROR_MACHINE_NAME))
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
        /**
         * Transition into a state in the error machine.
         */
        auto goToState(StateId name)
        {
            addConditionalTransitionToStateInCurrentMachine(
                std::move(condition), name, context);
            return StateBuilder<BbT, false, true>(std::move(context));
        }

        /**
         * Restart the FSM. This will transition into the entry state
         * of the main machine.
         */
        auto restart()
        {
            return goToState(RESTART_METASTATE_NAME);
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

    protected:
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
        /**
         * When ticked, check this condition.
         */
        auto when(Condition<BbT>&& condition)
        {
            return StateBuilderBase<BbT, IsSubmachine, IsErrorMachine>::
                whenBaseImpl(std::move(condition));
        }

        /**
         * When ticked, execute this action.
         */
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
        /**
         * Declare another condition for this state.
         */
        auto orWhen(Condition<BbT>&& condition)
        {
            return StateBuilderBase<BbT, IsSubmachine, IsErrorMachine>::
                whenBaseImpl(std::move(condition));
        }

        /**
         * Declare default action that is performed when no condition
         * is fulfilled.
         */
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
        /**
         * Declare additional state definition for this machine.
         */
        auto withState(StateId name)
        {
            insertNewStateIntoContext(name, context);

            return StateBuilderBeforePickingAnything<
                BbT,
                IsSubmachine,
                IsErrorMachine>(std::move(context));
        }

        /**
         * Finish declaration of this machine.
         */
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
        /**
         * Define entry state for the machine. Whenever execution
         * flow enters the currently-defined machine, it will go
         * to this entry state
         */
        auto withEntryState(StateId name)
        {
            getCurrentlyBuiltMachine(context).entryState = name;
            insertNewStateIntoContext(name, context);

            if constexpr (IsErrorMachine)
            {
                context.errorDestination.primary =
                    createFullStateName(ERROR_MACHINE_NAME, name);
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
        /**
         * Declare a named submachine. Submachines act like
         * functions, but for FSM. You can transition into it and
         * once it finishes, you will get to a specified state in the
         * calling machine.
         *
         * Submachine can also invoke other submachines, but there
         * can never be recursion.
         */
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

        /**
         * Declare the main machine of the FSM. You won't be able to
         * declare any additional submachines after this point.
         */
        auto withMainMachine()
        {
            return withSubmachine<false>(MAIN_MACHINE_NAME);
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
        /**
         * There is no automated way to get to the error machine. You
         * will only be able to transition into it using the error() calls.
         */
        auto noGlobalEntryCondition()
        {
            return MachineBuilderPreEntryPoint<BbT, false, true>(
                std::move(context));
        }

        /**
         * Global error condition is evaluated during each fsm::Fsm::tick()
         * before anything else. If condition is fulfilled, the FSM transitions
         * into entry state of the error machine.
         */
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
    /**
     * \brief FSM builder
     *
     * This builder provides a convenient API for constructing
     * hierarchical finite state machines.
     *
     * It has a few restrictions - machine names starting with two
     * underscores (__) are reserved by the library.
     *
     * You can only go to submachines that were previously defined
     * (you cannot recurse on a submachine, nor directly or indirectly).
     *
     * There can't be duplicate submachine names or duplicate state names
     * within a machine. State names can be duplicated across (sub)machines.
     */
    template<BlackboardTypeConcept BbT>
    class [[nodiscard]] Builder final
    {
    public:
        Builder() = default;
        Builder(Builder&&) = delete;
        Builder(const Builder&) = delete;

    public:
        /**
         * \brief Construct a FSM without a dedicated error-case submachine
         *
         * This means you must not use error() in a certain parts of the
         * builder. Calling this with no error machine causes runtime exception
         * when error().
         */
        auto withNoErrorMachine()
        {
            return detail::MainBuilder<BbT>(detail::BuilderContext<BbT> {});
        }

        /**
         * \brief Construct a FSM with a dedicate error-case machine
         *
         * This allows you to use error() calls and to set a global error
         * condition.
         */
        auto withErrorMachine()
        {
            auto&& context = detail::BuilderContext<BbT> {};
            detail::insertNewMachineIntoContext(
                detail::ERROR_MACHINE_NAME, context);
            return detail::GlobalErrorConditionBuilder<BbT>(std::move(context));
        }
    };
} // namespace fsm
