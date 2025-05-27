#include <raylib.h>

#include "gridMaze.h"

int main() {
    int width = 1601;
    int height = 1001;
    int gridSize = 1;

    if (width%2==0) {
        std::cout << "WARN!" << std::endl;
        std::cout << "Width must be an odd number" << std::endl;
    } else if (height%2==0) {
        std::cout << "WARN!" << std::endl;
        std::cout << "Height must be an odd number" << std::endl;
    } else {
        GridMaze gridMaze(width,height,gridSize);
        gridMaze.Start();
    }

    return 0;
}