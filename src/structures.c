#include "structures.h"

/* Create a new list. */
struct list *new_list()
{
  struct list * new = (struct list *) malloc(sizeof(struct list));
  new->first = NULL;
  new->last  = NULL;
  new->size  = 0;
  return new;
}

/* Add a element to a list. The element is stored in a 'item'. */
void list_add(struct list *alist, void *element)
{
  struct item *new = (struct item *) malloc(sizeof(struct item));
  new->value = element;
  new->next  = NULL;
  if (alist->first == NULL) {
    alist->first = new;
    alist->last  = new;
  } else {
    (*alist->last).next = new;
    alist->last = new;
  }
  alist->size++;
}

/* Extrae el primer elemento de la lista. Ojo con el free */
void *extract_first(struct list *alist)
{
  struct item *tmp = alist->first;
  void *val = tmp->value;
  if (alist->last == tmp)
    alist->last = NULL;
  alist->first = tmp->next;
  alist->size--;
  free(tmp);
  return val;
}

/* Delete a list (and free). */
void list_del(struct list *alist)
{
  struct item *act, *next;
  for(act = alist->first; act != NULL; act = next){
    next = act->next;
    free(act->value);
    free(act);
  }
  free(alist);
}

/******************************************************************************/
struct instance *new_instance(char *name)
{
  struct instance *ins = (struct instance *) malloc(sizeof(struct instance));
  ins->name      = name;
  ins->s_teacher = 0;
  ins->s_class   = 0;
  ins->s_room    = 0;
  return ins;
}

struct event *new_event(short duration)
{
  struct event *ev = (struct event *) malloc(sizeof(struct event));
  ev->duration = duration;
  ev->class = -1; ev->room = -1; ev->teacher = -1;
  return ev;
}

/* vim: set ts=2 sw=2 sts=2 tw=80 : */
