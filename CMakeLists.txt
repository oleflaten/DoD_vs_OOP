cmake_minimum_required(VERSION 3.16)

project(DoDOOP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable vectorization report flags for GCC and Clang compilers
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-ftree-vectorize -fopt-info-vec-optimized)
elseif(MSVC)
    add_compile_options(/Qvec-report:2)
endif()

add_executable(DoDOOP main.cpp)

include(GNUInstallDirs)
install(TARGETS DoDOOP
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
