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
  ev->class    = -1;
  ev->room     = -1;
  ev->teacher  = -1;
  return ev;
}

struct result *new_result (struct instance *ins)
{
  int i;
  struct result *r = (struct result *) malloc(sizeof(struct result));
  r->ins  = ins;
  r->txt  = (bool **)  malloc(ins->s_teacher * sizeof(bool *) );
  r->cxt  = (bool **)  malloc(ins->s_class   * sizeof(bool *) );
  r->rxt  = (bool **)  malloc(ins->s_room    * sizeof(bool *) );
  r->ph   = (float **) malloc(ins->s_event   * sizeof(float *));
  r->path = (short *)  malloc(ins->s_event   * sizeof(short)  );
  for(i=0; i < ins->s_teacher; r->txt[i++] = malloc(ins->s_time*sizeof(bool)));
  for(i=0; i < ins->s_class;   r->cxt[i++] = malloc(ins->s_time*sizeof(bool)));
  for(i=0; i < ins->s_room;    r->rxt[i++] = malloc(ins->s_time*sizeof(bool)));
  for(i=0; i < ins->s_event;   r->ph[i++] = malloc(ins->s_event*sizeof(float)));
  return r;
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
  for(i=0; i < r->ins->s_event;   free(r->ph[i++]) );
  free(r->txt ); 
  free(r->cxt ); 
  free(r->rxt ); 
  free(r->ph  ); 
  free(r->path);
  free(r);
}

/* vim: set ts=2 sw=2 sts=2 tw=80 : */
