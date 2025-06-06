#include "gridMaze.h"

GridMaze::GridMaze(int width, int height, int gridSize, int seed) : width(width), height(height), gridSize(gridSize), seed(seed) {
    
    grid = new bool*[width];
    // BFSIndex = new int*[width];
    ComplexGrid = new ComplexSquare*[width];
    for (int x = 0; x < width; x++) {
        grid[x] = new bool[height];
        // BFSIndex[x] = new int[height];
        ComplexGrid[x] = new ComplexSquare[height];
        for (int y = 0; y < height; y++) {
            grid[x][y] = false;
            // BFSIndex[x][y] = -1;
            ComplexGrid[x][y].x = x;
            ComplexGrid[x][y].y = y;
            ComplexGrid[x][y].parent = &ComplexGrid[x][y];
        }
    }

    start = Vec2(0,2);
    finish = start;
};

void GridMaze::Start() {
    srand(seed);

    InitWindow(width*gridSize, height*gridSize, "Hmmf's Mazes");
    SetTargetFPS(120);

    int frameCount = 0;

    // pausedDrawFrequency = drawFrequency;
    // drawFrequency = 0;
    // stack.push(start);

    if (CanFractal()) std::cout<<"Can Fractal"<<std::endl;

    ResetScreen();

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_RIGHT)) {
            if (drawFrequency != 1) {
                drawFrequency /= 2;
            } else {
                drawPerFrame*=2;
            }
        }
        if (IsKeyPressed(KEY_LEFT)) {
            if (drawPerFrame != 1) {
                drawPerFrame /= 2;
            } else {
                drawFrequency *= 2;
            }
        }
        if (IsKeyPressed(KEY_S)) {
            if (!mouseSolving) {
                int tempDrawPerFrame = drawPerFrame;
                int tempDrawFrequency = drawFrequency;
                drawPerFrame = drawQueue.size();
                drawFrequency = 1;
                Draw(drawFrequency);
                drawPerFrame = tempDrawPerFrame;
                drawFrequency = tempDrawFrequency;
            } else {
                mouseSolving = false;
                solved = true;
            }
        }
        if (IsKeyPressed(KEY_P)) {
            if (drawFrequency != 0) {
                pausedDrawFrequency = drawFrequency;
                drawFrequency = 0;
            }
            else if (drawFrequency == 0) drawFrequency = pausedDrawFrequency;
        }
        if (IsKeyPressed(KEY_SPACE)) {
            // if (drawQueue.size() == 0) GenFractals();
            int tempDrawPerFrame = drawPerFrame;
            int tempDrawFrequency = drawFrequency;
            drawPerFrame = 1;
            drawFrequency = 1;
            Draw(drawFrequency);
            drawPerFrame = tempDrawPerFrame;
            drawFrequency = tempDrawFrequency;
        }

        // Draw(frameCount++);

        if (!generated && frameCount >= 0) Generate();

        if (!Draw(frameCount++)) {
            if (!solved) {
                Solve();
            } else generated = false;
        };
    }

    // stack.push(start);
    // GenWilsons();
    // // while (GenWilsons());
    // PrintToConsole();

    CloseWindow();
}

void GridMaze::Reset() {
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
    
};

void GridMaze::ResetScreen() {
    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();
    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();
};

void GridMaze::ResetComplexGrid() {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            ComplexGrid[x][y].id = -1;
            ComplexGrid[x][y].parent = &ComplexGrid[x][y];
            ComplexGrid[x][y].gridColour = WHITE;
        }
    }
}

void GridMaze::PrintToConsole() {
    std::cout << std::endl;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            std::cout<< (grid[x][y] ? "□ " : "■ ");
        }
        std::cout << std::endl;
    }
}

bool GridMaze::CanFractal() {
    if ((width-1)%4 != 0 || (height-1)%4 != 0) return false;
    return true;
}

void GridMaze::Generate() {
    Reset();
    stack.push(start);
    
    int randGenID = rand()%5;
    // randGenID=3;

    functionTimeSeconds = (time(nullptr));
    functionTimeMiliSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    if (randGenID == 0) {
        randGenID = rand() % 3;
        if (randGenID == 0) {
            std::cout << "Starting Gen GDFS" << std::endl;
            while (GenGridDFS());
            SetWindowTitle("Hmmf's Maze : Generator : Grid Depth First Search");
        } else if (randGenID == 1) {
            std::cout << "Starting Gen LTDFS" << std::endl;
            while (GenLooseTightDFS());
            SetWindowTitle("Hmmf's Maze : Generator : Loose Tight Depth First Search");
        } else if (randGenID == 2) {
            std::cout << "Starting Gen LWDFS" << std::endl;
            while (GenLooseWideDFS());
            SetWindowTitle("Hmmf's Maze : Generator : Loose Wide Depth First Search");
        }
    } else if (randGenID == 1) {
        std::cout << "Starting Gen Kruskals" << std::endl;
        while (GenKruskals());
        SetWindowTitle("Hmmf's Maze : Generator : Kruskals Maze Algorithm");
    } else if (randGenID == 2) {
        std::cout << "Starting Gen Prims" << std::endl;
        while (GenPrims());
        SetWindowTitle("Hmmf's Maze : Generator : Prims Maze Algorithm");
    } else if (randGenID == 3) {
        std::cout << "Starting Gen RecursiveDivision" << std::endl;
        while (GenRecusiveDivision());
        SetWindowTitle("Hmmf's Maze : Generator : Recursive Division Algorithm");
    } else if (randGenID == 4) {
        std::cout << "Starting Gen Wilsons" << std::endl;
        while (GenWilsons());
        SetWindowTitle("Hmmf's Maze : Generator : Wilsons Algorithm");
    } else if (randGenID == 5) {
        std::cout << "Starting Gen Fractals" << std::endl;
        while (GenFractals());
        SetWindowTitle("Hmmf's Maze : Generator : Fractal Tessellation Algorithm");
    }

    std::cout << "Took:" << time(nullptr)-functionTimeSeconds << "s || " 
    << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - functionTimeMiliSeconds 
    << "ms" << std::endl;

    ResetScreen();

    if (randGenID == 3) {
        BeginDrawing();
        DrawRectangle(gridSize,gridSize,(width-2)*gridSize,(height-2)*gridSize,WHITE);
        EndDrawing();
        BeginDrawing();
        DrawRectangle(gridSize,gridSize,(width-2)*gridSize,(height-2)*gridSize,WHITE);
        EndDrawing();
    }

    GenFinish();
    drawQueue.push(DrawElement(start,GREEN));
    drawQueue.push(DrawElement(finish,RED));
    generated = true;
    solved = false;
}

