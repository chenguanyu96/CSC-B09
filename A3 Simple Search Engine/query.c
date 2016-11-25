#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include "freq_list.h"
#include "worker.h"



int main(int argc, char **argv) {

    char ch;
    char path[PATHLENGTH];
    char *startdir = ".";

    while((ch = getopt(argc, argv, "d:")) != -1) {
        switch (ch) {
            case 'd':
                startdir = optarg;
                break;
            default:
                fprintf(stderr, "Usage: queryone [-d DIRECTORY_NAME]\n");
                exit(1);
        }
    }
    // Open the directory provided by the user (or current working directory)
    struct dirent *entry;
    int num_of_subdir = 0;
    DIR *dirp;
    if((dirp = opendir(startdir)) == NULL) {
        perror("opendir");
        exit(1);
    }
    while ((entry = readdir(dirp))) {
        if (entry->d_type == DT_DIR) {
            if(strcmp(entry->d_name, ".") == 0 ||
               strcmp(entry->d_name, "..") == 0 ||
               strcmp(entry->d_name, ".svn") == 0){
                continue;
            }
            num_of_subdir++;
        }
    }
    closedir(dirp);
    if((dirp = opendir(startdir)) == NULL) {
        perror("opendir");
        exit(1);
    }


    /* For each entry in the directory, eliminate . and .., and check
     * to make sure that the entry is a directory, then call run_worker
     * to process the index file contained in the directory.
     * Note that this implementation of the query engine iterates
     * sequentially through the directories, and will expect to read
     * a word from standard input for each index it checks.
     */

    struct dirent *dp;
    FreqRecord *master_freq_array = malloc((MAXRECORDS+1)*sizeof(FreqRecord));
    if (master_freq_array == NULL) {
        perror("ERROR: Malloc failed");
        exit(1);
    }
    int *pc_pipe_array[num_of_subdir];
    int *cp_pipe_array[num_of_subdir];
    int process_num = 0;
    int overload = 0;
    int masterfreq_index = 0;
    int nbytes;
    char *buf = malloc(MAXWORD);
    if (buf == NULL) {
        perror("ERROR: Malloc failed");
        exit(1);
    }
    pid_t pid;
    while((nbytes = read(STDIN_FILENO, buf, MAXWORD)) > 0) {
        while((dp = readdir(dirp)) != NULL) {

            if(strcmp(dp->d_name, ".") == 0 ||
               strcmp(dp->d_name, "..") == 0 ||
               strcmp(dp->d_name, ".svn") == 0){
                continue;
            }
            strncpy(path, startdir, PATHLENGTH);
            strncat(path, "/", PATHLENGTH - strlen(path) - 1);
            strncat(path, dp->d_name, PATHLENGTH - strlen(path) - 1);

            struct stat sbuf;
            if(stat(path, &sbuf) == -1) {
                //This should only fail if we got the path wrong
                // or we don't have permissions on this entry.
                perror("ERROR: Stat");
                exit(1);
            } 
            
            // Only call run_worker if it is a directory
            // Otherwise ignore it.
            if(S_ISDIR(sbuf.st_mode)) {
                int pc_pipe[2];
                int cp_pipe[2];
                pc_pipe_array[process_num] = pc_pipe;
                cp_pipe_array[process_num] = cp_pipe;
                if (pipe(pc_pipe_array[process_num]) == -1) {
                    perror("ERROR: Pipe failed");
                    exit(101);
                }
                if (pipe(cp_pipe_array[process_num]) == -1) {
                    perror("ERROR: Pipe failed");
                    exit(101);
                }
                write(pc_pipe_array[process_num][1], buf, MAXWORD);
                if ((pid = fork()) == -1) {
                    perror("ERROR: Fork failed");
                    exit(101);
                } else if (pid == 0) {
                    close(pc_pipe_array[process_num][1]);
                    run_worker(path, pc_pipe_array[process_num][0], cp_pipe_array[process_num][1]);
                    close(cp_pipe_array[process_num][1]);
                    exit(0);
                } else {
                    int status;
                    waitpid(pid, &status, 0);
                    if (status  == 0) {
                        int read_bytes;
                        close(cp_pipe_array[process_num][1]);
                        while ((read_bytes = read(cp_pipe_array[process_num][0], &master_freq_array[masterfreq_index], sizeof(FreqRecord))) > 0) {
                            if (overload == 1) {
                                masterfreq_index = MAXRECORDS;
                            }
                            masterfreq_index++;
                            if (masterfreq_index > MAXRECORDS - 1) {
                                FreqRecord *frp = malloc(sizeof(FreqRecord));
                                if (frp == NULL) {
                                    perror("ERROR: Malloc failed");
                                    exit(1);
                                }
                                frp->freq = 0;
                                master_freq_array[MAXRECORDS] = *frp;
                                overload = 1;
                                masterfreq_index = MAXRECORDS - 1;
                            }
                        }
                        close(cp_pipe_array[process_num][0]);
                        close(pc_pipe_array[process_num][0]);
                        if (overload == 0) {
                            FreqRecord *frp = malloc(sizeof(FreqRecord));
                            if (frp == NULL) {
                                perror("ERROR: Malloc failed");
                                exit(1);
                            }
                            frp->freq = 0;
                            master_freq_array[masterfreq_index+1] = *frp;
                        }
                    }
                }
                process_num++;
            }
        }
        process_num = 0;
        overload = 0;
        masterfreq_index = 0;
        sort(master_freq_array);
        print_freq_records(master_freq_array);
        free(master_freq_array);
        master_freq_array = NULL;
        master_freq_array = malloc((MAXRECORDS+1)*sizeof(FreqRecord));
        if (master_freq_array == NULL) {
            perror("ERROR: Malloc failed");
            exit(1);
        }
        free(buf);
        buf = malloc(MAXWORD);
        if (buf == NULL) {
            perror("ERROR: Malloc failed");
            exit(1);
        }
    }
    return 0;
}
