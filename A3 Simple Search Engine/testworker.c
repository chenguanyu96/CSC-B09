#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "freq_list.h"
#include "worker.h"


int main(void) {
    char *listfile = "a3-2016/big/books/index";
    char *namefile = "a3-2016/big/books/filenames";
    Node *head = NULL;
    char **filenames = init_filenames();
    read_list(listfile, namefile, &head, filenames);
    char buf[MAXWORD];
    read(STDIN_FILENO, buf, MAXWORD);
    int index = 0;
    int last_char_index = 0;
    while (index < MAXWORD) {
        if (buf[index] >= 97 && buf[index] <= 122) {
            last_char_index = index;
        }
        index++;
    }
    buf[last_char_index+1] = '\0';
    FreqRecord *frp = get_word(head, filenames, buf);
    print_freq_records(frp);
}