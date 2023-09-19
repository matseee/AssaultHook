cmake_minimum_required(VERSION 3.27)
if(${CMAKE_VERSION} VERSION_LESS 3.27)
    cmake_policy(VERSION 3.27)
endif()

project(AssaultHook VERSION 0.1.0
                    DESCRIPTION "AssaultCube internal cheat"
                    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(memorylib STATIC src/memory/memory.cpp src/memory/memory.lnx.cpp)
target_include_directories(memorylib PUBLIC include)
target_compile_features(memorylib PUBLIC cxx_std_20)

# to build a executable application:
add_executable(AssaultHookInjector src/injector.cpp)
target_link_libraries(AssaultHookInjector PUBLIC memorylib)

# to build a library (STATIC, SHARED, MODULE):
# add_library(MemoryLibrary STATIC ${SOURCES})






# ----- old things -----
# set(INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dist/${CMAKE_BUILD_TYPE}/bin/")
# include_directories(src)
# include_directories(src/memory)
# set(SOURCES src/injector.cpp src/memory/memory.cpp src/memory/memory.lnx.cpp)
# file(GLOB SOURCES "src/*.cpp")
