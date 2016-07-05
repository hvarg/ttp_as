#ifndef STRUCTURES_H_
#define STRUCTURES_H_
#include <stdlib.h>

/* Entero que se entendera como NONE, no puede ser >= 0. */
#define NONE -1

struct instance{
  /* Equivalencias entre id (int) y su tag (str). */
  char  *name, **times, **teachers, **rooms, **classes;
  /* Numero de recursos. */
  short s_time, s_teacher, s_room, s_class, s_event;
  /* Arreglo de eventos, su indice es su id. */
  struct event **events;
};

struct event{
  /* Duracion del evento. */
  short duration;
  /* Id de los recursos requeridos (NONE: cualquiera). */
  short class, teacher, room;
};

/* Estructura de recursos asignados. */
struct r_as{
  short time, teacher, class, room;
  /* Valor de esta asignacion. */
  short value;
};

struct result{
  /* Puntero a la instancia resuelta. */
  struct instance *ins;
  /* Tablas de utilizacion de recursos por tiempo:
   *  txt: Teacher X Time
   *  cxt: Class X Time
   *  rxt: Room X Time */
  short   **txt, **cxt, **rxt;
  /* Camino de eventos. */
  short  *path;
  /* Recursos asignados a los eventos. */
  struct r_as **ev;
  /* Valor total del resultado. */
  float value;
};

/* Nodo de un set de tiempos. */
struct time_node{
  short id;
  struct time_node *next;
};

/* Set de tiempos. */
struct time_set{
  short size;
  struct time_node *first;
};


struct instance   *new_instance   (char *name);
struct event      *new_event      (short duration);
struct result     *new_result     (struct instance *ins);
struct time_set   *new_time_set   (short range);
void              del_instance    (struct instance *ins);
void              del_event       (struct event *ev);
void              del_result      (struct result *r);
void              del_time_set    (struct time_set *ts);
void              ts_rm           (struct time_set *ts, short id);
short             ts_get_and_rm   (struct time_set *ts, short i);

#endif
/* vim: set ts=2 sw=2 sts=2 tw=80 : */
