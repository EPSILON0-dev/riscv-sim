cmake_minimum_required(VERSION 3.20)

project(rvsim)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG v0.9.7
)
FetchContent_MakeAvailable(magic_enum)

set(SRC_FILES
    src/main.cpp
    src/sim/regs.cpp
    src/sim/operation.cpp
    src/sim/cpu.cpp
)

add_executable(rvsim ${SRC_FILES})
target_compile_options(rvsim PRIVATE -Wall -Wextra -Werror)
target_link_libraries(rvsim PRIVATE magic_enum::magic_enum)
