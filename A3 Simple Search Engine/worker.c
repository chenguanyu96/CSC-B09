#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include "freq_list.h"
#include "worker.h"

void sort(FreqRecord *frps) { 
    if (frps[0].freq == 0 || frps[1].freq == 0) { 
        return; 
    } 
    int i = 1; 
    while (i < MAXRECORDS && frps[i].freq != 0) {
        FreqRecord current = frps[i];
        int j = i -1; 
        while (j >= 0 && frps[j].freq < current.freq) {
            frps[j + 1] = frps[j]; 
            j = j - 1; 
        } 
        frps[j + 1] = current; 
        i++; 
    }
}

/* The function get_word should be added to this file */
FreqRecord *get_word(Node *head, char **filename, char *word) {
    FreqRecord *freqRecords = malloc (MAXRECORDS*sizeof(FreqRecord));
    if (freqRecords == NULL) {
        perror("ERROR: Malloc failed");
        exit(256);
    }
    Node *curr = head;
    int index = 0;
    int frp_index = 0;
    int last_record_index = 0;
    while (curr != NULL) {
        if (strcmp(curr->word, word) == 0) {
            while (index < MAXFILES) {
                if (curr->freq[index] == 0) {
                    index++;
                } else {
                    FreqRecord *freqRecord = malloc (sizeof(FreqRecord));
                    if (freqRecord == NULL) {
                        perror("ERROR: Malloc failed");
                        exit(256);
                    }
                    freqRecord->freq = curr->freq[index];
                    strcpy(freqRecord->filename, filename[index]);
                    freqRecords[frp_index] = *freqRecord;
                    last_record_index = frp_index;
                    frp_index++;
                    index++;
                }
            }
            curr = NULL;
        } else {
            curr = curr->next;
        }
    }
    FreqRecord *freqRecord = malloc (sizeof(FreqRecord));
    if (freqRecord == NULL) {
        perror("ERROR: Malloc failed");
        exit(256);
    }
    freqRecord->freq = 0;
    freqRecords[last_record_index+1] = *freqRecord;
    index = 0;
    frp_index = 0;
    last_record_index = 0;
    return freqRecords;
}


/* Print to standard output the frequency records for a word.
* Used for testing.
*/
void print_freq_records(FreqRecord *frp) {
	int i = 0;
	while(frp != NULL && frp[i].freq != 0) {
		printf("%d    %s\n", frp[i].freq, frp[i].filename);
		i++;
	}
}

/* run_worker
* - load the index found in dirname
* - read a word from the file descriptor "in"
* - find the word in the index list
* - write the frequency records to the file descriptor "out"
*/
void run_worker(char *dirname, int in, int out){
    Node *head = NULL;
	char listfile[PATHLENGTH];
    char namefile[PATHLENGTH];
    char buf[MAXWORD];
    int index = 0;
    int last_char_index = 0;
    strcpy(listfile, dirname);
    strcat(listfile, "/index");
    strcpy(namefile, dirname);
    strcat(namefile, "/filenames");
    char **filenames = init_filenames();
    read_list(listfile, namefile, &head, filenames);
    read(in, buf, MAXWORD);
    while (index < MAXWORD) {
        if ((buf[index] >= 65 && buf[index] <= 90) ||
            (buf[index] >= 97 && buf[index] <= 122) ||
            (buf[index] >= 48 && buf[index] <= 57)) {
            last_char_index = index;
        }
        index++;
    }
    buf[last_char_index+1] = '\0';
    FreqRecord *frp = malloc(sizeof(FreqRecord));
    frp = get_word(head, filenames, buf);
    index = 0;
    while (frp != NULL && frp[index].freq != 0) {                          
        close(in);
        write(out, &frp[index], sizeof(FreqRecord));
        index++;
    }
}
