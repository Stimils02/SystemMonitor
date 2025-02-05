#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/* Graphs code here
 *
 * TODO: 
 * 1. CPU History
 * 2. Memory and Swap History
 * 3. Network History
 */

#define BUF_MAX 2048
#define MAX_CPU 256

int read_cpu_fields(FILE *fd, unsigned long long int *fields) {
    int ret;
    char buffer[BUF_MAX];

    if (!fgets(buffer, BUF_MAX, fd)) {
        perror("fgets");
    }

  //  printf("buffer: %s\n", buffer);

    ret = sscanf (buffer, "c%*s %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu",
                            &fields[0],
                            &fields[1],
                            &fields[2],
                            &fields[3],
                            &fields[4],
                            &fields[5],
                            &fields[6],
                            &fields[7],
                            &fields[8],
                            &fields[9]);
    
    if (ret == 0) {
        return -1;
    }
    return 1;
}

char * get_cpu_usage() {
    FILE *fd = fopen("/proc/stat", "r");
    if (fd == NULL) {
        perror("fd");
    }
    unsigned long long int fields[10];
    unsigned long long int total_tick[MAX_CPU];
    unsigned long long int total_tick_old[MAX_CPU];
    unsigned long long int idle[MAX_CPU];
    unsigned long long int idle_old[MAX_CPU];
    unsigned long long int del_total_tick[MAX_CPU];
    unsigned long long int del_idle[MAX_CPU];
    int update_cycle = 0;
    int i;
    int cpus = 0;
    int count;
    double percent_usage;

    while (read_cpu_fields(fd, fields) != -1) {
        for (i = 0, total_tick[cpus] = 0; i < 10; i++) {
            total_tick[cpus] += fields[i];
        }
        idle[cpus] = fields[3];
        //printf("cpus: %d\n", cpus);
        cpus++;
    }

    char * usage_str = (char*)malloc(2048);
    memset(usage_str, '\0', 1024);

    //while(1) {
    sleep(1);
    fseek (fd, 0, SEEK_SET);
    fflush (fd);
    //printf ("[Update cycle %d]\n", update_cycle); 
    for (count = 0; count < cpus; count++) {
        total_tick_old[count] = total_tick[count];
        idle_old[count] = idle[count];
 
        if (!read_cpu_fields(fd, fields)) {
            return 0;
        }
 
        for (i=0, total_tick[count] = 0; i < 10; i++) {
            total_tick[count] += fields[i];
        }
        idle[count] = fields[3];

        del_total_tick[count] = total_tick[count] - total_tick_old[count];
        del_idle[count] = idle[count] - idle_old[count];
 
        percent_usage = ((del_total_tick[count] - del_idle[count]) / (double) del_total_tick[count]) * 100;
        char percent_usage_str[10];
        if (count != 0) {
           // printf("CPU%d Usage: %3.2lf%%\n", count - 1, percent_usage);
            snprintf(percent_usage_str, 10, "%3.2lf", percent_usage);
            strcat(usage_str, percent_usage_str);
            strcat(usage_str, ",");
        }
    }
    update_cycle++;
    //printf("\n");
   // }
    fclose(fd);
    return usage_str;
}

int read_mem_swap_info(FILE *fd, char *name, unsigned long long temp, char *unit) {
    int ret;
    char buffer[BUF_MAX];

    if (!fgets(buffer, BUF_MAX, fd)) {
        perror("fgets");
    }

    ret = sscanf(buffer, "%s %Lu %s", name, &temp, unit);

    //printf("temp: %lld\n", temp);

    //printf("ret: %d\n", ret);

    if (ret == 0) {
        return -1;
    }
    return temp;
}

