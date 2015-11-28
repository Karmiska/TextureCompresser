#include "Dds.h"
#include "../Tools.h"
#include <fstream>

using namespace std;

Dds::Dds(const string& filename)
    : mFilename(filename)
{
    fstream input;
    input.open(filename, ios::binary | ios::in);
    if(input.is_open())
    {
        if(!readHeader(input))
        {
            printf("Unsupported format.");
        }
        else
        {
            loadImage(input);
        }
        input.close();
    }
}

bool Dds::readHeader(std::fstream& file)
{
    // read dds header
    memset(&mHeader, 0, sizeof(mHeader));
    file.read(reinterpret_cast<char*>(&mHeader), sizeof(DdsHeader));

    // check that it's a format we support
    if(mHeader.magic != GETFOURCC('D', 'D', 'S', ' ') ||
       !(mHeader.flags & DDSD_LINEARSIZE) ||
       !(mHeader.pixelFormat.flags & DDPF_FOURCC) ||
       FOURCCBUF(mHeader.pixelFormat.fourCC) != GETFOURCC('D', 'X', 'T', '1'))
    {
        return false;
    }
    return true;
}

void Dds::loadImage(std::fstream& file)
{
    // allocate buffer for dxt data
    unsigned char* dxtBuffer = new unsigned char[mHeader.pitchOrLinearSize];
    file.read((char*)dxtBuffer, mHeader.pitchOrLinearSize);

    // grow our pixel storage and take pointer there
    mPixels.resize(mHeader.width * mHeader.height);
    Color* arrayData = mPixels.data();

    // loop through the input data in 4x4 chunks
    unsigned char* bufPtr = dxtBuffer;
    for(int y = (int)mHeader.height - 4; y >= 0; y -= 4)
    {
        for(int x = 0; x < (int)mHeader.width; x += 4)
        {
            Color colors[4];

            // get two 16bit rgb 565 reference colors (0 and 1)
            colors[0] = Color::from565rgb(GETUSHORT(bufPtr)); bufPtr += 2;
            colors[1] = Color::from565rgb(GETUSHORT(bufPtr)); bufPtr += 2;

            // calculate intermediate colors (2 and 3) by interpolating between 0 and 1.
            calculateIntermediateColors(colors[0], colors[1], colors[2], colors[3]);

            // the bufPtr at this point will have 16 values
            // each are 2bit indices to the 4 colors we have
            int line = 0;
            for(int i = 0; i < 16; i += 4)
            {
                // the data is vertically reversed
                // so we will turn it the right way.
                Color* dstPtr = arrayData + (((y + 3 - line) * mHeader.width) + x);
                *dstPtr = colors[((*bufPtr) & 0x3)]; ++dstPtr;
                *dstPtr = colors[((*bufPtr) & 0xC) >> 2]; ++dstPtr;
                *dstPtr = colors[((*bufPtr) & 0x30) >> 4];  ++dstPtr;
                *dstPtr = colors[((*bufPtr) & 0xC0) >> 6];
                ++line;
                ++bufPtr;
            }
        }
    }

    // free the dxt buffer
    delete[] dxtBuffer;
}

