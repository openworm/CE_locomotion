cmake_minimum_required(VERSION 3.17)
project(worm2d CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Extract package version from pyproject.toml so W2D_VERSION stays in sync
# ---------------------------------------------------------------------------
file(READ "${CMAKE_SOURCE_DIR}/pyproject.toml" _PYPROJECT_CONTENTS)
string(REGEX MATCH "version = \"([0-9]+\\.[0-9]+\\.[0-9]+)\"" _ "${_PYPROJECT_CONTENTS}")
if(CMAKE_MATCH_1)
    set(W2D_VERSION_STR "v${CMAKE_MATCH_1}")
else()
    set(W2D_VERSION_STR "v0.0.0")
    message(WARNING "[worm2d] Could not extract version from pyproject.toml; defaulting to v0.0.0")
endif()
message(STATUS "[worm2d] Package version: ${W2D_VERSION_STR}")
add_compile_definitions(W2D_VERSION_FROM_CMAKE="${W2D_VERSION_STR}")

# ---------------------------------------------------------------------------
# Find Python (for embedding)
# ---------------------------------------------------------------------------
find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Module)

# ---------------------------------------------------------------------------
# Python embedding support (optional — manylinux containers have no libpython)
# When OFF, the NeuroML/doNML mode is unavailable but all other functionality
# builds and runs normally.
# ---------------------------------------------------------------------------
option(PYTHON_EMBEDDING "Build C++ executables with Python embedding (required for doNML/NeuroML mode)" ON)

if(PYTHON_EMBEDDING)
    find_package(Python3 OPTIONAL_COMPONENTS Development.Embed)
    if(Python3_Development.Embed_FOUND)
        message(STATUS "[worm2d] Python embedding: enabled")
    else()
        message(WARNING "[worm2d] Python3 Development.Embed not found — disabling Python embedding (doNML mode unavailable)")
        set(PYTHON_EMBEDDING OFF)
    endif()
endif()

if(NOT PYTHON_EMBEDDING)
    message(STATUS "[worm2d] Python embedding: disabled")
    add_compile_definitions(WORM2D_NO_PYTHON_EMBEDDING)
endif()

# ---------------------------------------------------------------------------
# Derive a reliable Python library directory for rpath embedding.
# FindPython3 may leave Python3_LIBRARY_DIRS empty in conda/venv environments;
# fall back to extracting the directory from the full library path or the
# interpreter location so that @rpath/libpython resolves at runtime on macOS.
# ---------------------------------------------------------------------------
if(PYTHON_EMBEDDING)
    if(Python3_LIBRARY_DIRS)
        set(PYTHON_LIB_RPATH ${Python3_LIBRARY_DIRS})
    elseif(Python3_LIBRARIES)
        list(GET Python3_LIBRARIES 0 _py_first_lib)
        get_filename_component(PYTHON_LIB_RPATH "${_py_first_lib}" DIRECTORY)
    else()
        get_filename_component(_py_bin_dir "${Python3_EXECUTABLE}" DIRECTORY)
        get_filename_component(PYTHON_LIB_RPATH "${_py_bin_dir}/../lib" ABSOLUTE)
    endif()
    message(STATUS "[worm2d] Python library rpath: ${PYTHON_LIB_RPATH}")
else()
    set(PYTHON_LIB_RPATH "")
endif()

