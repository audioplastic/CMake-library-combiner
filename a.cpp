#include "a.h"
#include "b.h"

int aclass::method(int x, int y) {
    bclass b;
    return x * b.method(x,y);
}