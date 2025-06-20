#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

void die(const char *message);
void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);

#endif // UTILS_H
