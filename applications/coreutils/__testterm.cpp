
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
    printf(" \e[4munderline\e[m");
    printf(" \e[1mbold\e[m");
    printf(" \e[3minverted\e[m");
    printf(" \e[1;3;4mall at the same time\n\e[m");

    printf("\e[mUnicode: ÇüéâäàåçêëèïîìÄÅÉæÆôöòûùÿÖÜ¢£¥₧ƒáíóúñÑªº¿⌐¬½¼¡«»░▒▓│┤╡╢╖╕╣║╗╝╜╛┐└┴┬├─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀αßΓπΣσµτΦΘΩδ∞φε∩≡±≥≤⌠⌡÷≈°∙·√ⁿ²■☺☻♥♦♣♠•◘○◙♂♀♪♫☼⌂►◄↕‼¶§▬↨↑↓→←∟↔▲▼");

    return PROCESS_SUCCESS;
}
