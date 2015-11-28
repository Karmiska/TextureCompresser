#pragma once

#include "../Color.h"

class ImageIf
{
public:
    virtual ~ImageIf(){};
    virtual unsigned int getWidth() const = 0;
    virtual unsigned int getHeight() const = 0;
    virtual const std::vector<Color>& getPixels() const = 0;
    virtual void setPixels(const std::vector<Color>& pixels) = 0;
    virtual void save() = 0;
};
