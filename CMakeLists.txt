cmake_minimum_required(VERSION 3.5)
project(CE_Locomotion)

file(GLOB locomotion_src      Muscles.cpp NervousSystem.cpp random.cpp StretchReceptor.cpp TSearch.cpp Worm.cpp WormBody.cpp)

add_executable(locomotion ${locomotion_src} main.cpp)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(locomotion PUBLIC /O2 /LTCG)
else()
    target_compile_options(locomotion PUBLIC -O3 -flto)
endif()
