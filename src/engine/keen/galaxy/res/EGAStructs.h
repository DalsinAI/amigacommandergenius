/*
 * EGAStructs.h
 *
 *  Created on: 27.12.2009
 *
 * Copyright (c)2009-2010 by Gerstrong.
 * Copyright (c)2007 by Ignacio R. Morelle "Shadow Master". (shadowm2006@gmail.com)
 * Based on ModKeen 2.0.1 Copyright (c)2002-2004 Andrew Durdin. (andy@durdin.net)*
 */

#ifndef EGASTRUCTS_H_
#define EGASTRUCTS_H_

#include <vector>
#include <cstddef>
#include <SDL_endian.h>

// The EGAGRAPH tables are little-endian. After a memcpy() of n structs
// that hold only 16-bit fields, this puts them in the machine's order
// (nothing to do on a little-endian machine).
template <typename T>
inline void swapLE16Structs(T *p, const size_t n)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    unsigned short *w = reinterpret_cast<unsigned short*>(p);
    for(size_t i = 0 ; i < n*sizeof(T)/2 ; i++)
        w[i] = SDL_SwapLE16(w[i]);
#else
    (void) p; (void) n;
#endif
}


struct EpisodeInfoStruct
{
	unsigned long ExeImageSize;
	unsigned long OffEgaHead;
	unsigned long OffEgaDict;
	unsigned long NumChunks;
	unsigned int NumFonts, IndexFonts;
	unsigned int NumBitmaps, IndexBitmaps;
	unsigned int NumMaskedBitmaps, IndexMaskedBitmaps;
	unsigned int NumSprites, IndexSprites;
	unsigned int Num8Tiles, Index8Tiles;
	unsigned int Num8MaskedTiles, Index8MaskedTiles;
	unsigned int Num16Tiles, Index16Tiles;
	unsigned int Num16MaskedTiles, Index16MaskedTiles;
	unsigned int NumMisc, IndexMisc;	/* Includes ANSI images */
	unsigned int NumTexts, IndexTexts;
	unsigned int NumDemos, IndexDemos;
};

struct ChunkStruct
{
    unsigned long len = 0;
	std::vector<unsigned char> data;
};

struct BitmapHeadStruct
{
	unsigned short Width;
	unsigned short Height;
};

struct SpriteHeadStruct
{
	unsigned short Width;
	unsigned short Height;
	signed short OrgX;
	signed short OrgY;
	signed short Rx1, Ry1;
	signed short Rx2, Ry2;
	unsigned short Shifts;
};

struct FontHeadStruct
{
	unsigned short Height;
    unsigned short Offset[256];
    unsigned char Width[256];
};

#endif /* EGASTRUCTS_H_ */