void Dds::calculateIntermediateColors(
    const Color& c0,
    const Color& c1,
    Color& c2,
    Color& c3)
{
    // we calculate the colors by using linear interpolation
    // suggested at microsofts documentation (https://msdn.microsoft.com/en-us/library/bb694531(v=VS.85).aspx)
    if(c0 > c1)
    {
        c2 = Color((unsigned char)(((2.0 / 3.0)*(double)c0.red()) + ((1.0 / 3.0)*(double)c1.red())),
                   (unsigned char)(((2.0 / 3.0)*(double)c0.green()) + ((1.0 / 3.0)*(double)c1.green())),
                   (unsigned char)(((2.0 / 3.0)*(double)c0.blue()) + ((1.0 / 3.0)*(double)c1.blue())));

        c3 = Color((unsigned char)(((1.0 / 3.0)*(double)c0.red()) + ((2.0 / 3.0)*(double)c1.red())),
                   (unsigned char)(((1.0 / 3.0)*(double)c0.green()) + ((2.0 / 3.0)*(double)c1.green())),
                   (unsigned char)(((1.0 / 3.0)*(double)c0.blue()) + ((2.0 / 3.0)*(double)c1.blue())));
    }
    else
    {
        // 1bit alpha
        c2 = Color((unsigned char)(((1.0 / 2.0)*(double)c0.red()) + ((1.0 / 2.0)*(double)c1.red())),
                   (unsigned char)(((1.0 / 2.0)*(double)c0.green()) + ((1.0 / 2.0)*(double)c1.green())),
                   (unsigned char)(((1.0 / 2.0)*(double)c0.blue()) + ((1.0 / 2.0)*(double)c1.blue())));
        c3 = Color(255, 255, 255, 255);
    }
}

Dds::Dds(const string& filename,
         unsigned int width,
         unsigned int height,
         unsigned int depth)
    : mFilename(filename)
{
    // note: we aren't using depth as we'll be using DXT1 anyway
    createHeader(width, height);
}

void Dds::createHeader(int width, int height)
{
    // fill the DdsHeader structure
    memset(&mHeader, 0, sizeof(DdsHeader));
    mHeader.magic = GETFOURCC('D', 'D', 'S', ' ');
    mHeader.size = sizeof(DdsHeader) - 4;
    mHeader.width = width;
    mHeader.height = height;
    mHeader.mipMapCount = 1;
    mHeader.flags = DDSD_LINEARSIZE |
                    DDSD_CAPS | DDSD_HEIGHT |  DDSD_WIDTH |
                    DDSD_PIXELFORMAT | DDPF_ALPHAPIXELS | DDPF_ALPHA |
                    DDSCAPS_TEXTURE;
    mHeader.pitchOrLinearSize = ((mHeader.width / 4) * (mHeader.height / 4) * 8);

    mHeader.pixelFormat.size = sizeof(DdsPixelFormat);
    mHeader.pixelFormat.flags = DDPF_FOURCC;
    SETFOURCC(mHeader.pixelFormat.fourCC, GETFOURCC('D', 'X', 'T', '1'));
}

unsigned int Dds::getWidth() const
{
    return mHeader.width;
}

unsigned int Dds::getHeight() const
{
    return mHeader.height;
}

const vector<Color>& Dds::getPixels() const
{
    return mPixels;
}

void Dds::setPixels(const vector<Color>& pixels)
{
    mPixels = pixels;
}

void Dds::save()
{
    std::fstream input;
    input.open(mFilename, ios::binary | ios::out);
    if(input.is_open())
    {
        // the header has already been prepared in the constructors
        input.write(reinterpret_cast<char*>(&mHeader), sizeof(DdsHeader));
        writePixels(input);
        input.close();
    }
}

void Dds::writePixels(std::fstream& file)
{
    // BC1 compression uses 2x 2bytes rgb565 data + 16 2bit indices = 8 bytes;
    unsigned char* dst = new unsigned char[mHeader.pitchOrLinearSize];

    // loop through pixels in vertically reverse order in blocks of 4x4 pixels
    unsigned char* dstRun = dst;
    for(int y = (int)mHeader.height - 4; y >= 0; y -= 4)
    {
        for(int x = 0; x < (int)mHeader.width; x += 4)
        {
            // get 4x4 block of pixels
            std::vector<Color>&& block = getBlock(x, y);
            
            // get min and max reference colors
            Color colors[4];
            getMinMax(block, colors[0], colors[1]);

            // calculate intermediate colors (2 and 3) by interpolating between 0 and 1.
            calculateIntermediateColors(colors[0], colors[1], colors[2], colors[3]);

            // write the reference colors to buffer
            PUTUSHORT(dstRun, colors[0].get565bgr()); dstRun += 2;
            PUTUSHORT(dstRun, colors[1].get565bgr()); dstRun += 2;

            // compute indices by checking what indice is closest to the given color
            int currentByte = 0;
            for(int i = 16 - 4; i >= 0; i -= 4)
            {
                unsigned char i1 = getColorTableIndex(colors, block[i]);
                unsigned char i2 = getColorTableIndex(colors, block[i + 1]);
                unsigned char i3 = getColorTableIndex(colors, block[i + 2]);
                unsigned char i4 = getColorTableIndex(colors, block[i + 3]);

                // write indices to buffer
                dstRun[currentByte] = i4 << 6 | i3 << 4 | i2 << 2 | i1;

                ++currentByte;
            }

            dstRun += 4;
        }
    }

    // write buffer to file
    file.write((const char*)dst, mHeader.pitchOrLinearSize);
    delete[] dst;
}

