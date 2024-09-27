#include <fsm/Builder.hpp>

struct Blackboard : public fsm::BlackboardBase {};

int main()
{
	std::ignore = fsm::Builder<Blackboard>()
		.withNoErrorMachine()
		.withMainMachine()
			.withEntryState("Start")
				.exec([](Blackboard&) {}).andLoop()
			.done()
		.build();
}
