#include "process.h"
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <sys/resource.h>
#include <dirent.h>

extern char g_process_file;
int g_num_pids = 0;
int g_num_my_pids = 0;
int g_num_active_pids = 0;
char ** g_open_fds = NULL;
char ** g_open_types = NULL;
char ** g_open_paths = NULL;
int g_num_open_files;

void free_and_nullify(void ** pointer) {
    if (*pointer != NULL) {
        free(*pointer);
        *pointer = NULL;
    }
}

/*
 * Given a pid, returns the process name
 * by accessing /proc/ filesystem. Returns a char *
 * containing the process_name.
 */

/*char * get_process_name(int pid) {

    char command[100];

    char test_path[64];
    snprintf(test_path, 64, "/proc/%d/comm", pid);
    FILE * test_file = fopen(test_path, "r");

    char * no_file_pointer;
    
    if (test_file == NULL) {
        return NULL;
    }
    fclose(test_file);

    snprintf(command, sizeof(command), "cat /proc/%d/comm > memory_maps/name.txt", pid);
    system(command);

    FILE * output_file = fopen("memory_maps/name.txt", "r");

    if (output_file == NULL) {
        perror("fopen");
        return NULL;
    }

    char * buffer = (char*)malloc(128);

    fgets(buffer, 128, output_file);
    if (buffer == NULL) {
        perror("fgets");
        return NULL;
    }

    if (buffer[strlen(buffer) - 1] == '\n') {
        buffer[strlen(buffer) - 1] = '\0';
    }

    fclose(output_file);

    return buffer;
}*/ /* get_process_name() */

char * get_process_name(int pid) {
    char comm_path[64];
    snprintf(comm_path, sizeof(comm_path), "/proc/%d/comm", pid);

    FILE * comm_file = fopen(comm_path, "r");
    if (comm_file == NULL) {
        // perror("fopen");
        return NULL;
    }

    char buffer[128];
    if (fgets(buffer, sizeof(buffer), comm_file) == NULL) {
        fclose(comm_file);
        perror("fgets");
        return NULL;
    }

    fclose(comm_file);

    // Remove the newline character if it exists
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }

    // Allocate memory for the process name and copy the buffer
    char * process_name = strdup(buffer);
    if (process_name == NULL) {
        perror("strdup");
        return NULL;
    }

    return process_name;
}

/*
 * Given a pid, returns the status
 * of the given process as a char *.
 */

/*char * get_status(int pid) {

    char command[100];
    snprintf(command, sizeof(command), "cat /proc/%d/status | grep -i state | cut -d \' \' -f 1 > memory_maps/status.txt ", pid);
    system(command);*/

    /*
    States:
    - R: Running
    - S: Sleeping
    - Z: Zombie
    - T: Stopped
    - D: Uninterruptible sleep
    */

    /*FILE * output_file = fopen("memory_maps/status.txt", "r");

    if (output_file == NULL) {
        perror("fopen");
        return NULL;
    }

    char buffer = fgetc(output_file);

    if (buffer == EOF) {
        perror("fgets");
        return NULL;
    }

    while (buffer != ':') {
        buffer = fgetc(output_file);
    }
    buffer = fgetc(output_file);
    buffer = fgetc(output_file);

    if (buffer == EOF) {
        perror("fgets");
        return NULL;
    }
    
    char * stat_string = (char*)malloc(sizeof(char) * 128);

    switch(buffer) {
        case 'R':
            snprintf(stat_string, 128, "Running");
            break;
        case 'S':
            snprintf(stat_string, 128, "Sleeping");
            break;
        case 'Z':
            snprintf(stat_string, 128, "Zombie");
            break;            
        case 'T':
            snprintf(stat_string, 128, "Stopped");
            break;
        case 'D':
            snprintf(stat_string, 128, "Uninterruptible sleep");
            break;
        case 't':
            snprintf(stat_string, 128, "Stopped");
            break;
        default: // unkown, print the buffer
            snprintf(stat_string, 128, "%c", buffer);
        }

    fclose(output_file);
    return stat_string;
}*/ /* get_status() */

char * get_status(int pid) {
    char status_path[64];
    snprintf(status_path, sizeof(status_path), "/proc/%d/status", pid);

    FILE * status_file = fopen(status_path, "r");
    if (status_file == NULL) {
        //perror("fopen");
        return NULL;
    }

    char * status = NULL;
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), status_file) != NULL) {
        // Search for the line starting with "State:"
        if (strncmp(buffer, "State:", 6) == 0) {
            // Skip the "State:" prefix and leading whitespace
            char *state_str = buffer + 6;
            while (*state_str == ' ' || *state_str == '\t') {
                state_str++;
            }

            // Remove trailing newline character if it exists
            char *newline = strchr(state_str, '\n');
            if (newline != NULL) {
                *newline = '\0';
            }

            // Allocate memory for the status string and copy the state
            status = strdup(state_str);
            break; // Exit the loop after finding the state
        }
    }

    fclose(status_file);
    return status;
}

/*
 * return percentage CPU as char *
 */

/*char * get_cpu_info(int pid) {

    FILE* file = fopen("/proc/self/stat", "r");
    if (file == NULL) {
        perror("Failed to open /proc/self/stat");
        return NULL;
    }

    // Variables to store CPU usage values
    unsigned long utime, stime;

    // Read the values from the /proc/self/stat file
    if (fscanf(file, "%*d %*s %*c %*d %*d %*d %*d %*d %lu %lu %*lu %*lu %*lu %*lu %*lu %*lu",
               &utime, &stime) != 2) {
        perror("Failed to read /proc/self/stat");
        fclose(file);
        return NULL;
    }

    fclose(file);

    // Calculate total CPU time
    unsigned long total_time = utime + stime;

    // Get the clock ticks per second
    long clk_ticks = sysconf(_SC_CLK_TCK);

    // Calculate CPU usage as a percentage
    double cpu_usage = 100.0 * total_time / clk_ticks;

    printf("CPU Usage: %.2f%%\n", cpu_usage);

    return NULL;

}*/

