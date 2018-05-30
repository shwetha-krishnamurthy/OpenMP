#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <fstream>
#include <math.h>

#define _GNU_SOURCE

struct vec{
	double l, w, d;
};

struct body{
	double mass;
	double rad;
	struct vec cord;
	struct vec vel;
	struct vec force;
};

struct body *spheres = (struct body *)malloc(1000 * sizeof(struct body));
double l = 100, w = 200, d = 400;

double force(int s1, int s2)
{
	double G = 6.67*pow(10, -11);
	double r = sqrt(pow(((spheres + s1)->cord.l - (spheres + s2)->cord.l), 2) + pow(((spheres + s1)->cord.w - (spheres + s2)->cord.w), 2) + pow(((spheres + s1)->cord.d - (spheres + s2)->cord.d), 2));
	double F = G*(spheres + s1)->mass*(spheres + s2)->mass/(double)pow(r, 2);
	//printf("%f, %e\n", r, F);
	return F;
}

void calc_force(int s)
{
	double Fl = 0, Fw = 0, Fd = 0, F;
	int i1;
	struct vec f;
	
	#pragma omp parallel for schedule(dynamic) private(F) reduction(+: Fl, Fw, Fd)
		for(i1 = 0; i1<1000; i1++)
		{
			double length = sqrt(pow(((spheres + s)->cord.l - (spheres + i1)->cord.l),2) + pow(((spheres + s)->cord.w - (spheres + i1)->cord.w),2) + pow(((spheres + s)->cord.d - (spheres + i1)->cord.d),2));
			if(length != 0)
			{
				F = force(s, i1);
				//printf("%f\n", F);
				Fl += F * ((spheres + s)->cord.l - (spheres + i1)->cord.l) / length;
				Fw += F * ((spheres + s)->cord.w - (spheres + i1)->cord.w) / length;
				Fd += F * ((spheres + s)->cord.d - (spheres + i1)->cord.d) / length;
			}
		}

	(spheres + s)->force.l = Fl;
	(spheres + s)->force.w = Fw;
	(spheres + s)->force.d = Fd;
}


void initialize_coordinates()
{
	int i;
	#pragma omp parallel for schedule(dynamic)
		for(i = 0; i<1000; i++)
		{
			(spheres+i)->mass = 1;
			(spheres+i)->rad = 0.5;
			(spheres+i)->cord.l = (l+1)*((double)rand()/(double)RAND_MAX);
			(spheres+i)->cord.w = (w+1)*((double)rand()/(double)RAND_MAX);
			(spheres+i)->cord.d = (d+1)*((double)rand()/(double)RAND_MAX);
			(spheres+i)->vel.l = 0;
			(spheres+i)->vel.w = 0;
			(spheres+i)->vel.d = 0;
			//printf("l[%d] = %f, w[%d] = %f, d[%d] = %f\n", i, spheres[i].lcord, i, spheres[i].wcord, i, spheres[i].dcord);
		}

	#pragma omp parallel for schedule(dynamic)
		for (i = 0; i < 1000; ++i)
			{
				calc_force(i);
			}	
}

void update_pos_vel(int s, struct vec f_body)
{
	struct vec v_n12; 
	v_n12.l = (spheres + s)->vel.l + f_body.l*0.005/(2*(spheres + s)->mass);
	v_n12.w = (spheres + s)->vel.w + f_body.w*0.005/(2*(spheres + s)->mass);
	v_n12.d = (spheres + s)->vel.d + f_body.d*0.005/(2*(spheres + s)->mass);

	(spheres + s)->cord.l = (spheres + s)->cord.l + v_n12.l*0.005;
	(spheres + s)->cord.w = (spheres + s)->cord.w + v_n12.w*0.005;
	(spheres + s)->cord.d = (spheres + s)->cord.d + v_n12.d*0.005;

	calc_force(s);

	(spheres + s)->vel.l = v_n12.l + (spheres + s)->force.l*0.005/(2*(spheres + s)->mass);
	(spheres + s)->vel.w = v_n12.w + (spheres + s)->force.w*0.005/(2*(spheres + s)->mass);
	(spheres + s)->vel.d = v_n12.d + (spheres + s)->force.d*0.005/(2*(spheres + s)->mass);

	if((spheres + s)->cord.l > l)
		(spheres + s)->cord.l = l - ((spheres + s)->cord.l - l);
	
	if((spheres + s)->cord.w > w)
		(spheres + s)->cord.w = w - ((spheres + s)->cord.w - w);

	if((spheres + s)->cord.l > d)
		(spheres + s)->cord.d = d - ((spheres + s)->cord.d - d);


	if((spheres + s)->cord.l < 0)
		(spheres + s)->cord.l = -(spheres + s)->cord.l;
	
	if((spheres + s)->cord.w < 0)
		(spheres + s)->cord.w = -(spheres + s)->cord.w;

	if((spheres + s)->cord.l < 0)
		(spheres + s)->cord.d = -(spheres + s)->cord.d;
}

int main()
{
	initialize_coordinates();
	int i0;
	struct vec f_body;
	unsigned int count = 0;
	omp_set_num_threads(4);
	FILE *t = fopen("trajectory.txt", "w");
	FILE *log = fopen("log.txt", "w");
    fprintf(log, "#processors	: 4 \nvendor_id	: GenuineIntel \ncpu family	: 6\nmodel		: 58\nmodel name	: Intel(R) Core(TM) i3-3217U CPU @ 1.80GHz\nstepping	: 9\nmicrocode	: 0x17\ncpu MHz		: 1800.000\ncache size	: 3072 KB\n");
    fprintf(log, "Number of threads: %d\n", omp_get_num_threads());
	double sim_s, sim_e, step_s, step_e;
	sim_s = omp_get_wtime();
	while(count < 720*1000)
	{
		if(count%1000 == 0)
			fprintf(t, "%dth Step\n", count);

		step_s = omp_get_wtime();

		#pragma omp parallel for private(f_body)
		for(i0 = 0; i0 < 1000; i0++)
		{
			f_body = (spheres + i0)->force;
			update_pos_vel(i0, f_body);
			if(count%1000 == 0)
			{
				fprintf(t, "Ball %d at (%f, %f, %f)\n", i0, (spheres+i0)->cord.l, (spheres+i0)->cord.w, (spheres+i0)->cord.d);
			}
			//printf("Updated position of %d : (%e, %e, %e)\n", i0, (spheres+i0)->cord.l, (spheres+i0)->cord.w, (spheres+i0)->cord.d);
		}	

		step_e = omp_get_wtime();
		count ++;

		fprintf(log, "Time elapsed in %dth step: %fs\n", count, step_e - step_s);
		if(count%1000 == 0)
		{
			fprintf(t, "Time elasped in %dth step: %fs\n", count, step_e - step_s);
			fprintf(t, "TERMINATE\n");
		}
	}
	sim_e = omp_get_wtime();
	fprintf(log, "Total simulation time: %fs\n", sim_e - sim_s);
	return 0;
}