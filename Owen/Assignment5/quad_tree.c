#include "quad_tree.h"
#include <stdbool.h>

const double EPS = 1e-3;

//  traversing to the existing lowest node  
void insert(quad_node *curr, data_t *to_insert/* , int index */) {
  int which_curr;
  if (curr->leaf[0] == NULL && curr->leaf[1] == NULL && curr->leaf[2] == NULL  && curr->leaf[3] == NULL ) {
    if (curr->data == NULL){
      curr->data = to_insert;
      //      curr->index = index;
      //  remember to divide by mass before using it.
      curr->tot_mass = to_insert->mass;
      to_insert->which_quad = curr;
      curr->center_mass_x = to_insert->mass * to_insert->pos_x;
      curr->center_mass_y = to_insert->mass * to_insert->pos_y;
    } else {
      split(curr);
    
      int old_which = which_leaf(curr, *curr->data);
      if (old_which == -1) {
        printf("%s\n", "Its not in bounds for moving old data, might imply that two points are equal.");
        exit(EXIT_FAILURE);
      } 
      insert(curr->leaf[old_which],curr->data/* , curr->index */);
      which_curr = which_leaf(curr,*to_insert);
      insert(curr->leaf[which_curr],to_insert/* , index */);
      curr->data = NULL;
      curr->tot_mass = 0.0;
      curr->center_mass_x = 0.0;
      //curr->index = -1;

      curr->center_mass_y = 0.0;
    }
  } else {
    which_curr = which_leaf(curr,*to_insert);
    if (which_curr == -1) {
        printf("%s\n", "It is out of bounds for the root");
        exit(EXIT_FAILURE);
      
    } else { 
      insert(curr->leaf[which_curr],to_insert/* , index */);
    }
  }
}

//  topmost node, root might have the bounds as 0 0 1
//  splitting all four as of now, Might loook into if its better to just split  one direction
void split(quad_node* root) {
  size_t size = sizeof(quad_node);  
  quad_node *curr;
  double height_width =  ((quad_node *)root)->height_width *  0.5;
  for (int i = 0; i < 4; i++) {
    root->leaf[i] = malloc(size);    
    curr = (quad_node*)(root->leaf[i]);
    curr->data = NULL;    
    //curr->index = -1;
    curr->height_width = height_width;
    curr->low_bound_x = root->low_bound_x;
    curr->low_bound_y = root->low_bound_y;
    curr->center_mass_x = 0.0;
    curr->center_mass_y = 0.0;
    curr->tot_mass = 0.0;    
    for (int j = 0; j<4; j++) {
      curr->leaf[j] = NULL;
    }    
  }
  //  Creating different bounds.
  curr = (quad_node*)(root->leaf[1]);
  curr->low_bound_x = root->low_bound_x + height_width;
  curr = (quad_node*)(root->leaf[2]);
  curr->low_bound_y = root->low_bound_y + height_width;
  curr = (quad_node*)(root->leaf[3]);
  curr->low_bound_x = root->low_bound_x + height_width;
  curr->low_bound_y = root->low_bound_y + height_width;
}

int which_leaf(quad_node* root, data_t node) {
  int index = -1;
  quad_node *curr;
  for (int i = 0; i < 4; i++) {
    curr = root->leaf[i];
    if( curr != NULL && curr->low_bound_x <= node.pos_x &&
       curr->low_bound_x + curr->height_width > node.pos_x &&
       curr->low_bound_y <= node.pos_y &&
       curr->low_bound_y + curr->height_width > node.pos_y){
      index = i;
      break;
    }
  }
  return index;
}

void delete(quad_node *root) {
  for (int i = 0; i < 4; i++) {
    if (root->leaf[i] != NULL) {
      delete(root->leaf[i]);
      root->leaf[i] = NULL;
    }
  }
  free(root);
  root = NULL;
}

void update_mass(quad_node *root) {
  if (root->leaf[0] != NULL) {
    for (int i = 0; i < 4; i++) {
      if (root->leaf[i] != NULL) {
        update_mass(root->leaf[i]);
        root->center_mass_x += ((quad_node *)root->leaf[i])->center_mass_x;
        root->center_mass_y += ((quad_node *)root->leaf[i])->center_mass_y;
        root->tot_mass += ((quad_node *)root->leaf[i])->tot_mass;
      }
    }
  }
}

//  Using the centre of mass for both.  
void calc_force_aprox(data_t *root, quad_node *quad, force_direction_t *force) {
  data_t *point = root;
  double sq_distance, x_diff, y_diff, distance, distance_stability, cube_distance_stability, one_over_cube_distance_stability;
  double one_over_quad_mass = 1 / quad->tot_mass;
  x_diff = point->pos_x - (quad->center_mass_x * one_over_quad_mass); 
  y_diff = point->pos_y - (quad->center_mass_y * one_over_quad_mass);
  sq_distance = (x_diff * x_diff) + (y_diff * y_diff);
  
  distance = sqrt(sq_distance);
  distance_stability = (distance+EPS);
  cube_distance_stability = distance_stability * distance_stability * distance_stability;
  one_over_cube_distance_stability = 1 / cube_distance_stability;
  force->x +=  point->mass * quad->tot_mass * x_diff * one_over_cube_distance_stability;
  force->y +=  point->mass * quad->tot_mass * y_diff * one_over_cube_distance_stability;
}

