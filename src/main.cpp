#include <raylib.h>

#include "gridMaze.h"

int main() {
    int width = 45;
    int height = 45;
    int gridSize = 10;

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