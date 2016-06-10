#include <stdio.h>

int main(int argc, char *argv[])
{
    int c;
    for(c=getchar();c!=EOF;c=getchar()) {
        if(c == '\\') {
            c = getchar();
            switch (c) {
                case EOF:
                    fprintf(stderr,"EOF during escape\n");
                    return -1;
                case 'a':
                    putchar('\a');
                    break;
                case 'b':
                    putchar('\b');
                    break;
                case 'f':
                    putchar('\f');
                    break;
                case 'n':
                    putchar('\n');
                    break;
                case 'r':
                    putchar('\r');
                    break;
                case 't':
                    putchar('\t');
                    break;
                case 'v':
                    putchar('\v');
                    break;
                default:
                    putchar(c);
                    break;
            }
        } else {
            putchar(c);
        }
    }
}
