
add_executable(jarvis main.cpp)
target_compile_options(jarvis PUBLIC -fopenmp)
target_link_options(jarvis PUBLIC -fopenmp)

if (CMAKE_BUILD_TYPE MATCHES "Release")
    target_compile_options(jarvis PUBLIC -O3)
endif()
