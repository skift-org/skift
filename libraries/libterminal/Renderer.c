#include <libsystem/cstring.h>
#include <libterminal/Renderer.h>
#include <libterminal/Terminal.h>

void terminal_renderer_initialize(TerminalRenderer *renderer)
{
    memset(renderer, 0, sizeof(TerminalRenderer));
}

void terminal_renderer_destroy(TerminalRenderer *renderer)
{
    renderer->destroy(renderer);
    free(renderer);
}

