find_package(Threads REQUIRED)

add_executable(
    legacy_co_main
    main.cpp
    CTRNN.cpp
    TSearch.cpp
    WormAgent.cpp
    random.cpp
    argUtils.cpp
)
target_compile_features(legacy_co_main PRIVATE cxx_std_11)
target_include_directories(legacy_co_main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(legacy_co_main PRIVATE Threads::Threads)
set_target_properties(
    legacy_co_main
    PROPERTIES
        OUTPUT_NAME main
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/legacy/co
)
install(TARGETS legacy_co_main RUNTIME DESTINATION worm2d/legacy/co)
