#pragma once

namespace fsm::detail
{
    constexpr const char* MAIN_MACHINE_NAME = "__main__";
    constexpr const char* ERROR_MACHINE_NAME = "__error__";
    constexpr const char* RESTART_METASTATE_NAME = "__restart__";
    constexpr const char* RESTART_METASTATE_TRANSITION =
        "__error__:__restart__";
} // namespace fsm::detail
