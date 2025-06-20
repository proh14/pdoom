#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

void die(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

void *xmalloc(size_t size) {
  void *ptr = malloc(size);
  if (!ptr) {
    die("malloc failed");
  }
  return ptr;
}

void *xrealloc(void *ptr, size_t size) {
  void *new_ptr = realloc(ptr, size);
  if (!new_ptr) {
    die("realloc failed");
  }
  return new_ptr;
}
