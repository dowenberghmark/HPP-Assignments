#include "galaxy.h"

static double get_wall_seconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  double seconds = tv.tv_sec + (double) tv.tv_usec / 1000000;
  return seconds;
}


int main(int argc, char *argv[]) {
  if (argc != NR_ARGS) {
    print_usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  int N = atoi(argv[1]);
  char * file_name = argv[2];
  int n_steps = atoi(argv[3]);
  const double gravity = 100.0 / N;
  const double delta_t = atof(argv[4]);
  //  const double theta = atof(argv[5]);
  THETA = atof(argv[5]);
  const uint8_t graphics = atoi(argv[6]);

  double timer = get_wall_seconds();
  star_t *galaxy = malloc(N * sizeof(star_t));
 
  if (galaxy == NULL ) {
    printf("%s\n", "Out of memory");
    exit(EXIT_FAILURE);
  }
  //  Load the data
  read_config(file_name, galaxy, N);
  //memcpy(galaxy_next_step, galaxy, 6 * N * sizeof(double));
  if (graphics) {
    InitializeGraphics(argv[0], windowWidth, windowWidth);
    SetCAxes(0, 1);
  }

  force_direction_t *forces = malloc(N * sizeof(force_direction_t));
  force_direction_t *velo = malloc(N * sizeof(velo_t));
  data_t *node_data = malloc(sizeof(data_t) * N);
  double *one_over_mass = malloc(sizeof(double) * N);
  int i, k;
  
  double acceleration[2]; 


  for (i = 0; i < N; i++) {
    node_data[i].mass = galaxy[i].mass;
    one_over_mass[i] = 1 / galaxy[i].mass;
    node_data[i].pos_x = galaxy[i].pos_x;
    node_data[i].pos_y = galaxy[i].pos_y;
    velo[i].x = galaxy[i].velocity_x;
    velo[i].y = galaxy[i].velocity_y;
  }
 
  
  // Main driver for the simulation
  for (k = 0; k < n_steps; k++) {
    if (graphics) {
      ClearScreen();
    }
    quad_node *root = malloc(sizeof(quad_node));
    root->data = NULL;
    root->low_bound_x = 0;
    root->low_bound_y = 0;
    root->height_width = 1;
    root->index = -1;
    root->leaf[0] = NULL;
    root->leaf[1] = NULL;
    root->leaf[2] = NULL;
    root->leaf[3] = NULL;
    
    root->center_mass_x = 0.0;
    root->center_mass_y = 0.0;
    root->tot_mass = 0.0;
    
    /* printf("Creating daaata and inserting, time_step: %d\n", k); */
    for (i = 0; i < N; i++) {
      /* node_data[i].mass = galaxy[i].mass; */
      /* node_data[i].pos_x = galaxy[i].pos_x; */
      /* node_data[i].pos_y = galaxy[i].pos_y; */
      insert(root, (node_data+i), i);
      forces[i].x = 0.0;
      forces[i].y = 0.0;
    }
    update_mass(root);
    for (i = 0; i < N; i++) {
      if (graphics) {
        DrawCircle(galaxy[i].pos_x,  galaxy[i].pos_y, 1, 1, circleRadius, circleColor);
      }
      
      quad_node *this_node = search_node(root, i, node_data[i]);
      traverse_for_force(this_node, root, (double*)(&forces[i]) /*, theta*/);     
    
    }

    for (i = 0; i < N; i++) {
      // printf("force: %lf %lf\n", forces[i].x,forces[i].y ); 
      acceleration[0] = -1 * gravity * forces[i].x * one_over_mass[i];
      acceleration[1] = -1 * gravity * forces[i].y * one_over_mass[i];
      //printf("acc: %lf %lf\n",acceleration[0],acceleration[1] ); 
      velo[i].x = velo[i].x + delta_t * acceleration[0];
      velo[i].y = velo[i].y + delta_t * acceleration[1];
      node_data[i].pos_x = node_data[i].pos_x + delta_t * velo[i].x;
      node_data[i].pos_y = node_data[i].pos_y + delta_t * velo[i].y;
   
    }

    if (graphics) {
      Refresh();
      usleep(3000);
    }
    delete(root);
  } //  Ends time step loop
  if (graphics) {
    FlushDisplay();
    CloseDisplay();
  }

  for (i = 0; i < N; i++) {
    galaxy[i].pos_x = node_data[i].pos_x;
    galaxy[i].pos_y = node_data[i].pos_y;
    galaxy[i].velocity_x = velo[i].x;
    galaxy[i].velocity_y = velo[i].y;
  }
 
  // Dumping the data

  write_to_file(galaxy, N);
  free(forces);
  free(velo);
  free(galaxy);
  free(one_over_mass);
  free(node_data);
  printf("Wall clock time: %lf\n",  get_wall_seconds() - timer);
  return 0;
}

void print_usage(char *prg_name) {
  printf("Usage: %s N filename nsteps delta_t graphics\n", prg_name);
  printf("N, is the number of stars\n");
  printf("filename, path to the file with the initial configuration\n");
  printf("nsteps, the number of steps for the simulation\n");
  printf("delta_t, is the timestep size\n");
  printf("theta_max, is the threshold\n");
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
  int error_check = fread(galaxy, SIZE, AMOUNT_ELEMENTS, fd);
  if (error_check < 0) {
    exit(EXIT_FAILURE);
  }
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

  int error_check = fwrite(galaxy , SIZE, AMOUNT_ELEMENTS, fd);
  if (error_check < 0) {
    exit(EXIT_FAILURE);
  }
  
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
