#include <string.h>
#include <libxml/parser.h>

/* http://xmlsoft.org/examples/index.html */

char _STR_INS[] = "Instances";

xmlNode *search(xmlNode *root, char *string)
{
  xmlNode *node;
  for (node = root->children; node; node = node->next) {
    if (!strcmp(node->name,string)) 
		return node;
  }
  return NULL;
}

void print_child(xmlNode *root)
{
  xmlNode *node;
  for (node = root->children; node; node = node->next)
    printf("\t Child is <%s> (%i)\n", node->name, node->type);
}

void parser(char *filename)
{
  xmlDoc  *document = xmlReadFile(filename, NULL, 0);
  xmlNode *root     = xmlDocGetRootElement(document),
          *node;
  xmlFreeDoc(document);
  node = search(root, _STR_INS);
  print_child(node);
}
/* vim: set ts=2 sw=2 sts=2 tw=80 : */
