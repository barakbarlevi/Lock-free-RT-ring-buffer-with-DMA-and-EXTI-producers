#include <sys/stat.h>
#include <errno.h>
#include "uart.h"

// Redirect printf output to UART
int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        uart2_write(ptr[i]);
    }
    return len;
}

// Minimal stubs required for newlib (the C standard library)
int _close(int file) {
    return -1;
}

int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file) {
    return 1;
}

int _lseek(int file, int ptr, int dir) {
    return 0;
}

int _read(int file, char *ptr, int len) {
    return 0;
}

// Heap management (needed for malloc, but we'll provide minimal stubs)
extern char _end;  // Defined by linker
static char *heap_ptr = NULL;

void *_sbrk(int incr) {
    char *prev_heap_ptr;
    
    if (heap_ptr == NULL) {
        heap_ptr = &_end;
    }
    
    prev_heap_ptr = heap_ptr;
    heap_ptr += incr;
    
    return (void *)prev_heap_ptr;
}