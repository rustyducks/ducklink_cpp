cmake_minimum_required(VERSION 3.14)

option(CROSSCOMPILE_ARM "Cross compiling for arm architecture" OFF)


if(CROSSCOMPILE_ARM)
    if(DEFINED ENV{LINUX_ARM_TOOLCHAIN_PATH})
        set (LINUX_ARM_TOOLCHAIN_PATH $ENV{LINUX_ARM_TOOLCHAIN_PATH})
    else()
        set (LINUX_ARM_TOOLCHAIN_PATH /usr/lib/ccache)
    endif()
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/arm_build.cmake)
    add_compile_options(-Wall -Wextra -pedantic)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

project(Ducklink VERSION 0.1.0)
set (CMAKE_CXX_STANDARD 14)

include(GNUInstallDirs)
add_subdirectory(deps)

add_library(Ducklink STATIC src/SerialDucklink.cpp src/UDPDucklink.cpp ${Serialib_SRC_FILES})
target_include_directories(Ducklink PRIVATE ${Serialib_INCLUDE_DIR})
target_include_directories(Ducklink PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
        $<INSTALL_INTERFACE:include>
  # <prefix>/include/mylib)
)
set_target_properties(Ducklink PROPERTIES PUBLIC_HEADER "include/Ducklink/DucklinkBase.h;include/Ducklink/SerialDucklink.h;include/Ducklink/UDPDucklink.h;include/Ducklink/DucklinkBuffer.h")
### Install
install(TARGETS Ducklink
        EXPORT DucklinkTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        INCLUDES DESTINATION include
        PUBLIC_HEADER DESTINATION include/Ducklink
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    DucklinkConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(EXPORT DucklinkTargets
        FILE DucklinkTargets.cmake
        DESTINATION lib/cmake/Ducklink
)

configure_file(cmake/DucklinkConfig.cmake.in DucklinkConfig.cmake @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/DucklinkConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/DucklinkConfigVersion.cmake"
        DESTINATION lib/cmake/Ducklink
        )