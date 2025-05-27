#pragma once

class Vec2 {
    public:
        Vec2() : x(0), y(0) {};
        Vec2(int x, int y) : x(x), y(y) {};
        int x, y;
};

class Vec3 {
    public:
        Vec3() : x(0), y(0), id(0) {};
        Vec3(Vec2 v, int id) : x(v.x), y(v.y), id(id) {};
        Vec3(int x, int y, int id) : x(x), y(y), id(id) {};
        int x, y, id;
};