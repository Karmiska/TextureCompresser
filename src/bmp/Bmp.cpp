#include "Bmp.h"
#include "../Tools.h"
#include <fstream>
#include <stdio.h>
#include <math.h>

using namespace std;

#define HeaderFieldValue(sig) (((int)sig[0]) | ((int)sig[1] << 8))

Bmp::Bmp(const std::string& filename)
    : mFilename(filename)
{
    std::fstream input;
    input.open(filename, ios::binary | ios::in);
    if(input.is_open())
    {
        if(!readHeader(input))
        {
            printf("Unsupported bmp format.\n");
        }
        else
        {
            readPixels(input);
        }
        input.close();
    }
}

bool Bmp::readHeader(std::fstream& file)
{
    // read bmp headers
    memset(&mFileHeader, 0, sizeof(mFileHeader));
    memset(&mDibHeader, 0, sizeof(mDibHeader));
    file.read(reinterpret_cast<char*>(&mFileHeader), sizeof(BitmapFileHeader));
    file.read(reinterpret_cast<char*>(&mDibHeader), sizeof(BimapV3Header));
    return mDibHeader.bitsPerPixel == 24 &&
           mDibHeader.compression == 0;
}

void Bmp::readPixels(std::fstream& file)
{
    // grow our pixel storage and take pointer there
    mPixels.resize(mDibHeader.width * mDibHeader.height);
    
    // seek to where the pixel data begins
    file.seekg(mFileHeader.offsetToPixelArray, ios::beg);

    // setup
    int bytesPerPixel = mDibHeader.bitsPerPixel / 8;
    int bytesPerLine = getBytesPerLine();

    // allocate buffer for one line
    char* lineBuffer = new char[bytesPerLine];
    
    int pixelPos = 0;
    for(unsigned int y = 0; y < mDibHeader.height; ++y)
    {
        // read one line
        file.read(lineBuffer, bytesPerLine);
        char* xPtr = lineBuffer;

        // decode one line to pixel storage
        readLine(pixelPos, xPtr, mDibHeader.width);
        pixelPos += mDibHeader.width;
    }
    delete[] lineBuffer;
}

void Bmp::readLine(int pixelPos, char* buffer, int width)
{
    for(int x = 0; x < width; ++x)
    {
        mPixels[pixelPos] = Color(
            *(unsigned char*)(buffer + 2),
            *(unsigned char*)(buffer + 1),
            *(unsigned char*)buffer);
        buffer += 3;
        ++pixelPos;
    }
}

Bmp::Bmp(const std::string& filename,
         unsigned int width,
         unsigned int height,
         unsigned int depth)
    : mFilename(filename)
{
    createBmpHeader(width, height, depth);
}

void Bmp::createBmpHeader(unsigned int width,
                          unsigned int height,
                          unsigned int depth)
{
    memset(&mFileHeader, 0, sizeof(mFileHeader));
    memset(&mDibHeader, 0, sizeof(mDibHeader));

    mFileHeader.signature = HeaderFieldValue("BM");

    mDibHeader.size = sizeof(BimapV3Header);
    mDibHeader.width = width;
    mDibHeader.height = height;
    mDibHeader.planes = 1;
    mDibHeader.bitsPerPixel = depth;
    mDibHeader.compression = 0;

    // setup
    int bytesPerPixel = depth / 8;
    int bytesPerLine = getBytesPerLine();

    mDibHeader.imageSize = bytesPerLine * height;
    mDibHeader.xPixelsPerMeter = DEFAULT_PPM_X;
    mDibHeader.yPixelsPerMeter = DEFAULT_PPM_Y;
    
    mFileHeader.offsetToPixelArray = sizeof(BitmapFileHeader) + mDibHeader.size;
    mFileHeader.size = mFileHeader.offsetToPixelArray + mDibHeader.imageSize;
}

unsigned int Bmp::getWidth() const
{
    return mDibHeader.width;
}

unsigned int Bmp::getHeight() const
{
    return mDibHeader.height;
}

const std::vector<Color>& Bmp::getPixels() const
{
    return mPixels;
}

void Bmp::setPixels(const std::vector<Color>& pixels)
{
    mPixels = pixels;
}

void Bmp::save()
{
    std::fstream input;
    input.open(mFilename, ios::binary | ios::out);
    if(input.is_open())
    {
        // headers have been already setup in constructors
        input.write(reinterpret_cast<char*>(&mFileHeader), sizeof(BitmapFileHeader));
        input.write(reinterpret_cast<char*>(&mDibHeader), sizeof(BimapV3Header));
        writePixels(input);
        input.close();
    }
}

void Bmp::writePixels(std::fstream& file)
{
    // setup
    int bytesPerPixel = mDibHeader.bitsPerPixel / 8;
    int bytesPerLine = getBytesPerLine();

    // create line buffer
    char* lineBuffer = new char[bytesPerLine];

    int pixelPos = 0;
    for(unsigned int y = 0; y < mDibHeader.height; ++y)
    {
        writeLine(pixelPos, lineBuffer, mDibHeader.width);
        file.write(lineBuffer, bytesPerLine);
        pixelPos += mDibHeader.width;
    }
    delete[] lineBuffer;
}

void Bmp::writeLine(int pixelPos, char* buffer, int width)
{
    for(int x = 0; x < width; ++x)
    {
        *buffer = mPixels[pixelPos].blue(); ++buffer;
        *buffer = mPixels[pixelPos].green(); ++buffer;
        *buffer = mPixels[pixelPos].red(); ++buffer;
        ++pixelPos;
    }
}

int Bmp::getBytesPerLine() const
{
    int bytesPerPixel = mDibHeader.bitsPerPixel / 8;
    int bytesPerLine = bytesPerPixel * mDibHeader.width;

    // account for padding
    if(bytesPerLine % 4 != 0)
        bytesPerLine += 4 - (bytesPerLine % 4);

    return bytesPerLine;
}



