#pragma once

#include "logicwire.h"

class Component: public LogicWire
{
    private:
        string name;
        uint width;
        uint height;
        uint consuming;

    public:
        void setName(string name);
        void setSize(uint width, uint height);
        void setConsuming(uint comsuming);

        Component(const char* image_source)
        :   LogicWire(image_source)
        {};
};