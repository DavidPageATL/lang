cmake_minimum_required(VERSION 3.10)

# Set the project name
project(LangProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compile flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Include directories
include_directories(src)

# Add executable
add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/interpreter.cpp
)

# Optional: Add a library if you have multiple source files
# add_library(${PROJECT_NAME}_lib
#     src/utils.cpp
# )
# target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_lib)
