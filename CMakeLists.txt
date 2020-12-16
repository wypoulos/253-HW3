cmake_minimum_required(VERSION 3.14)
project(hw3)

    # Using -Wall is required:
    add_compile_options(-Wall)

    # These compile flags are highly recommended, but not required:
    add_compile_options(-Wextra -Wpedantic)

    # Optional super-strict mode:
    add_compile_options(-fmessage-length=80 -fno-diagnostics-show-option)
    add_compile_options(-fstack-protector-all -g -O3 -std=c++14 -Walloc-zero)
    add_compile_options(-Walloca -Wctor-dtor-privacy -Wduplicated-cond)
    add_compile_options(-Wduplicated-branches -Werror -Wfatal-errors -Winit-self)
    add_compile_options(-Wlogical-op -Wold-style-cast -Wshadow)
    add_compile_options(-Wunused-const-variable=1 -Wzero-as-null-pointer-constant)

    # add_compile_options must be BEFORE add_executable.

    # Create the executable hw0 from the source file main.cc:
    add_executable(hw3 hw3.cc)

    # Create a tar file every time:
    add_custom_target(hw3.tar ALL COMMAND tar cf hw3.tar hw3.cc CMakeLists.txt)
