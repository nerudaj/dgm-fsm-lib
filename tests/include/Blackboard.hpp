#pragma once

#include <DGM/classes/Types.hpp>
#include <string>
#include <vector>

struct Blackboard : dgm::fsm::BlackboardBase
{
    std::string data = "";
    size_t charIdx = 0;
    size_t wordStartIdx = 0;
    std::vector<std::vector<std::string>> csv = { {} };
};