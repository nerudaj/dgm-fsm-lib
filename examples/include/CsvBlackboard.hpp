#pragma once

#include <fsm/Types.hpp>

struct CsvBlackboard : fsm::BlackboardBase
{
    std::string data;
    size_t currentIdx = 0;
    size_t wordStartIdx = 0;
};
