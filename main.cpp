#/** * * * BME-VIK-MI_2018_02_08 * * *|1.HF| * * * * * * * * * *\
#*    _ _____   _____        __ _                              *
#*   (_)  __ \ / ____|      / _| |                             *
#*   |_| |__)| (___    ___ | |_| |___      ____ _ _ __ ___     *
#*   | |  _  / \___ \ / _ \|  _| __\ \ /\ / / _` | '__/ _ \    *
#*   | | | \ \ ____) | (_) | | | |_ \ V  V / (_| | | |  __/    *
#*   |_|_|  \_\_____/ \___/|_|  \__| \_/\_/ \__,_|_|  \___|    *
#*                                                             *
#*                   http://irsoftware.net                     *
#*                                                             *
#*              contact_adress: sk8Geri@gmail.com               *
#*                                                               *
#*       This file is a part of the work done by aFagylaltos.     *
#*         You are free to use the code in any way you like,      *
#*         modified, unmodified or copied into your own work.     *
#*        However, I would like you to consider the following:    *
#*                                                               *
#*  -If you use this file and its contents unmodified,         *
#*              or use a major part of this file,               *
#*     please credit the author and leave this note untouched.   *
#*  -If you want to use anything in this file commercially,      *
#*                please request my approval.                    *
#*                                                              *
#\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include <math.h>
#include <stdlib.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif // Win32 platform

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#define PI 3.141592653589793238462643383
#define NONE            0
#define HORIZONTALLY    1
#define VERTICALLY      2
#define BRICK           3
#define PLAYER          4
#define BALL            5

const int screenWidth = 600;
const int screenHeight = 600;
const uint numBricks = 6;
const int num_objects = 2 + numBricks + 1 + 1 + 1; //numWalls,numBricks,player,ball,n+1
const float radius = 1;
const int detail = 15;
const float corner_radius = 0.2f;

int index = 0;

struct Color {
    float r, g, b;

    Color() {
        r = g = b = 0;
    }

    Color(float r0, float g0, float b0) {
        r = r0;
        g = g0;
        b = b0;
    }
};

struct Margin {
    Margin() {}

    Margin(float x, float y, float w, float h) : x0(x), y0(y), w0(w), h0(h) {}

    float x0, y0, w0, h0;
} margins[4];

void drawCircle(float x, float y, float radius, Color color = Color(1, 1, 1)) {
    glColor3f(color.r, color.g, color.b);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= detail; i++) {
        float tmp = PI / 180.0f * 360.0f * ((float) i / (float) detail);
        glVertex2f(x + radius + radius * (float) cos(tmp), y + radius + radius * (float) sin(tmp));
    }
    glEnd();
}

void drawRectangle(float x, float y, float w, float h, Color color = Color(1, 1, 1)) {
    glColor3f(color.r, color.g, color.b);
    glBegin(GL_TRIANGLES);
    glVertex2f(x + 0.0f, y + 0.0f);
    glVertex2f(x + w, y + 0.0f);
    glVertex2f(x + w, y + h);

    glVertex2f(x + 0.0f, y + h);
    glVertex2f(x + 0.0f, y + 0.0f);
    glVertex2f(x + w, y + h);
    glEnd();
}

bool collidingCircles(float x1, float y1, float radius1, float x2, float y2, float radius2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float szumma = radius1 + radius2;

    if ((dx * dx) + (dy * dy) < szumma * szumma)
        return true;
    else
        return false;
}

uint collidingAABB(float x, float y, float w, float h, float x0, float y0, float w0, float h0) {
    if (x0 + corner_radius < x + w && x0 + w0 - corner_radius > x && ((y0 + h0 > y) && (y0 < y + h))) {
        return HORIZONTALLY;
    } else if (y0 + corner_radius < y && y0 + h0 - corner_radius > y && ((x0 < x + w) && (x0 + w0 > x))) {
        return VERTICALLY;
    }
    return NONE;
}

class Object {
public:
    Object() {}

    Object(float x, float y, float w, float h, Color color, uint type, float vx = 0, float vy = 0)
            : x(x), y(y), w(w), h(h), color(color), type(type), vx(vx), vy(vy) {
    }

    float getX() { return x; }

    float getY() { return y; }

    float getW() { return w; }

    float getH() { return h; }

    uint getType() { return type; }

    void force(float valueX, float valueY) {
        vx *= sqrt(valueX);
        vy *= sqrt(valueY);
    }

    void move(float dx, float dy) {
        x += dx;
        y += dy;
    }

    void update(float dt) {
        move(vx * dt, vy * dt);
    }

    void draw() {
        if (type == BALL) {
            drawCircle(x, y, radius, color);
            return;
        }
        drawRectangle(x, y, w, h, color);
    }

