#pragma once
#include <math.h>
#include <stdint.h>

class vector2i_t {
   public:
    int x;
    int y;
    vector2i_t() : x(0), y(0) {
    }
    vector2i_t(int x_, int y_) : x(x_), y(y_) {
    }
    vector2i_t(uint32_t x_, uint32_t y_) : x(x_), y(y_) {
    }
    bool operator==(const vector2i_t& rhs) {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const vector2i_t& rhs) {
        return x != rhs.x || y != rhs.y;
    }
    bool operator==(const vector2i_t& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    vector2i_t operator+(const vector2i_t& rhs) {
        return vector2i_t(x + rhs.x, y + rhs.y);
    }
    vector2i_t operator-(const vector2i_t& rhs) {
        return vector2i_t(x - rhs.x, y - rhs.y);
    }
    float distance(int x, int y) {
        float value_x = float(this->x) - x;
        float value_y = float(this->y) - y;
        return sqrt(value_x * value_x + value_y * value_y);
    }
    bool exact(int x, int y) {
        return this->x == x && this->y == y;
    }
    bool operator<(const vector2i_t& obj) const {
        return (x + 200 * y) < (obj.x + 200 * obj.y);
    }
    int pack(int width) {
        return this->x + this->y * width;
    }
    float px() {
        return x * 32.f + 16.f;
    }
    float py() {
        return y * 32.f + 16.f;
    }
};

class vector2_t {
   public:
    float x;
    float y;
    vector2_t() : x(0.f), y(0.f) {
    }
    vector2_t(float x, float y) : x(x), y(y) {
    }
    bool operator==(vector2_t& rhs) {
        return x == rhs.x && y == rhs.y;
    }
    bool operator==(const vector2_t& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    vector2_t operator+(const vector2_t& rhs) {
        return vector2_t(x + rhs.x, y + rhs.y);
    }
    vector2_t operator-(const vector2_t& rhs) {
        return vector2_t(x - rhs.x, y - rhs.y);
    }
    float distance(float x, float y) {
        float value_x = this->x - x;
        float value_y = this->y - y;
        return sqrt(value_x * value_x + value_y * value_y);
    }
    float pack(int width) {
        return this->x + this->y * width;
    }
    vector2i_t collapse(bool add_offset = true) {
        if (add_offset)
            return vector2i_t{ int32_t(floor((x + 8.f) / 32.f)), int32_t(floor((y + 8.f) / 32.f)) };
        else
            return vector2i_t{ int32_t(floorf(x / 32.f)), int32_t(floorf(y / 32.f)) };
    }
};

class vector3_t {
   public:
    float x;
    float y;
    float z;
    vector3_t() : x(0), y(0), z(0) {
    }
    vector3_t(float x, float y, float z) : x(x), y(y), z(z) {
    }
    bool operator==(vector3_t& rhs) {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
    bool operator==(const vector3_t& rhs) const {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
    vector3_t operator+(const vector3_t& rhs) {
        return vector3_t(x + rhs.x, y + rhs.y, z + rhs.z);
    }
    vector3_t operator-(const vector3_t& rhs) {
        return vector3_t(x - rhs.x, y - rhs.y, z - rhs.z);
    }
};

class vector3i_t {
   public:
    int x;
    int y;
    int z;
    vector3i_t() : x(0), y(0), z(0) {
    }
    vector3i_t(int x, int y, int z) : x(x), y(y), z(z) {
    }
    bool operator==(vector3i_t& rhs) {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
    bool operator==(const vector3i_t& rhs) const {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
    vector3i_t operator+(const vector3i_t& rhs) {
        return vector3i_t(x + rhs.x, y + rhs.y, z + rhs.z);
    }
    vector3i_t operator-(const vector3i_t& rhs) {
        return vector3i_t(x - rhs.x, y - rhs.y, z - rhs.z);
    }
};

class rect_t {
   public:
    float m_x;
    float m_y;
    float m_w;
    float m_h;
    rect_t() : m_x(0), m_y(0), m_w(0), m_h(0) {
    }
    rect_t(float x, float y, float w, float h) : m_x(x), m_y(y), m_w(w), m_h(h) {
    }
    rect_t(int x, int y) {
        m_x = x * 32.f;
        m_y = y * 32.f;
        m_w = 32.f;
        m_h = 32.f;
    }
    rect_t(float x, float y) : m_x(x), m_y(y), m_w(20.f), m_h(30.f) {
    }
    bool operator==(rect_t& rhs) {
        return m_x == rhs.m_x && m_y == rhs.m_y && m_w == rhs.m_w && m_h == rhs.m_h;
    }
    bool operator==(const rect_t& rhs) const {
        return m_x == rhs.m_x && m_y == rhs.m_y && m_w == rhs.m_w && m_h == rhs.m_h;
    }
    rect_t operator+(const rect_t& rhs) {
        return rect_t(m_x + rhs.m_x, m_y + rhs.m_y, m_w + rhs.m_w, m_h + rhs.m_h);
    }
    rect_t operator-(const rect_t& rhs) {
        return rect_t(m_x - rhs.m_x, m_y - rhs.m_y, m_w - rhs.m_w, m_h - rhs.m_h);
    }

    bool in_range(float val, float min, float max) {
        return (val >= min) && (val <= max);
    }

    bool collides(rect_t snd) {
        bool x_in = in_range(m_x, snd.m_x, snd.m_x + snd.m_w) || in_range(snd.m_x, m_x, m_x + m_w);
        bool y_in = in_range(m_y, snd.m_y, snd.m_y + snd.m_h) || in_range(snd.m_y, m_y, m_y + m_h);

        return x_in && y_in;
    }
};
class recti_t {
   public:
    int m_x;
    int m_y;
    int m_w;
    int m_h;
    recti_t() : m_x(0), m_y(0), m_w(0), m_h(0) {
    }
    recti_t(int x, int y, int w, int h) : m_x(x), m_y(y), m_w(w), m_h(h) {
    }
    bool operator==(recti_t& rhs) {
        return m_x == rhs.m_x && m_y == rhs.m_y && m_w == rhs.m_w && m_h == rhs.m_h;
    }
    bool operator==(const recti_t& rhs) const {
        return m_x == rhs.m_x && m_y == rhs.m_y && m_w == rhs.m_w && m_h == rhs.m_h;
    }
    recti_t operator+(const recti_t& rhs) {
        return recti_t(m_x + rhs.m_x, m_y + rhs.m_y, m_w + rhs.m_w, m_h + rhs.m_h);
    }
    recti_t operator-(const recti_t& rhs) {
        return recti_t(m_x - rhs.m_x, m_y - rhs.m_y, m_w - rhs.m_w, m_h - rhs.m_h);
    }
};
