#pragma once
#include <math.h>

class Matrix4{
    private:
        float array[16]; // all elements of a matrix 4x4 but such a line
    
    public:
        // get element pointer of the array
        float* ptr(){
            return &array[0];
        }

        // set value for everyone element of the array (a total of 16 elements)
        Matrix4(float a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9, float a10, float a11, float a12, float a13, float a14, float a15){
            array[0] = a0;   array[1] = a1;   array[2] = a2;   array[3] = a3;
            array[4] = a4;   array[5] = a5;   array[6] = a6;   array[7] = a7;
            array[8] = a8;   array[9] = a9;   array[10] = a10; array[11] = a11;
            array[12] = a12; array[13] = a13; array[14] = a14; array[15] = a15;
        }
        
        // get concrete value of an element of the array
        float at(int i, int y){
            return array[y * 4 + i];
        }

        // set value in one element of the array
        void set(int a, int b, float sum){
            array[b * 4 + a] = sum;
        }

        // fill the array using numbers of the identity matrix 4x4
        Matrix4(){
            array[0] = 1;  array[1] = 0;  array[2] = 0;  array[3] = 0;
            array[4] = 0;  array[5] = 1;  array[6] = 0;  array[7] = 0;
            array[8] = 0;  array[9] = 0;  array[10] = 1; array[11] = 0;
            array[12] = 0; array[13] = 0; array[14] = 0; array[15] = 1;
        };
};

class Vec3{
    private: 
        float x;
        float y;
        float z;

    public:
        Vec3(){};

        Vec3(float x, float y, float z){
            this->x = x;
            this->y = y;
            this->z = z;
        }
        friend class Qued;
};

class Qued{
    public:
        float x;
        float y;
        float z;
        float w;

    public:
        Qued(){};
        void normalize();
        void conjugate();

        Qued(Vec3 from, Vec3 to){
            float s = sqrt((1.0f + from.x * to.x + from.y * to.y + from.z * to.z) * 2.0f);
            if (s > 0.0f) {
                float is = 1.0f / s;
                this->x = (from.y * to.z - from.z * to.y) * is;
                this->y = (from.z * to.x - from.x * to.z) * is;
                this->z = (from.x * to.y - from.y * to.x) * is;
                this->w = s * 0.5f;
            }
            // else
            // {
            //     // Is this actually a problem?
            //     if (vector_equal(v1, kBasisZVector) || vector_equal(v0, kBasisZVector))
            //     {
            //         q = make_quaternion(0, 1, 0, 0);
            //     }
            //     else
            //     {
            //         q = kIdentityQuaternion;
            //     }
            //     // We arrive here for antiparallel vectors. Rotation axis is then undefined, but not rotating is
            //     // wrong. Probably the calling function should exclude this situation. For current
            //     // in-game use of this function we return (0,1,0,0), but generally that is also wrong.
            // }
        }
};

// Qued rotationBetween(Vec3 from, Vec3 to){
    
// }

// Vec3 matrixXvec(Matrix4 matrix, Vec3 vec){
    
// }

Matrix4 rotationMatrix(Qued qued);
Matrix4 multiplyMatrix(Matrix4 matrix1, Matrix4 matrix2);