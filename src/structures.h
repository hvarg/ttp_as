#ifndef STRUCTURES_H_
#define STRUCTURES_H_
#include <stdlib.h>

#define NONE -1

struct instance{
  /* Equivalentes entre id (int) y su tag (str). */
  char  *name, **times, **teachers, **rooms, **classes;
  /* Numero de recursos. */
  short s_time, s_teacher, s_room, s_class, s_event;
  /* Arreglo de eventos, su indice es su id. */
  struct event **events;
};

struct event{
  /* Duracion del evento.  */
  short duration;
  /* Id de los recursos requeridos (-1 cualquiera).*/
  short class, teacher, room;
};

struct r_as{
  short time, teacher, class, room;
};

struct result{
  /* Puntero a la instancia resuelta. */
  struct instance *ins;
  /* Tablas de utilización de recursos por tiempo:
   *  txt: Teacher X Time
   *  cxt: Class X Time
   *  rxt: Room X Time */
  short   **txt, **cxt, **rxt;
  /* Tabla de feromonas. ph[Evento_inicial][Evento_final]*/
  float  **ph;
  /* Camino de eventos. */
  short  *path;
  /* Recursos asignados a los eventos. */
  struct r_as **ev;
};

/* La asignacion de un evento. */

struct instance   *new_instance   (char *name);
struct event      *new_event      (short duration);
struct result     *new_result     (struct instance *ins);
void              del_instance    (struct instance *ins);
void              del_event       (struct event *ev);
void              del_result      (struct result *r);

#endif
/* vim: set ts=2 sw=2 sts=2 tw=80 : */
