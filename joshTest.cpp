#include "process.h"
#include <stdio.h>
#include <stdlib.h>
//#include "system_info.h"

/* add includes */
/* create .h file for all the other .c files */
/* helper functions in a seperate file too */
char g_process_file[FILE_SIZE] = {'\0'};

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Error need one input!\n");
        exit(0);
        return 0;
    }

    int pid = atoi(argv[1]);
    // 3246685

    int * pids = get_all_processes();

    char * name = get_process_name(pid);
    free_and_nullify((void **)&name);

    char * status = get_status(pid);
    free_and_nullify((void **)&status);

    char * info = get_cpu_info(pid);
    free_and_nullify((void **)&info);

    char * mem = get_memory(pid);
    free_and_nullify((void **)&mem);

    int childs = 0;
    int * children = get_children_pids(3246685, childs);
    free_and_nullify((void **)&children);

    char * date = get_start_date_time(pid);
    free_and_nullify((void **)&date);


    char * big_map = get_memory_maps(pid);
    int num_lines = 0;
    char ** parsed_map = parse_memory_maps(big_map, num_lines);
    
    for (int i = 0; i < num_lines; i++) {
        char * file_name = get_file_name(parsed_map[i]);
        char * vm_start = get_vm_start(parsed_map[i]);
        char * vm_end = get_vm_end(parsed_map[i]);
        char * vm_size = get_vm_size(parsed_map[i]);
        char * flags = get_flags(parsed_map[i]);
        char * vm_offset = get_vm_offset(parsed_map[i]);

        printf("%s %s %s %s %s %s\n", file_name, vm_start, vm_end, vm_size, flags, vm_offset);
        free_and_nullify((void **)&file_name);
        free_and_nullify((void **)&vm_start);
        free_and_nullify((void **)&vm_end);
        free_and_nullify((void **)&vm_size);
        free_and_nullify((void **)&flags);
        free_and_nullify((void **)&vm_offset);
    }

    // printf("num: %d\n", g_num_pids);
    // for (int i = 0; i < 25; i++) {
    //     char * name = get_process_name(pids[0]);
    //     printf("%d %s\n", pids[i], name);
    //     free_and_nullify((void **)&name);
    // }
    // char * name = get_process_name(pid);
    // char * usage = get_cpu_info(pid);
    // printf("%d %s %s", pid, name, usage);


    // int num_files = 0;
    // char * big_list_open_files = list_open_file(pid);
    // printf("%s\n", big_list_open_files);

    // int num_open = parse_list(big_list_open_files);
    // printf("%d\n", num_open);


    // char * shared = get_shared_memory(pid);
    // printf("shared: %s\n", shared);
    
    // char * date_time = get_start_date_time(pid);
    // printf("%s\n", date_time);

    // char * mem = get_memory(pid);
    // printf("%s\n", mem);

    // char * maps = get_memory_maps(pid);

    // char * file_name = get_file_name(maps);
    // char * vm_start = get_vm_start(maps);
    // char * vm_end = get_vm_end(maps);
    // char * vm_size = get_vm_size(maps);
    // char * flags = get_flags(maps);
    // char * vm_offset = get_vm_offset(maps);
    // printf("%s %s %s %s %s %s\n", file_name, vm_start, vm_end, vm_size, flags, vm_offset);
    // free_and_nullify((void **)&file_name);
    // free_and_nullify((void **)&vm_start);
    // free_and_nullify((void **)&vm_end);
    // free_and_nullify((void **)&vm_size);
    // free_and_nullify((void **)&flags);
    // free_and_nullify((void **)&vm_offset);

    // char * map = get_memory_maps(pid);

    // for (int i = 0; i < g_num_pids; i++) {
    //     int num_children = 0;
    //     int * children = get_children_pids(pids[i], num_children);

    //     printf("Parent %d\n", pids[i]);

    //     for (int j = 0; j < num_children; j++) {
    //         printf("Child %d\n", children[j]);
    //     }

    //     free_and_nullify((void **)&children);
    // }

    // int * active_pids = get_active_processes();
    // for (int i = 0; i < g_num_active_pids; i++) {
    //     printf("%d\n", active_pids[i]);
    // }

    // for (int i = 0; i < g_num_pids; i++) {
    //     char * name = get_process_name(pids[i]);
    //     printf("%d: %s\n", pids[i], name);
    //     free(name);
    // }


    // for (int i = 0; i < num_lines; i++) {
    //     printf("%s\n", parsed_map[i]);
    // }

    // for (int i = 0; i < g_num_pids; i++) {
    //     char * big_map = get_memory_maps(pids[i]);

    //     if (big_map == NULL) {
    //         continue;
    //     }

    //     int num_lines = 0;
    //     char ** parsed_map = parse_memory_maps(big_map, num_lines);
    //     printf("num lines: %d\n", num_lines);

    //     char * file_name = get_file_name(parsed_map[i]);
    //     char * vm_start = get_vm_start(parsed_map[i]);
    //     char * vm_end = get_vm_end(parsed_map[i]);
    //     char * vm_size = get_vm_size(parsed_map[i]);
    //     char * flags = get_flags(parsed_map[i]);
    //     char * vm_offset = get_vm_offset(parsed_map[i]);
    //     printf("%s %s %s %s %s %s\n", file_name, vm_start, vm_end, vm_size, flags, vm_offset);
    //     free_and_nullify((void **)&file_name);
    //     free_and_nullify((void **)&vm_start);
    //     free_and_nullify((void **)&vm_end);
    //     free_and_nullify((void **)&vm_size);
    //     free_and_nullify((void **)&flags);
    //     free_and_nullify((void **)&vm_offset);
    //     free_and_nullify((void **)&big_map);

    //     for (int j = 0; j < num_lines; j++) {
    //         free_and_nullify((void **)&parsed_map[j]);
    //     }
    //     free_and_nullify((void **)&parsed_map);
    // }


    

    // Copy file to g_shared_file here
