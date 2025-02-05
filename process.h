#define FILE_SIZE (8192)

// helper functions to get pid info
extern "C" {
    char * get_process_name(int pid);
    char * get_status(int pid);
    char * get_cpu_info(int pid);
    //char * get_cpu_info(int pid);
    char * get_memory(int pid);
    char * get_id(int pid);

    // process functions
    int stop_process(int pid);
    int continue_process(int pid);
    int kill_process(int pid);
    char * get_memory_maps(int pid);
    char * list_open_file(int pid);

    int * load_processes(); // do not use

    int * get_children_pids(int parent_pid, int &num_children);

    // int * get_all_processes();
    int* get_all_processes();
    int * get_active_processes(); // not done
    int * get_my_processes();
    char * get_process_vm_size(int pid);
    char * get_shared_memory(int pid);
    char * get_cpu_time(int pid);
    char * get_start_date_time(int pid);
    //char ** parse_list(char * open_files, int &number_files);
    int parse_list(char * open_files);
    char * get_current_user();

    // use get_memory_maps()
    // parse_memory_maps - number_lines will contain the number of lines
    // iterate of number lines and get the info you need, passing in each line
    char ** parse_memory_maps(char * big_map, int &number_lines);

    char * get_file_name(char * memory_map_line);
    char * get_vm_start(char * memory_map_line);
    char * get_vm_end(char * memory_map_line);
    char * get_vm_size(char * memory_map_line);
    char * get_flags(char * memory_map_line);
    char * get_vm_offset(char * memory_map_line);

    // writes to the g_process_file
    void write_process_info();

    extern int g_num_pids;
    extern int g_num_my_pids;
    extern int g_num_active_pids;

    extern char ** g_open_fds;
    extern char ** g_open_types;
    extern char ** g_open_paths;

    void free_and_nullify(void ** pointer);
}
