#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#define INIT_BUF 4096
#define DEBUG 0

typedef struct
{
    double *data;    // Pointer to hold all numbers
    size_t len;      // Number of elements in the data pointer
    size_t buf_size; // buffer size in bytes
} Data;

Data *read_input(FILE *f);
int compare_floats(const void *a, const void *b);
double pct(Data *data, double k);
double sum(Data *numbers);
double average(Data *numbers);
double variance(Data *numbers);
double stddev(Data *numbers);
void print_arr(Data *numbers);
void handle_signals(int signum, siginfo_t *info, void *ptr);
void catch_signals();
void print_csv(Data *numbers);
void print_json(Data *numbers);

int main(int argc, char **argv)
{
    char *usage =
        "Usage: numba_cruncha filename\n"
        "Positional Arguments:\n"
        "    filename:    Name of file to read from, alternately reads from stdin\n";

    Data *numbers;
    FILE *f;
    switch (argc)
    {
    case 1:
        // Read from stdin
        numbers = read_input(stdin);
        break;
    case 2:
        // Read from file
        f = fopen(argv[1], "r");
        if (f == NULL)
        {
            perror("Error opening input file");
            exit(EXIT_FAILURE);
        }
        numbers = read_input(f);
        fclose(f);
        break;
    default:
        fprintf(stderr, "Invalid options\n%s", usage);
        return EXIT_FAILURE;
    }

    if (DEBUG)
    {
        print_arr(numbers);
    }
    qsort((void *)numbers->data, numbers->len, sizeof(double), compare_floats);
    if (DEBUG)
    {
        print_arr(numbers);
    }

    print_json(numbers);

    free(numbers->data);
    free(numbers);
    return 0;
}

Data *read_input(FILE *f)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread, total_read = 0;
    Data *numbers = malloc(sizeof(Data));
    if (numbers == NULL) {
        perror("malloc failed!");
        exit(EXIT_FAILURE);
    } 
    numbers->len = 0;
    numbers->data = calloc(INIT_BUF, sizeof(double));
    numbers->buf_size = INIT_BUF * sizeof(double);
    if (numbers->data == NULL)
    {
        perror("Failed to allocate data buffer");
        exit(EXIT_FAILURE);
    }

    struct stat st;
    fstat(fileno(f), &st);
    // We only need a progress bar for larger files
    bool display_progress = st.st_size > 4096 * 1000;
    if (display_progress)
    {
        catch_signals();
        fprintf(stderr, "\e[?25l");
        fflush(stderr);
    }
    while ((nread = getline(&line, &len, f)) != -1)
    {
        if (display_progress && total_read % 1000 == 0)
        {
            char progress_bar[256] = "\0";
            double pct_complete = (double)total_read / (double)st.st_size;
            int bar_size = (int)(50.0 * pct_complete);
            sprintf(progress_bar, "\rReading file: [");
            for (int i = 0; i < 50; i++)
            {
                if (i < bar_size)
                {
                    sprintf(progress_bar + strlen(progress_bar), "#");
                }
                else
                {
                    sprintf(progress_bar + strlen(progress_bar), " ");
                }
            }
            sprintf(progress_bar + strlen(progress_bar), "] (%3.f%%)", pct_complete * 100);
            fprintf(stderr, "%s", progress_bar);
        }
        total_read += nread;
        if ((numbers->len + 1) * sizeof(double) > numbers->buf_size)
        {
            if ((numbers->data = realloc(numbers->data, numbers->buf_size * 2)) == NULL)
            {
                perror("Failed to reallocate data buffer");
                exit(EXIT_FAILURE);
            }
            numbers->buf_size = numbers->buf_size * 2;
        }
        if (nread > 1)
        {
            line[nread - 1] = '\0';
            char *end_ptr;
            double value = strtod(line, &end_ptr);
            if (line == end_ptr)
            {
                // Not a floating point value, ignore
                continue;
            }
            numbers->data[numbers->len] = value;
            numbers->len++;
        }
    }
    if (display_progress)
    {
        fprintf(stderr, "\e[?25h\n");
        fprintf(stderr, "Complete\n");
    }

    free(line);
    return numbers;
}

