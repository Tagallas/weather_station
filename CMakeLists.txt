cmake_minimum_required(VERSION 3.5)
project("weather station")
add_executable(main server_node.c)

# Linux/Unix configuration using pkg-config
find_package(PkgConfig)
pkg_check_modules(open62541 REQUIRED open62541)
target_link_libraries(main open62541)

# Alternative CMake-based library definition.
# This might not be included in some package distributions.
#
#   find_package(open62541 REQUIRED)
#   target_link_libraries(main open62541::open62541)