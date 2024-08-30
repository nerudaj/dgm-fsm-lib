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

bool isEof(const Blackboard& bb)
{
    return bb.data.size() == bb.charIdx;
}

void advanceChar(Blackboard& bb)
{
    ++bb.charIdx;
}

void storeWord(Blackboard& bb)
{
    bb.csv.back().push_back(
        bb.data.substr(bb.wordStartIdx, bb.charIdx - bb.wordStartIdx));
    bb.wordStartIdx = bb.charIdx + 1;
}

void startLine(Blackboard& bb)
{
    bb.csv.push_back({});
}

void nothing(Blackboard&) {}
