cmake_minimum_required(VERSION 3.10)

# Project Name
project(webrtc)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VCPKG_DIR "C:/vcpkg/vcpkg")

# Include vcpkg toolchain file if it exists
if(EXISTS "${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake")
endif()

# Detect Operating System
if (WIN32)
    message(STATUS "Configuring for Windows")
    include_directories("${VCPKG_DIR}/installed/x64-windows/include")
elseif (UNIX)
    message(STATUS "Configuring for Linux")
    # Include directories for uSockets and uWebSockets
    include_directories(/usr/local/include/uWebSockets)
else()
    message(FATAL_ERROR "Unsupported OS")
endif()

# Add Executable
add_executable(webrtc server.cpp)



# Link Libraries
if (WIN32)
target_link_libraries(webrtc PRIVATE 
"${VCPKG_DIR}/installed/x64-windows/lib/libssl.lib"
"${VCPKG_DIR}/installed/x64-windows/lib/libcrypto.lib"
"${VCPKG_DIR}/installed/x64-windows/lib/uSockets.lib"
"${VCPKG_DIR}/installed/x64-windows/lib/zlib.lib"
)
elseif (UNIX)
    # Linking against uWebSockets, OpenSSL, Zlib, and pthread
    target_link_libraries(webrtc PRIVATE 
        -l:uSockets.a 
        ssl 
        crypto 
        z 
        pthread
    )
endif()