/*
    // testing get_network_title()
    char *network_title = get_network_title();
    printf("Network title: %s\n", network_title);
    free_and_nullify((void **)&network_title);

    // testing get_kernel_version()
    char *kernel_version = get_kernel_version();
    printf("Kernel version: %s\n", kernel_version);
    free_and_nullify((void **)&kernel_version);

    // testing get_processor_version()
    char *processor_version = get_processor_version();
    printf("Processor version: %s\n", processor_version);
    free_and_nullify((void **)&processor_version);

    // testing get_amount_memory()
    char *amount_memory = get_amount_memory();
    printf("Memory: %s\n", amount_memory);
    free_and_nullify((void **)&amount_memory);

    // testing get_disk_storage()
    char *disk_storage = get_disk_storage();
    printf("Available disk space: %s\n", disk_storage);
    free_and_nullify((void **)&disk_storage);

    // testing get_os_name()
    char *os_name = get_os_name();
    printf("OS name: %s\n", os_name);
    free_and_nullify((void **)&os_name);

    // testing get_os_release_version()
    char *os_version = get_os_release_version();
    printf("OS release version: %s\n", os_version);
    free_and_nullify((void **)&os_version);

    // testing get_process_name()
    char * process_name = get_process_name(pid);
    printf("Name: %s\n", process_name);
    free_and_nullify((void **)&process_name);

    // testing get_status()
    char * process_status = get_status(pid);
    printf("Status: %s\n", process_status);
    free_and_nullify((void **)&process_status);

    // testing get_cpu_info()
    char * cpu_info = get_cpu_info(pid);
    printf("CPU Usage: %s%%\n", cpu_info);
    free_and_nullify((void **)&cpu_info);

    printf("\n");

    // testing get_memory()
    char * process_mem_info = get_memory(pid);
    printf("Memory: %s\n", process_mem_info);
    free_and_nullify((void **)&process_mem_info);

    char * mem_maps = get_memory_maps(pid);
    printf("Memory maps:\n%s\n", mem_maps);
    free_and_nullify((void **)&mem_maps);

    int * all_pids = get_all_processes();
    printf("Number of PIDs: %d\n", g_num_pids);
    free_and_nullify((void **)&all_pids);

    int * my_pids = get_my_processes();
    printf("Number of my PIDs: %d\n", g_num_my_pids);
    free_and_nullify((void **)&my_pids);
*/
    /*for (int i = 0; i < g_num_my_pids; i++) {
        printf("%d ", my_pids[i]);
    }*/
/*
    char * open_files = list_open_file(pid);
    printf("Open files:\n%s", open_files);
    free_and_nullify((void **)&open_files);

    char * vm_size = get_process_vm_size(pid);
    printf("VM Size: %s\n", vm_size);
    free_and_nullify((void **)&vm_size);

    char * shared_size = get_shared_memory(pid);
    printf("Shared: %s\n", shared_size);
    free_and_nullify((void **)&shared_size);

    char * time = get_cpu_time(pid);
    printf("CPU Time: %s\n", time);
    free_and_nullify((void **)&time);

    char * date_time = get_start_date_time(pid);
    printf("Start: %s\n", date_time);
    free_and_nullify((void **)&date_time);

    char * file_system = get_all_file_system_info();
    printf("Num mounts: %d\n", g_num_mounts);
    printf("File System:\n%s\n", file_system);
    free_and_nullify((void **)&file_system);
*/
    /*for (int i = 0; i < g_num_mounts; i++) {
        printf("%s\n", g_device_path[i]);
    }*/



    /* 
    ================================================
    manual tests for stop, continue, and kill processes
    ================================================
    */

    printf("\n");

    // to test:
    // 1. run "test_background &"
    // 2. note pid and change test_process below
    // 3. in terminal, run: "run PID#"
    /*int test_process = 111266;

    // testing stop_process()
    int stop_success = stop_process(test_process);
    if (stop_success) {
        printf("Stopped %d\n", test_process);
    }
    else {
        printf("Couldn't stop %d\n", test_process);
    }

    // testing continue_process()
    int continue_success = continue_process(test_process);
    if (continue_success) {
        printf("Continued %d\n", test_process);
    }
    else {
        printf("Couldn't continue %d\n", test_process);
    }
    // testing kill_process()
    int kill_success = kill_process(test_process);
    if (kill_success) {
        printf("Killed %d\n", test_process);
    }
    else {
        printf("Couldn't kill %d\n", test_process);
    }*/

    return 0;
}