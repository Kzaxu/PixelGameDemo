#ifndef __SPRITE_TRANSFORM_HPP
#define __SPRITE_TRANSFORM_HPP

#include "olcPixelGameEngine.hpp"
#include <stdio.h>

class SpriteTransform : public olc::PixelGameEngine {

public:

    class matrix33 {
    
    public:
        float data[9];
    
        matrix33() {
            memset(data, 0x00, 9 * sizeof(float));
        }
        matrix33(float* d) {
            memcpy(data, d, 9 * sizeof(float));
        }

        float& operator()(int x, int y) {
            return data[Offset(x, y)];
        }

        matrix33 operator*(matrix33 a) {
            matrix33 out;
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    out(i, j) = data[Offset(i, 0)] * a.data[Offset(0, j)] + 
                                data[Offset(i, 1)] * a.data[Offset(1, j)] + 
                                data[Offset(i, 2)] * a.data[Offset(2, j)];
                }
            }
            return out;
        }

        matrix33 Inverse() {
            matrix33 out;
            float det = data[0] * data[4] * data[8] + data[1] * data[5] * data[6] +
                        data[2] * data[3] * data[7] - data[0] * data[5] * data[7] -
                        data[1] * data[3] * data[8] - data[2] * data[4] * data[6];

            out.data[0] = (data[4] * data[8] - data[5] * data[7]) / det;
            out.data[1] = (data[2] * data[7] - data[1] * data[8]) / det;
            out.data[2] = (data[1] * data[5] - data[2] * data[4]) / det;
            out.data[3] = (data[5] * data[6] - data[3] * data[8]) / det;
            out.data[4] = (data[0] * data[8] - data[2] * data[6]) / det;
            out.data[5] = (data[2] * data[3] - data[0] * data[5]) / det;
            out.data[6] = (data[3] * data[7] - data[4] * data[6]) / det;
            out.data[7] = (data[1] * data[6] - data[0] * data[7]) / det;
            out.data[8] = (data[0] * data[4] - data[1] * data[3]) / det;

            return out;
        }

        int Offset(int x, int y) {
            return 3 * x + y;
        }

        void ToIndentity() {
            memset(data, 0x00, 9 * sizeof(float));
            data[0] = 1.f;
            data[4] = 1.f;
            data[8] = 1.f;
        }

        void ToTranslate(float x, float y) {
            ToIndentity();
            data[2] = x;
            data[5] = y;
        }

        void ToRotate(float theta) {
            data[0] = cosf(theta),  data[1] = sinf(theta), data[2] = 0.f,
            data[3] = -sinf(theta),  data[4] = cosf(theta), data[5] = 0.f,
            data[6] = 0.f,  data[7] = 0.f, data[8] = 1.f;
        }

        void Forward(float in_x, float in_y, float &out_x, float &out_y) {
            out_x = data[0] * in_x + data[1] * in_y + data[2];
            out_y = data[3] * in_x + data[4] * in_y + data[5];
        }

        void Show() {
            for (int i = 0; i < 3; ++i) {
                printf("%.3f %.3f %.3f\n", data[3 * i + 0], data[3 * i + 1], data[3 * i + 2]);
            } 
        }
    };

    SpriteTransform() {
        sAppName = "Sprite Transforms";
    }

protected:
    bool OnUserCreate() override {
        sprite_car = new olc::Sprite("resource/sprite_transfrom/car_top1.png");
        return true;
    }

    bool OnUserDestroy() override {
        delete sprite_car;

        return true;
    }

    
    bool OnUserUpdate(float fElapsedTime) override {

        UpdateStatus(fElapsedTime);
        Clear(olc::DARK_CYAN);

        // SetPixelMode(olc::Pixel::ALPHA);
        // DrawSprite(0, 0, sprite_car);
        // SetPixelMode(olc::Pixel::NORMAL);

        matrix33 t1, r1, t2;
        t1.ToTranslate(-sprite_car->width/2.f, -sprite_car->height/2.f);
        r1.ToRotate(rotate_angle);
        t2.ToTranslate(ScreenWidth()/2.f, ScreenHeight()/2.f);

        final_matrix = t2 * r1 * t1;

        DrawCar();

        return true;
    }

private:
    olc::Sprite* sprite_car;
    float rotate_angle = 0;
    matrix33 final_matrix;

    void UpdateStatus(float fElapsedTime) {
        if (GetKey(olc::Z).bHeld) rotate_angle += 2.f *  fElapsedTime;
        if (GetKey(olc::X).bHeld) rotate_angle -= 2.f *  fElapsedTime;
    }

    void DrawCar() {
        
        float sx, sy, ex, ey;
        float tmpx, tmpy;

        matrix33 matrix = final_matrix.Inverse();
        final_matrix.Forward(0, 0, tmpx, tmpy);
        sx = tmpx, sy = tmpy, ex = tmpx, ey = tmpy;
        final_matrix.Forward(sprite_car->width, 0, tmpx, tmpy);
        sx = std::min(sx, tmpx), sy = std::min(sy, tmpy);
        ex = std::max(ex, tmpx), ey = std::max(ey, tmpy);
        final_matrix.Forward(sprite_car->width, sprite_car->height, tmpx, tmpy);
        sx = std::min(sx, tmpx), sy = std::min(sy, tmpy);
        ex = std::max(ex, tmpx), ey = std::max(ey, tmpy);
        final_matrix.Forward(0, sprite_car->height, tmpx, tmpy);
        sx = std::min(sx, tmpx), sy = std::min(sy, tmpy);
        ex = std::max(ex, tmpx), ey = std::max(ey, tmpy);

        for (int i = sx; i <= ex; ++i) {
            for (int j = sy; j <=ey; ++j) {
                float ox, oy;
                matrix.Forward(i, j , ox, oy);
                olc::Pixel p = sprite_car->GetPixel(ox, oy);

                Draw(i, j, p);
            }
        }
    }

};


void TestSpriteTransform() {
    SpriteTransform game;
    if (game.Construct(200, 150, 4, 4)) {
        game.Start();
    }
}

#endif