/*char * get_cpu_info(int pid) {

    char command[100];
    sprintf(command, "get_cpu_usage %d > memory_maps/cpu_info.txt", pid);
    system(command);

    FILE * output_file = fopen("memory_maps/cpu_info.txt", "r");

    if (output_file == NULL) {
        perror("fopen");
        return NULL;
    }

    char * buffer = (char*)malloc(128);

    fgets(buffer, 128, output_file);
    if (buffer == NULL) {
        fclose(output_file);
        perror("fgets");
        return NULL;
    }

    if (buffer[strlen(buffer) - 1] == '\n') {
        buffer[strlen(buffer) - 1] = '\0';
    }

    fclose(output_file);
    return buffer;
}*/

char * get_cpu_info(int pid) {
    char stat_path[256];
    snprintf(stat_path, sizeof(stat_path), "/proc/%d/stat", pid);

    FILE *stat_file = fopen(stat_path, "r");
    if (stat_file == NULL) {
        perror("fopen");
        return NULL; // Error
    }

    // Read values from the /proc/PID/stat file
    unsigned long utime, stime;
    if (fscanf(stat_file, "%*d %*s %*c %*d %*d %*d %*d %*d %*lu %lu %*lu %*lu %*lu %*lu %*lu %*lu %*ld %*ld %*ld %*ld %*ld %*ld %*llu %lu", &utime, &stime) != 2) {
        fclose(stat_file);
        //perror("fscanf");
        return NULL; // Error
    }

    fclose(stat_file);

    // Calculate CPU usage percentage
    unsigned long total_time = utime + stime;
    long clock_ticks_per_second = sysconf(_SC_CLK_TCK);
    double cpu_usage = ((double)(total_time) / clock_ticks_per_second); // * 100

    char * str = (char*)malloc(sizeof(char) * 16);
    sprintf(str, "%.2f", cpu_usage);

    return str;
}


/*
 * Given a pid, accesses the /proc/pid/status
 * file and reads the memory usage of a given process.
 * Returns a char * with the memory in kB.
 * Same as RSS
 */
char * get_memory(int pid) {

    char command[100];
    sprintf(command, "grep -i vmrss /proc/%d/status | cut -d \' \' -f 5 > memory_maps/mem.txt", pid);
    int sys = system(command);
    if (sys) {
    }

    /*FILE * output_file = fopen("memory_maps/mem.txt", "r");

    if (output_file == NULL) {
        perror("fopen");
        return NULL;
    }


    fgets(buffer, 128, output_file);
    if (buffer == NULL) {
        perror("fgets");
        return NULL;
    }

    if (buffer[strlen(buffer) - 1] == '\n') {
        buffer[strlen(buffer) - 1] = '\0';
    }*/
    int mem_fd = open("memory_maps/mem.txt", O_RDONLY);
    if (mem_fd == -1) {
        close(mem_fd);
        perror("open mem.txt");
        return NULL;
    }

    char * buffer = (char*)malloc(128);
    size_t read_size = read(mem_fd, buffer, 128);
    close(mem_fd);

    if (read_size == 0) {
        buffer = strdup("0 kB");
        return buffer;
    }

    if (atoi(buffer) == 0) {
        free(buffer);
        buffer = strdup("0 kB");
    }
    else {
        buffer[read_size - 1] = '\0';
        strcat(buffer, " kB");
    }

    // fclose(output_file);
    return buffer;
} /* get_memory() */

// TODO: GET ID info
// shouldn't need this function

char * get_id(int pid) {
    return NULL;
}

// TODO: write to g_process_file

void write_process_info() {
}

/*
 * Stops (pauses) the process being run at the given PID.
 * Returns 1 if stopped, 0 otherwise.
 */
int stop_process(int pid) {

    if (kill(pid, SIGSTOP) == 0) {
        // sucess stopping process

        return 1;
    }
    else {
        // failed to stop process

        perror("Failed to stop process");
        return 0;
    }
}

/*
 * Continues the process being run at the given PID.
 * Returns 1 if continued, 0 otherwise.
 */

int continue_process(int pid) {
    if (kill(pid, SIGCONT) == 0) {
        // resume process

        return 1;

    }
    else {
        // failed to resume process
        
        perror("Failed to resume process");
        return 0;
    }
}

/*
 * Kills the process given the PID.
 * Returns 1 if killed, 0 on fail.
 */
int kill_process(int pid) {

    if (kill(pid, SIGTERM) == 0) {
        // success kill process

        return 1;
    }
    else {
        // failed to kill process

        perror("Failed to kill process");
        return 0;
    }

}

/*
 * Helper function to get_memory_maps()
 * Parses the corresponding memory_maps/PID#_map.txt
 * and the corresponding memory_maps/PID#_pmap.txt
 * and returns the same as what's written in the file descriptor
 * as a char *
 */

