#include "galaxy.h"
#include <pthread.h>

/* static double get_wall_seconds() { */
/*   struct timeval tv; */
/*   gettimeofday(&tv, NULL); */
/*   double seconds = tv.tv_sec + (double) tv.tv_usec / 1000000; */
/*   return seconds; */
/* } */


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
  double theta = atof(argv[5]);
  const int THREADS = atof(argv[7]);
  
  const uint8_t graphics = atoi(argv[6]);

  //  double timer = get_wall_seconds();
  star_t *galaxy = malloc(N * sizeof(star_t));
 
  if (galaxy == NULL ) {
    printf("%s\n", "Out of memory");
    exit(EXIT_FAILURE);
  }
  //  Load the data
  read_config(file_name, galaxy, N);

  if (graphics) {
    InitializeGraphics(argv[0], windowWidth, windowWidth);
    SetCAxes(0, 1);
  }

  pthread_barrier_init(&BARR, NULL, THREADS);
  force_direction_t *forces = malloc(N * sizeof(force_direction_t));
  velo_t *velo = malloc(N * sizeof(velo_t));
  data_t *node_data = malloc(sizeof(data_t) * N);
  double *one_over_mass = malloc(sizeof(double) * N);
  int i;
  thread_t t_data[THREADS];
  pthread_t t[THREADS];
  for (i = 0; i < THREADS; i++) {
    int start = i *(N / THREADS);
    t_data[i].start_point = start;
    t_data[i].end_point = (i+1) *(N / THREADS);
    t_data[i].gravity = gravity;
    t_data[i].theta = theta;
    t_data[i].delta_t = delta_t;
    t_data[i].velo = velo;
    t_data[i].forces = forces;
    t_data[i].node_data = node_data;
    t_data[i].one_over_mass = one_over_mass;
    t_data[i].graphics = graphics;
    t_data[i].n_steps = n_steps;
    
  }
  t_data[THREADS - 1].end_point = N;

  pthread_mutex_init(&ins_del, NULL);
  pthread_cond_init(&cond, NULL);
  
  for (i = 0; i < N; i++) {
    node_data[i].mass = galaxy[i].mass;
    one_over_mass[i] = 1 / galaxy[i].mass;
    node_data[i].pos_x = galaxy[i].pos_x;
    node_data[i].pos_y = galaxy[i].pos_y;
    velo[i].x = galaxy[i].velocity_x;
    velo[i].y = galaxy[i].velocity_y;
  }

  for (i = 0; i < THREADS; i++) {
    t_data[i].root = root;
    if (pthread_create(&t[i], NULL, (void *)thread_work, (void *)(t_data+i)))
      exit(EXIT_FAILURE);
    // printf("created thread %d\n", (int)t_data[i].t);
  }
  
  
 for (i = 0; i < THREADS; i++) {
    if (pthread_join(t[i], NULL))
      exit(EXIT_FAILURE);
  }
  

  // cleaning
  
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

  pthread_mutex_destroy(&ins_del);
  pthread_cond_destroy(&cond);

  
  // Dumping the data
  write_to_file(galaxy, N);
  free(forces);
  free(velo);
  free(galaxy);
  free(one_over_mass);
  free(node_data);
  pthread_barrier_destroy(&BARR);
  //  printf("Wall clock time: %lf\n",  get_wall_seconds() - timer);
  return 0;
}

void thread_work(void* arg){
  
  thread_t *data = (thread_t *)arg;
  int i, k; 
  int N = data->end_point;
  const int n_steps = data->n_steps;
  const int graphics = data->graphics;
  double acceleration[2];
  // Main driver for the simulation
  for (k = 0; k < n_steps; k++) {
    if (graphics) {
      ClearScreen();
    }
    //    printf("The current timestep: %d\n", k);
    pthread_mutex_lock(&ins_del);
    if (root == NULL) {
      init_root(&root);
      //printf("Created root: %p\n", root);
    }
    
    for (i = data->start_point; i < N; i++) {
      insert(root, ((data->node_data)+i)/* , i */);
      data->forces[i].x = 0.0;
      data->forces[i].y = 0.0;
     
    }
    //printf("%s\n", "Has inserted");
    pthread_mutex_unlock(&ins_del);
    pthread_barrier_wait(&BARR);
    
    if (data->start_point == 0) {
      update_mass(root);
    } 
    
    
    
    pthread_barrier_wait(&BARR);
  
    for (i = data->start_point; i < N; i++) {
      if (graphics) {
        DrawCircle(data->node_data[i].pos_x,  data->node_data[i].pos_y, 1, 1, circleRadius, circleColor);
      }
      //printf("%s\n","Calculating force" );
      
      traverse_for_force(&data->node_data[i], root, &data->forces[i], data->theta);
      
     }
  
    
    pthread_barrier_wait(&BARR);
  
    for (i = data->start_point; i < N; i++) {
      acceleration[0] = -1 * data->gravity * data->forces[i].x * data->one_over_mass[i];
      acceleration[1] = -1 * data->gravity * data->forces[i].y * data->one_over_mass[i];
      data->velo[i].x = data->velo[i].x + data->delta_t * acceleration[0];
      data->velo[i].y = data->velo[i].y + data->delta_t * acceleration[1];
    
      data->node_data[i].pos_x = data->node_data[i].pos_x + data->delta_t * data->velo[i].x;
      data->node_data[i].pos_y = data->node_data[i].pos_y + data->delta_t * data->velo[i].y;

    }

  
    if (graphics) {
      Refresh();
      usleep(3000);
    }

    if (data->start_point == 0) { // someone needs too delete root.
      //printf("%s\n", "Deleting tree" );
      delete(root);
      root = NULL;
    }

    /*  */
    pthread_barrier_wait(&BARR);
   
  } //  Ends time step loop
}

void print_usage(char *prg_name) {
  printf("Usage: %s N filename nsteps delta_t graphics\n", prg_name);
  printf("N, is the number of stars\n");
  printf("filename, path to the file with the initial configuration\n");
  printf("nsteps, the number of steps for the simulation\n");
  printf("delta_t, is the timestep size\n");
  printf("theta_max, is the threshold\n");
  printf("graphics, to enable graphics (1 = on, 0 = off)\n");
  printf("n_threads, number of threads \n");
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
