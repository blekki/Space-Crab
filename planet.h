#pragma once
#include "sphere.h"

#include "structures/xyz.h"

using namespace std;

class Planet: Sphere{
    private:
        int ID = 0;
        int textureID = 0;
        xyz_t position;
        //other characteristics
        float scale = 1.0f;
        float orbitSpeed = 30.0f;
        float rotateSpeed = 0.0001f;

    public:
        void setTexture(string texture);
        void setPosition(float x, float y, float z);
        void setRotateSpeed(float rotateSpeed);
        void setScale(float radius);
        float getX();
        float getY();
        float getZ();
        xyz_t getXYZ();

        virtual void draw() override;
        
    protected:
        virtual void vertex(float a[]) override;
};