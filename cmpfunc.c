/*
 * cmpfunc.c
 *
 *  Created on: 2013/02/18
 *      Author: leo
 */

#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <time.h>
#include "log2.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value"

#define	AVGCNT		5			// Average count
#define	INITCNT		1000000		// Initial loop count
#define	REPEAT		10			// count

typedef struct {
	const char *name;
	void (*func)();
} INFO;

void test_overhead(TYPE n)
{
}

void test_add(TYPE n)
{
	n = 3 + n;
}

void test_addf(TYPE n)
{
	n = 3.0 +(double) n;
}

void test_multi(TYPE n)
{
	n = 3 * n;
}

void test_multif(TYPE n)
{
	n = 3.0 *(double) n;
}

void test_div(TYPE n)
{
	n = n / 3;
}

void test_divf(TYPE n)
{
	n = (double)n / 3.0;
}

void test_inv(TYPE n)
{
	n = 3 / n;
}

void test_invf(TYPE n)
{
	n = 3.0 / (double)n;
}

void test_random(TYPE n)
{
	n = (TYPE)rand();
}

void test_log(TYPE n)
{
	n = (TYPE)log((double)n);
}

void test_log2(TYPE n)
{
	n = (TYPE)log2b(n);
}

void test_log2N(TYPE n)
{
	n = (TYPE)log2N(n);	// see above.
}

void test_log2c(TYPE n)
{
	n = (TYPE)log2c(n);	// see above.
}

#pragma GCC diagnostic pop

static int	diff(const void *p1, const void *p2) {
	int	rtn;
	double d1 = *(double *)p1, d2 = *(double *)p2;
	if (d1 < d2) rtn = -1;
	else if (d1 > d2) rtn = 1;
	else rtn = 0;
	return	rtn;
}

double	myTest(void (*func)(), unsigned long long repeat, TYPE value) {
#define	N	10
//	struct	rusage	usage;
	struct timeval from, to;
	double	usec[N], microsec;
	for (int i = 0; i < N;) {
		gettimeofday(&from, NULL);
//		getrusage(RUSAGE_SELF, &usage);
//		from = usage.ru_utime;
		for (int j = 0; j < repeat; j++) (*func)(value);
		gettimeofday(&to, NULL);
//		getrusage(RUSAGE_SELF, &usage);
//		to = usage.ru_utime;
		time_t sec = to.tv_sec - from.tv_sec;
		microsec = sec * 1000000. + to.tv_usec - from.tv_usec;	/* in micro seconds	*/
		usec[i++] = microsec;
	}
	qsort(usec, N, sizeof(double), diff);
	return usec[1] / 1000.;	// in msec.
}

int main(int argc, char **argv) {
	extern int getopt(int argc, char * const argv[], const char *optstring);
	extern	int optind;
	extern	char *optarg;
	unsigned long long	repeat = 1000000;
	TYPE	value = 1024;
	char	*prg = argv[0];
	char	*p = strrchr(prg, '/');
	int		opt;
	while ((opt = getopt(argc, argv, "n:")) != -1) {
		switch (opt) {
	    case 'n':
	    	repeat = atoi(optarg);
	    	break;
	    default: /* '?' */
	    	if (p != NULL) prg = p;
	        fprintf(stderr, "Usage: %s [-n RepeatCount] [value]\n", prg);
	        return EXIT_FAILURE;
	    }
	 }
	if (argc > optind) value = atoi(argv[optind]);
	srand(time(0));
	struct timeval from;
	if (gettimeofday(&from, NULL)) {	// try
		perror("Failed to gettimeofday() :");
		return EXIT_FAILURE;
	}
	double overhead = myTest(test_overhead, repeat, value);
	double base = myTest(test_random, repeat, value);
	printf("Test : %lld times  n = %ld overhead = %.1f base(random) = %.1lf (= %.1lf - O.H.)\n"
			, repeat, value, overhead, base - overhead, base);
	base -= overhead;
	const INFO test[] = {
			{"3+n", test_add},
			{"3*n", test_multi},
			{"n/3", test_div},
			{"3/n", test_inv},
			{"3.0+n", test_addf},
			{"3.0*n", test_multif},
			{"n/3.0", test_divf},
			{"3.0/n", test_invf},
			{"log(n)", test_log},
			{"log2(n)", test_log2},
			{"log2N(n)", test_log2N},
			{"log2c(n)", test_log2c},
	};
	int	count = sizeof(test) / sizeof(test[0]);
	for (int i = 0; i < count; i++) {
		double	usec = myTest(test[i].func, repeat, value);
		double diff = usec - overhead;
		printf("%10s : %.1f ( %.1f )\trate = %.2f\n", test[i].name, diff, usec, diff / base);
	}
	return EXIT_SUCCESS;
}