# ---------------------------------------------------------------------------
# nlohmann_json — bundled header-only library
# ---------------------------------------------------------------------------
set(NLOHMANN_JSON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/cpp")

# ---------------------------------------------------------------------------
# Common compiler flags
# ---------------------------------------------------------------------------
add_compile_options(-O3 -flto)

# ---------------------------------------------------------------------------
# Shared include directories used by multiple targets
# ---------------------------------------------------------------------------
set(CPP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp)

# ---------------------------------------------------------------------------
# Helper macro: build a binary and install it into worm2d/bin/<subdir>
# ---------------------------------------------------------------------------
macro(add_worm_binary TARGET_NAME SUBDIR)
    # Remaining args are source files
    set(SRCS ${ARGN})
    if("${SUBDIR}" STREQUAL "")
        set(_w2d_bin_path "bin/main")
    else()
        set(_w2d_bin_path "bin/${SUBDIR}/main")
    endif()
    message(STATUS "[worm2d] Configuring C++ binary: ${_w2d_bin_path}")
    add_executable(${TARGET_NAME} ${SRCS})
    # Ninja uses a flat build dir — give each target its own subdir to avoid
    # "multiple rules generate main" when several targets share OUTPUT_NAME main.
    if("${SUBDIR}" STREQUAL "")
        set_target_properties(${TARGET_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    else()
        set_target_properties(${TARGET_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${SUBDIR})
    endif()
    add_custom_command(TARGET ${TARGET_NAME} PRE_LINK
        COMMAND ${CMAKE_COMMAND} -E echo "[worm2d] Linking ${_w2d_bin_path} ..."
        VERBATIM
    )
    target_compile_options(${TARGET_NAME} PRIVATE -fPIE)
    target_include_directories(${TARGET_NAME} PRIVATE
        ${CPP_DIR}
        ${CPP_DIR}/Worm2D
        ${Python3_INCLUDE_DIRS}
        ${NLOHMANN_JSON_INCLUDE_DIR}
        /opt/homebrew/include
    )
    if(PYTHON_EMBEDDING)
        target_link_libraries(${TARGET_NAME} PRIVATE
            Python3::Python
            Threads::Threads
            ${CMAKE_DL_LIBS}
        )
        # Use CMake's INSTALL_RPATH so the rpath survives install_name_tool rewriting
        # at install time (raw -Wl,-rpath linker flags are stripped by CMake on macOS).
        if(APPLE)
            set_target_properties(${TARGET_NAME} PROPERTIES
                INSTALL_RPATH "@loader_path;${PYTHON_LIB_RPATH}"
                BUILD_WITH_INSTALL_RPATH TRUE
            )
        else()
            set_target_properties(${TARGET_NAME} PROPERTIES
                INSTALL_RPATH "${PYTHON_LIB_RPATH}"
                BUILD_WITH_INSTALL_RPATH TRUE
            )
        endif()
        target_link_directories(${TARGET_NAME} PRIVATE
            /opt/homebrew/lib
            ${PYTHON_LIB_RPATH}
        )
    else()
        target_link_libraries(${TARGET_NAME} PRIVATE
            Threads::Threads
            ${CMAKE_DL_LIBS}
        )
        target_link_directories(${TARGET_NAME} PRIVATE
            /opt/homebrew/lib
        )
    endif()
    install(TARGETS ${TARGET_NAME}
        RUNTIME DESTINATION worm2d/bin/${SUBDIR}
    )
endmacro()

find_package(Threads REQUIRED)

# ---------------------------------------------------------------------------
# Core shared sources (used by both main and Worm2D targets)
# ---------------------------------------------------------------------------
set(CORE_SRCS
    ${CPP_DIR}/Worm.cpp
    ${CPP_DIR}/WormBody.cpp
    ${CPP_DIR}/NervousSystem.cpp
    ${CPP_DIR}/StretchReceptor.cpp
    ${CPP_DIR}/Muscles.cpp
    ${CPP_DIR}/TSearch.cpp
    ${CPP_DIR}/random.cpp
    ${CPP_DIR}/argUtils.cpp
    ${CPP_DIR}/jsonUtils.cpp
    ${CPP_DIR}/utils.cpp
    ${CPP_DIR}/neuromlLocal/c302NervousSystem.cpp
    ${CPP_DIR}/neuromlLocal/c302ForW2D.cpp
    ${CPP_DIR}/neuromlLocal/owSignalSimulatorForWorm2D.cpp
    ${CPP_DIR}/neuromlLocal/owSignalSimulator.cpp
)

# ---------------------------------------------------------------------------
# Top-level main binary  →  worm2d/bin/main
# ---------------------------------------------------------------------------
add_worm_binary(worm2d_main ""
    ${CPP_DIR}/main.cpp
    ${CORE_SRCS}
)
set_target_properties(worm2d_main PROPERTIES OUTPUT_NAME main)

# ---------------------------------------------------------------------------
# C++ unit tests  (built separately, not installed)
# ---------------------------------------------------------------------------
add_executable(worm2d_tests
    ${CPP_DIR}/tests.cpp
    ${CPP_DIR}/NervousSystem.cpp
    ${CPP_DIR}/random.cpp
)
target_include_directories(worm2d_tests PRIVATE ${CPP_DIR})
target_link_libraries(worm2d_tests PRIVATE Threads::Threads)
set_target_properties(worm2d_tests PROPERTIES OUTPUT_NAME tests)

add_executable(worm2d_tests2
    ${CPP_DIR}/tests2.cpp
    ${CPP_DIR}/NervousSystem.cpp
    ${CPP_DIR}/random.cpp
    ${CPP_DIR}/jsonUtils.cpp
    ${CPP_DIR}/argUtils.cpp
)
target_include_directories(worm2d_tests2 PRIVATE
    ${CPP_DIR}
    ${Python3_INCLUDE_DIRS}
    ${NLOHMANN_JSON_INCLUDE_DIR}
    /opt/homebrew/include
)
target_link_libraries(worm2d_tests2 PRIVATE Threads::Threads ${CMAKE_DL_LIBS})
target_link_directories(worm2d_tests2 PRIVATE /opt/homebrew/lib)
set_target_properties(worm2d_tests2 PROPERTIES OUTPUT_NAME tests2)

# ---------------------------------------------------------------------------
# Worm2D shared sources
# ---------------------------------------------------------------------------
set(W2D_COMMON_SRCS
    ${CPP_DIR}/Worm2D/Evolvable.cpp
    ${CPP_DIR}/Worm2D/WormAgent.cpp
    ${CPP_DIR}/Worm2D/Worm2DSR.cpp
    ${CPP_DIR}/Worm2D/Simulation.cpp
    ${CPP_DIR}/Worm2D/Worm2D21.cpp
    ${CPP_DIR}/Worm2D/Worm2DCE.cpp
    ${CPP_DIR}/Worm2D/WormRS18.cpp
    ${CPP_DIR}/Worm2D/Worm21.cpp
    ${CPP_DIR}/Worm2D/Segment21.cpp
    ${CPP_DIR}/Worm2D/Worm2D.cpp
    ${CPP_DIR}/Worm2D/Evolution21.cpp
    ${CPP_DIR}/Worm2D/Evolution.cpp
    ${CPP_DIR}/Worm2D/EvolutionCE.cpp
    ${CPP_DIR}/Worm2D/EvolutionRS18.cpp
    ${CPP_DIR}/Worm2D/StretchReceptor18.cpp
    ${CPP_DIR}/Worm2D/StretchReceptorCE.cpp
    ${CPP_DIR}/Worm2D/StretchReceptor.cpp
    ${CPP_DIR}/Worm2D/TSearchCO.cpp
    ${CPP_DIR}/Worm2D/NSToMuscles.cpp
    ${CPP_DIR}/Worm2D/EvolutionCO.cpp
    ${CPP_DIR}/Worm2D/Worm2Dmods.cpp
    ${CPP_DIR}/Worm2D/jsonUtils.cpp
    # shared core files referenced by Worm2D
    ${CPP_DIR}/WormBody.cpp
    ${CPP_DIR}/NervousSystem.cpp
    ${CPP_DIR}/Muscles.cpp
    ${CPP_DIR}/TSearch.cpp
    ${CPP_DIR}/random.cpp
    ${CPP_DIR}/utils.cpp
    ${CPP_DIR}/neuromlLocal/c302ForW2D.cpp
    ${CPP_DIR}/neuromlLocal/owSignalSimulatorForWorm2D.cpp
    ${CPP_DIR}/neuromlLocal/owSignalSimulator.cpp
)

# Worm2D/main  →  worm2d/bin/Worm2D/main
add_worm_binary(worm2d_w2d_main "Worm2D"
    ${CPP_DIR}/Worm2D/main.cpp
    ${W2D_COMMON_SRCS}
)
set_target_properties(worm2d_w2d_main PROPERTIES OUTPUT_NAME main)

# Worm2D/main_osc  →  worm2d/bin/Worm2D/main_osc
add_worm_binary(worm2d_main_osc "Worm2D"
    ${CPP_DIR}/Worm2D/main_osc.cpp
    ${W2D_COMMON_SRCS}
)
set_target_properties(worm2d_main_osc PROPERTIES OUTPUT_NAME main_osc)

# ---------------------------------------------------------------------------
# RoyalSociety2018/main  →  worm2d/bin/RoyalSociety2018/main
# ---------------------------------------------------------------------------
set(RS18_DIR ${CPP_DIR}/RoyalSociety2018)
add_worm_binary(worm2d_rs18 "RoyalSociety2018"
    ${RS18_DIR}/main.cpp
    ${RS18_DIR}/Worm.cpp
    ${RS18_DIR}/WormBody.cpp
    ${RS18_DIR}/NervousSystem.cpp
    ${RS18_DIR}/StretchReceptor.cpp
    ${RS18_DIR}/Muscles.cpp
    ${RS18_DIR}/TSearch.cpp
    ${RS18_DIR}/random.cpp
    ${RS18_DIR}/jsonUtils.cpp
    ${CPP_DIR}/argUtils.cpp
)
target_include_directories(worm2d_rs18 BEFORE PRIVATE ${RS18_DIR})
set_target_properties(worm2d_rs18 PROPERTIES OUTPUT_NAME main)

# ---------------------------------------------------------------------------
# network2021/main  →  worm2d/bin/network2021/main
# ---------------------------------------------------------------------------
set(NET21_DIR ${CPP_DIR}/network2021)
add_worm_binary(worm2d_net21 "network2021"
    ${NET21_DIR}/main.cpp
    ${NET21_DIR}/Worm.cpp
    ${NET21_DIR}/WormBody.cpp
    ${NET21_DIR}/NervousSystem.cpp
    ${NET21_DIR}/Segment.cpp
    ${NET21_DIR}/Muscles.cpp
    ${NET21_DIR}/TSearch.cpp
    ${NET21_DIR}/random.cpp
    ${CPP_DIR}/argUtils.cpp
)
target_include_directories(worm2d_net21 BEFORE PRIVATE ${NET21_DIR})
set_target_properties(worm2d_net21 PROPERTIES OUTPUT_NAME main)

# ---------------------------------------------------------------------------
# CE_orientation/main  →  worm2d/bin/CE_orientation/main
# ---------------------------------------------------------------------------
set(CEO_DIR ${CPP_DIR}/CE_orientation)
add_worm_binary(worm2d_ceo "CE_orientation"
    ${CEO_DIR}/main.cpp
    ${CEO_DIR}/CTRNN.cpp
    ${CEO_DIR}/WormAgent.cpp
    ${CEO_DIR}/TSearch.cpp
    ${CEO_DIR}/random.cpp
    ${CPP_DIR}/argUtils.cpp
)
target_include_directories(worm2d_ceo BEFORE PRIVATE ${CEO_DIR})
set_target_properties(worm2d_ceo PROPERTIES OUTPUT_NAME main)
