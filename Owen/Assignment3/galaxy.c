#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <unistd.h>
#include "./graphics/graphics.h"
#include <sys/time.h>

static double get_wall_seconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  double seconds = tv.tv_sec + (double) tv.tv_usec / 1000000;
  return seconds;
}


#define NR_ARGS 6

const float circleRadius = 0.004, circleColor = 0;
const int windowWidth = 800;


#ifdef DEBUG
#define DEBUG_FLAG 1
#else
#define DEBUG_FLAG 0
#endif

typedef struct particle {
  double pos_x, pos_y, mass, velocity_x, velocity_y, brightness;
} star_t;

typedef struct forces {
  double x,y;
} force_direction_t;

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

int main(int argc, char *argv[]) {
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

  double timer = get_wall_seconds();
  star_t *galaxy = malloc(N * sizeof(star_t));
  star_t *galaxy_next_step = malloc(N * sizeof(star_t));

  if (galaxy == NULL && galaxy_next_step != NULL) {
    printf("%s\n", "Out of memory");
    exit(EXIT_FAILURE);
  }
  //  Load the data
  read_config(file_name, galaxy, N);
  memcpy(galaxy_next_step, galaxy, 6 * N * sizeof(double));
  if (graphics) {
    InitializeGraphics(argv[0], windowWidth, windowWidth);
    SetCAxes(0, 1);
  }

  force_direction_t *forces = calloc(N, sizeof(force_direction_t));
  
  int i, j = 0, k;
  const double EPS = 1e-3;
  double curr_force[2];
  double distance, sq_distance, acceleration[2];
  double x_diff, y_diff;
  double distance_stability;
  double cube_distance_stability;
  double one_over_cube_distance_stability;
  // Main driver for the simulation
  for (k = 0; k < n_steps; k++) {
    if (graphics) {
      ClearScreen();
    }
    
    for (i = 0; i < N; i++) {
      if (graphics) {
        DrawCircle(galaxy[i].pos_x,  galaxy[i].pos_y, 1, 1, circleRadius, circleColor);
      }
       curr_force[0] = 0.0; 
       curr_force[1] = 0.0; 

      for (j = i + 1; j < N; j++) {
        /* if (i != j) { */
          x_diff = galaxy[i].pos_x - galaxy[j].pos_x;
          y_diff = galaxy[i].pos_y - galaxy[j].pos_y;
          sq_distance = (x_diff * x_diff) + (y_diff * y_diff);
          distance = sqrt(sq_distance);
          distance_stability = (distance+EPS);
          cube_distance_stability = distance_stability * distance_stability * distance_stability;
          one_over_cube_distance_stability = 1 / cube_distance_stability;
          curr_force[0] =  galaxy[i].mass * galaxy[j].mass * x_diff * one_over_cube_distance_stability;
          curr_force[1] =  galaxy[i].mass *galaxy[j].mass * y_diff * one_over_cube_distance_stability;

          forces[i].x += curr_force[0];
          forces[j].x -= curr_force[0];
          forces[i].y += curr_force[1];
          forces[j].y -= curr_force[1];
          
        /* } */
      }
      acceleration[0] = -1 * gravity * forces[i].x / galaxy[i].mass;
      acceleration[1] = -1 * gravity * forces[i].y / galaxy[i].mass;
      galaxy_next_step[i].velocity_x = galaxy[i].velocity_x + delta_t * acceleration[0];
      galaxy_next_step[i].velocity_y = galaxy[i].velocity_y + delta_t * acceleration[1];
      galaxy_next_step[i].pos_x = galaxy[i].pos_x + delta_t * galaxy_next_step[i].velocity_x;
      galaxy_next_step[i].pos_y = galaxy[i].pos_y + delta_t * galaxy_next_step[i].velocity_y;
      forces[i].x = 0.0;
      forces[i].y = 0.0;
    }
    pointer_swap((void**)&galaxy, (void**)&galaxy_next_step);
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
  free(forces);
  free(galaxy_next_step);
  free(galaxy);

  printf("Wall clock time: %lf\n",  get_wall_seconds() - timer);
  return 0;
}

void print_usage(char *prg_name) {
  printf("Usage: %s N filename nsteps delta_t graphics\n", prg_name);
  printf("N, is the number of stars\n");
  printf("filename, path to the file with the initial configuration\n");
  printf("nsteps, the number of steps for the simulation\n");
  printf("delta_t, is the timestep size\n");
  printf("graphics, to enable graphics (1 = on, 0 = off)\n");
}

void read_config(char *file_name, star_t *galaxy, int N) {
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

void write_to_file(star_t *galaxy, int N) {
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

void printer(star_t *galaxy, int N) {
  star_t *s = galaxy;
  for (int i = 0; i < N; i++) {
    printf("i: %d, x: %fl, y: %fl, mass: %fl, velo_x: %fl, velo_y: %fl\n", i, (s+i)->pos_x, (s+i)->pos_y, (s+i)->mass, (s+i)->velocity_x, (s+i)->velocity_y);
  }
}

void pointer_swap(void **a, void **b) {
  void *tmp = *a;
  *a = *b;
  *b = tmp;
}
