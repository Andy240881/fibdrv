#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define FIB_DEV "/dev/fibonacci"
#define SAMPLE_SIZE 5000
#define MODE 2
#define OFFSET 100

int main()
{
    // long long sz;

    // char buf[1];
    // char write_buf[] = "testing writing";
    // int offset = 100; /* TODO: try test something bigger than the limit */
    FILE *fp = fopen("gnuplot_fibdrv.txt", "w");
    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }
    int m = 0;
    for (int i = 0; i <= 92; i++) {
        struct timespec t1, t2;
        lseek(fd, i, SEEK_SET);

        double k_time[MODE][SAMPLE_SIZE] = {0.0},
               u_time[MODE][SAMPLE_SIZE] = {0.0};
        double k_mean[OFFSET] = {0.0}, u_mean[OFFSET] = {0.0},
               sb[OFFSET] = {0.0};

        for (int n = 0; n < SAMPLE_SIZE; n++) {
            clock_gettime(CLOCK_MONOTONIC, &t1);
            k_time[m][n] =
                (double) write(fd, NULL, 0); /* runtime in kernel space */
            clock_gettime(CLOCK_MONOTONIC, &t2);
            u_time[m][n] = (long long) (t2.tv_sec * 1e9 + t2.tv_nsec) -
                           (t1.tv_sec * 1e9 + t1.tv_nsec);
            // printf("k time: %lf \n", k_time[m][n]);
            k_mean[i] += k_time[m][n];
            u_mean[i] += u_time[m][n];
            // printf("k mean: %lf \n", k_mean[i]);
        }
        // for (int m = 0; m < MODE; ++m) {
        k_mean[i] /= SAMPLE_SIZE;
        u_mean[i] /= SAMPLE_SIZE;
        printf("k mean: %.5lf \n", k_mean[i]);
        // }
        // TODO: Delete the outliers.
        // for (int m = 0; m < MODE; ++m) {
        fprintf(fp, "%d %lf %lf\n", i, k_mean[i], u_mean[i]);
        // }
    }
    close(fd);
    fclose(fp);
    return 0;
}
