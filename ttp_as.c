#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>

int main(int argc, const char * args[])
{
  if (argc < 2) {
    printf("Modo de uso: ./ttp_as archivo.xml\n");
    return EXIT_FAILURE;
  }
  char    *filename = (char*) args[1];
  xmlDoc  *document = xmlReadFile(filename, NULL, 0);
  xmlNode *root     = xmlDocGetRootElement(document),
          *node;
  xmlFreeDoc(document);

  printf("Root is <%s> (%i)\n", root->name, root->type);
  for (node = root->children; node; node = node->next)
    printf("\t Child is <%s> (%i)\n", node->name, node->type);
  return EXIT_SUCCESS;
}

/* vim: set ts=2 sw=2 sts=2 tw=80 : */
