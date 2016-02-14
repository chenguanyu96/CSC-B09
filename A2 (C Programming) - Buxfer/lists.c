#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lists.h"

/* Add a group with name group_name to the group_list referred to by
 * group_list_ptr. The groups are ordered by the time that the group was
 * added to the list with new groups added to the end of the list.
 *
 * Returns 0 on success and -1 if a group with this name already exists.
 *
 * (I.e, allocate and initialize a Group struct, and insert it
 * into the group_list. Note that the head of the group list might change
 * which is why the first argument is a double pointer.)
 */
int add_group(Group **group_list_ptr, const char *group_name) {
    /* Allocates space in memory for new_group in order to add the group the
     * group_list
     */
    Group *new_group = (Group*) malloc (sizeof(Group));

    /* Checks if malloc have allocated space.
     *     1) If it didn't, print out error message and exit with code 256
     */
    if (new_group == NULL) {
        perror("ERROR: Malloc failed\n");
        exit(256);

    /*     2) If it did, set the new group_name, set users, xcts and next pointers
     *        to NULL
     */
    } else {
        int size_to_malloc = (strlen(group_name)+1)*sizeof(char);
        new_group->name = (char*) malloc (size_to_malloc);

        /* Checks if malloc completed successfully, if it didn't complete 
         * successfully it will print out an error message to standard output.
         */
        if (new_group->name == NULL) {
            perror("ERROR: Malloc failed\n");
            exit(256);
        }
        strncpy(new_group->name, group_name, size_to_malloc);
        new_group->name[size_to_malloc] = '\0';
        /* All other pointers are set NULL until the embedded linked list are 
         * declared and initialized. 
         */
        new_group->users = NULL;
        new_group->xcts = NULL;
        new_group->next = NULL;
    }

    /* Checks if the list is empty.
     * If it is, it sets the new_group to the head of the list and 0 is returned
     */
    if (*group_list_ptr == NULL) {
        *group_list_ptr = new_group;
        return 0;

    /* If it is not an empty list, it will traverse the list to detect if a group
     * with the same name already exists.
     */
    } else {
        Group *curr = *group_list_ptr;

        /* Checks if the first group is the same name as the group to be added,
         * if it is, -1 is returned.
         */
        if (strcmp(curr->name, group_name) == 0) {
            free(new_group->name);

            /* Memory is freed and all variables points to NULL to avoid dangling
             * pointers
             */
            new_group->name = NULL;
            free(new_group);
            new_group = NULL;
            return -1;
        }
        while (curr->next != NULL) {

            /* Compares the current group name with the group_name (user wants
             * to add). 
             *     1) If a group with the same name exists, -1 is returned
             */
            if (strcmp(curr->name, group_name) == 0) {
                free(new_group->name);
                new_group->name = NULL;
                free(new_group);
                new_group = NULL;
                return -1;
            } else {
                curr = curr->next;
            }
        }

        /*    2) If no group with the same name exists, it makes the group and 0 is
         *       returned
         */
        curr->next = new_group;
        return 0;
    }
}

/* Print to standard output the names of all groups in group_list, one name
 *  per line. Output is in the same order as group_list.
 */
void list_groups(Group *group_list) {
    /* Checks group_list is empty, if it is, print statement stating that the
     * group_list is empty.
     */
    if (group_list == NULL) {
        printf("List is empty!\n");

    // If group_list is not empty, prints all group names, 1 on each line.
    } else {
        Group *curr = group_list;
        while (curr != NULL) {
            printf("%s\n", curr->name);
            curr = curr->next;
        }
    }
}

/* Search the list of groups for a group with matching group_name
 * If group_name is not found, return NULL, otherwise return a pointer to the
 * matching group list node.
 */
