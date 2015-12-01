#include <stdio.h>  /* for perror() */
#include <stdlib.h> /* for exit() */

void DieWithError(char *errorMessage)
{
    fprintf(stderr, "%s\n", errorMessage);
    fprintf(stderr, "Exiting...\n");
    exit(1);
}
