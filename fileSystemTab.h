#ifndef FILESYSTEMTAB_H
#define FILESYSTEMTAB_H

#include <QWidget>
#include "process.h"
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>


QWidget* createFileSystemTab();
char * get_all_file_system_info();
extern int g_num_mounts;
extern char ** g_device_path;
extern char ** g_directory_path;
extern char ** g_type;
extern char ** g_total;
extern char ** g_available;
extern char ** g_used;
extern char ** g_used_percent;



#endif // FILESYSTEMTAB_H