Group *find_group(Group *group_list, const char *group_name) {
    /* Places a pointer on the first node of the list in order to iterate through
     * the group_list to search for the group.
     */
    Group *curr = group_list;

    /* Checks if the current pointer is pointing to NULL, if it is, NULL is returned,
     * if the pointer is pointing to a group, the group name is checked. If the
     * group name matches with group_name, it is returned, otherwise the search
     * will continue.
     */
    while (curr != NULL) {
        if (strcmp(curr->name, group_name) == 0) {
            return curr;
        } else {
            curr = curr->next;
        }
    }

    // If no group name matches, NULL is returned.
    return NULL;
}

/* Add a new user with the specified user name to the specified group. Return zero
 * on success and -1 if the group already has a user with that name.
 * (allocate and initialize a User data structure and insert it into the
 * appropriate group list)
 */
int add_user(Group *group, const char *user_name) {
    /* Allocates space in memory for the user, and checks if malloc has been
     * completed successfully.
     */
    User *new_user = (User*) malloc (sizeof(User));

    /* If malloc is not completed succesfully, an error message will be printed
     * out to standard output and program will exit with 256 exit code
     */
    if (new_user == NULL) {
        perror("ERROR: Malloc failed\n");
        exit(256);

    /* If malloc has been completed successfully, the user name is set to user_name
     * and set initial balance to 0
     */
    } else {
        int size_to_malloc = (strlen(user_name)+1)*sizeof(char);
        new_user->name = (char*) malloc (size_to_malloc);

        /* Checks if malloc is completed succesfully, if it is not completed
         * successfully, and error message will be printed out the screen and exit
         * code of 256.
         */
        if (new_user->name == NULL) {
            perror("ERROR: Malloc failed\n");
            exit(256);
        }

        /* Copies the user_name given to the user name got the newly created
         * user.
         */
        strncpy(new_user->name, user_name, size_to_malloc);
        new_user->name[size_to_malloc] = '\0';

        /* Sets all other values to either 0.0 if it is double, or NULL if it is
         * a ppinter to another linked list. 
         */
        new_user->balance = 0.0;
        new_user->next=NULL;
    }

    // Returns the user if the user already exists in the group.
    User *prev_user = find_prev_user(group, user_name);

    /* If prev_user is NULL, meaning no user in the group exists with the same
     * name. We can add the new user in this case to the specified group.
     */
    if (prev_user == NULL) {
        prev_user = new_user;
        User *tmp = group->users;
        group->users = prev_user;
        group->users->next = tmp;
        return 0;

    /* If prev_user is not NULL, this means that a user with the same name already
     * exists, so we don't add the new user and just return -1. Free space for 
     * the user name and the user, also points the user to NULL to avoid dangling 
     * pointers
     */
    } else {
        free(new_user->name);
        new_user->name = NULL;
        free(new_user);
        new_user = NULL;
        return -1;
    }
}

/* Remove the user with matching user and group name and
 * remove all her transactions from the transaction list.
 * Return 0 on success, and -1 if no matching user exists.
 * Remember to free memory no longer needed.
 * (Wait on implementing the removal of the user's transactions until you
 * get to Part III below, when you will implement transactions.)
 */
int remove_user(Group *group, const char *user_name) {
    /* Gets the previous user before the user to be deleted if the to be deleted
     * user is not a head. If the to be deleted user is a head, the to be deleted
     * is returned. If the user doesn't exist in the group, NULL is returned.
     */
    User *prev_user = find_prev_user(group, user_name);

    /* If the user returned is not NULL, this means that user exists in the group,
     * therefore we check if the returned user is a head or the previous user.
     *      1) If it is a previous user, we set the previous user next pointer to
     *         the next next user and free space for the deleted user.
     */
    if (prev_user != NULL) {
        if (strcmp(prev_user->name, user_name) == 0) {
            group->users = prev_user->next;
            free(prev_user->name);
            free(prev_user);
            return 0;

        /*      2) If it is a head node, we reset the head to the next user and free
         *         up space for the deleted node.
         */
        } else if (strcmp(prev_user->next->name, user_name) == 0) {
            User *temp = prev_user->next;
            prev_user->next = prev_user->next->next;

            /* Free space for the user name and the user, also points the user to
             * NULL to avoid dangling pointers.
             */
            free(temp->name);
            temp->name = NULL;
            free(temp);
            temp = NULL;
        }

        // Return 0 if we the deletion is successful.
        return 0;

    /* If the user returned is NULL, that means the user name doesn't exist, no
     * users are deleted, and -1 is returned.
     */
    } else {
        return -1;
    }
}

