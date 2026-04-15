include(ExternalProject)

set(EXTERNAL_PROJECT_TOOLCHAIN_ARGS
    -DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
)

ExternalProject_Add(cxxopts
    GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
    GIT_TAG v3.0.0
    PREFIX ${CMAKE_BINARY_DIR}/cxxopts
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/cxxopts
               ${EXTERNAL_PROJECT_TOOLCHAIN_ARGS}
               -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
               -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
)

ExternalProject_Add(indicators
    GIT_REPOSITORY https://github.com/p-ranav/indicators.git
    GIT_TAG ac6c93ea2b1f97a220d10a0729a625b3f51e320b
    PREFIX ${CMAKE_BINARY_DIR}/indicators
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/indicators
               ${EXTERNAL_PROJECT_TOOLCHAIN_ARGS}
               -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
               -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
)

add_executable(astra-update astra-update.cpp)
add_dependencies(astra-update astraupdate)
add_dependencies(astra-update cxxopts)
add_dependencies(astra-update indicators)

add_executable(astra-boot astra-boot.cpp)
add_dependencies(astra-boot astraupdate)
add_dependencies(astra-boot cxxopts)
add_dependencies(astra-boot indicators)

include_directories(../include)
include_directories(${CMAKE_BINARY_DIR}/cxxopts/include)
include_directories(${CMAKE_BINARY_DIR}/indicators/include)

target_link_libraries(astra-update astraupdate)
target_link_libraries(astra-boot astraupdate)