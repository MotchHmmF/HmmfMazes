#pragma once

#include <raylib.h>
#include <iostream>
#include <stack>
#include <vector>
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <bits/stdc++.h>

class Vec2 {
    public:
        Vec2() : x(0), y(0) {};
        Vec2(int x, int y) : x(x), y(y) {};
        int x, y;
};

class DrawElement {
    public:
        DrawElement() : x(0), y(0), colour(BLACK) {};
        DrawElement(int x, int y, Color colour = BLACK) : x(x), y(y), colour(colour) {};
        DrawElement(Vec2 vec, Color colour = BLACK) : x(vec.x), y(vec.y), colour(colour) {};

        int x, y;
        Color colour;
};

class KruskalsSquare {
    public:
        KruskalsSquare() {};
        KruskalsSquare(int x, int y, int id) : x(x), y(y), id(id) {};

        bool operator==(const KruskalsSquare& other) const {
            return x == other.x && y == other.y;
        }

        void updateParent(KruskalsSquare* NewParent) {
            parent = NewParent;
            for (KruskalsSquare* child : children) {
                child->updateParent(NewParent);
            }
            while (children.size() != 0) children.pop_back();
        }

        int x = -1, y = -1, id = -1;
        KruskalsSquare* parent = nullptr;
        std::vector<KruskalsSquare*> children;
};


class Maze {
    public:
        Maze(int width=11, int height=11, int gridSize=10, int seed=static_cast<int>(time(nullptr)));
        
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

        // std::unordered_set<int>* KruskalsSets;
        KruskalsSquare** KruskalsGrid;

        bool generated = false, solved = false;
        int drawFrequency = 1, drawPerFrame = 5;
        std::queue<DrawElement> drawQueue;
        std::queue<DrawElement> drawBuffer;

        void Reset();
        void PrintToConsole();

        void Generate();
        void Solve();
        bool Draw(int frameCount);

        bool InBound(Vec2 pos);
        bool* GetNeighbours(Vec2 pos, bool generating = true);

        void GridStart();
        void GenFinish();

        bool GenGridDFS();
        bool GenLooseWideDFS();
        bool GenLooseTightDFS();
        bool GenKruskals();

        bool SolveDFS();
        bool SolveBFS();
   
};

// 0  1  2  3  4
// 5  6  7  8  9
// 10 11 12 13 14
// 15 16 17 18 19
// 20 21 22 23 24



