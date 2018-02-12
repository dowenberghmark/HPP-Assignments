#ifndef __QUAD_TREE
#define __QUAD_TREE
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
//  #include "galaxy.h"

typedef struct data_node{
  double pos_x,pos_y,mass;
} data_t;

typedef struct quad_tree{
  data_t * data;
  void *leaf[4]; //  3 top right, 2 top left, 0 bottom left, 1 bottom right. A leaf an quad_node
  void *parr;
  double low_bound_x, low_bound_y, height_width;
  double center_mass_x, center_mass_y, tot_mass;
  int index;
  
} quad_node;




void insert(quad_node *curr, data_t *to_insert, int index);
void delete(quad_node *root);
void split(quad_node *root); //  helper function.
quad_node *search_node(quad_node *root, int index, data_t meta);
void update_mass(quad_node *root);

void calc_force_aprox(quad_node *root, quad_node *quad, double *force);
void calc_force_point(quad_node *root, quad_node *quad, double *force);
void traverse_for_force(quad_node *start, quad_node *curr, double *force);
double threshold(quad_node *root, quad_node *center);

int which_leaf(quad_node *root, data_t node);


#endif //  __QUAD_TREE