char * parse_map_info(char * map_buffer, char * pmap_buffer) {

    int map_index = 0;
    int pmap_index = 0;
    char * written_buffer = (char *)malloc(sizeof(char) * FILE_SIZE);

    while (map_buffer[map_index] != '\0') {
        // read the map_buffer until the EOF

        char file_name[128];
        char vm_start[16];
        char vm_end[16];
        char vm_size[16];
        char permissions[5];
        char vm_offset[16];

        // VM Start (hex)

        int index = 0;
        while (map_buffer[map_index] != '-') {
            vm_start[index] = map_buffer[map_index];
            index++;
            map_index++;
        }
        vm_start[index] = '\0';

        map_index++; // get off '-'

        // VM End (hex)

        index = 0;
        while (map_buffer[map_index] != ' ') {
            vm_end[index] = map_buffer[map_index];
            index++;
            map_index++;
        }
        vm_end[index] = '\0';
        
        map_index++; // get off ' '

        // Flags (permissions) (rwxp)

        index = 0;
        while (map_buffer[map_index] != ' ') {

            if (index >= 5) {
                perror("permission flags parsing");
                return NULL;
            }

            permissions[index] = map_buffer[map_index];
            index++;
            map_index++;
        }
        permissions[index] = '\0';
        map_index++; // get off ' '

        // VM Offset (hex)

        index = 0;
        while (map_buffer[map_index] != ' ') {
            vm_offset[index] = map_buffer[map_index];
            index++;
            map_index++;
        }
        vm_offset[index] = '\0';

        map_index++; // get off ' '

        // Device info (format = fd:00)

        while(map_buffer[map_index] != ' ') {
            map_index++;
        }
        map_index++;

        // inode stuff

        while(map_buffer[map_index] != ' ') {
            map_index++;
        }
        map_index++;

        // maybe tab/whitespace here

        while((map_buffer[map_index] == ' ') || (map_buffer[map_index] == '\t')) {
            map_index++;
        }

        // filename (txt)

        index = 0;
        while((map_buffer[map_index]) != '\n' && (map_buffer[map_index] != '\0')) {
            file_name[index] = map_buffer[map_index];
            index++;
            map_index++;
        }
        file_name[index] = '\0';

        if (map_buffer[map_index] != '\0') {
            map_index++;
        }

        // parse pmaps output

        if (pmap_buffer != NULL) {
            // first iteration, skip the header

            int new_line_count = 0;

            while((new_line_count < 2) &&
                  (pmap_buffer[pmap_index] != '\0')) {
                
                if (pmap_buffer[pmap_index] == '\n') {
                    new_line_count++;
                }
                pmap_index++;
            }
        }

        while ((pmap_buffer[pmap_index] != '\n') && (pmap_buffer[pmap_index] != '\0')) {
            // read a line of pmap_buffer

            // start address

            while ((pmap_buffer[pmap_index] != ' ') && (pmap_buffer[pmap_index] != '\t')) {
                pmap_index++;
            }

            // pmap_index++;

            // skip over the spaces in between
            while (pmap_buffer[pmap_index] == ' ') {
                pmap_index++;
            }

            // VM Size (KiB)

            index = 0;
            while ((pmap_buffer[pmap_index] != ' ') && (pmap_buffer[pmap_index] != '\t')) {
                vm_size[index] = pmap_buffer[pmap_index];
                index++;
                pmap_index++;
            }
            vm_size[index] = '\0';
            
            // RSS

            // Private clean (bytes)
            // Private dirty (bytes)
            // Shared clean (bytes)
            // Shared dirty (bytes)

            // TODO
            // for now, reading until the end of the line
            while ((pmap_buffer[pmap_index] != '\n') &&
                (pmap_buffer[pmap_index] != '\0')) {
                pmap_index++;
            }

        }

        if (pmap_buffer[pmap_index] != '\0') {
            // get off \n

            pmap_index++;
        }


        // write the map aspects to the file
        char * to_write = (char*)malloc(sizeof(char) * FILE_SIZE);
        snprintf(to_write, FILE_SIZE, "%s %s %s %s %s %s\n", file_name, vm_start, vm_end, vm_size, permissions, vm_offset);

        strncat(written_buffer, to_write, FILE_SIZE - strlen(written_buffer));

        free(to_write);
        to_write = NULL;
    }

    return written_buffer;
} /* parse_map_info() */

/*char * get_memory_maps(int pid) {

    // filename (txt)
    // VM Start (hex)
    // VM End (hex)
    // VM Size (KiB)
    // Flags (permissions) (rwxp)
    // VM Offset (hex)
    // Private clean (bytes)
    // Private dirty (bytes)
    // Shared clean (bytes)
    // Shared dirty (bytes)


    char * to_write = parse_map_info(map_buffer, pmap_buffer, map_fd);

    close(map_fd);
    close(map_info_fd);
    close(pmap_info_fd);

    if (to_write == NULL) {
        free(map_buffer);
        map_buffer = NULL;

        free(pmap_buffer);
        pmap_buffer = NULL;

        perror("parse_map_info()");
        return NULL;
    }

    free(map_buffer);
    map_buffer = NULL;

    free(pmap_buffer);
    pmap_buffer = NULL;

    return to_write;
}*/


/*
 * Create a txt file in ./memory_maps/PID#_maps.txt
 * writes aspects:
 * filename (txt)
 * VM Start (hex)
 * VM End (hex)
 * VM Size (KiB)
 * Flags (permissions) (rwxp)
 * VM Offset (hex)
 * Private clean (bytes)
 * Private dirty (bytes)
 * Shared clean (bytes)
 * Shared dirty (bytes)
 * 
 * returns a char * that is the same as the PID#_map.txt
 * 
 * returns NULL if user doesn't have permissions 
 * OR
 * return NULL if process no longer exists
 */
