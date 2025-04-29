#include "maze.h"

Maze::Maze(int width, int height, int gridSize, int seed) : width(width), height(height), gridSize(gridSize), seed(seed) {
    
    grid = new bool*[width];
    for (int x = 0; x < width; x++) {
        grid[x] = new bool[height];
        for (int y = 0; y < height; y++) {
            grid[x][y] = false;
        }
    }

    start = Vec2(0,2);
    finish = start;
};

void Maze::Start() {
    srand(seed);

    InitWindow(width*gridSize, height*gridSize, "Hmmf's Mazes");
    SetTargetFPS(120);

    int frameCount = 0;

    while (!WindowShouldClose()) {
        if (!generated && frameCount >= 0) Generate();

        if (!Draw(frameCount++)) {
            // frameCount = -300;
            if (!solved) {
                Solve();
            } else generated = false;
        };
    }
}

void Maze::Reset() {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            grid[x][y] = false;
        }
    }

    while (stack.size() != 0) stack.pop();
    while (queue.size() != 0) queue.pop();
    while (drawQueue.size() != 0) drawQueue.pop();
    while (drawBuffer.size() != 0) drawBuffer.pop();

    start = finish;

    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();
    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();
    
};

void Maze::PrintToConsole() {
    std::cout << std::endl;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            std::cout<< (grid[x][y] ? "□ " : "■ ");
        }
        std::cout << std::endl;
    }
}

void Maze::Generate() {
    Reset();
    stack.push(start);
    
    int randGenID = rand()%3;
    if (randGenID == 0) {
        while (GenGridDFS());
        SetWindowTitle("Hmmf's Maze : Generator : Grid Depth First Search");
    } else if (randGenID == 1) {
        while (GenLooseTightDFS());
        SetWindowTitle("Hmmf's Maze : Generator : Loose Tight Depth First Search");
    } else if (randGenID == 2) {
        while (GenLooseWideDFS());
        SetWindowTitle("Hmmf's Maze : Generator : Loose Wide Depth First Search");
    }

    GenFinish();
    drawQueue.push(DrawElement(start,GREEN));
    drawQueue.push(DrawElement(finish,RED));
    generated = true;
    solved = false;
}

void Maze::Solve() {
    stack.push(start);

    int randSolvID = rand()%2;
    if (randSolvID == 0) {
        while(SolveDFS());
        SetWindowTitle("Hmmf's Maze : Solver : Depth First Search");
    } else if (randSolvID == 1) {
        while(SolveBFS());
        SetWindowTitle("Hmmf's Maze : Solver : Breadth First Search");
    }
    
    solved = true;

}

bool Maze::Draw(int frameCount) {
    BeginDrawing();
    DrawElement nextElement;

    while (drawBuffer.size() != 0) {
        nextElement = drawBuffer.front();
        drawBuffer.pop();
        DrawRectangle(nextElement.x*gridSize, nextElement.y*gridSize, gridSize, gridSize, nextElement.colour);
    }

    if (frameCount >= 0 && frameCount % drawFrequency == 0) {
        for (int _ = 0; _ < drawPerFrame; _++) {
            if (drawQueue.size() == 0) break;
            nextElement = drawQueue.front();
            drawQueue.pop();
            drawBuffer.push(nextElement);
            DrawRectangle(nextElement.x*gridSize, nextElement.y*gridSize, gridSize, gridSize, nextElement.colour);
        }
    }
    

    EndDrawing();
    if (drawQueue.size() == 0 && drawBuffer.size() == 0 && frameCount >= 0) return false;
    return true;
}

bool Maze::InBound(Vec2 pos) {
    return (pos.x >= 0 && pos.x < width && pos.y >= 0 && pos.y < height);
}

