#include <DGM/classes/Compiler.hpp>

dgm::fsm::detail::CompiledTransition
dgm::fsm::detail::Compiler::compileTransition(
    const TransitionContext& destination, const StateIndex& index)
{
    if (destination.secondary.empty())
        if (destination.primary.empty())
            return {};
        else
            return { index.getStateIndex(destination.primary) };
    return {
        index.getStateIndex(destination.primary),
        index.getStateIndex(destination.secondary),
    };
}
