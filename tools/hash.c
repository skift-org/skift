// Really simple hash just for quick check ;)

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    FILE *fp = fopen ( argv[1] , "rb" );
    if( !fp ) 
        perror(argv[1]),
        exit(1);

    fseek( fp , 0L , SEEK_END);
    long lSize = ftell( fp );
    rewind( fp );

    /* allocate memory for entire content */
    char *buffer = calloc(1, lSize+1);
    if( !buffer ) 
        fclose(fp),
        fputs("memory alloc fails",stderr),
        exit(1);

    /* copy the file into the buffer */
    if(1 != fread( buffer , lSize, 1 , fp))
        fclose(fp),
        free(buffer),
        fputs("entire read fails",stderr),
        exit(1);

    int hash = 0;

    for(long i = 0; i < lSize; i++)
        hash += ((char*)buffer)[i];

    printf("hash= %08x\n", hash);

    fclose(fp);
    free(buffer);

    return 0;
}
