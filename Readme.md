[![CI](https://github.com/nerudaj/dgm-fsm-lib/actions/workflows/main.yml/badge.svg?branch=main)](https://github.com/nerudaj/dgm-fsm-lib/actions/workflows/main.yml)

# fsm-cpp

This is a C++ library meant for building and running Final State Machines, with the emphasis on their usage in videogame AI. While FSMs are quite simple structures to implement, the code usually quickly devolves into spaghetti mess. This library enforces one particular paradigm to how FSM should look like so the code can stay consistent and hopefully clean.

## Table of contents

 * [FSM architecture](#fsm-architecture)
 * [Linking the library](#linking-the-library)
 * [Building the FSM](#building-the-fsm)
 * [Logging](#logging)
 * [Diagram exports](#diagram-exports)

## FSM architecture

FSM usually consists of two things - set of conditions that allow it to jump from state to state and some behaviour that is executed in each state. Quite often, those get intermixed, so following code is no exception:

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

This code implies FSM where each state in fact does job of multiple states but because the switch-case code would get really long, programmers tend to abbreviate. However, this obfuscates the general logic behind the FSM, which can really complicate developing game AI (with the second problem being that the AI designer would like to avoid C++ code and use predefined primitives instead).

This library mandates that each FSM state has an ordered set of conditions associated with transitions, exactly one behavior and a default transition.

1) First evaluate all conditions in order. If any condition is evaluated to true, FSM jumps to associated state (executes transition).
2) If no condition was true, execute behavior.
3) After executing behavior, execute default transition. State can jump back to itself, which means it is a **looping** state.

The above algorithm also defines clear boundaries for a single `tick` when updating the machine. The tick ends when FSM executes a transition, so you don't have to worry about the tick function getting stuck in an infinite loop.

In addition to these simple rules governing each state, a global error condition can be specified that is evaluated first when ticking the FSM and if evaluated to true, it transitions to an error submachine.

And since I am mentioning submachines, this library allows you to define hierarchical FSMs that can transition to sub-machines and when such sub-machine finishes, it transitions back to a given state in a calling machine. Sub-machine can also transition into other sub-machines, although recursion is prevented by design.

**Blackboards** are used to store all contextual information. Unlike many other similar libraries that give you string-indexed storages that can hold a couple predefined types, here you can just provide any struct, as long as it publicly inherits from `fsm::BlackboardBase`.

## Linking the library

TODO

## Building the FSM

All you need to do is to include `<fsm/Builder.hpp>` and construct the machine using a Builder object. For a simple CSV parser without quotation support, a builder code could look like this:

```cpp
auto&& machine = fsm::Builder<Blackboard>()
    .withErrorMachine()
        .noGlobalEntryCondition()
        .withEntryState("A")
           .exec(nothing).andLoop()
        .done()
    .withSubmachine("1")
        .withEntryState("A")
            .when(alwaysTrue).error()
            .otherwiseExec(nothing).andLoop()
        .done()
    .withSubmachine("2")
        .withEntryState("A")
            .exec(nothing).andGoToMachine("1").thenGoToState("B")
        .withState("B")
            .exec(nothing).andFinish()
        .done()
    .withMainMachine()
        .withEntryState("A")
            .exec(nothing).andGoToMachine("2").thenGoToState("B")
        .withState("B")
            .exec(nothing).andLoop()
        .done()
    .build();
```

Refer to [example code](examples/02-simple-fsm) for more info.

## Logging

The library comes pre-packaged with a simple CSV-based logger, but you can implement your own if you wish. The default logger can be used like this:

```cpp
#include <fsm/logging/CsvLogger.hpp>

//... create machine
auto&& logger = CsvLogger("path/to/log.csv");
machine.setLogger(logger); // logger must outlive machine
```

## Diagram exports

The library offers the ability to export a diagram representation of the FSM. Currently the only supported format is Mermaid, which you can paste into the [Mermaid online editor](https://mermaid.live/).

You can export just by including `<fsm/exports/MermaidExporter.hpp` and calling the export function right before the build function:

```cpp
auto&& machine = fsm::Builder<Blackboard>()
	// building machine
    .exportDiagram(fsm::MermaidExporter(std::cout))
	.build();
```
