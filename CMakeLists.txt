cmake_minimum_required(VERSION 3.20)

project(market_data_pipeline LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
enable_testing()

add_executable(test_spsc_queue
    tests/test_spsc_queue.cpp
)

target_include_directories(test_spsc_queue PRIVATE include)

target_compile_options(test_spsc_queue PRIVATE
    -UNDEBUG
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
)

add_test(NAME test_spsc_queue COMMAND test_spsc_queue)

add_executable(test_order_book
    tests/test_order_book.cpp
)

target_include_directories(test_order_book PRIVATE include)

target_compile_options(test_order_book PRIVATE
    -UNDEBUG
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
)

add_test(NAME test_order_book COMMAND test_order_book)

add_executable(test_replay
    tests/test_replay.cpp
)

target_include_directories(test_replay PRIVATE include)

target_compile_options(test_replay PRIVATE
    -UNDEBUG
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
)

add_test(NAME test_replay COMMAND test_replay)

add_executable(test_execution_simulator
    tests/test_execution_simulator.cpp
)

target_include_directories(test_execution_simulator PRIVATE include)

target_compile_options(test_execution_simulator PRIVATE
    -UNDEBUG
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
)

add_test(NAME test_execution_simulator COMMAND test_execution_simulator)

add_executable(bench_order_book
    benchmarks/bench_order_book.cpp
)

target_include_directories(bench_order_book PRIVATE include)

target_compile_options(bench_order_book PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
)
