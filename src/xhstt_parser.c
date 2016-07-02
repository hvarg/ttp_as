#include <string.h>
#include <libxml/parser.h>
#include "xhstt_parser.h"

/* http://xmlsoft.org/examples/index.html */
/* http://www.it.usyd.edu.au/~jeff/cgi-bin/hseval.cgi?op=spec */

static char _STR_INSTANCES_[] = "Instances",
            _STR_INSTANCE_[]  = "Instance",
            _STR_TIMES_[]     = "Times",
            _STR_TIME_[]      = "Time",
            _STR_RESOURCES_[] = "Resources",
            _STR_RESOURCE_[]  = "Resource",
            _STR_EVENTS_[]    = "Events",
            _STR_EVENT_[]     = "Event",
            _STR_RTYPE_[]     = "ResourceType",
/*            _STR_RTYPES_[]    = "ResourceTypes",
            _STR_STUDENT_[]   = "Student", */
            _STR_TEACHER_[]   = "Teacher",
            _STR_ROOM_[]      = "Room",
            _STR_CLASS_[]     = "Class",
            _STR_ID_[]        = "Id",
            _STR_DURATION_[]  = "Duration",
            _STR_ROLE_[]      = "Role",
            _STR_REFERENCE_[] = "Reference";

xmlNode *get_next(xmlNode *actual, char *string)
{
  xmlNode *node;
  for (node = actual->next; node; node = node->next) {
    if ( !strcmp((char *) node->name, string) )
      return node;
  }
  return NULL;
}

xmlNode *get_child(xmlNode *root, char *string)
{
  if ( !strcmp((char *) root->children->name, string) )
    return root->children;
  return get_next(root->children, string);
}

unsigned int count_childs(xmlNode *root, char *string)
{
  xmlNode *node;
  unsigned int count = 0;
  for (node = root->children; node; node = node->next) {
    if (!strcmp((char *) node->name, string))
      count++;
  }
  return count;
}

char * get_attr(xmlNode *node, char *string)
{
  xmlAttr *attr;
  for (attr = node->properties; attr; attr = attr->next) {
    if (!strcmp((char *) attr->name, string))
      return (char *) xmlNodeListGetString(node->doc, attr->children, 1);
  }
  return NULL;
}

void print_childs(xmlNode *root)
{
  xmlNode *node;
  for (node = root->children; node; node = node->next) {
    if (!xmlIsBlankNode(node))
      printf("\t<%s> (%i): [%s]\n", node->name, node->type, xmlNodeGetContent(node));
  }
}

