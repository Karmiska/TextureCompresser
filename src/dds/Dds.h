#pragma once

#include <string>
#include <vector>
#include "../image/ImageIf.h"
#include "../Color.h"
#include "DdsBoilerplate.h"

class Dds : public ImageIf
{
private:
    std::string mFilename;
    std::vector<Color> mPixels;
    DdsHeader mHeader;

    // reads DDS header
    bool readHeader(std::fstream& file);

    // decodes DXT data to mPixels
    void loadImage(std::fstream& file);
    
    // calculate intermediate colors (c2 and c3) by interpolating between 0 and 1.
    void calculateIntermediateColors(
        const Color& c0,
        const Color& c1,
        Color& c2,
        Color& c3);

    // fills mHeader structure with valid values
    void createHeader(int width, int height);

    // writes the contents of mPixels to file given
    // in DXT BC1 format
    void writePixels(std::fstream& file);

    // calculates the smallest and biggest color values
    void getMinMax(
        const std::vector<Color>& block,
        Color& minColor,
        Color& maxColor);

    // conveniency function to get 16 pixels (4x4) from given location
    std::vector<Color> getBlock(int xp, int yp);

    // calculate the distance of the color from each reference color
    // and return the indice of the closest match
    unsigned char getColorTableIndex(
        const Color(&colors)[4],
        const Color& cmpColor);

    // calculates color distance from another one based on luminance
    unsigned int calculateColorDistance(
        const Color& a,
        const Color& b);

    
public:
    Dds(const std::string& filename);
    Dds(const std::string& filename,
        unsigned int width,
        unsigned int height,
        unsigned int depth);

    unsigned int getWidth() const;
    unsigned int getHeight() const;

    const std::vector<Color>& getPixels() const;
    void setPixels(const std::vector<Color>& pixels);

    void save();
};