/* Print to standard output the names of all the users in group, one
 * per line, and in the order that users are stored in the list, namely
 * lowest payer first.
 */
void list_users(Group *group) {
    /* Check if user list is empty, if it is empty, a message is printed stating
     * the list is empty.
     */
    if (group->users == NULL) {
        printf("User list is empty!\n");

    /* If the list is not empty, traverse through the list of users and prints out
     * the user name and the balance associated with the user.
     */
    } else {
        User *curr = group->users;
        while (curr != NULL) {
            printf("%s: %.2f\n", curr->name, curr->balance);
            curr = curr->next;
        }
    }
}

/* Print to standard output the balance of the specified user. Return 0
 * on success, or -1 if the user with the given name is not in the group.
 */
int user_balance(Group *group, const char *user_name) {
    /* Initializes a pointer to the first user to traverse through the linked list
     * in order to get the balance of the specified user
     */
    User *curr = group->users;

    /* Traverses through the linked list to find the specified user and get the
     * balance and prints it to standard output.
     */
    while (curr != NULL) {
        if (strcmp(curr->name, user_name) == 0) {
            printf("%.2f\n", curr->balance);
            return 0;
        }
    }

    // The specified user was not found in the linked list, as a result -1 is returned.
    return -1;
}

/* Print to standard output the name of the user who has paid the least
 * If there are several users with equal least amounts, all names are output.
 * Returns 0 on success, and -1 if the list of users is empty.
 * (This should be easy, since your list is sorted by balance).
 */
int under_paid(Group *group) {
    // Checks if the user list is empty, if it is empty, -1 is returned.
    if (group->users == NULL) {
        return -1;

    /* If the user list is not empty, the first user in the list is printed and
     * it checks if the next users have the same pay as the first user. 0 is
     * returned if it is completed successfully.
     */
    } else {
        printf("%s\n", group->users->name);
        User *curr = group->users->next;

        /* Traverses through the user list and tests if other balances are equal
         * to the balance of the first user in the group.
         */
        while (group->users->balance == curr->balance) {
            printf("%s\n", curr->name);
            curr = curr->next;
        }
        return 0;
    }
}

/* Return a pointer to the user prior to the one in group with user_name. If
 * the matching user is the first in the list (i.e. there is no prior user in
 * the list), return a pointer to the matching user itself. If no matching user
 * exists, return NULL.
 *
 * The reason for returning the prior user is that returning the matching user
 * itself does not allow us to change the user that occurs before the
 * matching user, and some of the functions you will implement require that
 * we be able to do this.
 */
User *find_prev_user(Group *group, const char *user_name) {
    /* Initializes and delcares a pointer to the first user in the list in order
     * to find th target user.
     */
    User *curr = group->users;

    /* Traverses through the linked list of users to find the target user and
     * returns the previous user if the user to be returned is not a head.
     */
    if (curr != NULL) {
        if (strcmp(curr->name, user_name) == 0) {
            return curr;
        }
        while (curr != NULL && curr->next != NULL) {
            if (strcmp(curr->next->name, user_name) == 0) {
                return curr;
            }
            curr = curr->next;
        }
    }

    // If no user with the same user name are found, NULL is returned.
    return NULL;
}

/* Pushes a new xct to the head of the list of xcts. This ensures that all least
 * paid users are always at the front of the list.
 */
