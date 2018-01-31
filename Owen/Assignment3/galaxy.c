#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#define NR_ARGS 6

#ifdef DEBUG
#define DEBUG_FLAG 1
#else
#define DEBUG_FLAG 0
#endif

typedef struct particle
{
  double pos_x, pos_y, mass, velocity_x, velocity_y, brightness;
} star_t;

//  Writting help messages
void print_usage(char *prg_name);
//  Reading in the configuration file into struct
void read_config(char *file_name, star_t *galaxy, int N);
//  Writing the current state of the galaxy to a predefined outputfile
void write_to_file(star_t *galaxy, int N);
//  Printing the galaxys params
void printer(star_t *galaxy, int N);

int main(int argc, char *argv[]){
  
  if (argc != NR_ARGS) {
    print_usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  int N = atoi(argv[1]);
  char * file_name = argv[2];
  int n_steps = atoi(argv[3]);
  double gravity = 100.0 / N;
  double delta_t = atof(argv[4]);
  const uint8_t graphics = atoi(argv[5]);

  star_t *galaxy = malloc(N * sizeof(star_t));
  if (galaxy == NULL) {
    printf("%s\n", "Out of memory");
    exit(EXIT_FAILURE);
  }

  read_config(file_name,galaxy, N);





















  
  write_to_file(galaxy, N);

  free(galaxy);
  return 0;
}

void print_usage(char *prg_name){
  printf("Usage: %s N filename nsteps delta_t graphics\n", prg_name);
  printf("N, is the number of stars\n");
  printf("filename, path to the file with the initial configuration\n");
  printf("nsteps, the number of steps for the simulation\n");
  printf("delta_t, is the timestep size\n");
  printf("graphics, to enable graphics (1 = on, 0 = off)\n");
}

void read_config(char *file_name, star_t *galaxy, int N){
  
  FILE *fd;
  
  const int SIZE = sizeof(double);
  const int AMOUNT_ELEMENTS = 6*N;
  fd = fopen(file_name, "r");

  if (fd == NULL) {
    printf("Couldn't find the file: %s\n", file_name);
    exit(EXIT_FAILURE);
  }
  
  //for (int i = 0; i < N; i++) {
  fread(galaxy, SIZE, AMOUNT_ELEMENTS, fd);
  //}

  
  fclose(fd);
}

void write_to_file(star_t *galaxy, int N){
  
  FILE *fd;
  
  const int SIZE = sizeof(double);
  const int AMOUNT_ELEMENTS = 6*N;
  fd = fopen("./result.gal", "w+");

  if (fd == NULL) {
    printf("Couldn't write to the output file\n");
    exit(EXIT_FAILURE);
  }
  
  //  for (int i = 0; i < N; i++) {
  fwrite(galaxy , SIZE, AMOUNT_ELEMENTS, fd);
  //}  
  
  fclose(fd);
}

void printer(star_t *galaxy, int N){
  star_t *s = galaxy;
  for (int i = 0; i < N; i++) {
    printf("i: %d, x: %fl, y: %fl, mass: %fl, velo_x: %fl, velo_y: %fl, brightness: %fl\n", i, (s+i)->pos_x, (s+i)->pos_y, (s+i)->mass, (s+i)->velocity_x, (s+i)->velocity_y, (s+i)->brightness);
  }
  
}
