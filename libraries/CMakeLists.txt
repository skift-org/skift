include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libc)

add_subdirectory(libsystem)
add_subdirectory(libio)
add_subdirectory(libcompression)