cmake_minimum_required(VERSION 3.18)
project(Worm2D VERSION 0.1.0 LANGUAGES CXX)

add_subdirectory(native)
add_subdirectory(legacy)

set(WORM2D_NEUROML_RUNTIME_FILES
    neuromlLocal/__init__.py
    neuromlLocal/build_network.py
    neuromlLocal/create_new_lems_file.py
    neuromlLocal/main_sim.py
    neuromlLocal/regenerate.py
    neuromlLocal/utils.py
    neuromlLocal/cell_W2Dosc.xml
    neuromlLocal/cell_syn_W2D.xml
    neuromlLocal/musc_W2D.xml
    neuromlLocal/musc_X.xml
    neuromlLocal/musc_X_cells.xml
    neuromlLocal/syn_W2D.xml
)
install(FILES ${WORM2D_NEUROML_RUNTIME_FILES} DESTINATION neuromlLocal)
install(FILES F2_fig_behavior.py DESTINATION .)
