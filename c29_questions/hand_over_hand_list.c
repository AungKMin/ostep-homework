#include "wrappers.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

// Main problem to avoid:
//  - while traversing the list, we have to wait until traverse->next is unlocked
//  - but if traverse->next is waiting for traverse to unlock in some other thread, we have a deadlock
//  - note: while holding traverse's lock, there's actually no danger of the next node being deleted since to do that, the delete code must grab traverse's lock to fully delete next
// To avoid this:
//  - just have one pointer (next, no prev)
//  - in all traversals, we will reach for traverse->next, never backwards; since the lock grab is always in one direction throughout all threads, we never have a deadlock

// we'll use a sentinel node to make sure there is at least one lock for the start of list_insert to try to grab (we don't want delete to delete the head node in between the point where we check whether there is a head node and the point where  we grab its lock)

typedef struct __Node { 
    int key;
    struct __Node* next;
    pthread_mutex_t* lock;
} Node;

typedef struct __List { 
    Node* head;
} List;

void node_init(Node* n) { 
    n->next = NULL;
    n->lock = malloc(sizeof(pthread_mutex_t));
    Pthread_mutex_init(n->lock, NULL);
}

void list_init(List* l) { 
    l->head = malloc(sizeof(Node));
    if (l->head == NULL) {
        perror("malloc failed");
        return;
    }
    node_init(l->head);
}

// position is the index that this new node will take
int list_insert(List* l, int key, int position) { 
    // printf("list_insert start\n");
    assert(position >= 0);
    Node* new = malloc(sizeof(Node));
    if (new == NULL) { 
        perror("malloc failled");
        return -1;
    } 
    node_init(new);
    new->key = key;

    // we'll try to get traverse to the node BEFORE the desired position
    Node* traverse = l->head;
    // lock of the previous node so we can unlock it after obtaining the next lock
    pthread_mutex_t* prev_lock = traverse->lock;
    // note that we can afford to do the previous two lines before locking since the sentinel won't be deleted
    Pthread_mutex_lock(prev_lock);
    while (position != 0) { 
        if (traverse->next == NULL) { 
            Pthread_mutex_unlock(prev_lock);
            return -1;
        }
        traverse = traverse->next;
        Pthread_mutex_lock(traverse->lock);
        Pthread_mutex_unlock(prev_lock);
        prev_lock = traverse->lock;
        --position;
    }

    // traverse is the node just BEFORE the position where we want to insert
    // we will have the lock on traverse
    // note that it's safe to assume traverse->next will not change between these lines since for a thread to insert or delete at traverse->next's position, that thread needs the lock on traverse
    Node* temp = traverse->next;
    traverse->next = new;
    new->next = temp;
    Pthread_mutex_unlock(traverse->lock);
    return 0;
}

int list_delete(List* l, int position) { 
  // we'll try to get traverse to the node BEFORE the desired position
    Node* traverse = l->head;
    // lock of the previous node so we can unlock it after obtaining the next lock
    pthread_mutex_t* prev_lock = traverse->lock;
    // note that we can afford to do the previous two lines before locking since the sentinel won't be deleted
    Pthread_mutex_lock(prev_lock);
    while (position != 0) { 
        if (traverse->next == NULL) { 
            Pthread_mutex_unlock(prev_lock);
            return -1;
        }
        traverse = traverse->next;
        Pthread_mutex_lock(traverse->lock);
        Pthread_mutex_unlock(prev_lock);
        prev_lock = traverse->lock;
        --position;
    }

    // traverse is the node just BEFORE the position where we want to insert
    // we will have the lock on traverse
    // note that it's safe to assume traverse->next will not change between these lines since for a thread to insert or delete at traverse->next's position, that thread needs the lock on traverse
    if (traverse->next != NULL) { 
        pthread_mutex_t* temp_lock = traverse->next->lock;
        Pthread_mutex_lock(temp_lock);
        Node* temp_node = traverse->next;
        traverse->next = (traverse->next)->next;
        free(temp_node);
    }
    Pthread_mutex_unlock(traverse->lock);
    return 0;
}

