# Integration

This library is a C++23 library, so don't forget to always enable it for your target using `set ( CMAKE_CXX_STANDARD 23 )`.

## CMake Package Manager

Assuming that you've enabled CPM in your CMake code, you can bring this library in like this:

```cmake
CPMAddPackage("gh:nerudaj/fsm-cpp#2.0.0")

set ( CMAKE_CXX_STANDARD 23 )

target_link_libraries ( ${MYTARGET} PUBLIC fsm-cpp )
```

Full example is available [here](../integration_tests/cpm).