int parser(char *filename, struct instance ***ins)
{
  xmlDoc  *document = xmlReadFile(filename, NULL, 0);
  xmlNode *root     = xmlDocGetRootElement(document),
          *node, *n_inst, *n_time, *n_res, *n_event, *tmp, *aux;
  short s_ins,  i, j, k,
        sr=0, sc=0, st=0;
  char *ref, *value;

  node  = get_child(root, _STR_INSTANCES_);
  s_ins = count_childs(node, _STR_INSTANCE_);

  if (!s_ins) {
    fprintf(stderr, "%s has no instances.", filename);
    return -1;
  }

  struct instance **instances = malloc(s_ins * sizeof(struct instance *));

  for (n_inst = get_child(node, _STR_INSTANCE_), i = 0;
       n_inst;
       n_inst = get_next(n_inst, _STR_INSTANCE_), i++) {
    instances[i] = new_instance(get_attr(n_inst, _STR_ID_));
    //printf("%s\n", instances[i]->name); fflush(stdout);

    /* Obteniendo los Times. */
    n_time               = get_child(n_inst, _STR_TIMES_);
    instances[i]->s_time = count_childs(n_time, _STR_TIME_);
    instances[i]->times  = malloc(instances[i]->s_time * sizeof(char*));
    for (node = get_child(n_time, _STR_TIME_), j = 0;
         node;
         node = get_next(node, _STR_TIME_), j++) {
      instances[i]->times[j] = get_attr(node, _STR_ID_);
    }

    //printf("Times\n"); fflush(stdout);
    /* Obteniendo los Resources. */
    n_res = get_child(n_inst, _STR_RESOURCES_);
    for (node = get_child(n_res, _STR_RESOURCE_);
         node;
         node = get_next(node, _STR_RESOURCE_)) {
      ref = get_attr( get_child(node, _STR_RTYPE_), _STR_REFERENCE_ );
      if      ( !strcmp(ref, _STR_ROOM_) )        instances[i]->s_room++;
      else if ( !strcmp(ref, _STR_TEACHER_) )     instances[i]->s_teacher++;
      else if ( !strcmp(ref, _STR_CLASS_) )       instances[i]->s_class++;
      else {
        fprintf(stderr, "Undefined resource type.\n");
        return -2;
      }
    }

    instances[i]->teachers = malloc(instances[i]->s_teacher * sizeof(char *));
    instances[i]->classes  = malloc(instances[i]->s_class   * sizeof(char *));
    instances[i]->rooms    = malloc(instances[i]->s_room    * sizeof(char *));
    
    for (node = get_child(n_res, _STR_RESOURCE_);
         node;
         node = get_next(node, _STR_RESOURCE_)) {
      ref = get_attr( get_child(node, _STR_RTYPE_), _STR_REFERENCE_ );
      if ( !strcmp(ref, _STR_ROOM_) )
        instances[i]->rooms[sr++]    = get_attr(node, _STR_ID_);
      else if ( !strcmp(ref, _STR_TEACHER_) )
        instances[i]->teachers[st++] = get_attr(node, _STR_ID_);
      else if ( !strcmp(ref, _STR_CLASS_) )
        instances[i]->classes[sc++]  = get_attr(node, _STR_ID_);
    }

    //printf("Resources\n"); fflush(stdout);
    /* Obteniendo los Events. */
    n_event               = get_child(n_inst, _STR_EVENTS_);
    instances[i]->s_event = count_childs(n_event, _STR_EVENT_);
    instances[i]->events  = malloc(instances[i]->s_event*sizeof(struct event*));
    for (node = get_child(n_event, _STR_EVENT_), j = 0;
         node;
         node = get_next(node, _STR_EVENT_), j++) {
      instances[i]->events[j] = new_event (
          atoi((char *) xmlNodeGetContent(get_child(node, _STR_DURATION_)))
      );
      for (tmp = get_child(get_child(node, _STR_RESOURCES_), _STR_RESOURCE_);
           tmp;
           tmp = get_next(tmp, _STR_RESOURCE_)) {
        if ( !(value = get_attr(tmp, _STR_REFERENCE_)) ) break;
        if ( !(aux = get_child(tmp, _STR_RTYPE_)) )
          if ( !(aux = get_child(tmp, _STR_ROLE_)) ) {
            fprintf(stderr, "IDK the resource type of some event.\n");
            return -3;
          } else
            ref = (char *) xmlNodeGetContent(aux);
        else 
          ref = get_attr(aux, _STR_REFERENCE_);

        if (!strcmp(ref, _STR_CLASS_)) {
          for (k = 0; k < instances[i]->s_class; k++)
            if (!strcmp(instances[i]->classes[k], value)) {
              instances[i]->events[j]->class = k; break;
            }
        }
        else if (!strcmp(ref, _STR_TEACHER_)) {
          for (k = 0; k < instances[i]->s_teacher; k++)
            if (!strcmp(instances[i]->teachers[k], value)) {
              instances[i]->events[j]->teacher = k; break;
            }
        }
        else if (!strcmp(ref, _STR_ROOM_)) {
          for (k = 0; k < instances[i]->s_room; k++)
            if (!strcmp(instances[i]->rooms[k], value)) {
              instances[i]->events[j]->room = k; break;
            }
        }
      }
    }
  }
  
  printf("The file has %d instances.\n", s_ins);
  for (i = 0; i < s_ins; i++) {
    printf(" Instance[%d] = %s\n",i, instances[i]->name);
    printf("  Times[0:%d] = {", instances[i]->s_time);
    for (j = 0; j < instances[i]->s_time; j++) {
      if ( j != 0) printf(", ");
      printf("%s", instances[i]->times[j]);
    }
    printf("}\n  Teachers[0:%d] = {", instances[i]->s_teacher);
    for (j = 0; j < instances[i]->s_teacher; j++) {
      if ( j != 0) printf(", ");
      printf("%s", instances[i]->teachers[j]);
    }
    printf("}\n  Classes[0:%d] = {", instances[i]->s_class);
    for (j = 0; j < instances[i]->s_class; j++) {
      if ( j != 0) printf(", ");
      printf("%s", instances[i]->classes[j]);
    }
    printf("}\n  Rooms[0:%d] = {", instances[i]->s_room);
    for (j = 0; j < instances[i]->s_room; j++) {
      if ( j != 0) printf(", ");
      printf("%s", instances[i]->rooms[j]);
    }
    printf("}\n  Events[0:%d] = \n", instances[i]->s_event);
    for (j = 0; j < instances[i]->s_event; j++) {
      printf("   Event[%d] = {Duration: %d, ", j, instances[i]->events[j]->duration); 
      printf("Class: %d (%s), Teacher: %d (%s), Room: %d (%s)}\n",
          instances[i]->events[j]->class,
          instances[i]->events[j]->class == -1 ? "NULL" :
            instances[i]->classes[instances[i]->events[j]->class],
          instances[i]->events[j]->teacher,
          instances[i]->events[j]->teacher == -1 ? "NULL" :
            instances[i]->teachers[instances[i]->events[j]->teacher],
          instances[i]->events[j]->room,
          instances[i]->events[j]->room == -1 ? "NULL" : 
            instances[i]->rooms[instances[i]->events[j]->room]);
    }
  }

  xmlFreeDoc(document);
  xmlCleanupParser();
  *ins = instances;
  return s_ins;
}

/* vim: set ts=2 sw=2 sts=2 tw=80 : */