    void Intersects(float dt, Object *obj) {
        if (type == BRICK && obj->getType() == BRICK)
            return;

        float x0 = obj->getX();
        float y0 = obj->getY();
        float w0 = obj->getW();
        float h0 = obj->getH();

        uint side = collidingAABB(x, y, w, h, x0, y0, w0, h0);

        if (type == BALL && obj->getType() == PLAYER) {

            if (side == HORIZONTALLY) {
                force(1.2f, 1.2f);
                move(-(vx + x0 + w0) * dt, -vy * dt);
                vy *= -1;
            } else if (side == VERTICALLY) {
                force(1.2f, 1.2f);
                move(-(vx + x0 + w0) * dt, -vy * dt);
                vx *= -1;
            } else if (side == NONE) {
                float r = radius;
                float cx = x + r;
                float cy = y + r;

                if (collidingCircles(cx, cy, r, x0, y0 + h0, corner_radius)) {
                    force(1.2f, 1.2f);
                    move(-(vx + x0 + w0) * dt, -vy * dt);
                    float tmp = vx;
                    vx = vy;
                    vy = tmp;
                } else if (collidingCircles(cx, cy, r, x0 + w0, y0 + h0, corner_radius)) {
                    force(1.2f, 1.2f);
                    move(-(vx + x0 + w0) * dt, -vy * dt);
                    float tmp = vx;
                    vx = -vy;
                    vy = tmp;
                } else if (collidingCircles(cx, cy, r, x0, y0, corner_radius)) {
                    force(1.2f, 1.2f);
                    move(-(vx + x0 + w0) * dt, -vy * dt);
                    float tmp = vx;
                    vx = -vy;
                    vy = -tmp;
                } else if (collidingCircles(cx, cy, r, x0 + w0, y0, corner_radius)) {
                    force(1.2f, 1.2f);
                    move(-(vx + x0 + w0) * dt, -vy * dt);
                    float tmp = vx;
                    vx = -vy;
                    vy = tmp;
                }
            }
            return;
        }

        if (side != NONE) {
            if (type == BALL && obj->getType() == BRICK) {
                move(-vx * dt, -vy * dt);
                float tmpX = vx;
                float m = radius * radius * PI;
                float M = w * h;

                obj->vx += tmpX * (M - m) / (M + m);
                vx = -1.0f * m / M * (tmpX + obj->vx);

                if (side == HORIZONTALLY)
                    vy *= -1;
            } else if (side == HORIZONTALLY) {
                vy *= -1;
            } else if (side == VERTICALLY) {
                vx *= -1;
            }
        }
    }

private:
    float x, y, w, h;
    Color color;
    uint type;
    float vx, vy;
} objects[num_objects];

class Room {
public:
    Room() {
    }

    void update(float dt) {
        for (int i = 0; i < num_objects - 1; i++) {
            for (int j = i + 1; j < num_objects; j++)
                objects[i].Intersects(dt, &objects[j]);

            objects[i].update(dt);
        }
    }

    void draw() {
        for (int i = 0; i < num_objects; i++)
            objects[i].draw();
    }

    void addActor(Object obj) {
        objects[index++] = obj;
    }

} room;

Object *findObject(uint type) {
    for (int i = 0; i < num_objects; i++)
        if (objects[i].getType() == type)
            return &objects[i];

    return NULL;
}

void onInitialization() {
    glViewport(0, 0, screenWidth, screenHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 50, 0, 50);

    room.addActor(Object(28, 22, 2, 2, Color(1, 1, 0), BALL, -10, 2));
    room.addActor(Object(33, 28, 1, 4, Color(0, 1, 0), PLAYER));
    room.addActor(Object(5, 5, 40, 1, Color(1, 1, 1), HORIZONTALLY));
    room.addActor(Object(5, 44, 40, 1, Color(1, 1, 1), HORIZONTALLY));

    margins[0] = Margin(5, 5, 1, 40);
    margins[1] = Margin(45, 5, 1, 40);
    margins[2] = Margin(5, 44, 40, 1);
    margins[3] = Margin(5, 5, 40, 1);

    float step = 38.0f / numBricks;
    for (size_t i = 0; i < numBricks; i++) {
        room.addActor(Object(5, 6 + i * step, 1, step, Color(1, 0, 0), BRICK));
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void onDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    room.draw();
    glutSwapBuffers();
}

void onKeyboard(unsigned char key, int x, int y) {
    Object *player = findObject(PLAYER);
    float vx = 0;
    float vy = 0;

    float xx = player->getX();
    float yy = player->getY();
    float ww = player->getW();
    float hh = player->getH();
    bool collided = false;

    if (key == 'i')
        vy = 1;
    else if (key == 'm')
        vy = -1;
    else if (key == 'j')
        vx = -1;
    else if (key == 'k')
        vx = 1;

    for (int i = 0; i < 4; i++) {
        float x0 = margins[i].x0;
        float y0 = margins[i].y0;
        float w0 = margins[i].w0;
        float h0 = margins[i].h0;

        if (collidingAABB(xx + vx, yy + vy, ww, hh, x0, y0, w0, h0))
            collided = true;
    }

    if (!collided) player->move(vx, vy);
}

void onMouse(int button, int state, int x, int y) {
}

void onIdle() {
    static float time = 0;
    const float Dt = 0.1;

    float newTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    for (; time < newTime; time += Dt) {
        if (time + Dt > newTime) {
            room.update(newTime - time);
            time = newTime;
            break;
        } else {
            room.update(Dt);
        }
    }
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Breakout - lesson 01");

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    onInitialization();

    glutDisplayFunc(onDisplay);
    glutMouseFunc(onMouse);
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);

    glutMainLoop();

    return 0;
}