char * get_memory_maps(int pid) {

    // open or create the PID_map.txt file and truncate it
    //char * file_path = (char *)malloc(sizeof(char) * FILE_SIZE);

    /*
     * write all the following aspects of the PID
     * including a <SPACE> between each of them
     * for easy future reading
     * example:
     * /usr/lib/bash HEX HEX 4.0 rwxp HEX 0 0 0 0
     */

    /*
     * write the PID original map file for reading.
     * Contains spaces between the following:
     * Address Range
     * Permissions
     * Offset
     * Device
     * Inode
     * File Name (Mapping Name)
     */

    // check if current user has permissions

    char file_p[128];
    snprintf(file_p, 128, "/proc/%d/maps", pid);
    FILE * file = fopen(file_p, "r");

    if (file == NULL) {
        return NULL;
    }

    char * map_buffer = (char*)malloc(sizeof(char) * FILE_SIZE);
    size_t bytes_read = fread(map_buffer, sizeof(char), FILE_SIZE, file);
    if (bytes_read) {
        
    }


    // int map_info_fd = open("/proc/%d/maps", O_RDONLY);
    // if (map_info_fd == -1) {
    //     close(map_info_fd);
    //     perror("open map_info");
    //     return NULL;
    // }

    // bytes_read = read(map_info_fd, map_buffer, FILE_SIZE);
    if (bytes_read == -1) {

        free(map_buffer);
        map_buffer = NULL;

        // close(map_info_fd);
        // close(pmap_info_fd);

        perror("read map_info");
        return NULL;
    }

    char command2[64];
    snprintf(command2, 64, "pmap -x %d > memory_maps/pmap_info.txt", pid);
    int sys = system(command2);
    if (sys) {
    }

    int pmap_info_fd = open("memory_maps/pmap_info.txt", O_RDONLY);
    if (pmap_info_fd == -1) {
        // close(map_info_fd);
        close(pmap_info_fd);
        perror("open pmap_info");
        return NULL;
    }

    // read the whole map and pmap file

    char * pmap_buffer = (char*)malloc(sizeof(char) * FILE_SIZE);
    bytes_read = read(pmap_info_fd, pmap_buffer, FILE_SIZE);
    if (bytes_read == -1) {

        free(map_buffer);
        map_buffer = NULL;

        free(pmap_buffer);
        pmap_buffer = NULL;

        // close(map_info_fd);
        close(pmap_info_fd);

        perror("read pmap_info");
        return NULL;
    }

    // filename (txt)
    // VM Start (hex)
    // VM End (hex)
    // VM Size (KiB)
    // Flags (permissions) (rwxp)
    // VM Offset (hex)
    // Private clean (bytes)
    // Private dirty (bytes)
    // Shared clean (bytes)
    // Shared dirty (bytes)

    char * to_write = parse_map_info(map_buffer, pmap_buffer);

    // close(map_info_fd);
    close(pmap_info_fd);

    if (to_write == NULL) {
        free(map_buffer);
        map_buffer = NULL;

        free(pmap_buffer);
        pmap_buffer = NULL;

        perror("parse_map_info()");
        return NULL;
    }

    free(map_buffer);
    map_buffer = NULL;

    free(pmap_buffer);
    pmap_buffer = NULL;

    return to_write;

} /* get_memory_maps() */

/*
 * returns an array of strings
 * the \n being the seperator of the line
 */
char ** parse_memory_maps(char * big_map, int &number_lines) {
    // filename (txt)
    // VM Start (hex)
    // VM End (hex)
    // VM Size (KiB)
    // Flags (permissions) (rwxp)
    // VM Offset (hex)

    // NOT DONE NOT DONE NOT DONE 
    // Private clean (bytes)
    // Private dirty (bytes)
    // Shared clean (bytes)
    // Shared dirty (bytes)

    int num_lines = 0;
    int index = 0;

    char ** parsed_map = (char**)malloc(sizeof(char*) * FILE_SIZE);

    while (big_map[index] != '\0') {

        char * line = (char*)malloc(sizeof(char) * FILE_SIZE);
        int line_index = 0;

        while (big_map[index] != '\n' &&
               big_map[index] != '\0') {
            line[line_index] = big_map[index];
            line_index++;
            index++;
        }
        line[line_index] = '\0';
        index++;

        parsed_map[num_lines] = line;
        num_lines++;
    }
    if (num_lines != 0) {
        number_lines = num_lines;
        return parsed_map;

    }
    else {
        return NULL;
    }
}

// filename (txt)

char * get_file_name(char * memory_map_line) {

    int index = 0;
    if (memory_map_line[0] == ' ') {
        char * none = strdup("/");
        return none;
        index++;
    }
    int new_index = 0;
    char * file_name = (char*)malloc(sizeof(char) * 256);
    while (memory_map_line[index] != ' ') {
        file_name[new_index] = memory_map_line[index];
        new_index++;
        index++;
    }
    file_name[new_index] = '\0';
    return file_name;
}

// VM Start (hex)
char * get_vm_start(char * memory_map_line) {

    if (strstr(memory_map_line, "vsyscall")) {
        return strdup("ffffffffff600000");
    }
    
    int index = 0;
    int space_count = 0;

    if (memory_map_line[0] == ' ') {
        index++;
        space_count++;
    }
    char * vm_start = (char *)malloc(sizeof(char) * 16);

    while (space_count < 1) {
        if (memory_map_line[index] == ' ') {
            space_count++;
        }
        index++;
    }

    if (strstr(memory_map_line, "(deleted)")) {
        index += 10;
    }
    
    int new_index = 0;
    while (memory_map_line[index] != ' ') {
        vm_start[new_index] = memory_map_line[index];
        new_index++;
        index++;
    }
    vm_start[new_index] = '\0';
    return vm_start;
}

// VM End (hex)
char * get_vm_end(char * memory_map_line) {
    
    int index = 0;
    int space_count = 0;

    if (strstr(memory_map_line, "vsyscall")) {
        return strdup("ffffffffff601000");
    }

    if (memory_map_line[0] == ' ') {
        index++;
        space_count++;
    }
    char * vm_end = (char *)malloc(sizeof(char) * 16);

    while (space_count < 2) {
        if (memory_map_line[index] == ' ') {
            space_count++;
        }
        index++;
    }
    
    int new_index = 0;
    while (memory_map_line[index] != ' ') {
        vm_end[new_index] = memory_map_line[index];
        new_index++;
        index++;
    }
    vm_end[new_index] = '\0';
    return vm_end;
}

// VM Size (KiB)
char * get_vm_size(char * memory_map_line) {
    
    int index = 0;
    int space_count = 0;

    if (strstr(memory_map_line, "vsyscall")) {
        return strdup("4096");
    }

    if (memory_map_line[0] == ' ') {
        index++;
        space_count++;
    }
    char * vm_size = (char *)malloc(sizeof(char) * 16);

    while (space_count < 3) {
        if (memory_map_line[index] == ' ') {
            space_count++;
        }
        index++;
    }
    
    int new_index = 0;
    while (memory_map_line[index] != ' ') {
        vm_size[new_index] = memory_map_line[index];
        new_index++;
        index++;
    }
    vm_size[new_index] = '\0';
    return vm_size;
}

