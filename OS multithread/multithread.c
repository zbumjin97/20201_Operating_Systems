#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER ;
pthread_cond_t  c = PTHREAD_COND_INITIALIZER ;
int number[100] = {};
int result[10][101] = {};

void *func1(int index) {
        *result[index] = number;
        srand(time(NULL));
	for(int j = 0; j < 100000; j++) {
                int idx = rand() % 100;
                result[index][idx + 1]++;

                pthread_mutex_lock(&m);
                pthread_cond_signal(&c);
                pthread_mutex_unlock(&m);

                pthread_mutex_lock(&m);
                pthread_cond_wait(&c, &m);
                pthread_mutex_unlock(&m);
        }
}

void *func2(int index) {
        *result[index] = number;
        for(int j = 0; j < 100000; j++) {
                pthread_mutex_lock(&m);
                pthread_cond_wait(&c, &m);
                pthread_mutex_unlock(&m);

                int idx = rand() % 100;
                result[index][idx + 1]++;

                pthread_mutex_lock(&m);
                pthread_cond_signal(&c);
                pthread_mutex_unlock(&m);
        }
}

int main() {
        pthread_t thread[10];
        int i, j;
	int sum[10] = {};
	int report[101] = {};
	FILE *f;
        for(i = 0; i < 10; i++) {
		if(i == 0) pthread_create(&thread[i], NULL, func1, i);
		else pthread_create(&thread[i], NULL, func2, i);
        }
        for(i = 0; i < 10; i++) pthread_join(thread[i], NULL);
	f = fopen("result.dat", "w");
	if(f != NULL) {
		for(i = 0; i < 10; i++) {
			for(j = 1; j < 101; j++) sum[i] += result[i][j];
			result[i][0] = sum[i];
			fprintf(f, "\nThread%d: ", i);
			for(j = 0; j < 101; j++) fprintf(f, "%5d ", result[i][j]);
			fprintf(f, "\n");
		}
		fprintf(f, "\nResult: ");
		for(i = 0; i < 101; i++) {
			for(j = 0; j < 10; j++) report[i] += result[j][i];
			fprintf(f, "%5d ", report[i]);
		}
	}
	else printf("Writing disabled");
	exit(0);
}
