#include <stdio.h>
#include <stdlib.h>
#include "xhstt_parser.h"

int main(int argc, const char * args[])
{
  if (argc < 2) {
    printf("Modo de uso: ./ttp_as archivo.xml\n");
    return EXIT_FAILURE;
  }

	parser((char *) args[1]);

  return EXIT_SUCCESS;
}

/* vim: set ts=2 sw=2 sts=2 tw=80 : */