void push_xct(Group *group, Xct *new_xct) {
    /* Checks if the xcts list is empty. 
     *     1) If it is empty, it pushes the new xct to the head of list and makes
     *        it the new head of the list. 
     */
    if (group->xcts == NULL) {
        group->xcts = new_xct;

    /*     2) If the xcts list is not empty, the old head is stored as a temporary
     *        variable and the new xct tob e added is made to be the new head.
     *        The new head is then linked to the old head. 
     */
    } else {
        Xct *tmp = group->xcts;
        group->xcts = new_xct;
        group->xcts->next = tmp;
    }
}

/* This sorts the user in acsending order by the balance. This is completed after
 * the user's balance is updated.
 */
void sort_user(Group *group, User *prev_user, const char *user_name) {
    if (strcmp(prev_user->name, user_name) == 0) {
        group->users = prev_user->next;
        User *curr = group->users;
        while (curr != NULL) {
            if (curr->balance > prev_user->balance) {

            } else if (curr->next == NULL) {

            } else if (curr->next->balance > prev_user->balance) {

            }
        }
    } else {
        User *tmp = prev_user->next;
        prev_user->next = prev_user->next->next;
        User *curr = group->users;
        while (curr->next->balance > tmp->balance) {
            curr = curr->next;
        }
        tmp->next = curr->next;
        curr->next = tmp;
    }
    
}

/* Add the transaction represented by user_name and amount to the appropriate
 * transaction list, and update the balances of the corresponding user and group.
 * Note that updating a user's balance might require the user to be moved to a
 * different position in the list to keep the list in sorted order. Returns 0 on
 * success, and -1 if the specified user does not exist.
 */
int add_xct(Group *group, const char *user_name, double amount) {
    Xct *new_xct = (Xct*) malloc (sizeof(Xct));
    if (new_xct == NULL) {
        perror("ERROR: Malloc failed\n");
        exit(256);
    } else {
        int size_to_malloc = (strlen(user_name)+1)*sizeof(char);
        new_xct->name = (char*) malloc (size_to_malloc);
        if (new_xct->name == NULL) {
            perror("ERROR: Malloc failed\n");
            exit(256);
        }
        strncpy(new_xct->name, user_name, size_to_malloc);
        new_xct->amount = amount;
        new_xct->next = NULL;
    }
    User *prev_user = find_prev_user(group, user_name);
    if (prev_user == NULL) {
        free(new_xct->name);
        new_xct->name = NULL;
        free(new_xct);
        new_xct = NULL;
        return -1;
    } else if (strcmp(prev_user->name, user_name) == 0) {
        push_xct(group, new_xct);
        prev_user->balance += amount;
        sort_user(group, prev_user, user_name);
        return 0;
    } else {
        push_xct(group, new_xct);
        prev_user->next->balance += amount;
        sort_user(group, prev_user, user_name);
        return 0;
    }
}

/* Print to standard output the num_xct most recent transactions for the
 * specified group (or fewer transactions if there are less than num_xct
 * transactions posted for this group). The output should have one line per
 * transaction that prints the name and the amount of the transaction. If
 * there are no transactions, this function will print nothing.
 */
void recent_xct(Group *group, long nu_xct) {
    if (group->xcts == NULL) {
        return;
    } else {
        Xct *curr = group->xcts;
        int i;
        for (i = 0; i < nu_xct; i++) {
            if (curr != NULL) {
                printf("%s: %.2f\n", curr->name, curr->amount);
            } else {
                return;
            }
            curr = curr->next;
        }
    }
}

/* Remove all transactions that belong to the user_name from the group's
 * transaction list. This helper function should be called by remove_user.
 * If there are no transactions for this user, the function should do nothing.
 * Remember to free memory no longer needed.
 */
void remove_xct(Group *group, const char *user_name) {
    //Xct *curr = group->xcts;
}