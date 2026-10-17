find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Embed)
find_package(Threads REQUIRED)
find_path(NLOHMANN_JSON_INCLUDE_DIR nlohmann/json.hpp REQUIRED)

set(WORM2D_SOURCES
    Muscles.cpp
    NervousSystem.cpp
    TSearch.cpp
    WormBody.cpp
    random.cpp
    utils.cpp
    neuromlLocal/c302ForW2D.cpp
    neuromlLocal/owSignalSimulator.cpp
    neuromlLocal/owSignalSimulatorForWorm2D.cpp
    simulator/Evolution.cpp
    simulator/Evolvable.cpp
    simulator/NSToMuscles.cpp
    simulator/Segment21.cpp
    simulator/Simulation.cpp
    simulator/StretchReceptor.cpp
    simulator/StretchReceptor18.cpp
    simulator/StretchReceptorCE.cpp
    simulator/Worm21.cpp
    simulator/Worm2D.cpp
    simulator/Worm2D21.cpp
    simulator/Worm2DCE.cpp
    simulator/Worm2DSR.cpp
    simulator/WormRS18.cpp
    simulator/jsonUtils.cpp
    simulator/main_osc.cpp
)

add_executable(main_osc ${WORM2D_SOURCES})
target_compile_features(main_osc PRIVATE cxx_std_17)
target_compile_options(main_osc PRIVATE -Wall)
target_include_directories(
    main_osc
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/simulator
        ${CMAKE_CURRENT_SOURCE_DIR}/neuromlLocal
        ${NLOHMANN_JSON_INCLUDE_DIR}
)
target_link_libraries(main_osc PRIVATE Python3::Python Threads::Threads ${CMAKE_DL_LIBS})

if(UNIX AND NOT APPLE)
    find_library(UTIL_LIBRARY util)
    if(UTIL_LIBRARY)
        target_link_libraries(main_osc PRIVATE ${UTIL_LIBRARY})
    endif()
endif()

set_target_properties(
    main_osc
    PROPERTIES
        OUTPUT_NAME main_osc
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

install(TARGETS main_osc RUNTIME DESTINATION worm2d/bin)
