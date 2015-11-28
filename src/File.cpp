#include "File.h"

using namespace std;

// this is a naive implementation of getting a filename extension
// in actual code one would use platform specific services
// such as PathFindExtension
string getFileExtension(string filename)
{
    size_t extensionPos = filename.find_last_of('.');
    if(extensionPos != string::npos)
    {
        return filename.substr(extensionPos, filename.length()-extensionPos);
    }
    return "";
}
