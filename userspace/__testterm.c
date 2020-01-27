/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/io/Stream.h>

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    printf("\e[mColors: ");

    for (int i = 0; i < 8; i++)
    {
        printf("\e[%dm ", 40 + i);
    }

    for (int i = 0; i < 8; i++)
    {
        printf("\e[%dm ", 100 + i);
    }

    printf("\n");

    printf("\e[mStyles:");
    printf(" regular");
    printf(" \e[;4munderline");
    printf(" \e[;1mbold");
    printf(" \e[;3minverted\n");

    printf("\e[mUnicode: ÇüéâäàåçêëèïîìÄÅÉæÆôöòûùÿÖÜ¢£¥₧ƒáíóúñÑªº¿⌐¬½¼¡«»░▒▓│┤╡╢╖╕╣║╗╝╜╛┐└┴┬├─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀αßΓπΣσµτΦΘΩδ∞φε∩≡±≥≤⌠⌡÷≈°∙·√ⁿ²■☺☻♥♦♣♠•◘○◙♂♀♪♫☼⌂►◄↕‼¶§▬↨↑↓→←∟↔▲▼");

    return 0;
}
