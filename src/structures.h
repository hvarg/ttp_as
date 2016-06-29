#ifndef STRUCTURES_H_
#define STRUCTURES_H_
#include <stdlib.h>

struct item{
  void *value;
  struct item *next;
};

struct list{
  struct item *first;
  struct item *last;
  unsigned int size;
};

struct list *new_list();
void list_add(struct list *alist, void *element);
void *extract_first(struct list *alist);
void list_del(struct list *alist);

/***********************************/
struct instance{
  char  *name, **times, **teachers, **rooms, **classes;
  short s_time, s_teacher, s_room, s_class, s_event;
  struct event **events;
};

struct event{
  short duration, class, teacher, room;
};

struct instance *new_instance (char *name);
struct event    *new_event    (short duration);

#endif
/* vim: set ts=2 sw=2 sts=2 tw=80 : */
