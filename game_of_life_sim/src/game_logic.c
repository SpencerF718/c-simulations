#include "game_logic.h"
#include <stdio.h>


GameGrid* initializeGrid(size_t numRows, size_t numCols) {

    GameGrid* grid = calloc(1, sizeof(GameGrid));   

    if (!grid) {
        return NULL;
    }

    grid->numRows = numRows;
    grid->numCols = numCols;

    grid->cells = calloc(numRows, sizeof(bool*));

    if (!grid->cells) {
        free(grid);
        return NULL;
    }

    for (size_t i = 0; i < numRows; i++) {
        grid->cells[r] = calloc(numCols,sizeof(bool));
        
        if (!grid->cells[r]) {
            for (size_t j = 0; j < i; j++) {
                free(grid->cells[i]);
            }

            free(grid->cells);
            free(grid);
            return NULL;
        }
    }   
    return grid;
}
