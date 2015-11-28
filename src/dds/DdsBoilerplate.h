#pragma once

#define DDSD_CAPS                  0x00000001
#define DDSD_HEIGHT                0x00000002
#define DDSD_WIDTH                 0x00000004
#define DDSD_PITCH                 0x00000008
#define DDSD_PIXELFORMAT           0x00001000
#define DDSD_MIPMAPCOUNT           0x00020000
#define DDSD_LINEARSIZE            0x00080000
#define DDSD_DEPTH                 0x00800000

#define DDPF_ALPHAPIXELS           0x00000001
#define DDPF_ALPHA                 0x00000002
#define DDPF_FOURCC                0x00000004
#define DDPF_PALETTEINDEXED8       0x00000020
#define DDPF_RGB                   0x00000040
#define DDPF_LUMINANCE             0x00020000
#define DDPF_NORMAL                0x80000000  // nvidia specific

#define DDSCAPS_COMPLEX            0x00000008
#define DDSCAPS_TEXTURE            0x00001000
#define DDSCAPS_MIPMAP             0x00400000

#define DDSCAPS2_CUBEMAP           0x00000200
#define DDSCAPS2_CUBEMAP_POSITIVEX 0x00000400
#define DDSCAPS2_CUBEMAP_NEGATIVEX 0x00000800
#define DDSCAPS2_CUBEMAP_POSITIVEY 0x00001000
#define DDSCAPS2_CUBEMAP_NEGATIVEY 0x00002000
#define DDSCAPS2_CUBEMAP_POSITIVEZ 0x00004000
#define DDSCAPS2_CUBEMAP_NEGATIVEZ 0x00008000
#define DDSCAPS2_CUBEMAP_ALL_FACES \
   (DDSCAPS2_CUBEMAP_POSITIVEX | DDSCAPS2_CUBEMAP_NEGATIVEX | \
    DDSCAPS2_CUBEMAP_POSITIVEY | DDSCAPS2_CUBEMAP_NEGATIVEY | \
    DDSCAPS2_CUBEMAP_POSITIVEZ | DDSCAPS2_CUBEMAP_NEGATIVEZ)

#define DDSCAPS2_VOLUME            0x00200000

typedef enum
{
    CompressionNone,
    CompressionBC1,
    CompressionBC2,
    CompressionBC3,
    CompressionBC3N,
    CompressionBC4,
    CompressionBC5,
    CompressionRXGB,
    CompressionAEXP,
    CompressionYCOCG,
    CompressionYCOCGS
} DdsCompression;

#pragma pack(push)
#pragma pack(1)
struct DdsPixelFormat
{
    unsigned int size;
    unsigned int flags;
    char fourCC[4];
    unsigned int RGBBitCount;
    unsigned int RBitMask;
    unsigned int GBitMask;
    unsigned int BBitMask;
    unsigned int ABitMask;
};

typedef struct
{
    unsigned int caps1;
    unsigned int caps2;
    unsigned int reserved[2];
} DdsCaps;

typedef struct
{
    unsigned int           magic;
    unsigned int           size;
    unsigned int           flags;
    unsigned int           height;
    unsigned int           width;
    unsigned int           pitchOrLinearSize;
    unsigned int           depth;
    unsigned int           mipMapCount;
    unsigned int           reserved1[11];
    DdsPixelFormat         pixelFormat;
    DdsCaps                ddsCaps;
    unsigned int           reserved2;
} DdsHeader;
#pragma pack(pop)
