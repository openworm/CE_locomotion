find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Embed)
find_package(Threads REQUIRED)
find_path(NLOHMANN_JSON_INCLUDE_DIR nlohmann/json.hpp REQUIRED)

add_executable(
    legacy_example_main
    main.cpp
    jsonUtils.cpp
    argUtils.cpp
    Worm.cpp
    WormBody.cpp
    NervousSystem.cpp
    StretchReceptor.cpp
    Muscles.cpp
    TSearch.cpp
    random.cpp
    neuromlLocal/c302NervousSystem.cpp
    neuromlLocal/c302ForW2D.cpp
    neuromlLocal/owSignalSimulatorForWorm2D.cpp
    neuromlLocal/owSignalSimulator.cpp
)
target_compile_features(legacy_example_main PRIVATE cxx_std_11)
target_include_directories(
    legacy_example_main
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/neuromlLocal
        ${NLOHMANN_JSON_INCLUDE_DIR}
)
target_link_libraries(
    legacy_example_main
    PRIVATE Python3::Python Threads::Threads ${CMAKE_DL_LIBS}
)
if(UNIX AND NOT APPLE)
    find_library(LEGACY_EXAMPLE_UTIL_LIBRARY util)
    if(LEGACY_EXAMPLE_UTIL_LIBRARY)
        target_link_libraries(legacy_example_main PRIVATE ${LEGACY_EXAMPLE_UTIL_LIBRARY})
    endif()
endif()
set_target_properties(
    legacy_example_main
    PROPERTIES
        OUTPUT_NAME main
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/legacy/example
)
install(TARGETS legacy_example_main RUNTIME DESTINATION worm2d/legacy/example)
