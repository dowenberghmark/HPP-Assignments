#ifndef __GALAXY_H
#define __GALAXY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <unistd.h>
#include "./graphics/graphics.h"
#include <sys/time.h>
#include "quad_tree.h"
#define NR_ARGS 8

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

/* typedef struct forces { */
/*   double x,y; */
/* } force_direction_t; */
typedef struct velocity {
  double x,y;
}velo_t;
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



#endif //  __GALAXY_H
