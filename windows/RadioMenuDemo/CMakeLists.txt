cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(RadioMenuDemo
        DESCRIPTION "Displays radio-type menu items"
        LANGUAGES C)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_compile_definitions(UNICODE)
add_compile_definitions(_UNICODE)
add_compile_options(/GS-)

add_executable(rbm WIN32 rbm.c)

# Don't depend on the C runtime library and its startup in Release builds
target_link_options(rbm PRIVATE $<$<CONFIG:RelWithDebInfo>:/NODEFAULTLIB /ENTRY:startup>)
target_link_options(rbm PRIVATE $<$<CONFIG:Release>:/NODEFAULTLIB /ENTRY:startup>)

target_link_libraries(rbm kernel32.lib)
target_link_libraries(rbm gdi32.lib user32.lib)
target_link_libraries(rbm Shcore.lib)

add_executable(rbma WIN32 rbma.c)
target_link_libraries(rbma gdi32.lib user32.lib)
