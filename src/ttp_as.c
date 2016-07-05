#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include "xhstt_parser.h"
#include "structures.h"

#define MY_SEED   time(NULL) //82 // Semilla para random. Puede ser un entero.
#define DEF_INS   0               // Indice de instancia con la que trabajar.
#define MAX_ITER  1000            // Maximo de iteraciones por defecto.
#define MAX_ANTS  20              // Hormigas por defecto.
#define _PH_MIN   0.01            // Feromona minima por defecto.
#define _PH_INV   100             // Inversa por defecto.
#define _EVAP     0.3             // Evaporacion por defecto.
#define _Q        0.05            // Q por defecto.
#define _ALPHA    2               // Alpha por defecto.
#define _BETA     0.5             // Beta por defecto.


float PH_MIN,PH_INV, EVAP, Q, ALPHA, BETA;  // Parametros.
float  **pheromone;                         // Feromonas.
short p_size;                               // Tamaño del arreglo de feromonas.

/* La siguiente funcion fue obtenida del comentario de Laurence Gonsalves en:
 *http://stackoverflow.com/questions/822323/how-to-generate-a-random-number-in-c
 * el 23/06/2015. */
/* Returns an integer in the range [0, n).
 * Uses rand(), and so is affected-by/affects the same seed. */
int randint(int n) {
    if ((n - 1) == RAND_MAX) {
        return rand();
    } else {
        long end = RAND_MAX / n; // truncate skew
        assert (end > 0L);
        end *= n;
        int r;
        while ((r = rand()) >= end);
        return r % n;
    }
}

/* Imprime el resultado por salida estandar. */
void print_res(struct result *R)
{
  int j, i;
#ifdef VERBOSE
  printf("\nTxT\t\tCXT\t\tRXT\n");
  for (i = 0; i < R->ins->s_time; i++) {
    for (j = 0; j < R->ins->s_teacher; j++) printf("%d ", R->txt[j][i]);
    printf("  ");
    for (j = 0; j < R->ins->s_class  ; j++) printf("%d ", R->cxt[j][i]);
    printf("  ");
    for (j = 0; j < R->ins->s_room   ; j++) printf("%d ", R->rxt[j][i]);
    printf("\n");
  }
  printf("\nPheromones:\n");
  for (i = 0; i < R->ins->s_event; i++){
    for (j = 0; j < R->ins->s_event; j++) printf("%0.3f ", pheromone[j][i]);
    printf("\n");
  }
#endif
  printf("\nPath:  ");
  for (j = 0; j < R->ins->s_event - 1; j++)
    printf("%d -> ", R->path[j]);
  printf("%d", R->path[j]);

  printf("Value: %f\n", R->value);
  for (j = 0; j < R->ins->s_event; j++) {
    printf("Event[%d], duration: %d\n", j, R->ins->events[j]->duration);
    for (i = 0; i < R->ins->events[j]->duration; i++) {
      printf("\t%d: Teacher: %s, Class: %s, Room: %s, Time: %s\n", i,
          R->ins->teachers[R->ev[j][i].teacher],
          R->ins->classes[R->ev[j][i].class],
          R->ev[j][i].room == -1?
             "None" : R->ins->rooms[R->ev[j][i].room],
          R->ev[j][i].time == -1?
             "None" : R->ins->times[R->ev[j][i].time]);
    }
  }
}

/* Evaporacion de la */
void ph_evap()
{
  int i, j;
  float tmp;
  for (i = 0; i < p_size; i++)
    for (j = 0; j < p_size; j++) {
      tmp = pheromone[i][j] * (1 - EVAP);
      pheromone[i][j] = tmp > PH_MIN ? tmp : PH_MIN;
    }
}

/* Agrega feromona al camino indicado. */
void ph_update(short *path)
{
   int i, j;
   for (i = 0, j = 1; j < p_size; i++, j++) {
     pheromone[path[i]][path[j]] += Q;
   }
}

/* Verifica si cierto evento ya fue realizado. */
int in_path(short *path, short value)
{
  short i;
  for (i = 0;; i++) {
    if (path[i] == NONE)        return 0;
    else if (path[i] == value)  return 1;
  }
}

/* Determina el siguiente evento con cierta probabilidad. */
short next_event(struct result *R, short actual)
{
  int total = 0, part = 0, i;
  for (i = 0; i < R->ins->s_event; i++)
    if (!in_path(R->path, i)) 
      total += (int) pow((pheromone[actual][i]*PH_INV), ALPHA) + 
 			               pow((R->ins->events[i]->duration), BETA);
  total = randint(total);
  for (i = 0;; i++) {
    if (!in_path(R->path, i))
      part += (int) pow((pheromone[actual][i]*PH_INV), ALPHA) +
									  pow((R->ins->events[i]->duration), BETA);
    if (part > total) return i;
  } 
}

/* Asigna recursos a cierto evento, si lo logra le da valor 0, si no hay
 * recursos disponibles le asigna -1. */