// Flags (permissions) (rwxp)
char * get_flags(char * memory_map_line) {
    
    int index = 0;
    int space_count = 0;

    if (strstr(memory_map_line, "vsyscall")) {
        return strdup("--xp");
    }

    if (memory_map_line[0] == ' ') {
        index++;
        space_count++;
    }
    char * flags = (char *)malloc(sizeof(char) * 16);

    while (space_count < 4) {
        if (memory_map_line[index] == ' ') {
            space_count++;
        }
        index++;
    }
    
    int new_index = 0;
    while (memory_map_line[index] != ' ') {
        flags[new_index] = memory_map_line[index];
        new_index++;
        index++;
    }
    flags[new_index] = '\0';
    return flags;
}

// VM Offset (hex)
char * get_vm_offset(char * memory_map_line) {
    if (strstr(memory_map_line, "vsyscall")) {
        return strdup("00000000");
    }
    int index = 0;
    int space_count = 0;

    if (memory_map_line[0] == ' ') {
        index++;
        space_count++;
    }
    char * vm_offset = (char *)malloc(sizeof(char) * 16);

    while (space_count < 5) {
        if (memory_map_line[index] == ' ') {
            space_count++;
        }
        index++;
    }
    
    int new_index = 0;
    while (memory_map_line[index] != '\n' && memory_map_line[index] != '\0' && memory_map_line[index] != ' ') {
        vm_offset[new_index] = memory_map_line[index];
        new_index++;
        index++;
    }
    vm_offset[new_index] = '\0';
    return vm_offset;
}

// TODO

/*
 * Write to a file in the format
 * FD type Object (file/socket)
 * 
 * Gets all the open files from a PID using the proc file system
 * and writes to PID_open_files.txt
 * 
 * returns a char * the same format as the file with
 * \n between each entry
 * 
 * TODO JOSH THIS IS BROKEN - last fd printed in this DOESN'T exist
 * My guess is that the PID of the current process is causing a bug,
 * so the fd doesn't exist after the process closes
 */

