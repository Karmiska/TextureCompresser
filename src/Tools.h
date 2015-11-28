#pragma once

#define GETFOURCC(a, b, c, d) \
    ((unsigned int)((unsigned int)(a)      ) | \
                   ((unsigned int)(b) <<  8) | \
                   ((unsigned int)(c) << 16) | \
                   ((unsigned int)(d) << 24))

#define FOURCCBUF(buf) \
    ((unsigned int)((unsigned int)(buf)[0]      ) | \
                   ((unsigned int)(buf)[1] <<  8) | \
                   ((unsigned int)(buf)[2] << 16) | \
                   ((unsigned int)(buf)[3] << 24))

#define SETFOURCC(buf, v) \
        (buf)[0] = ((v)      ) & 0xff; \
        (buf)[1] = ((v) >>  8) & 0xff; \
        (buf)[2] = ((v) >> 16) & 0xff; \
        (buf)[3] = ((v) >> 24) & 0xff;

#define GETUSHORT(buf) \
    (((unsigned short)(buf)[0]) | \
    ((unsigned short)(buf)[1] << 8))

#define PUTUSHORT(buf, s) \
   (buf)[0] = ((s)     ) & 0xff; \
   (buf)[1] = ((s) >> 8) & 0xff;