int list_print(List* l) { 
    // start at head->next since at head is a sentinel
    // we need the lock on head so that head->next isn't deleted
    // printf("list_print start\n");
    pthread_mutex_t* prev_lock = l->head->lock;
    Pthread_mutex_lock(prev_lock);
    Node* traverse = l->head->next;
    while (traverse != NULL) { 
        // we know traverse = traverse->next could not have been deleted as long as we hol prev_lock
        Pthread_mutex_lock(traverse->lock);
        Pthread_mutex_unlock(prev_lock);
        printf("%d - ", traverse->key);
        prev_lock = traverse->lock;
        traverse = traverse->next;
    }
    Pthread_mutex_unlock(prev_lock);
    printf("\n");
    return 0;
}


typedef struct __InsertArgs { 
    List* l;
    int key;
    int position;
} InsertArgs;

typedef struct __PrintArgs { 
    List* l;
} PrintArgs;

typedef struct __DeleteArgs { 
    List* l;
    int position;
} DeleteArgs;

void* insert_worker(void* arg) { 
    // printf("insert_worker start\n");
    InsertArgs* args = (InsertArgs*) arg;
    if (list_insert(args->l, args->key, args->position) != 0) { 
        printf("DELETE UNSUCCESSFUL\n");
    }
    return NULL;
}

void* print_worker(void* arg) {
    // printf("print_worker start\n");
    PrintArgs* args = (PrintArgs*) arg;
    if (list_print(args->l) != 0) { 
        printf("PRINT UNSUCCESSFUL\n");
    }
    return NULL;
}

void* delete_worker(void* arg) { 
    // printf("delete_worker start\n");
    DeleteArgs* args = (DeleteArgs*) arg;
    if (list_delete(args->l, args->position) != 0) { 
        printf("DELETE UNSUCCESSFUL\n"); 
    }
    return NULL;
}

int main (int argc, char** argv) { 
    List* l = malloc(sizeof(List));
    list_init(l);
    char mode = argv[1][0];
    pthread_t* threads[100];
    int thread_index = 0;
    // 1 thread
    if (mode == 's') { 
        for (int i = 2; i < argc; ++i) { 
            char command = argv[i][0];
            if (command == 'i') { 
                ++i;
                int key = strtol(argv[i], NULL, 10);
                ++i;
                int position = strtol(argv[i], NULL, 10);
                list_insert(l, key, position);
            } else if (command == 'p') {
                    list_print(l);
            } else if (command == 'd') { 
                ++i;
                int position = strtol(argv[i], NULL, 10);     
                list_delete(l, position);          
            } else {
                    perror("not recognized token");
                    exit(1);
            }
        }
    } else { 
        for (int i = 2; i < argc; ++i) { 
            char command = argv[i][0];
            if (command == 'i') { 
                ++i;
                int key = strtol(argv[i], NULL, 10);
                ++i;
                int position = strtol(argv[i], NULL, 10);
                threads[thread_index] = malloc(sizeof(pthread_t));
                InsertArgs* args = (InsertArgs*) malloc(sizeof(InsertArgs));
                args->l = l;
                args->key = key;
                args->position = position;
                Pthread_create(threads[thread_index], NULL, insert_worker, args);
            } else if (command == 'p') {
                threads[thread_index] = malloc(sizeof(pthread_t));
                PrintArgs* args = (PrintArgs*) malloc(sizeof(PrintArgs));
                args->l = l;
                Pthread_create(threads[thread_index], NULL, print_worker, args);
            } else if (command == 'd') { 
                ++i;
                int position = strtol(argv[i], NULL, 10);
                threads[thread_index] = malloc(sizeof(pthread_t));
                DeleteArgs* args = (DeleteArgs*) malloc(sizeof(DeleteArgs));
                args->l = l;
                args->position = position;
                Pthread_create(threads[thread_index], NULL, delete_worker, args);
            } else {
                    perror("not recognized token");
                    exit(1);
            }
            ++thread_index;
        }
        printf("number of threads: %d\n", thread_index);
        for (int i = 0; i < thread_index; ++i) { 
            long int return_val;
            Pthread_join(*threads[i], (void**)&return_val);
            printf("thread_exit: %ld\n", return_val);
        }
        printf("final list: \n");
        list_print(l);
    }
    return 0;
}