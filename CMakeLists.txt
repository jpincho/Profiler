cmake_minimum_required ( VERSION 3.1 )

project ( Profiler )

set ( CMAKE_CXX_STANDARD 11 )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )

option ( PROFILER_IMGUI_WINDOW "Build ImGui interface" TRUE )
configure_file ( ProfilerConfig.h.in ${CMAKE_CURRENT_LIST_DIR}/ProfilerConfig.h )
file ( GLOB SOURCES "*.cpp" "*.h" ProfilerConfig.h.in AstyleConfig.astylerc )
add_library ( ${PROJECT_NAME} ${SOURCES} )
