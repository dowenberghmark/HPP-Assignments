#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <unistd.h>
#include "./graphics/graphics.h"

#define NR_ARGS 6

const float circleRadius=0.004, circleColor=0;
const int windowWidth=800;


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
//  Swapping pointers
void pointer_swap(void **a, void **b);


int main(int argc, char *argv[]){
  
  if (argc != NR_ARGS) {
    print_usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  int N = atoi(argv[1]);
  char * file_name = argv[2];
  int n_steps = atoi(argv[3]);
  double gravity = -100.0 / N;
  double delta_t = atof(argv[4]);
  const uint8_t graphics = atoi(argv[5]);

  star_t *galaxy = malloc(N * sizeof(star_t));
  star_t *galaxy_next_step = malloc(N * sizeof(star_t));
  if (galaxy == NULL && galaxy_next_step != NULL) {
    printf("%s\n", "Out of memory");
    exit(EXIT_FAILURE);
  }
  //  Load the data 
  read_config(file_name,galaxy, N);
  memcpy(galaxy_next_step, galaxy, 6 * N * sizeof(double));

  if (graphics) {
    InitializeGraphics(argv[0],windowWidth,windowWidth);
    SetCAxes(0,1);
  }
  
  int i, j, k;
  double force, norm_dist_vec, dist_cube, rel_distance, distance;
  const double eps = 1e-3;
  
  // Main driver for the simulation
  for (k = 0; k < n_steps; k++) {
    // printer(galaxy, 1);
    pointer_swap((void**)&galaxy,(void**) &galaxy_next_step);
    if (graphics) {
      ClearScreen();
    }
    for (i = 0; i < N; i++) {
      if (graphics == 1) {
        DrawCircle(galaxy[i].pos_x,  galaxy[i].pos_y, 1, 1, circleRadius, circleColor);
      }
      
      force = 0.0;
      double e_x = 1.0, e_y = 1.0;
      if (galaxy[i].velocity_x < 0) 
        e_x = -1.0;
      
      if (galaxy[i].velocity_y < 0) 
        e_y = -1.0;
      
      for (j = 0; j < N; j++) {
        if (i == j) {
        } else {
          distance = (galaxy[i].pos_x - galaxy[j].pos_x) * (galaxy[i].pos_x - galaxy[j].pos_x) + (galaxy[i].pos_y - galaxy[j].pos_y) * (galaxy[i].pos_y - galaxy[j].pos_y);
          distance = sqrt(distance);
          
          rel_distance = (galaxy[i].pos_x - galaxy[j].pos_x) * e_x + (galaxy[i].pos_y - galaxy[j].pos_y) * e_y;

          norm_dist_vec = rel_distance / distance;
          dist_cube = ( distance + eps);

          //  dist_cube = (norm_dist_vec);
          dist_cube = dist_cube * dist_cube * dist_cube;
          //  Distance is much less
          if (distance * 100.0 < 1.0) {
            force += (galaxy[j].mass  / dist_cube) * rel_distance;
          } else {
            force += (galaxy[j].mass  / (distance * distance)) * norm_dist_vec;
          }
        }
      }
      force *= gravity * galaxy[i].mass;
      double accel = force / galaxy[i].mass * delta_t;
      galaxy_next_step[i].velocity_x = galaxy[i].velocity_x + accel;
      galaxy_next_step[i].velocity_y = galaxy[i].velocity_y + accel;
      galaxy_next_step[i].pos_x = galaxy[i].pos_x + (galaxy[i].velocity_x * delta_t);
      galaxy_next_step[i].pos_y = galaxy[i].pos_y + (galaxy[i].velocity_y * delta_t);

    //    pointer_swap((void**)&galaxy,(void**) &galaxy_next_step);
    }
    if (graphics) {
      Refresh();
      usleep(3000);
    }
  }
  if (graphics) {
    FlushDisplay();
    CloseDisplay();
   }

  // Dumping the data

  write_to_file(galaxy, N);
  free(galaxy_next_step);
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
  

  fread(galaxy, SIZE, AMOUNT_ELEMENTS, fd);
  
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
  

  fwrite(galaxy , SIZE, AMOUNT_ELEMENTS, fd);

  
  fclose(fd);
}

void printer(star_t *galaxy, int N){
  star_t *s = galaxy;
  for (int i = 0; i < N; i++) {
    printf("i: %d, x: %fl, y: %fl, mass: %fl, velo_x: %fl, velo_y: %fl\n", i, (s+i)->pos_x, (s+i)->pos_y, (s+i)->mass, (s+i)->velocity_x, (s+i)->velocity_y);
  }
  
}

void pointer_swap(void **a, void **b){
  void *tmp = *a;
  *a = *b;
  *b = tmp;
}
