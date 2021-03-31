file(GLOB_RECURSE COMPRESSION_SRC
     "*.h"
     "*.cpp"
)

add_library(libcompression ${COMPRESSION_SRC})