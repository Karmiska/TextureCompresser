#include <stdio.h>
#include <string>
#include "image/Image.h"

using namespace std;

int main(int argc, char** argv)
{
    if(argc < 3)
    {
        printf("TextureCompresser needs file input and output. \n");
        printf("Supported formats:\n");
        printf("BMP 24bit uncompressed\n");
        printf("DDS DXT BC1\n");
        printf("Example: TextureCompresser inputImage.bmb outputImage.dds\n");
        return 1;
    }

    auto input = Image::createImage(argv[1]);
    if(!input)
    {
        printf("Could not create image: %s", argv[1]);
        return 1;
    }

    auto output = Image::createImage(argv[2], input->getWidth(), input->getHeight(), 24);
    if(!output)
    {
        printf("Could not create image: %s", argv[1]);
        return 1;
    }
    output->setPixels(input->getPixels());
    output->save();
    
    return 0;
}
