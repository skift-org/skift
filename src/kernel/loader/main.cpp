#include <karm-main/main.h>

#include "loader.h"

CliResult entryPoint(CliArgs) {
    Sys::errln("");
    Sys::errln(".__                    .___");
    Sys::errln("|  |   _________     __| _/___________");
    Sys::errln("|  |  /  _ \\__  \\   / __ |/ __ \\_  __ \\");
    Sys::errln("|  |_(  <_> ) __ \\_/ /_/ \\  ___/|  | \\/");
    Sys::errln("|____/\\____(____  /\\____ |\\___  >__|");
    Sys::errln("                \\/      \\/    \\/");
    Sys::errln("");

    return Loader::load("/boot/kernel.elf");
}