// conveniency function to get 16 pixels (4x4) from given location
std::vector<Color> Dds::getBlock(int xp, int yp)
{
    std::vector<Color> colorBlock;
    for(int y = yp; y < yp + 4; ++y)
    {
        for(int x = xp; x < xp + 4; ++x)
        {
            colorBlock.push_back(mPixels[(mHeader.width * y) + x]);
        }
    }
    return colorBlock;
}

// calculates the smallest and biggest color values
void Dds::getMinMax(const std::vector<Color>& block, Color& minColor, Color& maxColor)
{
    minColor = Color(255, 255, 255, 255);
    maxColor = Color(0, 0, 0, 0);

    for(auto col : block)
    {
        if(col.red() < minColor.red()) { minColor.setRed(col.red()); }
        if(col.green() < minColor.green()) { minColor.setGreen(col.green()); }
        if(col.blue() < minColor.blue()) { minColor.setBlue(col.blue()); }
        if(col.alpha() < minColor.alpha()) { minColor.setAlpha(col.alpha()); }

        if(col.red() > maxColor.red()) { maxColor.setRed(col.red()); }
        if(col.green() > maxColor.green()) { maxColor.setGreen(col.green()); }
        if(col.blue() > maxColor.blue()) { maxColor.setBlue(col.blue()); }
        if(col.alpha() > maxColor.alpha()) { maxColor.setAlpha(col.alpha()); }
    }
}

// calculate the distance of the color from each reference color
// and return the indice of the closest match
unsigned char Dds::getColorTableIndex(
    const Color (&colors)[4],
    const Color& cmpColor)
{
    unsigned int distanceToC0 = calculateColorDistance(colors[0], cmpColor);
    unsigned int distanceToC1 = calculateColorDistance(colors[1], cmpColor);
    unsigned int distanceToC2 = calculateColorDistance(colors[2], cmpColor);
    unsigned int distanceToC3 = calculateColorDistance(colors[3], cmpColor);

    unsigned int minValue = numeric_limits<unsigned int>::max();
    minValue = minValue > distanceToC0 ? distanceToC0 : minValue;
    minValue = minValue > distanceToC1 ? distanceToC1 : minValue;
    minValue = minValue > distanceToC2 ? distanceToC2 : minValue;
    minValue = minValue > distanceToC3 ? distanceToC3 : minValue;

    if(distanceToC0 == minValue) { return 0; }
    else if(distanceToC1 == minValue) { return 1; }
    else if(distanceToC2 == minValue) { return 2; }
    else { return 3; }
}

// calculates color distance from another one based on luminance
unsigned int Dds::calculateColorDistance(const Color& a, const Color& b)
{
    return (unsigned int)(((((double)a.red() - (double)b.red()) * .299) * (((double)a.red() - (double)b.red()) * .299)) +
                          ((((double)a.green() - (double)b.green()) * .587) * (((double)a.green() - (double)b.green()) * .587)) +
                          ((((double)a.blue() - (double)b.blue()) * .114) * (((double)a.blue() - (double)b.blue()) * .114)));
}