void assign(struct result *R, short id)
{
  struct time_set *ts = new_time_set(R->ins->s_time);
  short i, j, t, r, d = R->ins->events[id]->duration;
  for (i = 0; i < R->ins->s_time; i++) {
    if ( R->txt[R->ins->events[id]->teacher][i] != NONE)    ts_rm(ts, i);
    else if ( R->cxt[R->ins->events[id]->class][i] != NONE) ts_rm(ts, i);
    else if (R->ins->s_room != 0) {
      if ( R->ins->events[id]->room == -1 ) {
        for (j = 0; j < R->ins->s_room; j++)
          if (R->rxt[j][i] == NONE) break;
        if (j == R->ins->s_room) ts_rm(ts, i);
      } else if (R->rxt[R->ins->events[id]->room][i] != NONE)  ts_rm(ts, i);
    }
  }
  if (d <= ts->size)
    for (i = 0; i < d; i++) {
      t = ts_get_and_rm(ts, randint(ts->size));
      if ( R->ins->events[id]->room == -1 && R->ins->s_room != 0)
        while (R->rxt[(r = randint(R->ins->s_room))][t] != NONE);
      else r = R->ins->events[id]->room;
      R->txt[R->ins->events[id]->teacher][t] = id;
      R->cxt[R->ins->events[id]->class][t]   = id;
      if (R->ins->s_room != 0) R->rxt[r][t]  = id;
      R->ev[id][i].teacher = R->ins->events[id]->teacher;
      R->ev[id][i].class   = R->ins->events[id]->class;
      R->ev[id][i].room    = r;
      R->ev[id][i].time    = t;
      R->ev[id][i].value   = 0;
    }
  else for(i = 0; i < d; R->ev[id][i++].value = -1);
  del_time_set(ts);
}

/* El camino que sigue 1 hormiga. */
void ant(struct result *R)
{
  short actual = randint(R->ins->s_event), next, 
        i = 0, j;
  R->path[i] = actual;
  assign(R, actual);
  while (R->path[R->ins->s_event-1] == NONE) {
    next = next_event(R, actual);
    R->path[++i] = next;
    assign(R, next);
    actual = next;
  }
  for (i = 0; i < R->ins->s_event; i++) 
    for (j = 0; j < R->ins->events[i]->duration; j++)
      R->value += R->ev[i][j].value;
}

int main(int argc, const char * args[])
{
  /* Inicializacion. */
  struct instance **ins = NULL;
  struct result **results, *best = NULL;
  int   s_ant, max_iter, s_ins, i, j, best_index = -1;
  float best_value = -1000.0;
  if (argc < 2) {
    printf("Modo de uso: ./ttp_as archivo.xml [#hormigas] [#iteraciones]\n");
    printf("#hormigas > 1, otros parametros en el archivo 'params.' \n");
    return EXIT_FAILURE;
  }

  s_ant    = argc > 2 ? atoi(args[2]) : MAX_ANTS;
  max_iter = argc > 3 ? atoi(args[3]) : MAX_ITER;
  s_ins = parser((char *) args[1], &ins);
  if ( s_ins < 0 ) {
    fprintf(stderr, "Error en el archivo XML (return %d)", s_ins );
    return EXIT_FAILURE;
  }
  srand(MY_SEED);

  FILE *fp = fopen("params", "r");
  if (fp == NULL) {
    PH_MIN =  _PH_MIN;    EVAP   =  _EVAP;    ALPHA  =  _ALPHA;
    PH_INV =  _PH_INV;    Q      =  _Q;       BETA   =  _BETA;
  } else {
    fscanf(fp,"%f %f %f %f %f %f", &PH_MIN, &PH_INV, &Q, &EVAP, &ALPHA, &BETA);
    fclose(fp);
  }

  /* Solo se calcula para la instancia DEF_INS. */
  printf("Instancia: %s, %d profesores, %d clases, %d salas, %d tiempos.\n", 
      ins[DEF_INS]->name, ins[DEF_INS]->s_teacher, ins[DEF_INS]->s_class, 
      ins[DEF_INS]->s_room, ins[DEF_INS]->s_time);
  printf("Resolviendo con %d hormigas, %d iteraciones maximas y los siguientes",
      s_ant, max_iter);
  printf(" parametros:\nph_min:%g  inv:%g  Q:%g  evap:%g  alpha:%g  beta:%g\n",
      PH_MIN, PH_INV, Q, EVAP, ALPHA, BETA);
  results   = (struct result **) malloc(s_ant * sizeof(struct result *));
  p_size    = ins[DEF_INS]->s_event;
  pheromone = (float **) malloc(p_size * sizeof(float *));
  for (i=0; i < p_size; i++) {
    pheromone[i] = (float *) malloc(p_size * sizeof(float));
    for (j = 0; j < p_size; pheromone[i][j++] = PH_MIN);
  }

  /* Hormigas. */
  for (i = 0; i < max_iter; i++) {
    for (j = 0; j < s_ant; j++) {
      results[j] = new_result(ins[DEF_INS]);
      ant(results[j]);
    }
    for (j = 1; j < s_ant; j++) {
      if (results[j]->value > best_value){
        best_index = j;
        best_value = results[j]->value;
      }
    }
    printf("iter:%d = %0.4f (%0.4f)      \r",
        i,  best_value,best ? best->value : -1000);
    ph_evap();
    for (j = 0; j < s_ant; j++) {
      if (results[j]->value == best_value) ph_update(results[j]->path);
      if (j != best_index) del_result(results[j]);
    }
    if (best_value > (best?best->value:-1000)) {
      best = results[best_index];
      results[best_index] = NULL;
    }
    ph_update(best->path);
    best_index = -1;
    best_value = -1000.0;
  }

  print_res(best);

  /* Liberando memoria. */
  del_result(best);
  for(i = 0; i < s_ins; del_instance(ins[i++]));

  return EXIT_SUCCESS;
}

/* vim: set ts=2 sw=2 sts=2 tw=80 : */
