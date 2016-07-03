#include "structures.h"

struct instance *new_instance (char *name)
{
  struct instance *ins = (struct instance *) malloc(sizeof(struct instance));
  ins->name      = name;
  ins->s_teacher = 0;
  ins->s_class   = 0;
  ins->s_room    = 0;
  ins->times     = NULL;
  ins->teachers  = NULL;
  ins->rooms     = NULL;
  ins->classes   = NULL;
  ins->events    = NULL;
  return ins;
}

struct event *new_event (short duration)
{
  struct event *ev = (struct event *) malloc(sizeof(struct event));
  ev->duration = duration;
  ev->class    = NONE;
  ev->room     = NONE;
  ev->teacher  = NONE;
  return ev;
}

struct result *new_result (struct instance *ins)
{
  int i, j;
  struct result *r = (struct result *) malloc(sizeof(struct result));
  r->ev   = (struct r_as **) malloc(ins->s_event * sizeof(struct r_as *));
  r->ins  = ins;
  r->path = (short *)  malloc(ins->s_event   * sizeof(short));
  r->txt  = (short **) malloc(ins->s_teacher * sizeof(short *));
  r->cxt  = (short **) malloc(ins->s_class   * sizeof(short *));
  r->rxt  = (short **) malloc(ins->s_room    * sizeof(short *));
  for(i=0; i < ins->s_event;   r->path[i++] = NONE);
  for (i=0; i < ins->s_teacher; i++) {
    r->txt[i] = (short *) malloc(ins->s_time*sizeof(short));
    for (j = 0; j < ins->s_time; r->txt[i][j++] = NONE);
  }
  for (i=0; i < ins->s_class; i++) {
    r->cxt[i] = (short *) malloc(ins->s_time*sizeof(short));
    for (j = 0; j < ins->s_time; r->cxt[i][j++] = NONE);
  }
  for (i=0; i < ins->s_room; i++) {
    r->rxt[i] = (short *) malloc(ins->s_time*sizeof(short));
    for (j = 0; j < ins->s_time; r->rxt[i][j++] = NONE);
  }
  for (i=0; i < ins->s_event; i++) {
    r->ev[i] = malloc(ins->events[i]->duration * sizeof(struct r_as));
    for (j = 0; j < ins->events[i]->duration; j++) {
      r->ev[i][j].teacher = ins->events[i]->teacher;
      r->ev[i][j].class   = ins->events[i]->class;
      r->ev[i][j].room    = ins->events[i]->room;
      r->ev[i][j].time    = NONE;
    }
  }
  r->value = 0.0;
  return r;
}

struct time_set *new_time_set (short range)
{
  int i;
  struct time_set  *ts = malloc(sizeof(struct time_set));
  struct time_node *actual;
  ts->size  = range;
  ts->first = malloc(sizeof(struct time_node));
  for (i = 0, actual = ts->first; i < range; actual = actual->next, i++) {
    actual->id = i;
    if (i + 1 < range)
      actual->next = malloc(sizeof(struct time_node));
    else
      actual->next = NULL;
  }
  return ts;
}

void del_instance (struct instance *ins)
{
  int i;
  if (ins->times    != NULL) free(ins->times   );
  if (ins->teachers != NULL) free(ins->teachers);
  if (ins->rooms    != NULL) free(ins->rooms   );
  if (ins->classes  != NULL) free(ins->classes );
  if (ins->events   != NULL) 
    for (i = 0; i < ins->s_event; del_event(ins->events[i++]));
  free(ins);
}

void del_event (struct event *ev)
{
  free(ev);
}

/* Siempre usar antes de del_instance. */
void del_result (struct result *r)
{
  int i;
  for(i=0; i < r->ins->s_teacher; free(r->txt[i++]));
  for(i=0; i < r->ins->s_class;   free(r->cxt[i++]));
  for(i=0; i < r->ins->s_room;    free(r->rxt[i++]));
  for(i=0; i < r->ins->s_event;   free(r->ev[i++]));
  free(r->path);
  free(r->txt); 
  free(r->cxt); 
  free(r->rxt); 
  free(r->ev); 
  free(r);
}

void del_time_set (struct time_set *ts)
{
  struct time_node *actual, *next;
  for(actual = ts->first; actual; actual = next) {
    next = actual->next;
    free( actual );
  }
  free(ts);
}

void ts_rm (struct time_set *ts, short id)
{
  struct time_node *actual, *prev;
  for (actual = ts->first; actual; actual = actual->next) {
    if (actual->id == id) {
      if (actual == ts->first) ts->first = actual->next;
      else prev->next = actual->next;
      free(actual);
      ts->size--;
      return;
    } else if (actual->id > id) return;
    prev = actual;
  }
}

short ts_get_and_rm(struct time_set *ts, short i)
{
  short j, id;
  struct time_node *actual, *prev;
  for (j = 0, actual = ts->first; actual; actual = actual->next, j++) {
    if (j == i) {
      if (actual == ts->first) ts->first = actual->next;
      else prev->next = actual->next;
      id = actual->id;
      free(actual);
      ts->size--;
      return id ;
    }
    prev = actual;
  }
  return -1;
}
/* vim: set ts=2 sw=2 sts=2 tw=80 : */