char *get_memory_usage() {
    FILE *fd = fopen("/proc/meminfo", "r");
    if (fd == NULL) {
        perror("fd");
    }
    unsigned long long int temp = 0;
    char *name = (char*)malloc(1024);
    char *unit = (char*) malloc(8);
    char * usage_str = (char*)malloc(2048);
    unsigned long long int amount;

    double mem_total = 0;
    double mem_free = 0;
    double swap_total = 0;
    double swap_free = 0;
    
    while ((int)(amount = read_mem_swap_info(fd, name, temp, unit)) != -1) {
        //printf("amount: %lld\n", amount);
        if (strcmp(name, "MemTotal:") == 0) {
            mem_total = (double) amount / 1000000;
            //printf("mem_total: %lf\n", mem_total);
        } else if (strcmp(name, "MemFree:") == 0) {
            mem_free = (double) amount / 1000000;
            //printf("mem_free: %lf\n", mem_free);
        } else if (strcmp(name, "SwapTotal:") == 0) {
            swap_total = (double) amount / 1000000;
            //printf("swap_total: %lf\n", swap_total);
        } else if (strcmp(name, "SwapFree:") == 0) {
            swap_free = (double) amount / 1000000;
            //printf("swap_free: %lf\n", swap_free);
        }

        snprintf(usage_str, 100, "%lld", amount);

        //printf("name: %s\nmem: %s\nunit: %s\n", name, usage_str, unit);
    }

    double mem_used = mem_total - mem_free;
    double swap_used = swap_total - swap_free;

    snprintf(usage_str, 1024, "%3.2lf,%3.2lf\n%3.2lf,%3.2lf", mem_used, mem_total, swap_used, swap_total);

    return usage_str;
}

int read_network_info(FILE *fd, unsigned long long int *fields) {
    int ret;
    char buffer[BUF_MAX];

    if (!fgets(buffer, BUF_MAX, fd)) {
        return -1;
        perror("fgets");
        printf("here");
    }

    char *temp = (char*)malloc(16);

    ret = sscanf(buffer, "%s %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu",
                            temp,
                            &fields[0],
                            &fields[1],
                            &fields[2],
                            &fields[3],
                            &fields[4],
                            &fields[5],
                            &fields[6],
                            &fields[7],
                            &fields[8],
                            &fields[9],
                            &fields[10],
                            &fields[11],
                            &fields[12],
                            &fields[13],
                            &fields[14],
                            &fields[15]);
    
    //printf("ret: %d\n", ret);
    
    free(temp);
    temp = NULL;

    if (ret == 0) {
        return -1;
    }
    if (ret != 17) {
        return 0;
    }
    return 1;
}

char *get_network_usage() {
    FILE *fd = fopen("/proc/net/dev", "r");
    if (fd == NULL) {
        perror("fd");
    }
    unsigned long long int fields[16];
    int ret;
    unsigned long long int bytes_received_old = 0;
    unsigned long long int bytes_sent_old = 0;
    unsigned long long int bytes_received_new = 0;
    unsigned long long int bytes_sent_new = 0;

    while ((ret = read_network_info(fd, fields)) != -1) {
        //printf("ret: %d\n", ret);
        if (ret == 1) {
            //printf("old loop\n");
            bytes_received_old += fields[0];
            bytes_sent_old += fields[8];
        }
    }
    fclose(fd);
    fd = fopen("/proc/net/dev", "r");
    while ((ret = read_network_info(fd, fields)) != -1) {
        if (ret == 1) {
            //printf("new loop\n");
            bytes_received_new += fields[0];
            bytes_sent_new += fields[8];
        }
    }

    //printf("received_old = %lld\n", bytes_received_old);
    //printf("received_new = %lld\n", bytes_received_new);
    //printf("sent_old = %lld\n", bytes_sent_old);
    //printf("sent_new = %lld\n", bytes_sent_new);

    char *usage_str = (char*)malloc(1024);
    double tb_received = bytes_received_new / 1000000000000;
    double tb_sent = bytes_sent_new / 1000000000000;
    double receive_speed = (double) (bytes_received_new - bytes_received_old) / 1000;
    double send_speed = (double) (bytes_sent_new - bytes_sent_old) / 1000;

    //printf("receive speed: %lf\n", receive_speed);
    //printf("send speed: %lf\n", send_speed);

    snprintf(usage_str, 1024, "%3.2lf/%3.2lf %3.2lf/%3.2lf", receive_speed, send_speed, tb_received, tb_sent);

    return usage_str;
}
