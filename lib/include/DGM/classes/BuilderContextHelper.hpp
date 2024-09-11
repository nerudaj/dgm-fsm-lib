#pragma once

#include <DGM/classes/BuilderContext.hpp>
#include <DGM/classes/Helper.hpp>

namespace dgm

{
    namespace fsm
    {
        namespace detail
        {
            template<BlackboardTypeConcept BbT>
            static inline MachineBuilderContext<BbT>&
            getCurrentlyBuiltMachine(BuilderContext<BbT>& context)
            {
                return context.machines.at(context.currentlyBuiltMachine);
            }

            template<BlackboardTypeConcept BbT>
            static inline StateBuilderContext<BbT>&
            getCurrentlyBuiltState(BuilderContext<BbT>& context)
            {
                auto& machine = getCurrentlyBuiltMachine(context);
                return machine.states.at(machine.currentlyBuiltState);
            }

            template<BlackboardTypeConcept BbT>
            static inline void insertNewMachineIntoContext(
                MachineId name, BuilderContext<BbT>& context)
            {
                context.currentlyBuiltMachine = name;
                context.machines[name] = {};
            }

            template<BlackboardTypeConcept BbT>
            static inline void insertNewStateIntoContext(
                StateId name, BuilderContext<BbT>& context)
            {
                auto& machine = getCurrentlyBuiltMachine(context);

                if (machine.states.contains(name))
                    throw Error(std::format(
                        "Trying to redeclare state with name {} in machine "
                        "{}",
                        name,
                        context.currentlyBuiltMachine));

                machine.currentlyBuiltState = name;
                machine.states[name] = {};
            }

            template<BlackboardTypeConcept BbT>
            static inline void addConditionalTransitionToStateInCurrentMachine(
                Condition<BbT>&& condition,
                StateId stateName,
                BuilderContext<BbT>& context)
            {
                getCurrentlyBuiltState(context).conditions.push_back(
                    ConditionalTransitionContext {
                        .condition = std::move(condition),
                        .destination = TransitionContext {
                            .primary = createFullStateName(
                                context.currentlyBuiltMachine, stateName) } });
            }
        } // namespace detail
    }     // namespace fsm
} // namespace dgm