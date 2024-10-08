cmake_minimum_required(VERSION 3.23)

enable_testing()

add_executable(unitTests
    ConditionalStream.h
    ConditionalStream.cpp
    utils.h
    utils.cpp
    testInstrumentedClass.cpp
    testMemoryManager.cpp
    testFunctionTraits.cpp
    testTaskQueue.cpp
    testLambda.cpp
    testLambdaUsage.cpp
    testStdFnFromLambda.cpp
    testPackagedTask.cpp
    testFreeFunction.cpp
    testCompareConstructs.cpp
    testPackingSolution.cpp
)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG d83fee138a9ae6cb7c03688a2d08d4043a39815d # release-1.10.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

target_link_libraries(unitTests
    MyLibrary
    gtest_main
)

target_include_directories(unitTests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

include(GoogleTest)
gtest_discover_tests(unitTests DISCOVERY_MODE PRE_TEST)