// 0  1  2  3  4
// 5  6  7  8  9
// 10 11 12 13 14
// 15 16 17 18 19
// 20 21 22 23 24
bool* Maze::GetNeighbours(Vec2 pos, bool generating) {
    bool* neighbours = new bool[25]{false};
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (InBound(Vec2(pos.x + i - 2, pos.y + j - 2))) {
                if (generating) neighbours[i + (j * 5)] = !grid[pos.x + i - 2][pos.y + j -2];
                else neighbours[i + (j * 5)] = grid[pos.x + i - 2][pos.y + j -2];
            }
        }
    }

    // for (int i = 0; i < 25; i++) {
    //     if (i % 5 == 0) std::cout << std::endl;
    //     std::cout << (neighbours[i] ? "□ " : "■ ");
    // }
    // std::cout << std::endl;

    return neighbours;
}

void Maze::GridStart() {
    stack.pop();
    if (start.x == 0 || start.x == width-1) {
        if (start.y % 2 != 1) {
            start.y -= 1;
        }
        if (start.x == 0) stack.push(Vec2(1,start.y));
        else stack.push(Vec2(width-2, start.y));
    } else {
        if (start.x % 2 != 1) {
            start.x -= 1;
        }
        if (start.y == 0) stack.push(Vec2(start.x,1));
        else stack.push(Vec2(start.x,height-2));
    }
    grid[start.x][start.y] = true;
}

void Maze::GenFinish() {
    std::vector<int> canditates = {0,1,2,3};
    if (start.x == 0) canditates.erase(canditates.begin() + 0);
    if (start.x == width-1) canditates.erase(canditates.begin() + 1);
    if (start.y == 0) canditates.erase(canditates.begin() + 2);
    if (start.y == height-1) canditates.erase(canditates.begin() + 3);

    int rndWall = canditates.at(rand() % 3);
    if (rndWall == 0 || rndWall == 1) {
        int y = rand() % height;
        if (rndWall == 0) {
            if (grid[1][y]) finish = Vec2(0,y);
            else GenFinish();
        } else {
            if (grid[width-2][y]) finish = Vec2(width-1,y);
            else GenFinish();
        }
    } else {
        int x = rand() % width;
        if (rndWall == 2) {
            if (grid[x][1]) finish = Vec2(x,0);
            else GenFinish();
        } else {
            if (grid[x][height-2]) finish = Vec2(x,height-1);
            else GenFinish();
        }
    }
    grid[finish.x][finish.y] = true;
}

bool Maze::GenGridDFS() {
    if (stack.size() == 0) return false;
    else if (stack.size() == 1) GridStart();

    Vec2 pos = stack.top();
    grid[pos.x][pos.y] = true;

    bool* neighbours = GetNeighbours(pos);
    std::vector<Vec2> options;
    if (neighbours[2]) options.push_back(Vec2(0,-1));
    if (neighbours[10]) options.push_back(Vec2(-1,0));
    if (neighbours[14]) options.push_back(Vec2(1,0));
    if (neighbours[22]) options.push_back(Vec2(0,1));

    if (options.size() == 0) {
        stack.pop();
        drawQueue.push(DrawElement(pos,WHITE));
    } else {
        drawQueue.push(DrawElement(pos,BLUE));
        Vec2 randDir = options.at(rand() % options.size());
        drawQueue.push(DrawElement(pos.x + randDir.x, pos.y + randDir.y, WHITE));
        grid[pos.x + randDir.x][pos.y + randDir.y] = true;
        randDir.x = randDir.x * 2 + pos.x;
        randDir.y = randDir.y * 2 + pos.y;
        stack.push(randDir);
    }

    return true;
}

