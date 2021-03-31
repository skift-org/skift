file(GLOB_RECURSE SYSTEM_SRC
     "*.h"
     "*.cpp"
)

add_library(libsystem ${SYSTEM_SRC})