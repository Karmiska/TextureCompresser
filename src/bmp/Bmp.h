#pragma once

#include <string>
#include <vector>
#include "../image/ImageIf.h"
#include "../Color.h"
#include "BmpBoilerplate.h"

class Bmp : public ImageIf
{
private:
    std::string mFilename;
    BitmapFileHeader mFileHeader;
    BimapV3Header mDibHeader;
    std::vector<Color> mPixels;

    bool readHeader(std::fstream& file);
    void createBmpHeader(unsigned int width,
                         unsigned int height,
                         unsigned int depth);

    void readPixels(std::fstream& file);
    void readLine(int pixelPos, char* buffer, int width);

    void writePixels(std::fstream& file);
    void writeLine(int pixelPos, char* buffer, int width);

    int getBytesPerLine() const;

public:
    Bmp(const std::string& filename);
    Bmp(const std::string& filename, 
        unsigned int width,
        unsigned int height,
        unsigned int depth);

    unsigned int getWidth() const;
    unsigned int getHeight() const;
    
    const std::vector<Color>& getPixels() const;
    void setPixels(const std::vector<Color>& pixels);

    void save();
};