void calc_force_point(data_t *root, quad_node *quad, force_direction_t *force) {
  data_t *point = root;
  double sq_distance, x_diff, y_diff, distance , distance_stability, cube_distance_stability, one_over_cube_distance_stability;
  
  x_diff = point->pos_x - (quad->data->pos_x); 
  y_diff = point->pos_y - (quad->data->pos_y);
  sq_distance = (x_diff * x_diff) + (y_diff * y_diff);

  distance = sqrt(sq_distance);
  distance_stability = (distance+EPS);
  cube_distance_stability = distance_stability * distance_stability * distance_stability;
  one_over_cube_distance_stability = 1 / cube_distance_stability;
  force->x +=  point->mass * quad->data->mass * x_diff * one_over_cube_distance_stability;
  force->y +=  point->mass * quad->data->mass * y_diff * one_over_cube_distance_stability;
}


void traverse_for_force(data_t* start, quad_node* curr, force_direction_t *force, double theta){
  double thres = threshold(start, curr);
  bool s[4] = {curr->leaf[0] == NULL, curr->leaf[1] == NULL, curr->leaf[2] == NULL, curr->leaf[3] == NULL};
  if(thres <= theta /* && curr->data != NULL */  ) {
    calc_force_aprox(start, curr, force);
  }
  else if ( !s[0] || !s[1] || !s[2] || !s[3]) {
    for (int i = 0; i < 4; i++) {
      if ( /* !s[i]  && */ ((quad_node *)curr->leaf[i])->tot_mass != 0.0)
        traverse_for_force(start, curr->leaf[i], force, theta);
    }
     
  } else if ((s[0] && s[1] && s[2]  && s[3] && /* curr->data != NULL && */ start != curr->data)) {
     calc_force_point(start, curr, force);
  }
}

double threshold(data_t *root, quad_node *center) {
  /* printf("root, %p\n",root); */
  /* printf("center, %p\n",center); */
  data_t *point = root;
  double sq_distance, cen_x, cen_y;
  const double one_over_two = 1 / 2;
  /* printf("%s\n", "segis here"); */
  cen_x = center->low_bound_x + center->height_width * one_over_two;
    /* printf("%s\n", "segis here1"); */
  cen_y = center->low_bound_y + center->height_width * one_over_two;
    /* printf("%s\n", "segis here2"); */
  sq_distance = (point->pos_x - cen_x) * (point->pos_x - cen_x) + (point->pos_y - cen_y) * (point->pos_y - cen_y);
    /* printf("%s\n", "segis here3"); */
  return (center->height_width / sqrt(sq_distance));
}

/* quad_node *search_node(quad_node *root, int index, data_t meta) { */
/*   quad_node *ret; */
/*   int which = which_leaf(root, meta); */
/*   if (index == root ->index && which == -1) { */
/*     ret = root; */
/*   } else { */
/*     ret = search_node(root->leaf[which], index, meta); */
/*   } */
/*   return ret; */
/* } */

void init_root(quad_node **root){
  (*root) = malloc(sizeof(quad_node));
  (*root)->data = NULL;
  (*root)->low_bound_x = 0;
  (*root)->low_bound_y = 0;
  (*root)->height_width = 1;
  /* (root)->index = -1; */
  (*root)->leaf[0] = NULL;
  (*root)->leaf[1] = NULL;
  (*root)->leaf[2] = NULL;
  (*root)->leaf[3] = NULL;
   
  (*root)->center_mass_x = 0.0;
  (*root)->center_mass_y = 0.0;
  (*root)->tot_mass = 0.0;
    


}

#ifdef QUAD
int main(int argc, char *argv[]){
  printf("Debugging/testing quad_tree \n");
  quad_node *root = malloc(sizeof(quad_node));
  root->data = NULL;
  root->low_bound_x = 0;
  root->low_bound_y = 0;
  root->height_width = 1;
  root->leaf[0] = NULL;
  root->leaf[1] = NULL;
  root->leaf[2] = NULL;
  root->leaf[3] = NULL;
  int N = 3;
  data_t *test_points = malloc(sizeof(data_t) * N);
  for (int i = 0; i < N; i++) {
    test_points[i].pos_x = 0.05 + i * 0.02;
    test_points[i].pos_y = 0.05;
    test_points[i].mass = 0.05 + i;
    test_points[2].pos_y = 0.51;
    insert(root, &test_points[i], i);
  }
  update_mass(root);  
  delete(root);
  free(test_points);

  return 0;
}
#endif // QUAD
