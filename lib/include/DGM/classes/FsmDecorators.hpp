#pragma once

#include <DGM/classes/Fsm.hpp>

namespace dgm
{
	namespace fsm
	{
		namespace decorator
		{

			template<class BlackboardType>
			[[nodiscard]]
			auto And(Condition<BlackboardType> cond1, Condition<BlackboardType> cond2)
			{
				return[cond1, cond2] (const BlackboardType& bb) -> Condition<BlackboardType>
				{
					return cond1(bb) && cond2(bb);
				};
			}

			template<class BlackboardType>
			[[nodiscard]]
			auto Or(Condition<BlackboardType> cond1, Condition<BlackboardType> cond2)
			{
				return[cond1, cond2] (const BlackboardType& bb) -> Condition<BlackboardType>
				{
					return cond1(bb) || cond2(bb);
				};
			}

			template<class BlackboardType>
			[[nodiscard]]
			auto Not(Condition<BlackboardType> cond1)
			{
				return[cond1] (const BlackboardType& bb) -> Condition<BlackboardType>
				{
					return !cond1(bb);
				};
			}

			template<class BlackboardType>
			[[nodiscard]]
			auto Merge(Logic<BlackboardType> logic1, Logic<BlackboardType> logic2)
			{
				return [logic1, logic2] (BlackboardType& bb)
				{
					logic1(bb);
					logic2(bb);
				};
			}

		}
	}
}