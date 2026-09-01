cmake_minimum_required(VERSION 3.20)

project(DoD_OOP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable vectorization report flags for GCC compiler
# This tells what, if any, loops are optimized to SIMD
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GCC flags
    add_compile_options(-ftree-vectorize -fopt-info-vec-optimized)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
    # Clang / Apple Clang (macOS) flags
    add_compile_options(-fvectorize -Rpass=loop-vectorize) # -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize)
elseif(MSVC)
    # --------------------------------------------------------------------------
        # Choose ONE block below to test different SIMD register widths:
        # --------------------------------------------------------------------------

        # 1. No Auto-Vectorization (Force Scalar Processing)
        # /O2 optimizes, but /d2Qvec- disables the loop vectorizer
        # add_compile_options(/O2 /d2Qvec-)

        # 2. 128-bit Vectorization (4 floats)
        # /arch:SSE2 is the default on x64, so explicit flag is optional
        # add_compile_options(/O2 /arch:SSE2)

        # 3. 256-bit Vectorization (8 floats)
        add_compile_options(/O2 /arch:AVX2)

        # 4. 512-bit Vectorization (16 floats) - (depricated)
        # Only works on AMD Zen4 cpus and Intel 11th gen and LOWER.
        # add_compile_options(/O2 /arch:AVX512)


    add_compile_options(/Qvec-report:1) #  Level 1 prints ONLY successfully vectorized loops (DoD for loop)
                                        #  Level 2 prints ALL loops, including those that were not vectorized (OOP for loops)
endif()

add_executable(DoD_OOP main.cpp)

include(GNUInstallDirs)
install(TARGETS DoD_OOP
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