int compare_floats(const void *a, const void *b)
{
    double fa = *(const double *)a;
    double fb = *(const double *)b;
    return (fa > fb) - (fa < fb);
}

double pct(Data *numbers, double p)
{
    // Uses linear interpolation, C = 1. See Wikipedia.
    // Results should match numpy, and Excel (percentile.inc)
    // Assumes that `numbers->data` is sorted.
    double rank = (p / 100 * (numbers->len - 1));
    int lower = (int)rank;
    double decimal = rank - (double)lower;
    return numbers->data[lower] + (numbers->data[lower + 1] - numbers->data[lower]) * decimal;
}

double sum(Data *numbers)
{
    double result = 0;
    for (int i = 0; i < numbers->len; i++)
    {
        result += *(numbers->data + i);
    }
    return result;
}

double average(Data *numbers)
{
    return sum(numbers) / numbers->len;
}

double variance(Data *numbers)
{
    double avg = average(numbers);
    double var = 0;
    for (int i = 0; i < numbers->len; i++)
    {
        var += pow(*(numbers->data + i) - avg, 2);
    }
    return var / (double)numbers->len;
}

double stddev(Data *numbers)
{
    return sqrt(variance(numbers));
}

void print_arr(Data *numbers)
{
    printf("[ ");
    for (int i = 0; i < numbers->len; i++)
    {
        printf("%f ", *(numbers->data + i));
    }
    printf("]\n");
}

void handle_signal(int signum, siginfo_t *info, void *ptr)
{
    write(fileno(stdout), "\nExiting...\e[?25h\n", 19);
    exit(EXIT_FAILURE);
}

void catch_signals()
{
    static struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = handle_signal;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGINT, &act, 0);
}

void print_csv(Data *numbers)
{
    printf(
        "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n"
        "%.2f,%ld,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
        "Sum", "Count", "Mean", "Variance", "StdDev", "Min", "Max", "p25", "p30", "p40", "p50", "p60", "p70", "p75", "p80", "p90", "p95", "p99", "p99.9", "p99.99",
        sum(numbers), numbers->len, average(numbers), variance(numbers), stddev(numbers), numbers->data[0], numbers->data[numbers->len - 1],
        pct(numbers, 25), pct(numbers, 30), pct(numbers, 40), pct(numbers, 50), pct(numbers, 60), pct(numbers, 70), pct(numbers, 75), pct(numbers, 80),
        pct(numbers, 90), pct(numbers, 95), pct(numbers, 99), pct(numbers, 99.9), pct(numbers, 99.99));
}

void print_json(Data *numbers)
{
    printf(
        "{\"%s\":%.2f,\"%s\":%ld,\"%s\":%.2f,\"%s\":%.2f,\"%s\":%.2f,\"%s\":%.2f,\"%s\":%.2f,\"%s\":%.2f,"
        "\"%s\":%.2f,\"%s\":%.2f,\"%s\":%.2f,\"%s\":%.2f,\"%s\":%.2f,\"%s\":%.2f,\"%s\":%.2f,\"%s\":%.2f,"
        "\"%s\":%.2f,\"%s\":%.2f,\"%s\":%.2f,\"%s\":%.2f}\n",
        "Sum", sum(numbers),
        "Count", numbers->len,
        "Mean", average(numbers),
        "Variance", variance(numbers),
        "StdDev", stddev(numbers),
        "Min", numbers->data[0],
        "Max", numbers->data[numbers->len - 1],
        "p25", pct(numbers, 25),
        "p30", pct(numbers, 30),
        "p40", pct(numbers, 40),
        "p50", pct(numbers, 50),
        "p60", pct(numbers, 60),
        "p70", pct(numbers, 70),
        "p75", pct(numbers, 75),
        "p80", pct(numbers, 80),
        "p90", pct(numbers, 90),
        "p95", pct(numbers, 95),
        "p99", pct(numbers, 99),
        "p99.9", pct(numbers, 99.9),
        "p99.99", pct(numbers, 99.99)  
         );
}
