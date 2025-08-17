#ifdef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <stdbool.h>
#include <stddef.h>


typedef struct {
    size_t numRows;
    size_t numCols;
    bool **cells*;
} GameGrid;


GameGrid* initializeGride(size_t numRows, size_t numCols);


#endif


