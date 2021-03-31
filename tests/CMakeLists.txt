file(GLOB_RECURSE TEST_SRC
     "*.h"
     "*.cpp"
)

add_executable(tests ${TEST_SRC})
target_link_libraries(tests libio libsystem libcompression)
target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/libraries ${CMAKE_SOURCE_DIR}/libraries/libc)