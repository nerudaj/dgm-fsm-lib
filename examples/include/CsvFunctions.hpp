#pragma once

#include "CsvBlackboard.hpp"
#include <print>

constexpr bool isEof(const CsvBlackboard& bb) noexcept
{
	return bb.currentIdx >= bb.data.size();
}

constexpr bool isSeparator(const CsvBlackboard& bb) noexcept
{
	return bb.data[bb.currentIdx] == ',';
}

constexpr bool isNewline(const CsvBlackboard& bb) noexcept
{
	return bb.data[bb.currentIdx] == '\n';
}

void advanceChar(CsvBlackboard& bb)
{
	++bb.currentIdx;
}

void handleSeparator(CsvBlackboard& bb)
{
	std::println(
		"Word: {}",
		bb.data.substr(bb.wordStartIdx, bb.currentIdx - bb.wordStartIdx));
	++bb.currentIdx;
	bb.wordStartIdx = bb.currentIdx;
}

void handleNewline(CsvBlackboard&)
{
	std::println("==Next line==");
}