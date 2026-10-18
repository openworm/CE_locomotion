find_package(Threads REQUIRED)
find_path(NLOHMANN_JSON_INCLUDE_DIR nlohmann/json.hpp REQUIRED)

add_executable(
    legacy_rs18_main
    main.cpp
    Worm.cpp
    WormBody.cpp
    NervousSystem.cpp
    StretchReceptor.cpp
    Muscles.cpp
    TSearch.cpp
    random.cpp
    argUtils.cpp
    jsonUtils.cpp
)
target_compile_features(legacy_rs18_main PRIVATE cxx_std_11)
target_include_directories(
    legacy_rs18_main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${NLOHMANN_JSON_INCLUDE_DIR}
)
target_link_libraries(legacy_rs18_main PRIVATE Threads::Threads)
set_target_properties(
    legacy_rs18_main
    PROPERTIES
        OUTPUT_NAME main
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/legacy/rs18
)
install(TARGETS legacy_rs18_main RUNTIME DESTINATION worm2d/legacy/rs18)
