#pragma once

namespace fsm::detail
{

    inline void
    you_see_this_error_because_you_used_empty_string_in_fsm_builder() noexcept
    {
    }

    template<class CharT>
    class [[nodiscard]] NonEmptyString final
    {
    public:
        using StringT = std::basic_string_view<CharT>;

    public:
        consteval NonEmptyString(const CharT* str) noexcept : data(str)
        {
            if (data.empty())
                you_see_this_error_because_you_used_empty_string_in_fsm_builder();
        }

        [[nodiscard]] constexpr StringT get() const noexcept
        {
            return data;
        }

    private:
        StringT data;
    };

} // namespace fsm::detail