void GridMaze::Solve() {
    if (!mouseSolving) {
        stack.push(start);
        functionTimeSeconds = (time(nullptr));
        functionTimeMiliSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    int randSolvID = rand()%4;
    // randSolvID = 2;

    if (mouseSolving) randSolvID = 3;

    if (randSolvID == 0) {
        std::cout << "Starting Solve DFS" << std::endl;
        while(SolveDFS());
        SetWindowTitle("Hmmf's Maze : Solver : Depth First Search");
    } else if (randSolvID == 1) {
        std::cout << "Starting Solve BFS" << std::endl;
        ResetComplexGrid();
        while(SolveBFS());
        SetWindowTitle("Hmmf's Maze : Solver : Breadth First Search");
    } else if (randSolvID == 2) {
        randSolvID = rand()%2;
        if (randSolvID == 0) {
            std::cout << "Starting Solve RHWF" << std::endl;
            while(SolveRHWall());
            SetWindowTitle("Hmmf's Maze : Solver : Right Hand Wall Follower");
        } else {
            std::cout << "Starting LHWF" << std::endl;
            while(SolveLHWall());
            SetWindowTitle("Hmmf's Maze : Solver : Left Hand Wall Follower");
        }
    } else if (randSolvID == 3) {
        if (mouseSolving != true) {
            stack.push(start);
            std::cout << "Starting Solve Mouse" << std::endl;
            functionTimeSeconds = static_cast<unsigned int>(time(nullptr));
        }
        mouseSolving = true;
        for (int _ = 0; _ < drawPerFrame; _++) {
            if (!SolveMouse()) {
                std::cout<<"It made it here"<< stack.size() << std::endl;
                mouseSolving = false;
                break;
            }
        }
        

        SetWindowTitle("Hmmf's Maze : Solver : Randomised Mouse Search");
        // if (static_cast<unsigned int>(time(nullptr)) - functionTimeSeconds > 30) {
        //     std::cout << "Mouse solver took longer than 30s and gave up" << std::endl;
        //     solved = true;
        //     mouseSolving=false;
        // }
    }
    
    if (!mouseSolving) {
        std::cout << "Took:" << time(nullptr)-functionTimeSeconds << "s || " 
        << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - functionTimeMiliSeconds 
        << "ms" << std::endl;
        solved = true;
    }

}

bool GridMaze::Draw(int frameCount) {
    BeginDrawing();
    DrawElement nextElement;

    while (drawBuffer.size() != 0) {
        nextElement = drawBuffer.front();
        drawBuffer.pop();
        DrawRectangle(nextElement.x*gridSize, nextElement.y*gridSize, gridSize, gridSize, nextElement.colour);
    }

    if (frameCount >= 0 && drawFrequency != 0 && frameCount % drawFrequency == 0) {
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

bool GridMaze::InBound(Vec2 pos) {
    return (pos.x >= 0 && pos.x < width && pos.y >= 0 && pos.y < height);
}

// 0  1  2  3  4
// 5  6  7  8  9
// 10 11 12 13 14
// 15 16 17 18 19
// 20 21 22 23 24
// id: 1=GridDFS 2=LWDFS
// 3= LTDFS 4= URDL
bool* GridMaze::GetNeighbours(Vec2 pos, bool generating, int id) {
    bool* neighbours = new bool[25]{false};

    int size = 0;
    const int* shape;
    switch (id) {
        case 0:
            size = sizeof(GRIDDFS) / sizeof(GRIDDFS[0]);
            shape = GRIDDFS;
            break;
        case 1:
            size = sizeof(LWDFS) / sizeof(LWDFS[0]);
            shape = LWDFS;
            break;
        case 2:
            size = sizeof(LTDFS) / sizeof(LTDFS[0]);
            shape = LTDFS;
            break;
        case 3:
            size = sizeof(URDL) / sizeof(URDL[0]);
            shape = URDL;
            break;
        default:
            size = sizeof(ALL)/ sizeof(ALL[0]);
            shape = ALL;
    }

    // std::cout << size << std::endl;
    for (int i = 0; i < size; i++) {
        int y = shape[i]/5 - 2;
        int x = shape[i]%5 - 2;
        // std::cout << "( " << x << "," << y << ") ";
        if (InBound(Vec2(pos.x + x, pos.y + y))) {
            if (generating) neighbours[shape[i]] = !grid[pos.x + x][pos.y + y];
            else neighbours[shape[i]] = grid[pos.x + x][pos.y + y];
        }
    }

    // for (int i = 0; i < 25; i++) {
    //     if (i%5==0) std::cout<<std::endl;
    //     std::cout << (neighbours[i] ? "□" : "■");
    // }


    return neighbours;
}

void GridMaze::GridStart() {
    stack.pop();
    if (start.x == 0 || start.x == width-1) {
        if (start.y % 2 != 1) start.y -= 1;
        if (start.x == 0) stack.push(Vec2(1,start.y));
        else stack.push(Vec2(width-2, start.y));
    } else {
        if (start.x % 2 != 1) start.x -= 1;
        if (start.y == 0) stack.push(Vec2(start.x,1));
        else stack.push(Vec2(start.x,height-2));
    }
    grid[start.x][start.y] = true;
}

void GridMaze::GenFinish() {
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

bool GridMaze::GenGridDFS() {
    if (stack.size() == 0) return false;
    else if (stack.size() == 1) GridStart();

    Vec2 pos = stack.top();
    grid[pos.x][pos.y] = true;

    bool* neighbours = GetNeighbours(pos,true,0);
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

bool GridMaze::GenLooseWideDFS() {
    if (stack.size() == 0) return false;

    Vec2 pos = stack.top();
    grid[pos.x][pos.y] = true;

    bool* neighbours = GetNeighbours(pos,true,1);
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

bool GridMaze::GenLooseTightDFS() {
    if (stack.size() == 0) return false;

    Vec2 pos = stack.top();
    grid[pos.x][pos.y] = true;

    bool* neighbours = GetNeighbours(pos,true,2);
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

bool GridMaze::GenKruskals() {
    if (stack.size() == 1) {
        GridStart();
        stack.pop();

        int setID = 0;

        for (int x = 0; x < width; x++) {
            if (x == 0 || x == width-1) continue;
            for (int y = 1; y < height-1; y++) {
                if (x%2 == 1 && y%2 == 1) {
                    ComplexGrid[x][y].id = setID++;
                    ComplexGrid[x][y].parent = &ComplexGrid[x][y];
                } else if (!(x%2 == 0 && y%2 == 0) && !(x%2 == 1 && y%2 == 1)) {
                    vector.push_back(Vec2(x,y));
                }
            }
        }

        std::shuffle(vector.begin(), vector.end(), std::default_random_engine());
    }

    if (vector.size() == 0) return false;

    Vec2 pos = vector.back();
    vector.pop_back();

    int x = pos.x;
    int y = pos.y;
    
    ComplexSquare* firstSquare;
    ComplexSquare* secondSquare;

    if (x % 2 == 0) {
        firstSquare = ComplexGrid[x-1][y].getParent();
        secondSquare = ComplexGrid[x+1][y].getParent();
    } else if (y % 2 == 0) {
        firstSquare = ComplexGrid[x][y-1].getParent();
        secondSquare = ComplexGrid[x][y+1].getParent();
    }
    
    if (firstSquare->parent->id != secondSquare->parent->id) {
        secondSquare->updateParent(firstSquare);
        // std::cout << " Updated: 1:" << firstSquare->parent->id << " 2:" << secondSquare->parent->id;
        for (int i = -1; i <= 1; i++) {
            if (x%2==0) {
                if (!grid[x+i][y]) {
                    grid[x+i][y] = true;
                    drawQueue.push(DrawElement(x+i,y,WHITE));
                }
            } else {
                if (!grid[x][y+i]) {
                    grid[x][y+i] = true;
                    drawQueue.push(DrawElement(x,y+i,WHITE));
                }
            }
            
        }
    }

    return true;
}

bool GridMaze::GenPrims() {
    if (stack.size() != 0) {
        GridStart();
        Vec2 top = stack.top();
        grid[top.x][top.y] = false;
        stack.pop();

        // complexVector.push_back(new ComplexSquare(top.x,top.y));

        int randx = (rand() % ((width-3)/2))+1;
        int randy = (rand() % ((height-3)/2))+1;
        if (randx%2==0) randx++;
        if (randy%2==0) randy++;
        complexVector.push_back(new ComplexSquare(randx,randy));
        
    }

    if (complexVector.size() == 0) return false;

    int randID = rand() % complexVector.size();
    ComplexSquare* pos = complexVector.at(randID);
    complexVector.erase(complexVector.begin() + randID);

    if (grid[pos->x][pos->y]) return true;

    grid[pos->x][pos->y] = true;
    

    if (pos->parent != nullptr) {
        if (pos->x > pos->parent->x) {
            grid[pos->x-1][pos->y] = true;
            drawQueue.push(DrawElement(pos->x-1,pos->y,WHITE));
        }
        if (pos->x < pos->parent->x) {
            grid[pos->x+1][pos->y] = true;
            drawQueue.push(DrawElement(pos->x+1,pos->y,WHITE));
        }
        if (pos->y > pos->parent->y) {
            grid[pos->x][pos->y-1] = true;
            drawQueue.push(DrawElement(pos->x,pos->y-1,WHITE));
        }
        if (pos->y < pos->parent->y) {
            grid[pos->x][pos->y+1] = true;
            drawQueue.push(DrawElement(pos->x,pos->y+1,WHITE));
        }
    }

    drawQueue.push(DrawElement(pos->x,pos->y,WHITE));

    bool* neighbours = GetNeighbours(Vec2(pos->x,pos->y),true,0);
    if (neighbours[2]) complexVector.push_back(new ComplexSquare(pos->x,pos->y-2,pos));
    if (neighbours[10]) complexVector.push_back(new ComplexSquare(pos->x-2,pos->y,pos));
    if (neighbours[14]) complexVector.push_back(new ComplexSquare(pos->x+2,pos->y,pos));
    if (neighbours[22]) complexVector.push_back(new ComplexSquare(pos->x,pos->y+2,pos));

    return true;
}

bool GridMaze::GenRecusiveDivision(Vec2 topLeft, Vec2 bottomRight) {
    if (stack.size() != 0) {
        GridStart();
        Vec2 top = stack.top();
        grid[top.x][top.y] = false;
        stack.pop();    
        
        for (int x = 1; x < width-1; x++) {
            for (int y = 1; y < height-1; y++) {
                grid[x][y] = true;
            }
        }
        topLeft = Vec2(1,1);
        bottomRight = Vec2(width-2,height-2);
    }

    if (topLeft.x == bottomRight.x || topLeft.y == bottomRight.y) return false;

    int chamberWidth = bottomRight.x-topLeft.x;
    int chamberHeight = bottomRight.y-topLeft.y;

    int xWall = (rand()%chamberWidth) / 2 * 2 + 1;
    int yWall = (rand()%chamberHeight) / 2 * 2 + 1;

    for (int stepY = topLeft.y; stepY <= bottomRight.y; stepY++) {
        if (stepY == 0) {
            PrintToConsole();
            std::cout << "topLeft (" << topLeft.x << "," << topLeft.y << ") | bottomRight (" << bottomRight.x << "," << bottomRight.y << ")" << std::endl;
            std::cout << "xWall:" << xWall << "  yWall:" << yWall << std::endl;
            std::cout << "Break Here" << std::endl;
        }
        drawQueue.push(DrawElement(topLeft.x + xWall, stepY, BLACK));
        grid[topLeft.x + xWall][stepY] = false;
    }

    for (int stepX = topLeft.x; stepX <= bottomRight.x; stepX++) {
        if (topLeft.y+yWall == 0) {
            PrintToConsole();
            std::cout << "topLeft (" << topLeft.x << "," << topLeft.y << ") | bottomRight (" << bottomRight.x << "," << bottomRight.y << ")" << std::endl;
            std::cout << "xWall:" << xWall << "  yWall:" << yWall << std::endl;
            std::cout << "Break Here" << std::endl;
        }
        drawQueue.push(DrawElement(stepX, topLeft.y + yWall, BLACK));
        grid[stepX][topLeft.y+yWall] = false;
    }

    int randNoWallBreak = rand()%4;
    int wallBreak = 0;
    // Left of xWall
    if (randNoWallBreak != 0) {
        wallBreak = (rand() % xWall) / 2 * 2;
        drawQueue.push(DrawElement(topLeft.x + wallBreak, topLeft.y + yWall, WHITE));
        grid[topLeft.x + wallBreak][topLeft.y + yWall] = true;
    }
    // Right of xWall
    if (randNoWallBreak != 1) {
        wallBreak = (rand() % (chamberWidth-xWall)) / 2 * 2 + xWall + 1;
        drawQueue.push(DrawElement(topLeft.x + wallBreak, topLeft.y + yWall, WHITE));
        grid[topLeft.x + wallBreak][topLeft.y + yWall] = true;
    }
    // Up from yWall
    if (randNoWallBreak != 2) {
        wallBreak = (rand() % yWall) / 2 * 2;
        drawQueue.push(DrawElement(topLeft.x + xWall, topLeft.y + wallBreak, WHITE));
        grid[topLeft.x + xWall][topLeft.y + wallBreak] = true;
    }
    // Down from yWall
    if (randNoWallBreak != 3) {
        wallBreak = (rand() % (chamberHeight - yWall)) / 2 * 2 + yWall + 1;
        drawQueue.push(DrawElement(topLeft.x + xWall, topLeft.y + wallBreak, WHITE));
        grid[topLeft.x + xWall][topLeft.y + wallBreak] = true;
    }
    // Recursive calling
    // TopLeft
    GenRecusiveDivision(topLeft, Vec2(topLeft.x + xWall - 1, topLeft.y + yWall - 1));
    // TopRight
    GenRecusiveDivision(Vec2(topLeft.x + xWall + 1, topLeft.y), Vec2(bottomRight.x, topLeft.y + yWall - 1));
    // Bottom Left
    GenRecusiveDivision(Vec2(topLeft.x, topLeft.y + yWall + 1), Vec2(topLeft.x + xWall - 1, bottomRight.y));
    // Bottom Right
    GenRecusiveDivision(Vec2(topLeft.x + xWall + 1, topLeft.y + yWall + 1), bottomRight);

    return false;
}

bool GridMaze::GenWilsons() {
    if (stack.size() == 1 && stack.top().x == start.x && stack.top().y == start.y) {
        GridStart();
        stack.pop();
        for (int x = 1; x < width; x+=2) {
            for (int y = 1; y < height; y+=2) {
                vector.push_back(Vec2(x,y));
            }
        }

        std::shuffle(vector.begin(), vector.end(), std::default_random_engine());
        while(vector.size() > 0) {
            queue.push(vector.back());
            vector.pop_back();
        }

        if (queue.size() >= 50000) {
            std::cout<<"Quickening Wilsons"<<std::endl;
            wilsonsQuicken = true;
        } else {
            std::cout<<"Regular Wilsons"<<std::endl;
            wilsonsQuicken = false;
            int randx = rand()%(width/2)*2+1;
            int randy = rand()%(height/2)*2+1;

            drawQueue.push(DrawElement(randx, randy, WHITE));
            grid[randx][randy] = true;
        }
    }

    if (vector.size() == 0) {

        if (queue.size() == 0) return false;

        Vec2 pos = queue.front();
        queue.pop();

        if (grid[pos.x][pos.y]) return true;

        vector.push_back(pos);
        drawQueue.push(DrawElement(pos.x,pos.y,LIME));
    } else {
        Vec2 pos = vector.back();

        std::vector<Vec2>Directions = {Vec2(-1,0),Vec2(0,-1),Vec2(1,0),Vec2(0,1)};
        Vec2 randDir = Directions.at(rand()%4);

        if (!InBound(Vec2(randDir.x+randDir.x+pos.x,randDir.y+randDir.y+pos.y))) {
            return GenWilsons();
        }

        pos.x += randDir.x;
        pos.y += randDir.y;
        if (vector.size() > 2 && vector.at(vector.size()-2).x == pos.x && vector.at(vector.size()-2).y == pos.y) {
            return GenWilsons();
        }
        vector.push_back(pos);
        drawQueue.push(DrawElement(pos.x,pos.y,GRAY));
        pos.x += randDir.x;
        pos.y += randDir.y;
        drawQueue.push(DrawElement(pos.x,pos.y,BLUE));
        if (wilsonsQuicken && (time(nullptr)-functionTimeSeconds)==4) {
            std::cout<<"Quickened Wilsons"<<std::endl;
            wilsonsQuicken = false;
            grid[pos.x][pos.y] = true;
        }
        bool loop = false;

        if (grid[pos.x][pos.y]) {
            drawQueue.push(DrawElement(pos.x,pos.y,WHITE));
            while(vector.size() > 0) {
                pos = vector.back();
                vector.pop_back();
                grid[pos.x][pos.y] = true;
                drawQueue.push(DrawElement(pos.x,pos.y,WHITE));
            }
            return true; 
        }

        for(int i = 0; i < vector.size(); i+=2) {
            if (pos.x == vector.at(i).x && pos.y == vector.at(i).y) {
                Vec2 back;
                while (vector.size() >= i) {
                    if (vector.size() == 1) break;
                    back = vector.back();
                    drawQueue.push(DrawElement(back.x,back.y,BLACK));
                    vector.pop_back();
                }
                loop=true;
            }
        }

        if (!loop) {
            vector.push_back(pos);
            drawQueue.push(DrawElement(pos.x,pos.y,GRAY));
        }
    }
    
    return true;
}

bool GridMaze::GenFractals() {
    if (stack.size() != 0) {
        // GridStart();
        stack.pop();
    }
    
    if (vector.size() == 0) {
        // Base case 3x3
        for (int x = 1; x <= 3; x+=2) {
            for (int y = 1; y <= 3; y+=2) {
                grid[x][y] = true;
                drawQueue.push(DrawElement(x, y, WHITE));
            }
        }

        int randID = rand()%4;
        if (randID != 0) {
            grid[1][2] = true;
            drawQueue.push(DrawElement(1, 2, WHITE));
        }
        if (randID != 1) {
            grid[3][2] = true;
            drawQueue.push(DrawElement(3, 2, WHITE));
        }
        if (randID != 2) {
            grid[2][1] = true;
            drawQueue.push(DrawElement(2, 1, WHITE));
        }
        if (randID != 3) {
            grid[2][3] = true;
            drawQueue.push(DrawElement(2, 3, WHITE));
        }

        vector.push_back(Vec2(3,3));
    } else if (vector.size() == 1) {
        std::cout<<"Fractal Extender"<<std::endl;
        Vec2 size = vector.back();
        vector.pop_back();

        // Copy Right
        for (int x = 1; x <= size.x; x++) {
            for (int y = 1; y <= size.y; y++) {
                if (grid[x][y]) {
                    grid[x+size.x+1][y] = true;
                    drawQueue.push(DrawElement(x+size.x+1, y, WHITE));
                }
            }
        }
        // Copy Down
        for (int x = 1; x < (size.x+1)*2; x++) {
            for (int y = 1; y <= size.y; y++) {
                if (grid[x][y]) {
                    grid[x][y+size.y+1] = true;
                    drawQueue.push(DrawElement(x, y+size.y+1, WHITE));
                }
            }
        }
        // Connect
        int randWall;
        int randID = rand()%4;
        // Top
        if (randID != 0) {
            randWall = (rand()%(size.x/2))*2+1;
            grid[size.x+1][randWall] = true;
            drawQueue.push(DrawElement(size.x+1,randWall,WHITE));
        }
        // Left
        if (randID != 1) {
            randWall = (rand()%(size.y/2))*2+1;
            grid[randWall][size.y+1] = true;
            drawQueue.push(DrawElement(randWall,size.y+1,WHITE));
        }
        // Right
        if (randID != 2) {
            randWall = (rand()%(size.y/2))*2+1;
            grid[size.x+1+randWall][size.y+1] = true;
            drawQueue.push(DrawElement(size.x+1+randWall,size.y+1,WHITE));
        }
        // Down
        if (randID != 3) {
            randWall = (rand()%(size.x/2))*2+1;
            grid[size.x+1][size.y+1+randWall] = true;
            drawQueue.push(DrawElement(size.x+1,size.y+1+randWall,WHITE));
        }

        if (!InBound(Vec2((size.x*2+1)*2+1,(size.y*2+1)*2+1))) {
            vector.push_back(size);
        }
        vector.push_back(Vec2(size.x*2+1,size.y*2+1));
        
    } else if (vector.size() == 2) {
        std::cout<<"Right space filler"<<std::endl;
        Vec2 size = vector.back();

        int maxFractleSize = 3;
        int remainingWidth = width-size.x-3;

        if (remainingWidth < 3) {
            std::cout<<"Right Space Filler Done"<<std::endl;
            vector.push_back(size);
            return true;
        }

        while (maxFractleSize <= remainingWidth) {
            maxFractleSize *= 2;
            maxFractleSize += 1;
        }
        maxFractleSize -= 1;
        maxFractleSize /= 2;
        
        int fractalCount = (size.y+1)/(maxFractleSize+1);
        int randWall;

        std::cout << size.y << "/" << maxFractleSize << "=" << fractalCount << std::endl;

        // Copy Side
        for (int i = 0; i < fractalCount; i++ ){
            for (int x = 1; x <= maxFractleSize; x++) {
                for (int y = 1; y <= maxFractleSize; y++) {
                    if (grid[x][y]) {
                        grid[x+size.x+1][y+(i*(maxFractleSize+1))] = true;
                        drawQueue.push(DrawElement(x+size.x+1, y+(i*(maxFractleSize+1)), WHITE));
                    }
                }
            }
            randWall = (rand()%(maxFractleSize/2))*2+1;
            grid[1+size.x][randWall+((i*(maxFractleSize+1)))] = true;
            drawQueue.push(DrawElement(1+size.x,randWall+((i*(maxFractleSize+1))), WHITE));
        }

        vector.pop_back();
        vector.push_back(Vec2(size.x+maxFractleSize+1,size.y));
        
    } else if (vector.size() == 3) {
        std::cout<<"Down Space Filler"<<std::endl;
        Vec2 size = vector.back();

        int maxFractleSize = 3;
        int remainingHeight = height-size.y-3;

        if (remainingHeight < 3) {
            std::cout<<"Done"<<std::endl;
            GenFinish();
            return false;
        }

        while (maxFractleSize <= remainingHeight) {
            maxFractleSize *= 2;
            maxFractleSize += 1;
        }
        maxFractleSize -= 1;
        maxFractleSize /= 2;
        
        int fractalCount = (size.x+1)/(maxFractleSize+1);
        int randWall;

        std::cout << size.x << "/" << maxFractleSize << "=" << fractalCount << std::endl;

        // Copy Side
        for (int i = 0; i < fractalCount; i++ ){
            for (int x = 1; x <= maxFractleSize; x++) {
                for (int y = 1; y <= maxFractleSize; y++) {
                    if (grid[x][y]) {
                        grid[x+(i*(maxFractleSize+1))][y+size.y+1] = true;
                        drawQueue.push(DrawElement(x+(i*(maxFractleSize+1)), y+size.y+1, WHITE));
                    }
                }
            }
            randWall = (rand()%(maxFractleSize/2))*2+1;
            grid[randWall+((i*(maxFractleSize+1)))][size.y+1] = true;
            drawQueue.push(DrawElement(randWall+((i*(maxFractleSize+1))),1+size.y, WHITE));
        }

        vector.pop_back();
        vector.push_back(Vec2(size.x,size.y+maxFractleSize+1));
    }

    return true;
}

bool GridMaze::SolveDFS() {
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
        // drawQueue.push(DrawElement(randDir,ORANGE));
        stack.push(randDir);
    }

    return true;
}

bool GridMaze::SolveBFS() {
    if (stack.size() != 0) {
        queue.push(Vec2(stack.top()));
        stack.pop();
    }

    if (queue.size() == 0) return false;

    Vec2 pos = queue.front();
    queue.pop();
    grid[pos.x][pos.y] = false;
    // BFSIndex[pos.x][pos.y] = pos.id;

    // drawQueue.push(DrawElement(pos,ORANGE));
    drawQueue.push(DrawElement(pos,YELLOW));

    if (pos.x == finish.x && pos.y == finish.y) {
        pos = ComplexGrid[pos.x][pos.y].GetParentVec();
        while (!(pos.x == 0 || pos.x == width-1) && !(pos.y == 0 || pos.y == height-1)) {
            // std::cout<<"(" << pos.x << "," << pos.y << ") : ";
            drawQueue.push(ComplexGrid[pos.x][pos.y].GetDraw());
            pos = ComplexGrid[pos.x][pos.y].GetParentVec();
        }
        // std::cout<<std::endl<<std::endl;
        return false;
    }

    bool* neighbours = GetNeighbours(pos, false);
    if (neighbours[7]) {
        queue.push(Vec2(pos.x,pos.y-1));
        ComplexGrid[pos.x][pos.y-1].parent = &ComplexGrid[pos.x][pos.y];
    }
    if (neighbours[11]) {
        queue.push(Vec2(pos.x-1,pos.y));
        ComplexGrid[pos.x-1][pos.y].parent = &ComplexGrid[pos.x][pos.y];
    }
    if (neighbours[13]) {
        queue.push(Vec2(pos.x+1,pos.y));
        ComplexGrid[pos.x+1][pos.y].parent = &ComplexGrid[pos.x][pos.y];
    }
    if (neighbours[17]) {
        queue.push(Vec2(pos.x,pos.y+1));
        ComplexGrid[pos.x][pos.y+1].parent = &ComplexGrid[pos.x][pos.y];
    }

    return true;

}

bool GridMaze::SolveMouse() {
    if (stack.size() != 0) {
        ResetComplexGrid();
        mouse.SetPos(start);
        stack.pop();
    }

    if (mouse.x == finish.x && mouse.y == finish.y) return false;

    if (!(mouse.x == start.x && mouse.y == start.y)) {
        unsigned char step = 1, start = 40;
        Color mousePath = ComplexGrid[mouse.x][mouse.y].gridColour;
        if (mousePath.r == WHITE.r && mousePath.g == WHITE.g && mousePath.b == WHITE.b) {
            mousePath = CLITERAL(Color){ start, 0, 0, 255 };
        } else if (mousePath.r != 0) {
            mousePath.r+=step;
            if (mousePath.r < start) {
                mousePath.r = 0;
                mousePath.b = start;
            }
        }   else if (mousePath.b != 0) {
            mousePath.b+=step;
            if (mousePath.b < start) {
                mousePath.b = 0;
                mousePath.g = start;
            }
        }else if (mousePath.g != 0) {
            mousePath.g+=step;
            if (mousePath.g < start) {
                mousePath.g = 0;
                mousePath.r = start;
            }
        }
        ComplexGrid[mouse.x][mouse.y].gridColour = mousePath;
    
        drawQueue.push(DrawElement(mouse.x, mouse.y, mousePath));

    }

    if (!mouse.up && InBound(Vec2(mouse.x,mouse.y-1)) && grid[mouse.x][mouse.y-1]) {
        vector.push_back(Vec2(0,-1));
    }
    if (!mouse.down && InBound(Vec2(mouse.x,mouse.y+1)) && grid[mouse.x][mouse.y+1]) {
        vector.push_back(Vec2(0,1));
    }
    if (!mouse.left && InBound(Vec2(mouse.x-1,mouse.y)) && grid[mouse.x-1][mouse.y]) {
        vector.push_back(Vec2(-1,0));
    }
    if (!mouse.right && InBound(Vec2(mouse.x+1,mouse.y)) && grid[mouse.x+1][mouse.y]) {
        vector.push_back(Vec2(1,0));
    }

    if (vector.size() == 0) {
        if (mouse.up) mouse.MoveUp();
        else if (mouse.down) mouse.MoveDown();
        else if (mouse.left) mouse.MoveLeft();
        else if (mouse.right) mouse.MoveRight();
    } else {
        int rndID = rand() % vector.size();
        Vec2 dir = vector.at(rndID);
        vector.clear();
        if (dir.x == 1) mouse.MoveRight();
        else if (dir.x == -1) mouse.MoveLeft();
        else if (dir.y == 1) mouse.MoveDown();
        else if (dir.y == -1) mouse.MoveUp();
    }

    if (!(mouse.x == start.x && mouse.y == start.y)) {
        if (width*height < 500) {
            drawQueue.push(DrawElement(mouse.x, mouse.y, GRAY));
        }
    }

    return true;
}

bool GridMaze::SolveRHWall() {
    if (stack.size() != 0) {
        mouse.SetPos(start);
        stack.pop();
    }

    if (mouse.x == finish.x && mouse.y == finish.y) {
        return false;
    };


    if (mouse.up) {
        // x L x
        // 1 x 3
        // x 2 x
        if (InBound(Vec2(mouse.x-1,mouse.y)) && grid[mouse.x-1][mouse.y]) {
            mouse.MoveLeft();
            // std::cout << "Moved Left" << std::endl;
        } else if (InBound(Vec2(mouse.x,mouse.y+1)) && grid[mouse.x][mouse.y+1]) {
            mouse.MoveDown();
            // std::cout << "Moved Down" << std::endl;
        } else if (InBound(Vec2(mouse.x+1,mouse.y)) && grid[mouse.x+1][mouse.y]) {
            mouse.MoveRight();
            // std::cout << "Moved Right" << std::endl;
        } else {
            mouse.MoveUp();
            // std::cout << "Moved Up" << std::endl;
        }
    } else if (mouse.right) {
        // x 1 x
        // 2 x L
        // x 3 x
        if (InBound(Vec2(mouse.x,mouse.y-1)) && grid[mouse.x][mouse.y-1]) {
            mouse.MoveUp();
            // std::cout << "Moved Up" << std::endl;
        } else if (InBound(Vec2(mouse.x-1,mouse.y)) && grid[mouse.x-1][mouse.y]) {
            mouse.MoveLeft();
            // std::cout << "Moved Left" << std::endl;
        } else if (InBound(Vec2(mouse.x,mouse.y+1)) && grid[mouse.x][mouse.y+1]) {
            mouse.MoveDown();
            // std::cout << "Moved Down" << std::endl;
        } else {
            mouse.MoveRight();
            // std::cout << "Moved Right" << std::endl;
        }
    } else if (mouse.down) {
        // x 2 x
        // 3 x 1
        // x L x
        if (InBound(Vec2(mouse.x+1,mouse.y)) && grid[mouse.x+1][mouse.y]) {
            mouse.MoveRight();
            // std::cout << "Moved Right" << std::endl;
        } else if (InBound(Vec2(mouse.x,mouse.y-1)) && grid[mouse.x][mouse.y-1]) {
            mouse.MoveUp();
            // std::cout << "Moved Up" << std::endl;
        } else if (InBound(Vec2(mouse.x-1,mouse.y)) && grid[mouse.x-1][mouse.y]) {
            mouse.MoveLeft();
            // std::cout << "Moved Left" << std::endl;
        } else {
            mouse.MoveDown();
            // std::cout << "Moved Down" << std::endl;
        }
    } else if (mouse.left) {
        // x 3 x
        // L x 2
        // x 1 x
        if (InBound(Vec2(mouse.x,mouse.y+1)) && grid[mouse.x][mouse.y+1]) {
            mouse.MoveDown();
            // std::cout << "Moved Down" << std::endl;
        } else if (InBound(Vec2(mouse.x+1,mouse.y)) && grid[mouse.x+1][mouse.y]) {
            mouse.MoveRight();
            // std::cout << "Moved Right" << std::endl;
        } else if (InBound(Vec2(mouse.x,mouse.y-1)) && grid[mouse.x][mouse.y-1]) {
            mouse.MoveUp();
            // std::cout << "Moved Up" << std::endl;
        } else {
            mouse.MoveLeft();
            // std::cout << "Moved Left" << std::endl;
        }
        
    }

    drawQueue.push(DrawElement(Vec2(mouse.x,mouse.y), YELLOW));
    // Draw(drawFrequency);

    return true;
}

bool GridMaze::SolveLHWall() {
    if (stack.size() != 0) {
        // ResetComplexGrid();
        mouse.SetPos(start);
        stack.pop();
    }

    if (mouse.x == finish.x && mouse.y == finish.y) return false;

    if (mouse.up) {
        // x L x
        // 3 x 1
        // x 2 x
        if (InBound(Vec2(mouse.x+1,mouse.y)) && grid[mouse.x+1][mouse.y]) {
            mouse.MoveRight();
        } else if (InBound(Vec2(mouse.x,mouse.y+1)) && grid[mouse.x][mouse.y+1]) {
            mouse.MoveDown();
        } else if (InBound(Vec2(mouse.x-1,mouse.y)) && grid[mouse.x-1][mouse.y]) {
            mouse.MoveLeft();
        } else {
            mouse.MoveUp();
        }
    } else if (mouse.right) {
        // x 3 x
        // 2 x L
        // x 1 x
        if (InBound(Vec2(mouse.x,mouse.y+1)) && grid[mouse.x][mouse.y+1]) {
            mouse.MoveDown();
        } else if (InBound(Vec2(mouse.x-1,mouse.y)) && grid[mouse.x-1][mouse.y]) {
            mouse.MoveLeft();
        } else if (InBound(Vec2(mouse.x,mouse.y-1)) && grid[mouse.x][mouse.y-1]) {
            mouse.MoveUp();
        } else {
            mouse.MoveRight();
        }
    } else if (mouse.down) {
        // x 2 x
        // 1 x 3
        // x L x
        if (InBound(Vec2(mouse.x-1,mouse.y)) && grid[mouse.x-1][mouse.y]) {
            mouse.MoveLeft();
        } else if (InBound(Vec2(mouse.x,mouse.y-1)) && grid[mouse.x][mouse.y-1]) {
            mouse.MoveUp();
        } else if (InBound(Vec2(mouse.x+1,mouse.y)) && grid[mouse.x+1][mouse.y]) {
            mouse.MoveRight();
        } else {
            mouse.MoveDown();
        }
    } else if (mouse.left) {
        // x 1 x
        // L x 2
        // x 3 x
        if (InBound(Vec2(mouse.x,mouse.y-1)) && grid[mouse.x][mouse.y-1]) {
            mouse.MoveUp();
        } else if (InBound(Vec2(mouse.x+1,mouse.y)) && grid[mouse.x+1][mouse.y]) {
            mouse.MoveRight();
        } else if (InBound(Vec2(mouse.x,mouse.y+1)) && grid[mouse.x][mouse.y+1]) {
            mouse.MoveDown();
        } else {
            mouse.MoveLeft();
        }
        
    }

    drawQueue.push(DrawElement(Vec2(mouse.x,mouse.y), ORANGE));


    return true;
}