cmake_minimum_required(VERSION 3.16)

project(DoDOOP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable vectorization report flags for GCC and Clang compilers
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GCC flags
    add_compile_options(-ftree-vectorize -fopt-info-vec-optimized)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
    # Clang / Apple Clang (macOS) flags
    add_compile_options(-fvectorize -Rpass=loop-vectorize) # -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize)
elseif(MSVC)
    # MSVC (Visual Studio) flags
    # add_compile_options(/Qvec-report:1) #  Level 1 prints ONLY successfully vectorized loops (e.g., line 76)
    add_compile_options(/Qvec-report:1) #  Level 2 prints ALL loops, including those that were not vectorized (e.g., line 90)
endif()

add_executable(DoDOOP main.cpp)

include(GNUInstallDirs)
install(TARGETS DoDOOP
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