bool Maze::GenLooseWideDFS() {
    if (stack.size() == 0) return false;

    Vec2 pos = stack.top();
    grid[pos.x][pos.y] = true;

    bool* neighbours = GetNeighbours(pos);
    std::vector<Vec2> options;
    if (neighbours[1] && neighbours[2] && neighbours[3] && neighbours[6] && neighbours[7] && neighbours[8]) options.push_back(Vec2(0,-1));
    if (neighbours[5] && neighbours[10] && neighbours[15] && neighbours[6] && neighbours[11] && neighbours[16]) options.push_back(Vec2(-1,0));
    if (neighbours[9] && neighbours[14] && neighbours[19] && neighbours[8] && neighbours[13] && neighbours[18]) options.push_back(Vec2(1,0));
    if (neighbours[21] && neighbours[22] && neighbours[23] && neighbours[16] && neighbours[17] && neighbours[18]) options.push_back(Vec2(0,1));

    if (options.size() == 0) {
        stack.pop();
        drawQueue.push(DrawElement(pos,WHITE));
    } else {
        drawQueue.push(DrawElement(pos,BLUE));
        Vec2 randDir = options.at(rand() % options.size());
        randDir.x = randDir.x + pos.x;
        randDir.y = randDir.y + pos.y;
        grid[randDir.x][randDir.y] = true;
        stack.push(randDir);
    }

    return true;

}

bool Maze::GenLooseTightDFS() {
    if (stack.size() == 0) return false;

    Vec2 pos = stack.top();
    grid[pos.x][pos.y] = true;

    bool* neighbours = GetNeighbours(pos);
    std::vector<Vec2> options;
    if (neighbours[2] && neighbours[6] && neighbours[7] && neighbours[8]) options.push_back(Vec2(0,-1));
    if (neighbours[10] && neighbours[6] && neighbours[11] && neighbours[16]) options.push_back(Vec2(-1,0));
    if (neighbours[14] && neighbours[8] && neighbours[13] && neighbours[18]) options.push_back(Vec2(1,0));
    if (neighbours[22] && neighbours[16] && neighbours[17] && neighbours[18]) options.push_back(Vec2(0,1));

    if (options.size() == 0) {
        stack.pop();
        drawQueue.push(DrawElement(pos,WHITE));
    } else {
        drawQueue.push(DrawElement(pos,BLUE));
        Vec2 randDir = options.at(rand() % options.size());
        randDir.x = randDir.x + pos.x;
        randDir.y = randDir.y + pos.y;
        grid[randDir.x][randDir.y] = true;
        stack.push(randDir);
    }

    return true;

}

bool Maze::SolveDFS() {
    if (stack.size() == 0) return false;

    Vec2 pos = stack.top();
    grid[pos.x][pos.y] = false;

    if (pos.x == finish.x && pos.y == finish.y) return false;

    bool* neighbours = GetNeighbours(pos, false);
    std::vector<Vec2> options;
    if (neighbours[7]) options.push_back(Vec2(0,-1));
    if (neighbours[11]) options.push_back(Vec2(-1,0));
    if (neighbours[13]) options.push_back(Vec2(1,0));
    if (neighbours[17]) options.push_back(Vec2(0,1));

    if (options.size() == 0) {
        stack.pop();
        drawQueue.push(DrawElement(pos,LIGHTGRAY));
    } else {
        drawQueue.push(DrawElement(pos,YELLOW));
        Vec2 randDir = options.at(rand() % options.size());
        randDir.x = randDir.x + pos.x;
        randDir.y = randDir.y + pos.y;
        drawQueue.push(DrawElement(randDir,ORANGE));
        stack.push(randDir);
    }

    return true;
}

bool Maze::SolveBFS() {
    if (stack.size() != 0) {
        queue.push(stack.top());
        stack.pop();
    }
    if (queue.size() == 0) return false;

    Vec2 pos = queue.front();
    queue.pop();
    grid[pos.x][pos.y] = false;

    drawQueue.push(DrawElement(pos,ORANGE));
    drawQueue.push(DrawElement(pos,YELLOW));

    if (pos.x == finish.x && pos.y == finish.y) return false;

    bool* neighbours = GetNeighbours(pos, false);
    if (neighbours[7]) queue.push(Vec2(pos.x,pos.y-1));
    if (neighbours[11]) queue.push(Vec2(pos.x-1,pos.y));
    if (neighbours[13]) queue.push(Vec2(pos.x+1,pos.y));
    if (neighbours[17]) queue.push(Vec2(pos.x,pos.y+1));

    return true;

}