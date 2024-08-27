#pragma once

#include <DGM/classes/Fsm.hpp>

namespace dgm
{
	namespace fsm
	{
		namespace decorator
		{
			template<class ... BlackboardTypes>
			[[nodiscard]]
			auto And(ConditionCallable<BlackboardTypes...> auto... conds)
			{
				return[conds...] (const BlackboardTypes&... bb)
					{
						return (conds(bb...) && ... && [] () {});
					};
			}

			template<class ... BlackboardTypes>
			[[nodiscard]]
			auto Or(ConditionCallable<BlackboardTypes...> auto... conds)
			{
				return[conds...] (const BlackboardTypes&... bb)
					{
						return (conds(bb...) || ... || [] () {});
					};
			}

			template<class ... BlackboardTypes>
			[[nodiscard]]
			auto Not(ConditionCallable<BlackboardTypes...> auto cond1)
			{
				return[cond1] (const BlackboardTypes&... bb)
					{
						return !cond1(bb...);
					};
			}

			template<class ... BlackboardTypes>
			[[nodiscard]]
			auto Merge(LogicCallable<BlackboardTypes...> auto... logics)
			{
				return [logics...] (BlackboardTypes&... bb)
					{
						(logics(bb...), ...);
					};
			}

		}
	}
}