#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "xhstt_parser.h"
#include "structures.h"

#define MY_SEED   82 //time(NULL) // Semilla para random. Puede ser un entero.
#define OUT_FILE  "/dev/stdout"
#define DEF_INS   0
#define MAX_ITER  1000
#define MAX_ANTS  20

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

void print_res(struct result *R)
{
  int i, j;
  printf("TxT           CXT           RXT\n");
  for (i = 0; i < R->ins->s_time; i++) {
    for (j = 0; j < R->ins->s_teacher; j++) printf("%d ", R->txt[j][i]);
    printf("  ");
    for (j = 0; j < R->ins->s_class  ; j++) printf("%d ", R->cxt[j][i]);
    printf("  ");
    for (j = 0; j < R->ins->s_room   ; j++) printf("%d ", R->rxt[j][i]);
    printf("\n");
  }
  /*printf("\nPheromones:\n");
  for (i = 0; i < R->ins->s_event; i++){
    for (j = 0; j < R->ins->s_event; j++) printf("%0.3f ", R->ph[j][i]);
    printf("\n");
  }*/
  printf("\nPath: ");
  for (j = 0; j < R->ins->s_event - 1; j++) printf("%2d --> ", R->path[j]);
  printf("%2d\n", R->path[j]);
}

int in_path(short *path, short value)
{
  short i;
  for (i = 0;; i++) {
    if (path[i] == NONE)        return 0;
    else if (path[i] == value)  return 1;
  }
}

short next_event(struct result *R, short actual)
{
  int total = 0, part = 0, i;
  for (i = 0; i < R->ins->s_event; i++)
    if (!in_path(R->path, i)) total += (int) (R->ph[actual][i] * 1000);
  total = randint(total);
  for (i = 0;; i++) {
    if (!in_path(R->path, i)) part += (int) (R->ph[actual][i] * 1000);
    if (part > total) return i;
  } 
}

float ant(struct result *R)
{
  float value = 0.0;
  short actual = randint(R->ins->s_event), next, i = 0;
  R->path[i] = actual;
  while (R->path[R->ins->s_event-1] == NONE) {
    next = next_event(R, actual);
    R->path[++i] = next;
    actual = next;
  }
  return value;
}

int main(int argc, const char * args[])
{
  struct instance **ins = NULL;
  struct result **results;
  int s_ant, max_iter, s_ins, i;
  if (argc < 2) {
    printf("Modo de uso: ./ttp_as archivo.xml [#hormigas] [#iteraciones]\n");
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
  /* Solo para la instancia DEF_INS. */
  printf("Instancia: %s, con %d hormigas y %d iteraciones maximas.\n", 
      ins[DEF_INS]->name, s_ant, max_iter);
  results = (struct result **) malloc(s_ant * sizeof(struct result *));
  for(i = 0; i < s_ant; results[i++] = new_result(ins[DEF_INS]));

  ant(results[0]);
  print_res(results[0]);

  for(i = 0; i < s_ant; del_result(results[i++]));
  for(i = 0; i < s_ins; del_instance(ins[i++]));

  return EXIT_SUCCESS;
}

/* vim: set ts=2 sw=2 sts=2 tw=80 : */
