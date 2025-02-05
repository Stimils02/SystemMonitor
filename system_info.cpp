#include "system_info.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "system_info.h"

/* Basic system information code here
 *
 * TODO: 
 * 1. Network title
 * 2. OS release version
 * 3. Kernel version
 * 4. Amount of memory
 * 5. Processor version
 * 6. Disk storage
 */

char *get_network_title() {
    // Get command output
    const char *cmd = "uname -a";
    FILE *fd = popen(cmd, "r");
    if (fd == NULL) {
        perror("popen");
        return NULL;
    }
    char *output = (char*)malloc(1024);
    fgets(output, 1023, fd);
    pclose(fd);
    fd = NULL;

    char *temp = output;
    char *start = output;
    char *network_title = (char*)malloc(1024);
    
    while (*temp != ' ') {
        temp++;
    }
    temp++;
    start = temp;

    while(*temp != ' ') {
        temp++;
    }
    *temp = '\0';

    strncpy(network_title, start, temp - start + 1);
    //printf("temp-start: %d\n", temp - start);
    strcat(network_title, "\0");
    free(output);
    output = NULL;
    return network_title;
}

char *get_os_name() {
    const char *cmd = "cat /etc/*-release | grep DISTRIB_ID";
    FILE *fd = popen(cmd, "r");
    if (fd == NULL) {
        perror("name popen");
        return NULL;
    }
    char *output = (char*)malloc(1024);
    fgets(output, 1023, fd);
    
    char *temp = output;
    while (*temp != '=') {
        temp++;
    }
    char *start = ++temp;
    while (*temp != '\n') {
        temp++;
    }
    *temp = '\0';
    char *name = (char*)malloc(1024);
    strncpy(name, start, temp - start + 1);
    if (fd == NULL) {
        perror("popen");
        return NULL;
    }
    
    return name;
}

char *get_os_release_version() {
    const char *cmd = "cat /etc/*-release | grep DISTRIB_RELEASE";
    FILE *fd = popen(cmd, "r");
    if (fd == NULL) {
        perror("name popen");
        return NULL;
    }
    char *output = (char*)malloc(1024);
    fgets(output, 1023, fd);
    
    char *temp = output;
    while (*temp != '=') {
        temp++;
    }
    char *start = ++temp;
    while (*temp != '\n') {
        temp++;
    }
    *temp = '\0';
    char *version = (char*)malloc(1024);
    strncpy(version, start, temp - start + 1);
    if (fd == NULL) {
        perror("popen");
        return NULL;
    }
    
    return version;
}

char *get_kernel_version() {
    // get kernel name
    const char *name_cmd = "uname";
    FILE *name_fd = popen(name_cmd, "r");
    if (name_fd == NULL) {
        perror("name popen");
        return NULL;
    }
    char *name_out = (char*)malloc(1024);
    fgets(name_out, 1023, name_fd);
    // get rid of the \n at the end
    char *temp = name_out;
    while(*temp != '\n') {
        temp++;
    }
    *temp = '\0';

    // get kernel version
    const char *version_cmd = "uname -r";
    FILE *version_fd = popen(version_cmd, "r");
    if (version_fd == NULL) {
        perror("version popen");
        return NULL;
    }
    char *version_out = (char*)malloc(1024);
    fgets(version_out, 1023, version_fd);
    // get rid of the \n at the end
    temp = version_out;
    for (int i = 0; i < 2; i++) {
        while(*temp != '\n') {
            temp++;
        }
        *temp = '\0';
    }

    // build final string
    char *final = (char*)malloc(1024);
    strcpy(final, name_out);
    strcat(final, " ");
    strcat(final, version_out);

    free(name_out);
    name_out = NULL;
    free(version_out);
    version_out = NULL;
    pclose(name_fd);
    name_fd = NULL;
    pclose(version_fd);
    version_fd = NULL;

    return final;
}

char *get_amount_memory() {
    const char *cmd = "free -g -h -t | grep Mem";
    FILE *fd = popen(cmd, "r");
    if (fd == NULL) {
        perror("name popen");
        return NULL;
    }
    char *output = (char*)malloc(1024);
    fgets(output, 1023, fd);

    char *temp = output;
    while (*temp != ' ') {
        temp++;
    }
    while (*temp == ' ') {
        temp++;
    }
    char *start = temp;
    while(*temp != 'G') {
        temp++;
    }
    *temp = '\0';

    char *amount_memory = (char*)malloc(1024);
    strncpy(amount_memory, start, temp - start + 1);
    strcat(amount_memory, " GiB");

    return amount_memory;
}

char *get_processor_version() {
    // get command output
    const char *name_cmd = "lscpu | grep name";
    FILE *name_fd = popen(name_cmd, "r");
    if (name_fd == NULL) {
        perror("name popen");
        return NULL;
    }
    char *name_output = (char*)malloc(1024);
    fgets(name_output, 1023, name_fd);

    // manipulate output to get processor version only
    char *temp = name_output;
    for (int i = 0; i < 2; i++) {
        while (*temp != ' ') {
            temp++;
        }

        while (*temp == ' ') {
            temp ++;
        }
    }

    char *start = temp;
    while (*temp != '\n') {
        temp++;
    }
    *temp = '\0';

    // build final string
    char *processor_version = (char*)malloc(1024);
    strncpy(processor_version, start, temp - start + 1);

    // get number of cores
    const char *number_cmd = "lscpu | grep CPU\\(s\\):";
    FILE *number_fd = popen(number_cmd, "r");
    if (number_fd == NULL) {
        perror("number popen");
        return NULL;
    }
    char *number_output = (char*) malloc(1024);
    fgets(number_output, 1023, number_fd);

    temp = number_output;
    while (*temp != ' ') {
        temp++;
    }
    
    while (*temp == ' ') {
        temp++;
    }
    start = temp;
    while (*temp != '\n') {
        temp++;
    }
    *temp = '\0';
    strcat(processor_version, " x ");
    strcat(processor_version, start);

    pclose(name_fd);
    name_fd = NULL;
    free(name_output);
    name_output = NULL;
    pclose(number_fd);
    number_fd = NULL;
    free(number_output);
    number_output = NULL;

    return processor_version;
}

char *get_disk_storage() {
    const char *cmd = "df -h --total | grep 'total' | awk '{print $4}'";
    FILE *fd = popen(cmd, "r");
    if (fd == NULL) {
        perror("number popen");
        return NULL;
    }
    char *output = (char*)malloc(1024);
    fgets(output, 1023, fd);

    char *temp = output;
    while (*temp != '\n') {
        temp++;
    }
    *temp = '\0';

    return output;
}
