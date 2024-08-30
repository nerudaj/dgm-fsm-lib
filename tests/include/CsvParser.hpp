#pragma once

#include "Blackboard.hpp"

bool isEscapeChar(const Blackboard& bb)
{
    return bb.data[bb.charIdx] == '"';
}

bool isSeparatorChar(const Blackboard& bb)
{
    return bb.data[bb.charIdx] == ',';
}

bool isNewlineChar(const Blackboard& bb)
{
    return bb.data[bb.charIdx] == '\n';
}

void advanceChar(Blackboard& bb)
{
    ++bb.charIdx;
}

void storeWord(Blackboard& bb)
{
    bb.csv.back().push_back(
        bb.data.substr(bb.wordStartIdx, bb.charIdx - bb.wordStartIdx));
}

void startLine(Blackboard& bb)
{
    bb.csv.push_back({});
}

void nothing(Blackboard&) {}
