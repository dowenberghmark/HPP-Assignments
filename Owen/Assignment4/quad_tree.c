#include "quad_tree.h"
//  traversing to the existing lowest node  
void insert(quad_node *curr, data_t *to_insert, int index) {
  int which_curr;
  
  if (curr->leaf[0] == NULL && curr->leaf[1] == NULL && curr->leaf[2] == NULL  && curr->leaf[3] == NULL && curr->data == NULL) {
    curr->data = to_insert;
    curr->index = index;
    //  remember to divide by mass before using it.
    curr->tot_mass = to_insert->mass;
    curr->center_mass_x = to_insert->mass * to_insert->pos_x;
    curr->center_mass_y = to_insert->mass * to_insert->pos_y;
  } else if (curr->leaf[0] == NULL) {
    split(curr);
    
    int old_which = which_leaf(curr, *curr->data);
    if (old_which == -1) {
      printf("%s\n", "Its not in bounds for moving old data, might imply that two points are equal.");
      exit(EXIT_FAILURE);
    }
    insert(curr->leaf[old_which],curr->data, curr->index);
    which_curr = which_leaf(curr,*to_insert);
    insert(curr->leaf[which_curr],to_insert, index);
    curr->data = NULL;
    curr->tot_mass = 0.0;
    curr->center_mass_x = 0.0;
    curr->center_mass_y = 0.0;
  } else {
    which_curr = which_leaf(curr,*to_insert);
    if (which_curr == -1) {
      if (curr->parr == NULL) {
        printf("%s\n", "It is out of bounds for the root field");
        exit(EXIT_FAILURE);
      }
      insert(curr->parr,to_insert, index);
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
    curr->level = 1 + root->level;
    curr->height_width = height_width;
    curr->low_bound_x = root->low_bound_x;
    curr->low_bound_y = root->low_bound_y;
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
    if(curr->low_bound_x <= node.pos_x &&
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
  //   printf("tot: %lf, x: %lf, y: %lf \n",root->tot_mass, root->center_mass_x,root->center_mass_y);
  }
}

double threshold(quad_node *root, quad_node *center) {
  data_t *point = root->data;
  double sq_distance, cen_x, cen_y;
  cen_x = center->low_bound_x + center->height_width / 2;
  cen_y = center->low_bound_y + center->height_width / 2;
  sq_distance = (cen_x - point->pos_x) * (cen_x - point->pos_x) + (cen_y - point->pos_y) * (cen_y - point->pos_y);
  return (center->height_width / sqrt(sq_distance));
}

quad_node *search_node(quad_node *root, int index) {
  quad_node *ret;
  if (root->index == index) {
    return root;
  }
  
  for (int i = 0; i < 4; i++) {
    ret = search_node(root->leaf[i], index);
    if (ret != NULL)
      break;
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
