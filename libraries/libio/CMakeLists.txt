file(GLOB_RECURSE IO_SRC
     "*.h"
     "*.cpp"
)

add_library(libio ${IO_SRC})