char * list_open_file(int pid) {

    // get the whole proc/PID/fd

    char proc_fd_command[64];
    snprintf(proc_fd_command, 64, "ls -l /proc/%d/fd > memory_maps/%d_proc_fd.txt", pid, pid);
    int sys = system(proc_fd_command);
    if (sys) {
    }

    // gets all the fds and file/socket path
    // format: fd path

    char fd_command[128];
    snprintf(fd_command, 128, "cat memory_maps/%d_proc_fd.txt | awk '{print $9 \" \" $11}' > memory_maps/%d_fds.txt", pid, pid);
    sys = system(fd_command);
    if (sys) {
    }

    char file_path[64];
    snprintf(file_path, 64, "memory_maps/%d_fds.txt", pid);
    int fds_fd = open(file_path, O_RDONLY);
    if (fds_fd == -1) {
        close(fds_fd);
        perror("open fds.txt");
        return NULL;
    }

    char fds_buf[FILE_SIZE];
    size_t bytes_read = read(fds_fd, fds_buf, FILE_SIZE);
    close(fds_fd);
    if (bytes_read == -1) {
        perror("read fds.txt");
        return NULL;
    }

    // iterate over the file

    int fds_index = 0;

    while ((fds_buf[fds_index] == '\n') ||
           (fds_buf[fds_index] == '\0') ||
           (fds_buf[fds_index] == ' ')) {
        // skip over the first line (blank or EOF)

        fds_index++;
    }

    char * to_write = (char *)malloc(sizeof(char) * FILE_SIZE);
    for (int i = 0; i < FILE_SIZE; i++) {
        to_write[i] = '\0';
    }

    while ((fds_buf[fds_index] != '\0')) {
        // iterate a line at a time

        char read_fd[8];
        int read_index = 0;

        while(fds_buf[fds_index] != ' ') {
            // get the fd

            read_fd[read_index] = fds_buf[fds_index];
            read_index++;
            fds_index++;
        }
        fds_index++;
        read_fd[read_index] = '\0';

        int fd = atoi(read_fd);

        /*struct rlimit rlim;

        if (fd > 100000) {
            break;
        }*/

        char socket_file[512];
        read_index = 0;

        while ((fds_buf[fds_index] != '\n') &&
                (fds_buf[fds_index] != '\0')) {
            // get the socket/file path

            socket_file[read_index] = fds_buf[fds_index];
            read_index++;
            fds_index++;
        }
        fds_index++;
        socket_file[read_index] = '\0';

        // determine if it's a socket or a file

        char type[32];
        if (strstr(type, "socket") != NULL) {
            snprintf(type, 8, "socket");
            type[6] = '\0';
        }
        else {
            snprintf(type, 6, "file");
            type[4] = '\0';
        }

        // append to to_write
        // fd type socket/file path

        char fd_type_path[FILE_SIZE];
        snprintf(fd_type_path, FILE_SIZE, "%d %s %s\n", fd, type, socket_file);

        strcat(to_write, fd_type_path);
    }

    // write to the file

    char open_file_path[64];
    snprintf(open_file_path, 64,"memory_maps/%d_open_files.txt", pid);

    int pid_list_fd = open(open_file_path, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (pid_list_fd == -1) {
        free(to_write);
        to_write = NULL;
        perror("open open_files.txt");
        return NULL;
    }
    
    size_t bytes_written = write(pid_list_fd, to_write, strlen(to_write));
    close(pid_list_fd);
    
    return to_write;
}

// return number of fds / files open
int parse_list(char * open_files) {

    int index = 0;
    int num_open = 0;

    if (open_files == NULL) {
        return 0;
    }
    g_open_fds = (char**)malloc(sizeof(char*) * 128);
    g_open_types = (char**)malloc(sizeof(char*) * 128);
    g_open_paths = (char**)malloc(sizeof(char*) * 128);

    while (open_files[index] != '\0') {

        while (open_files[index] != '\n' && open_files[index] != '\0') {

            // fd
            char * new_fd = (char*)malloc(sizeof(char) * 64);
            int new_index = 0;
            while (open_files[index] != ' ') {
                new_fd[new_index++] = open_files[index++];
            }
            index++;
            new_fd[new_index] = '\0';

            // type
            char * new_type = (char*)malloc(sizeof(char) * 64);
            new_index = 0;
            while (open_files[index] != ' ') {
                new_type[new_index++] = open_files[index++];
            }
            index++;
            new_type[new_index] = '\0';

            // path
            char * new_path = (char*)malloc(sizeof(char) * FILE_SIZE);
            new_index = 0;
            while (open_files[index] != '\n') {
                new_path[new_index++] = open_files[index++];
            }
            new_path[new_index] = '\0';

            if (open_files[index] != '\0') {
                index++;
            }

            g_open_fds[num_open] = new_fd;
            g_open_types[num_open] = new_type;
            g_open_paths[num_open] = new_path;

            num_open++;
        }
    }
    return num_open;
}

/*int * get_all_processes() {
    DIR * proc_dir;
    struct dirent * entry;

    int * pid_array = (int*)malloc(sizeof(int) * FILE_SIZE * 4);
    int num_pids = 0;

    proc_dir = opendir("/proc");
    if (proc_dir == NULL) {
        perror("opendir");
        free(pid_array);
        pid_array = NULL;
        return NULL;
    }

    int smallest_pid = 0;

    // iterate over pids in /proc directory
    while ((entry = readdir(proc_dir)) != NULL) {
        // if directory is a number

        if (entry->d_type == DT_DIR && (atoi(entry->d_name) != 0)) {

            if (atoi(entry->d_name) > smallest_pid) {
                // add the PID to the array
                pid_array[num_pids] = atoi(entry->d_name);
                num_pids++;
            }
        }
    }
    printf("num pids: %d", num_pids);
    closedir(proc_dir);

    if (num_pids > 0) {
        g_num_pids = num_pids;
        return pid_array;
    }
    else {
        free(pid_array);
        pid_array = NULL;
        return NULL;
    }
}*/

/*
 * Load all process info into a file.
 * The get_active_processes() function will
 * use this in order to find the processes
 * that are active. 
 * 
 * This MUST be called before get_active_processes().
 * 
 * returns a malloced array that must be freed after use
 * of all the process PIDs
 * 
 * if there are no process/PIDs or there is an error,
 * return NULL
 * 
 * g_num_pids contains the number of current processes/PIDs running
 */

int * get_all_processes() {

    // get all ps -faux information

    //char all_command[] = "ps -a > memory_maps/all_processes.txt";
    //system(all_command);

    // get all the PIDs

    char all_pids_command[] = "ps -a | awk '{print $1}' > memory_maps/all_pids.txt";
    int sys = system(all_pids_command);
    if (sys) {
    }

    int all_pids_fd = open("memory_maps/all_pids.txt", O_RDONLY);
    if (all_pids_fd == -1) {
        close(all_pids_fd);
        perror("open in get_all_processes()");
        return NULL;
    }

    // get an int array containing all the PIDs

    char pid_buf[FILE_SIZE * 4];
    size_t bytes_read = read(all_pids_fd, pid_buf, FILE_SIZE * 4);
    close(all_pids_fd);
    if (bytes_read == -1) {
        perror("read all_pids.txt");
        return NULL;
    }

    int buf_index = 0;

    // iterate over the first line (PID\n) of the file

    buf_index += 4;

    int * pid_array = (int *)malloc(sizeof(int) * FILE_SIZE * 2);
    int num_pids = 0;

    while (pid_buf[buf_index] != '\0') {
        // iterate until reach EOF

        char line[16] = {'\0'};
        int line_index = 0;

        // iterate over a line at a time

        while ((pid_buf[buf_index] != '\n') &&
               (pid_buf[buf_index] != '\0')) {

            line[line_index] = pid_buf[buf_index];
            line_index++;
            buf_index++;
        }
        line[line_index] = '\0';

        if (pid_buf[buf_index] != '\0') {
            // get off \n

            buf_index++;
        }

        char test_path[64];
        snprintf(test_path, 64, "/proc/%s", line);

        FILE * test_file = fopen(test_path, "r");
        if (test_file != NULL) {
            pid_array[num_pids] = atoi(line);
            num_pids++;
            fclose(test_file);
        }
    }

    if (num_pids > 0) {
        g_num_pids = num_pids;
        return pid_array;
    }
    else {
        return NULL;
    }
}

int * get_children_pids(int parent_pid, int &num_children) {

    int children[FILE_SIZE];
    int childCount = 0;
    num_children = 0;

    char path[100];
    snprintf(path, sizeof(path), "/proc/%d/task/%d/children", parent_pid, parent_pid);

    FILE * file;
    file = fopen(path, "r");
    if (file == NULL) {
        //perror("open child file");
        return NULL;
    }

    char line[256];
    while (fgets(line, 256, file) != NULL) {
        int child_pid = atoi(line);
        if (child_pid != parent_pid) {
            children[childCount++] = child_pid;
        }
    }

    fclose(file);

    int * child_pids = (int*)malloc(childCount * sizeof(int));
    memcpy(child_pids, children, childCount * sizeof(int));
    num_children = childCount;

    if (num_children != 0) {
        return child_pids;
    }
    else {
        return NULL;
    }
}

/*
 * returns an int * that must be free'd
 * returns an array containing all PIDs of 
 * processes that are running
 * 
 * MUST be run after get_all_processes() function is called
 */

int * get_active_processes() {

    char get_active_command[128];
    snprintf(get_active_command, 128, "ps -au | awk '{print $2 \" \" $8}' | grep R > memory_maps/active_processes.txt");
    int sys = system(get_active_command);
    if (sys) {
    }

    int active_process_fd = open("memory_maps/active_processes.txt", O_RDONLY);
    if (active_process_fd == -1) {
        close(active_process_fd);
        perror("open in active_processes()");
        return NULL;
    }

    // get an int array containing all the active process PIDs

    char pid_buf[FILE_SIZE];
    size_t bytes_read = read(active_process_fd, pid_buf, FILE_SIZE);
    close(active_process_fd);
    if (bytes_read == -1) {
        perror("read active_processes.txt");
        return NULL;
    }

    int buf_index = 0;

    int * pid_array = (int *)malloc(sizeof(int) * FILE_SIZE);
    int num_pids = 0;

    while (pid_buf[buf_index] != '\0') {
        // iterate until reach EOF

        char line[16];
        int line_index = 0;

        // iterate over a line at a time

        while ((pid_buf[buf_index] != ' ') &&
               (pid_buf[buf_index] != '\0')) {

            line[line_index] = pid_buf[buf_index];
            line_index++;
            buf_index++;
        }
        line[line_index] = '\0';

        while ((pid_buf[buf_index] != '\n') &&
               (pid_buf[buf_index] != '\0')) {

            buf_index++;
        }
        line[line_index] = '\0';

        if (pid_buf[buf_index] != '\0') {
            // get off \n

            buf_index++;
        }

        pid_array[num_pids] = atoi(line);
        num_pids++;
    }

    if (num_pids > 0) {
        g_num_active_pids = num_pids - 1;

        return pid_array;
    }
    else {
        return NULL;
    }
}

/*
 * returns an int * that must be free'd
 * returns an array containing all PIDs of 
 * processes that are of the current user
 * 
 * if there are no processes, returns NULL pointer
 */

int * get_my_processes() {

    char my_process_command[] = "ps -u | awk '{print $2}' > memory_maps/my_processes.txt";
    int sys = system(my_process_command);
    if (sys) {
    }

    int my_processes_fd = open("memory_maps/my_processes.txt", O_RDONLY);
    if (my_processes_fd == -1) {
        close(my_processes_fd);
        perror("open in get_my_processes()");
        return NULL;
    }

    // get an int array containing all the user's PIDs

    char pid_buf[FILE_SIZE];
    size_t bytes_read = read(my_processes_fd, pid_buf, FILE_SIZE);
    close(my_processes_fd);
    if (bytes_read == -1) {
        perror("read my_processes.txt");
        return NULL;
    }

    int buf_index = 0;

    // iterate over the first line (PID\n) of the file

    buf_index += 4;

    int * pid_array = (int *)malloc(sizeof(int) * FILE_SIZE);
    int num_pids = 0;

    while (pid_buf[buf_index] != '\0') {
        // iterate until reach EOF

        char line[16];
        int line_index = 0;

        // iterate over a line at a time

        while ((pid_buf[buf_index] != '\n') &&
               (pid_buf[buf_index] != '\0')) {

            line[line_index] = pid_buf[buf_index];
            line_index++;
            buf_index++;
        }
        line[line_index] = '\0';

        if (pid_buf[buf_index] != '\0') {
            // get off \n

            buf_index++;
        }

        pid_array[num_pids] = atoi(line);
        num_pids++;
    }

    if (num_pids > 0) {
        g_num_my_pids = num_pids;
        return pid_array;
    }
    else {
        return NULL;
    }

}

/*
 * returns all processes 
 * 
 * PID
 * #
 * #
 * #
 * 
 */

int * load_processes() {

    // All Processes

    char all_command[64];
    snprintf(all_command, 64, "ps -faux | cut -c 10-16 > memory_maps/all_processes.txt");
    int sys = system(all_command);
    if (sys) {
    }

    // My Processes

    char my_command[64];
    snprintf(my_command, 64, "ps -u | cut -c 10-16 > memory_maps/my_processes.txt");
    sys = system(my_command);
    if (sys) {
    }

    // Active Processes

    char status_command[64];
    snprintf(status_command, 64, "ps -faux | cut -c 50-53 > memory_maps/process_status.txt");
    sys = system(status_command);
    if (sys) {
    }

    int status_process_fd = open("memory_maps/process_status.txt", O_RDONLY);
    if (status_process_fd == -1) {
        close(status_process_fd);
        perror("open in load_process()");
        return NULL;
    }

    char pid_buffer[FILE_SIZE];
    size_t bytes_read = read(status_process_fd, pid_buffer, FILE_SIZE);
    close(status_process_fd);
    if (bytes_read == -1) {
        perror("read process_status.txt");
        return NULL;
    }

    // iterate over the status_processes file for all the PIDs, in order to
    // find the ones that are actually running
    // and put them into running_process.txt

    int buf_index = 0;
    int iteration = 0;
    int running_iteration[FILE_SIZE];
    int num_running_processes = 0;

    // iterate over STAT \n

    while (pid_buffer[buf_index] != '\n') {
        buf_index++;
    }
    buf_index++;

    while (pid_buffer[buf_index] != '\0') {
        // iterate over the whole buf_index

        char line[5];
        int line_index = 0;

        while ((pid_buffer[buf_index] != '\n') &&
               (pid_buffer[buf_index] != '\0') &&
               (line_index < 5)) {
            // iterate over each line (containing the status)

            line[line_index] = pid_buffer[buf_index];
            line_index++;
            buf_index++;
        }

        line[line_index] = '\0';

        if (strchr(line, 'R') != NULL) {
            // running process

            running_iteration[num_running_processes] = iteration;
            num_running_processes++;
        }

        if (pid_buffer[buf_index] == '\n') {
            buf_index++;
        }
        iteration++;
    }

    int all_process_fd = open("memory_maps/all_processes.txt", O_RDONLY);
    if (all_process_fd == -1) {
        close(all_process_fd);

        perror("open all_process_fd");
        return NULL;
    }

    char all_process_buffer[FILE_SIZE * 4];
    size_t bytes_read2 = read(all_process_fd, all_process_buffer, FILE_SIZE * 4);
    close(all_process_fd);
    int ap_index = 0;

    while (all_process_buffer[ap_index] != '\n') {
        // skip over "PID"

        ap_index++;
    }
    ap_index++;

    int all_process_iteration = 0;

    int * running_processes = (int *)malloc(sizeof(int) * 512);

    for (int i = 0; i < num_running_processes; i++) {
        // iterate over each of the previously found running processes

        while (running_iteration[i] != all_process_iteration) {
            // find the correct process and get its PID

            while ((all_process_buffer[ap_index] != '\n') &&
                   (all_process_buffer[ap_index] != '\0')) {
                ap_index++;
            }

            if (all_process_buffer[ap_index] == '\0') {
                perror("iterating over all_processes.txt");
                return NULL;
            }
            else {
                ap_index++;
            }
            all_process_iteration++;
        }

        // correct all_process iteration, now grab the PID

        char running_pid[16];
        int r_pid = 0;
        while ((all_process_buffer[ap_index] != '\n') &&
               (all_process_buffer[ap_index] != '\0')) {
            running_pid[r_pid] = all_process_buffer[ap_index];
            r_pid++;
            ap_index++;
        }
        ap_index++;
        //running_pid[r_pid] = '\0';
        running_processes[i] = atoi(running_pid);
    }

    // write the running_process PIDs to running_process.txt

    int running_fd = open("memory_maps/running_process.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (running_fd == -1) {
        free(running_processes);
        close(running_fd);
        perror("open running_process.txt");
        return NULL;
    }

    char to_write[FILE_SIZE * 4];
    for (int i = 0; i < num_running_processes; i++) {
        snprintf(to_write, strlen(to_write) + 1, "%d", running_processes[i]);

        if (i != num_running_processes - 1) {
            snprintf(to_write, FILE_SIZE * 4, " ");
        }
    }

    size_t bytes_written = write(running_fd, to_write, strlen(to_write));
    free(running_processes);
    close(running_fd);

    if (bytes_written == -1) {
        perror("write");
        return NULL;
    }

    return running_processes;
}

/*
 * returns the current user as char *
 */
char * get_current_user() {
    int sys = system("whoami > memory_maps/whoami.txt");
    if (sys) {
    }

    int fd = open("memory_maps/whoami.txt", O_RDONLY);
    if (fd == -1) {
        close(fd);
        perror("open whoami");
        return NULL;
    }

    char * user = (char*)malloc(sizeof(char) * 128);
    size_t bytes_read = read(fd, user, 128);
    close(fd);
    if (bytes_read == -1) {
        perror("read");
        return NULL;
    }

    return user;
} /* get_current_user() */

char * get_process_vm_size(int pid) {

    char command[128];
    snprintf(command, 128, "cat /proc/%d/status | grep VmSize | awk '{print $2}' > memory_maps/%d_vm_size.txt", pid, pid);
    int sys = system(command);
    if (sys) {
    }

    char * vm_size = (char*)malloc(sizeof(char) * 128);
    char file_path[128];
    snprintf(file_path, 128, "memory_maps/%d_vm_size.txt", pid);
    
    int fd = open(file_path, O_RDONLY);
    size_t bytes_read = read(fd, vm_size, 128);
    close(fd);
    if (bytes_read == -1) {
        perror("read");
        return NULL;
    }
    vm_size[bytes_read - 1] = '\0';
    strcat(vm_size, " MiB");

    return vm_size;

}

/*
 * Returns shared mem as kB
 */

char * get_shared_memory(int pid) {

    char command[128];
    //snprintf(command, 128, "cat /proc/%d/smaps | awk '/Shared/ {sum += $2} END {print sum}' > memory_maps/%d_shared.txt", pid, pid);
    snprintf(command, 128, "pmap -x %d | grep shared | awk '{sum += $3} END {print sum}' > memory_maps/%d_shared.txt", pid, pid);
    int sys = system(command);
    if (sys) {
    }

    char * shared = (char*)malloc(sizeof(char) * 128);
    char file_path[128];
    snprintf(file_path, 128, "memory_maps/%d_shared.txt", pid);
    
    int fd = open(file_path, O_RDONLY);
    size_t bytes_read = read(fd, shared, 128);
    close(fd);
    if (bytes_read == -1) {
        perror("read");
        return NULL;
    }
    shared[bytes_read] = '\0';

    if (atoi(shared) == 0) {
        shared[0] = '0';
        shared[1] = '\0';
    }

    return shared;
} /* get_shared_memory() */

char * get_cpu_time(int pid) {
    
    char command[128];
    snprintf(command, 128, "ps -p %d -o time= | awk '{print $1}' > memory_maps/%d_time.txt", pid, pid);
    int sys = system(command);
    if (sys) {
    }

    char * time = (char*)malloc(sizeof(char) * 128);
    char file_path[128];
    snprintf(file_path, 128, "memory_maps/%d_time.txt", pid);
    
    int fd = open(file_path, O_RDONLY);
    size_t bytes_read = read(fd, time, 128);
    close(fd);
    if (bytes_read == -1) {
        perror("read");
        return NULL;
    }

    time[bytes_read] = '\0';

    return time;

}

/*
 * returns NULL if doesn't exist
 */
char * get_start_date_time(int pid) {
    
    char test_path[64];
    snprintf(test_path, 64, "/proc/%d", pid);
    FILE * test_file = fopen(test_path, "r");
    if (test_file == NULL) {
        return NULL;
    }
    fclose(test_file);

    char command[128];
    snprintf(command, 128, "ls -ld /proc/%d | awk '{print $6 \" \" $7 \" \" $8}' > memory_maps/%d_ld.txt", pid, pid);
    int sys = system(command);
    if (sys) {
    }
    
    char file_path[64];
    snprintf(file_path, 64, "memory_maps/%d_ld.txt", pid);

    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        perror("ld.txt");
        return NULL;
    }

    char * date_time = (char*)malloc(sizeof(char) * 128);
    size_t bytes_read = read(fd, date_time, 128);
    close(fd);

    if (bytes_read == -1) {
        perror("read");
        return NULL;
    }
    date_time[bytes_read] = '\0';

    return date_time;
}