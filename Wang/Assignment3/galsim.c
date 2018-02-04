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

typedef struct particle {
		double x,y,mass,v_x,v_y,brightness;
	}particle;

// Writting help message
void print_usage(char *prg_name);
//Reading in the configuration file into struct
void read_config(char *file_name, particle *p, int N);
//write the result into output file
void write_to_file(particle *p, int N);
//swap pointers
void pointer_swap(void **a, void **b);


int main(int argc, const char** argv) {

        /* get inputs*/
 
	if(argc != 6) {
		print_usage(argv[0]);
 		return -1;
  	}

	/*test of argc & argv*/
	/*
	printf("=======================================\n");
	printf("%d\n", N);
	printf("%s\n", filename);
	printf("%d\n", nsteps);
	printf("%f\n",delta_t) ;
	printf("%d\n",graphics);*/

	int N=atoi(argv[1]);
	const char* filename=argv[2];
	int nsteps=atoi(argv[3]);
	double delta_t=atof(argv[4]);
	const uint8_t graphics=atoi(argv[5]);

	double timer = get_wall_seconds();
	particle *p = malloc(N * sizeof(particle));
  particle *p_next = malloc(N * sizeof(particle));

  
	if (p == NULL && p_next != NULL) {
		printf("%s\n", "Out of memory");
		return -1;
	}

	read_config(filename,p,N);
	memcpy(p_next, p, 6*N*sizeof(double));
	

	/*start simulation*/
	if (graphics){
		//int L=1, W=1;
		//const float circleRadius=0.025, circleColor=0;
		const int windowWidth=800;
		InitializeGraphics(argv[0],windowWidth,windowWidth);
	 	SetCAxes(0,1);
	}

	float epsilon=0.001;
	double G=100.0/N;
	double F_x; double F_y;
	double r;
	double temp;   
	
	for(int step=0;step<nsteps;step++){
		if(graphics){
			 ClearScreen();
    		}

		for(int i=0;i<N;i++){

			if (graphics) {
        DrawCircle(p[i].x,  p[i].y, 1, 1, 0.025,0);
      }

			F_x=0.0;F_y=0.0;
			for(int j=0;j<N;j++){
				if(i!=j){

					r=sqrt(pow((p[i].x)-(p[j].x),2)+pow((p[i].y)-(p[j].y),2));
					temp=-G*(p[i].mass)*(p[j].mass)/pow(r+epsilon,3);
					F_x=F_x+temp*((p[i].x)-(p[j].x));
					F_y=F_y+temp*(p[i].y-(p[j].y));
					
				}
			}
			p_next[i].v_x=p[i].v_x+delta_t*(F_x/p[i].mass);
			p_next[i].v_y=p[i].v_y+delta_t*(F_y/p[i].mass);
			p_next[i].x=p[i].x+delta_t*(p_next[i].v_x);
			p_next[i].y=p[i].y+delta_t*(p_next[i].v_y);

		}

		pointer_swap((void**)&p, (void**)&p_next);
		
		if(graphics){
		Refresh();
		/* Sleep a short while to avoid screen flickering. */
		usleep(3000);
		}
	}
	
	if(graphics){
		FlushDisplay();
  	CloseDisplay();
	}

	write_to_file(p, N);
	free(p);
	free(p_next);	
	
	printf("Wall clock time: %lf\n",  get_wall_seconds() - timer);

	
	return 0;
}

void print_usage(char *prg_name){
	printf("The program %s expects 5 input arguments:\n",prg_name);
	printf("N: number of stars/particles to simulate\n");
	printf("filename: the filename of the file to read the initial configuration from\n");
	printf("nsteps: is the number of time steps.\n");
	printf("delta_t: timestep\n");
	printf("graphics:1 or 0 meaning graphics on/off.\n");
}

void read_config(char *filename, particle *p, int N){
  
  FILE *fd;
  
  const int SIZE = sizeof(double);
  const int AMOUNT_ELEMENTS = 6*N;
  fd = fopen(filename, "r");

  if (fd == NULL) {
    printf("Couldn't find the file: %s\n", filename);
    exit(EXIT_FAILURE);
  }
  

  fread(p, SIZE, AMOUNT_ELEMENTS, fd);
  
  fclose(fd);
}

void pointer_swap(void **a, void **b){
  void *tmp = *a;
  *a = *b;
  *b = tmp;
}

void write_to_file(particle *p, int N){
  
  FILE *fd;
  
  const int SIZE = sizeof(double);
  const int AMOUNT_ELEMENTS = 6*N;
  fd = fopen("./result.gal", "w+");

  if (fd == NULL) {
    printf("Couldn't write to the output file\n");
    exit(EXIT_FAILURE);
  }
  
  fwrite(p , SIZE, AMOUNT_ELEMENTS, fd);
  fclose(fd);
}

	
	
