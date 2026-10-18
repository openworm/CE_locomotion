find_package(Threads REQUIRED)

add_executable(
    legacy_net21_main
    main.cpp
    Worm.cpp
    WormBody.cpp
    Segment.cpp
    NervousSystem.cpp
    Muscles.cpp
    TSearch.cpp
    random.cpp
    argUtils.cpp
)
target_compile_features(legacy_net21_main PRIVATE cxx_std_11)
target_include_directories(legacy_net21_main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(legacy_net21_main PRIVATE Threads::Threads)
set_target_properties(
    legacy_net21_main
    PROPERTIES
        OUTPUT_NAME main
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/legacy/net21
)
install(TARGETS legacy_net21_main RUNTIME DESTINATION worm2d/legacy/net21)
