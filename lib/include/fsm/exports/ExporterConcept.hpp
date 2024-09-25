#pragma once

#include <fsm/Types.hpp>
#include <fsm/detail/BuilderContext.hpp>

namespace fsm
{
    template<class T, class BbT>
    concept ExporterTypeConcept =
        requires(T&& object, const detail::BuilderContext<BbT>& context) {
            {
                object.exportDiagram(context)
            } -> std::same_as<void>;
        };
}