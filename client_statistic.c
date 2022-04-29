#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define FIB_DEV "/dev/fibonacci"
#define SAMPLE_SIZE 5000
#define MODE 3
#define OFFSET 93

int main()
{
    // long long sz;

    // char buf[1];
    // char write_buf[] = "testing writing";
    // int offset = 100; /* TODO: try test something bigger than the limit */
    FILE *fp_1 = fopen("gnuplot_fibdrv_1.txt", "w");
    FILE *fp_2 = fopen("gnuplot_fibdrv_2.txt", "w");
    FILE *fp_3 = fopen("gnuplot_fibdrv_3.txt", "w");
    FILE *fp_list[MODE] = {fp_1, fp_2, fp_3};
    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }
    double k_time[MODE][SAMPLE_SIZE] = {0.0}, u_time[MODE][SAMPLE_SIZE] = {0.0};
    double k_mean[OFFSET] = {0.0}, u_mean[OFFSET] = {0.0}, sd_k[MODE] = {0.0},
           sd_u[MODE] = {0.0}, result_k[OFFSET] = {0.0},
           result_u[OFFSET] = {0.0};
    int count_k[OFFSET] = {0}, count_u[OFFSET] = {0};
    for (int m = 0; m < MODE; ++m) {
        for (int i = 0; i <= 92; i++) {
            struct timespec t1, t2;
            lseek(fd, i, SEEK_SET);
            for (int n = 0; n < SAMPLE_SIZE; n++) {
                clock_gettime(CLOCK_MONOTONIC, &t1);
                k_time[m][n] =
                    (double) write(fd, NULL, m); /* runtime in kernel space */
                clock_gettime(CLOCK_MONOTONIC, &t2);
                u_time[m][n] = (long long) (t2.tv_sec * 1e9 + t2.tv_nsec) -
                               (t1.tv_sec * 1e9 + t1.tv_nsec);
                // printf("k time: %lf \n", k_time[m][n]);
                k_mean[i] += k_time[m][n];
                u_mean[i] += u_time[m][n];
                // printf("k mean: %lf \n", k_mean[i]);
            }
            k_mean[i] /= SAMPLE_SIZE;
            u_mean[i] /= SAMPLE_SIZE;
            for (int n = 0; n < SAMPLE_SIZE; n++) {
                sd_k[m] +=
                    (k_time[m][n] - k_mean[i]) * (k_time[m][n] - k_mean[i]);
            }
            for (int n = 0; n < SAMPLE_SIZE; n++) {
                sd_u[m] +=
                    (u_time[m][n] - u_mean[i]) * (u_time[m][n] - u_mean[i]);
            }
            sd_k[m] = sqrt(sd_k[m] / (SAMPLE_SIZE - 1));
            sd_u[m] = sqrt(sd_u[m] / (SAMPLE_SIZE - 1));
            for (int n = 0; n < SAMPLE_SIZE; n++) { /* remove outliers */
                if (k_time[m][n] <= (k_mean[i] + 2 * sd_k[m]) &&
                    k_time[m][n] >= (k_mean[i] - 2 * sd_k[m])) {
                    result_k[i] += k_time[m][n];
                    count_k[i]++;
                }
            }
            for (int n = 0; n < SAMPLE_SIZE; n++) { /* remove outliers */
                if (u_time[m][n] <= (u_mean[i] + 2 * sd_u[m]) &&
                    u_time[m][n] >= (u_mean[i] - 2 * sd_u[m])) {
                    result_u[i] += u_time[m][n];
                    count_u[i]++;
                }
            }
            result_k[i] /= count_k[i];
            result_u[i] /= count_u[i];
            fprintf(fp_list[m], "%d %lf %lf\n", i, result_k[i], result_u[i]);
        }
    }
    close(fd);
    fclose(fp_1);
    fclose(fp_2);
    return 0;
}
