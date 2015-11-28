#include "Image.h"
#include "../File.h"
#include "../bmp/Bmp.h"
#include "../dds/Dds.h"

using namespace std;

unique_ptr<ImageIf> Image::createImage(const std::string& filename,
                             int width,
                             int height,
                             int depth)
{
    string extension = getFileExtension(filename);
    if(extension == ".bmp")
    {
        if(width != -1 && height != -1 && depth != -1)
        {
            return unique_ptr<ImageIf>(new Bmp(filename, width, height, depth));
        }
        else
        {
            return unique_ptr<ImageIf>(new Bmp(filename));
        }
    }
    else if(extension == ".dds")
    {
        if(width != -1 && height != -1 && depth != -1)
        {
            return unique_ptr<ImageIf>(new Dds(filename, width, height, depth));
        }
        else
        {
            return unique_ptr<ImageIf>(new Dds(filename));
        }
    }
    return unique_ptr<ImageIf>(nullptr);
}
