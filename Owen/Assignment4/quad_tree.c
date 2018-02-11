#include "quad_tree.h"

const double EPS = 1e-3;

//  traversing to the existing lowest node  
void insert(quad_node *curr, data_t *to_insert, int index) {
  int which_curr;
  /* printf("inserting %d x: %lf y: %lf\n", index, to_insert->pos_x, to_insert->pos_y); */
  if (curr->leaf[0] == NULL && curr->leaf[1] == NULL && curr->leaf[2] == NULL  && curr->leaf[3] == NULL && curr->data == NULL) {
    curr->data = to_insert;
    curr->index = index;
    //  remember to divide by mass before using it.
    curr->tot_mass = to_insert->mass;
    curr->center_mass_x = to_insert->mass * to_insert->pos_x;
    curr->center_mass_y = to_insert->mass * to_insert->pos_y;
  } else if (curr->leaf[0] == NULL && curr->leaf[1] == NULL && curr->leaf[2] == NULL  && curr->leaf[3] == NULL) {
    split(curr);
    
    int old_which = which_leaf(curr, *curr->data);
    if (old_which == -1) {
      printf("%s\n", "Its not in bounds for moving old data, might imply that two points are equal.");
      printf("at curr x: %lf y: %lf\n", curr->data->pos_x, curr->data->pos_y);
      printf("at curr x: %lf y: %lf\n", to_insert->pos_x, to_insert->pos_y);
      exit(EXIT_FAILURE);
      /* if (curr->parr != NULL) { */
      /*   insert(curr->parr, curr->data, curr->index); */
      /* } */

    } 
    insert(curr->leaf[old_which],curr->data, curr->index);
    which_curr = which_leaf(curr,*to_insert);
    insert(curr->leaf[which_curr],to_insert, index);
    curr->data = NULL;
    curr->tot_mass = 0.0;
    curr->center_mass_x = 0.0;
    curr->index = -1;

    curr->center_mass_y = 0.0;
  } else {
    which_curr = which_leaf(curr,*to_insert);
    if (which_curr == -1) {
      if (curr->parr == NULL) {
        printf("%s\n", "It is out of bounds for the root field");
        exit(EXIT_FAILURE);
      }
      //    insert(curr->parr,to_insert, index);
    } else { 
      insert(curr->leaf[which_curr],to_insert, index);
    }
  }
}

//  topmost node, root might have the bounds as 0 0 1
//  splitting all four as of now, Might loook into if its better to just split  one direction
void split(quad_node* root) {
  size_t size = sizeof(quad_node);
  
  quad_node *curr;
  double height_width =  ((quad_node *)root)->height_width /  2;
  for (int i = 0; i < 4; i++) {
    root->leaf[i] = malloc(size);
    
    curr = (quad_node*)(root->leaf[i]);
    
    curr->data = NULL;
    curr->parr = root;
    curr->index = -1;
    curr->level = 1 + root->level;
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
  //  printf("data_t x: %lf y: %lf \n", node.pos_x, node.pos_y);
  for (int i = 0; i < 4; i++) {
    curr = root->leaf[i];
    //  printf("The bounds_x: %lf, bounds_y: %lf, width: %lf\n", curr->low_bound_x, curr->low_bound_y, curr->height_width);
    
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
  // printf("tot: %lf, x: %lf, y: %lf \n",root->tot_mass, root->center_mass_x,root->center_mass_y);
  }
}

//  Using the centre of mass for both.  
void calc_force(quad_node *root, quad_node *quad, double *force) {
  data_t *point = root->data;
  double sq_distance, x_diff, y_diff, distance =0.0, distance_stability, cube_distance_stability, one_over_cube_distance_stability;
  double one_over_mass = 1 / quad->tot_mass;

  /* printf("point: %lf %lf\n", point->pos_x , point->pos_y); */
  /* printf("quad: %lf %lf %lf\n", quad->center_mass_x, quad->center_mass_y, quad->tot_mass); */

  x_diff = point->pos_x - quad->center_mass_x * one_over_mass; 
  y_diff = point->pos_y - quad->center_mass_y * one_over_mass;
  sq_distance = (x_diff * x_diff) + (y_diff * y_diff);

  /* printf("sq_dist: %lf\n", sq_distance); */

  distance = sqrt(sq_distance);
  distance_stability = (distance+EPS);
  cube_distance_stability = distance_stability * distance_stability * distance_stability;
  one_over_cube_distance_stability = 1 / cube_distance_stability;
  force[0] +=  point->mass * quad->tot_mass * x_diff * one_over_cube_distance_stability;
  force[1] +=  point->mass *quad->tot_mass * y_diff * one_over_cube_distance_stability;  
}

void traverse_for_force(quad_node* start, quad_node* curr, double *force){
  data_t node;
  int camefrom = -1;
  if (start->data == NULL) {
    exit(EXIT_FAILURE);
  }
  //  printf("trav force: %lf %lf\n", force[0],force[1] );
  node.pos_x = start->data->pos_x;
  node.pos_y = start->data->pos_y;
  camefrom = which_leaf(curr, node);
  double thres = 1.0;
  
  for (int i = 0; i < 4; i++) {
    if (curr->leaf[i] != NULL) {
      thres = threshold(start, curr->leaf[i]);
    
      if (camefrom == i || thres > 0.02) {
        if (curr->leaf[i] != NULL) {
          traverse_for_force(start, curr->leaf[i], force);
        } 
      } else {
        if (curr->leaf[i] != NULL && ((quad_node*)curr->leaf)[i].data != NULL && start->index != ((quad_node*)curr->leaf[i])->index && ((quad_node*)curr->leaf[i])->index != -1) {
          calc_force(start, curr->leaf[i], force);
        }
      }
    }
  }
}


double threshold(quad_node *root, quad_node *center) {
  data_t *point = root->data;
  double sq_distance, cen_x, cen_y;
  cen_x = center->low_bound_x + center->height_width / 2;
  cen_y = center->low_bound_y + center->height_width / 2;
  sq_distance = (cen_x - point->pos_x) * (cen_x - point->pos_x) +
      (cen_y - point->pos_y) * (cen_y - point->pos_y);
  return (center->height_width / sqrt(sq_distance));
}

quad_node *search_node(quad_node *root, int index, data_t meta) {  
  quad_node *ret;
  int which = which_leaf(root, meta);
  if (index == root ->index && which == -1) {
    ret = root;
  } else {
    ret = search_node(root->leaf[which], index, meta);
  }
  return ret;
}


#ifdef QUAD
int main(int argc, char *argv[]){
  printf("Debugging/testing quad_tree \n");
  
  quad_node *root = malloc(sizeof(quad_node));
  root->parr = NULL;
  root->data = NULL;
  root->low_bound_x = 0;
  root->low_bound_y = 0;
  root->height_width = 1;
  root->level = 1;
  root->leaf[0] = NULL;
  root->leaf[1] = NULL;
  root->leaf[2] = NULL;
  root->leaf[3] = NULL;
  int N = 3;
  data_t *test_points = malloc(sizeof(data_t) * N);
  for (int i = 0; i < N; i++) {
    //    printf("Inserting point: %d\n", i);
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
