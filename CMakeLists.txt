cmake_minimum_required(VERSION 3.16)
project(GNMSCALEPS4 VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(gnm_core
    src/core/image.cpp
    src/graphics/pipeline.cpp
    src/hardware/profile.cpp
)
target_include_directories(gnm_core PUBLIC include)

add_executable(gnm_scale src/main.cpp)
target_link_libraries(gnm_scale PRIVATE gnm_core)

enable_testing()
add_executable(test_pipeline tests/test_pipeline.cpp)
target_link_libraries(test_pipeline PRIVATE gnm_core)
add_test(NAME pipeline_tests COMMAND test_pipeline)
