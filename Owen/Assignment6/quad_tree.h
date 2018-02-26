#ifndef __QUAD_TREE
#define __QUAD_TREE
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
//  #include "galaxy.h"

typedef struct data_node{
  double pos_x,pos_y,mass;
  void *which_quad;
} data_t;

typedef struct forces {
  double x,y;
} force_direction_t;

typedef struct quad_tree{
  data_t *data;
  //struct quad_tree *leaf[4]; //  3 top right, 2 top left, 0 bottom left, 1 bottom right. A leaf an quad_node
  struct quad_tree *leaf;
  double low_bound_x, low_bound_y, height_width;
  double center_mass_x, center_mass_y, tot_mass;
  //int index;
  
} quad_node;
//  Inserting the star into the quad tree
void insert(quad_node *curr, data_t *to_insert/* , int index */);
//  Deleting the quad tree
void delete(quad_node *root);
//  Creating memory for children leafs
void split(quad_node *root); //  helper function.
//  Searching for a node with the specific index_t where meta speeds up the search traversing towards the right node 
// quad_node *search_node(quad_node *root, int index, data_t meta);
//  Calculates the center of mass and point for a tree
void update_mass(quad_node *root);
//  Calculating the force with the center of mass and center point
void calc_force_aprox(quad_node *root, quad_node *quad, double *force);
// Calculating the force for a point to point
void calc_force_point(quad_node *root, quad_node *quad, double *force);
//  Traverser for using the different force calculations 
void traverse_for_force(quad_node *start, quad_node *curr, double *force, double theta);
//  Calculates the threshold value
double threshold(quad_node *root, quad_node *center);
//  Finds which leaf the node should contain in.
int which_leaf(quad_node *root, data_t node);


#endif //  __QUAD_TREE
