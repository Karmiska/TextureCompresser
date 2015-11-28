#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../Color.h"
#include "ImageIf.h"

// Factory class to create any supported image

class Image
{
public:
    static std::unique_ptr<ImageIf> createImage(
        const std::string& filename,
        int width = -1,
        int height = -1,
        int depth = -1);
};
