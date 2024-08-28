#pragma once

#include <DGM/classes/BuilderContext.hpp>
#include <DGM/classes/StateIndex.hpp>

namespace dgm
{
    namespace fsm
    {
        std::vector<unsigned> compileDestinations(
            const Destination& destination, const detail::StateIndex& index);
    }
} // namespace dgm