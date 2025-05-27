#pragma once

#include <raylib.h>
#include <iostream>
#include <stack>
#include <vector>
#include <queue>
#include <algorithm>
#include <random>
#include <chrono>
// #include <bits/stdc++.h>
#include "vectors.h"

class DrawElement {
    public:
        DrawElement() : x(0), y(0), colour(BLACK) {};
        DrawElement(int x, int y, Color colour = BLACK) : x(x), y(y), colour(colour) {};
        DrawElement(Vec2 vec, Color colour = BLACK) : x(vec.x), y(vec.y), colour(colour) {};
        DrawElement(Vec3 vec, Color colour = BLACK) : x(vec.x), y(vec.y), colour(colour) {};

        int x, y;
        Color colour;
};

class ComplexSquare {
    public:
        ComplexSquare() {};
        ComplexSquare(int x, int y) : x(x), y(y) {};
        ComplexSquare(int x, int y, int id) : x(x), y(y), id(id) {};
        ComplexSquare(int x, int y, ComplexSquare* parent) : x(x), y(y), parent(parent) {};

        ComplexSquare* getParent() {
            if (parent->id == id) return parent;

            ComplexSquare* higherParent = parent->getParent();
            if (higherParent->id != parent->id) {
                parent = higherParent;
            }
            return parent;
        }

        DrawElement GetDraw(Color colour = ORANGE) {
            return DrawElement(x,y,colour);
        }

        Vec2 GetParentVec() {
            return Vec2(parent->x,parent->y);
        }

        void updateParent(ComplexSquare* NewParent) {
            parent = NewParent;
        }

        int x = -1, y = -1, id = -1;
        ComplexSquare* parent = nullptr;
        Color gridColour = WHITE;
};

class Mouse {
    public:
        Mouse() {};

        void SetPos(Vec2 pos) {
            x = pos.x;
            y = pos.y;
            up = false;
            right = false;
            left = false;
            down = true;
        };
        void MoveUp() {
            y--;
            up = false;
            right = false;
            left = false;
            down = true;
        };
        void MoveDown() {
            y++;
            up = true;
            right = false;
            left = false;
            down = false;
        };
        void MoveLeft() {
            x--;
            up = false;
            right = true;
            left = false;
            down = false;
        };
        void MoveRight() {
            x++;
            up = false;
            right = false;
            left = true;
            down = false;
        };
        int x = -1, y = -1;
        bool up = true, right = false, down = false, left = false;
};

class GridMaze {
    public:
        GridMaze(int width=11, int height=11, int gridSize=10, int seed=static_cast<int>(time(nullptr)));
        
        void Start();
    
    private:
        int width, height, gridSize;
        bool** grid;
        Vec2 start;
        Vec2 finish;

        unsigned int seed;

        std::stack<Vec2> stack;
        std::queue<Vec2> queue;
        std::vector<Vec2> vector;
        std::vector<ComplexSquare*> complexVector;

        ComplexSquare** ComplexGrid;
        Mouse mouse;
        
        time_t functionTimeSeconds;
        long long functionTimeMiliSeconds;

        bool generated = false, solved = false, mouseSolving = false;
        int drawFrequency = 1, drawPerFrame = 1, pausedDrawFrequency = 0;
        std::queue<DrawElement> drawQueue;
        std::queue<DrawElement> drawBuffer;

        void Reset();
        void ResetScreen();
        void ResetComplexGrid();
        void PrintToConsole();

        void Generate();
        void Solve();
        bool Draw(int frameCount);

        bool InBound(Vec2 pos);
        bool* GetNeighbours(Vec2 pos, bool generating = true, int id = 3);

        void GridStart();
        void GenFinish();

        bool GenGridDFS();
        bool GenLooseWideDFS();
        bool GenLooseTightDFS();
        bool GenKruskals();
        bool GenPrims();

        bool SolveDFS();
        bool SolveBFS();
        bool SolveMouse();
        bool SolveRHWall();
        bool SolveLHWall();

        const int GRIDDFS[4] = {2, 10, 14, 22};
        const int LWDFS[20] = {1, 2, 3, 5, 6, 7, 8, 9, 10, 11, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23};
        const int LTDFS[12] = {2, 6, 7, 8, 10, 11, 13, 14, 16, 17, 18, 22};
        const int URDL[4] = {7, 11, 13, 17};
        const int ALL[25] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};

   
};

// 0  1  2  3  4
// 5  6  7  8  9
// 10 11 12 13 14
// 15 16 17 18 19
// 20 21 22 23 24



