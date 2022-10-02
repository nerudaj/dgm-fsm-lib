# dgm-fsm-lib

This is a C++ library meant for building and running Final State Machines, with the emphasis on their usage in videogame AI. While FSMs are quite simple structures to implement, the code usually quickly devolves into spaghetti mess. This library enforces one particular paradigm to how FSM should look like so the code can stay consistent and hopefully clean.

## Table of contents

 * [FSM architecture](#fsm-architecture)
 * [How to build the FSM](#how-to-build-the-fsm)
	* [dgm::fsm::Builder](#dgm-fsm-builder)
	* [dgm::fsm::Factory](#dgm-fsm-factory)

## FSM architecture

FSM usually consists of two things - set of conditions that allow it to jump from state to state and some behaviour that is executed in a particular state. Quite often, those get intermixed, so following code is no exception:

```c++
case State::DoSomething:
{
	if (shouldTransition())
	{
		someBootstrapLogic();
		return State::NewState;
	}
	
	doSomething();
	
	if (shouldTransitionElsewhere())
	{
		anotherBootstrapLogic();
		return State::OtherState;
	}
	
	doAnotherThing();
} break;
```

This code implies FSM where each state in fact does job of multiple states but because the switch-case code would get really long, programmers tend to abbreviate. However, this obfuscates the general logic behind the FSM, which can really complicate developing game AI (with the second problem being that the AI designed would like to avoid C++ code and use predefined primitives instead).

FSMs created with this library require that each state has a set of conditions and target state for each condition if it is evaluated to true, exactly one behavior function and a state to jump to afterwards. A condition with associate target state is called a **transition**. For such state, the following rules apply:

1) First evaluate all transitions in order. If any condition is evaluated to true, FSM jumps to associated state.
2) If no condition was true, execute behavior.
3) After executing behavior, jump to associated state. State can jump back to itself, which means it is a **looping** state.

Algorithm called above also defines boundaries for single "tick" when updating the FSM. In other words, once FSM jumps to another state, `update` function returns, so while badly defined FSM can loop inifitely on some state, each individual `update` will always finish.

This library realies heavily on `std::function` object usage which means that it allows you to glue primitive functions together, so your conditions or behaviors can be quite complex if you need to.

Also, this library on usage of **Blackboards** for storing information your behaviors and conditions might require.

## How to build the FSM

There are three supported ways of building the FSM. You can always use just the `Fsm.hpp` header and build everything by hand, but that is discouraged. Instead, it is recommended to use `FsmBuilder.hpp` which helps you with a prepared set of builders. Or you can use `FsmFactory.hpp` and load the FSM from file. Loading requires a working implementation of a `dgm::fsm::LoaderInterface` so you can either implement your own, or you can use existing JSON parser (from `FsmJsonLoader.hpp`). Format of the JSON is described below.

For the examples, we want to build super simple CSV parser. Suppose we already have our Blackboard definition and all logic primites we need:

```c++ 
struct Blackboard {
	const std::string CSV;
	std::vector<std::vector<std::string>> data = { {} };
	unsigned pos = 0;
	std::string currentWord = "";
};

class CsvParser {
public:
	static bool isEof(const Blackboard& bb) {
		return bb.CSV.size() <= bb.pos;
	}

	static bool isComma(const Blackboard& bb) {
		return bb.CSV[bb.pos] == ',';
	}

	static bool isNewline(const Blackboard& bb) {
		return bb.CSV[bb.pos] == '\n';
	}

	static void storeWord(Blackboard& bb) {
		bb.data.back().push_back(bb.currentWord);
		bb.currentWord = "";
		bb.pos++;
	}

	static void handleNewline(Blackboard& bb) {
		bb.data.push_back({});
	}

	static void advanceChar(Blackboard& bb) {
		bb.currentWord += bb.CSV[bb.pos];
		bb.pos++;
	}
};

```

This CSV parser will be super simple, it only deals with commas for delimiting colums and newlines for delimiting rows. The `Blackboard::data` is the output matrix for parsed CSV, the `Blackboard::CSV` is the input string. You can notice that all function primitives in `CsvParser` are static - since the `Blackboard` is the only thing that carries state, function primites can be free standing functions.

Also notice that conditions must accept `const Blackboard&`, while behaviors accept `Blackboard&`, because they are allowed to mutate its state.

### dgm::fsm::Builder

When using the builder pattern, it is recommended for you to define scoped enum with all necessary states:

```c++
enum class State
{
	Start,
	CommaFound,
	NewlineFound,
	End
};
```

Integers are also allowed, but they're not as descriptive. Now the FSM construction:

```c++
#include <FsmBuilder.hpp>
#include <FsmDecorators.hpp>

auto buildFsm() {
	using dgm::fsm::decorator::Merge;

	return dgm::fsm::Builder<Blackboard, State>()
		.with(State::Start)
			.when(CsvParser::isEof).goTo(State::End)
			.orWhen(CsvParser::isComma).goTo(State::CommaFound)
			.orWhen(CsvParser::isNewline).goTo(State::NewlineFound)
			.otherwiseExec(CsvParser::advanceChar).andLoop()
		.with(State::CommaFound)
			.exec(CsvParser::storeWord).andGoTo(State::Start)
		.with(State::NewlineFound)
			.exec(Merge<Blackboard>(
				CsvParser::handleNewline,
				CsvParser::storeWord)
			).andGoTo(State::Start)
		.build();
}
```

As you can see, `Blackboard` and `State` are required template parameters for the builder (and FSM itself) so typechecking can be performed. The parser logic should be pretty obvious from the FSM structure. Note use of the `Merge` decorator to fuse two primitives together to make more complicated behaviour. Also note that decorators require explicit template instantiation since C++ type deduction system is sometimes garbage.

Returned object is your fsm. You can use `reset` to set to any state you need. By default it will be set to the default value of the `State` enum (`Start` in this case). Call `update` to perform single FSM "tick".

### dgm::fsm::Factory

Alternative to building your FSM in code (and the need to recompile it each time the structure changes) can be to define it in some external file and load it at runtime. This requires use of the `dgm::fsm::Factory` and appropriate loader.

```c++
#include <FsmFactory.hpp>
#include <FsmJsonLoader.hpp>

auto buildFsm(const std::string &path) {
	dgm::fsm::JsonLoader loader;
	dgm::fsm::Factory<Blackboard> factory(loader);
	
	factory.registerPredicate("isEof", CsvParser::isEof);
	factory.registerPredicate("isComma", CsvParser::isComma);
	factory.registerPredicate("isNewline", CsvParser::isNewline);
	
	factory.registerLogic("advanceChar", CsvParser::advanceChar);
	factory.registerLogic("handleNewline", CsvParser::handleNewline);
	factory.registerLogic("storeWord", CsvParser::storeWord);
	
	return factory.loadFromFile(path);
}
```

Note that `Factory` only accepts `Blackboard` type, but no state type. This is so external file does not depend on how many state you've allowed it to have and translates everything to integers. But, you have to register available predicates and behaviors and names under which they will be parsed in the source file.

Since this example uses `JsonLoader`, let's go over how the JSON file looks like:

```json
[
	{
		"name": "StateStart",
		"transitions": [
			{
				"condition": "isEof",
				"target": "StateFinish"
			},
			{
				"condition": "isComma",
				"target": "StateCommaFound"
			},
			{
				"condition": "isNewline",
				"target": "StateNewlineFound"
			}
		],
		"behaviors": [
			"advanceChar"
		],
		"defaultTransition": "StateStart"
	},
	{
		"name": "StateCommaFound",
		"transitions": [],
		"behaviors": [
			"storeWord"
		],
		"defaultTransition": "StateStart"
	},
	{
		"name": "StateNewlineFound",
		"transitions": [],
		"behaviors": [
			"handleNewline",
			"storeWord"
		],
		"defaultTransition": "StateStart"
	},
	{
		"name": "StateFinish",
		"transitions": [],
		"behaviors": [],
		"defaultTransition": "StateFinish"
	}
]
```

As mentioned before, state names are converted into integers. Those integers will be indices in the main JSON array.

> NOTE: If you were inclined to help me with some visual designed for the FSM - that would be very appreciated! Also, you can use `getAnnotations` method on the factory to get JSON string with all registered primitives (which would be necessary import for such designer).

## Logging

