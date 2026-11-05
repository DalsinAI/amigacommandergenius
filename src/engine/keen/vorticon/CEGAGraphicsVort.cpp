/*
 * CEGAGraphicsVort.cpp
 *
 *  Created on: 07.03.2010
 *      Author: gerstrong
 */

#include "CEGAGraphicsVort.h"
#include "engine/core/CResourceLoader.h"

#include "fileio/CTileLoader.h"

#ifdef TARGET_WIN32
#include <dir.h>
#endif
#include <fstream>
#include <vector>
#include <base/interface/FindFile.h>

#include <fileio/KeenFiles.h>

namespace vorticon
{

#define SAFE_SDL_FREE(x) if(x) { SDL_FreeSurface(x); x = nullptr; }

int CEGAGraphicsVort::getNumSprites()
{ return NumSprites; }
short CEGAGraphicsVort::getNumTiles()
{ return Num16Tiles; }

bool CEGAGraphicsVort::loadData( int version, unsigned char *p_exedata )
{
  std::vector<char> data;
  
  const std::string  &gamedir = gKeenFiles.gameDir;    
  
  const auto &exefile = gKeenFiles.exeFile;
  const int  episode  = exefile.getEpisode();        
  
  
  // assure that the last used resources are freed
  
  // Set the palette, so the proper colours are loaded
  gGraphics.Palette.setupColorPalettes(p_exedata, episode);
  
  std::ifstream HeadFile;
  OpenGameFileR(HeadFile, ((gamedir != "") ? gamedir + "/" : "") +
		          "egahead.ck" + itoa(episode), std::ios::binary);
  
  if(!HeadFile)
    return false;
  
  char byte;
  while(!HeadFile.eof())
    {
      HeadFile.read(&byte,1);
      data.push_back(byte);
    }
  HeadFile.close();
  
  // Now copy the data to the EGAHEAD Structure (little-endian; the
  // counts are words, read as words)
  LatchPlaneSize = GETLONGWORD(reinterpret_cast<gs_byte*>(&data[0]+0));
  SpritePlaneSize = GETLONGWORD(reinterpret_cast<gs_byte*>(&data[0]+4));
  BitmapTableStart = GETLONGWORD(reinterpret_cast<gs_byte*>(&data[0]+8));
  SpriteStart = GETLONGWORD(reinterpret_cast<gs_byte*>(&data[0]+12));
  
  // At this point there are differences to the 134 version
  if(version == 134)
  {
      FontTiles = GETWORD(reinterpret_cast<gs_byte*>(&data[0]+16+6));
      FontLocation = GETLONGWORD(reinterpret_cast<gs_byte*>(&data[0]+18+6));
      ScreenTiles = GETWORD(reinterpret_cast<gs_byte*>(&data[0]+22+6));
      ScreenLocation = GETLONGWORD(reinterpret_cast<gs_byte*>(&data[0]+24+6));
      Num16Tiles = GETWORD(reinterpret_cast<gs_byte*>(&data[0]+28+6));
      Tiles16Location = GETLONGWORD(reinterpret_cast<gs_byte*>(&data[0]+30+6));
      NumBitmaps = GETWORD(reinterpret_cast<gs_byte*>(&data[0]+34+6));
      BitmapLocation = GETLONGWORD(reinterpret_cast<gs_byte*>(&data[0]+36+6));
      NumSprites = GETWORD(reinterpret_cast<gs_byte*>(&data[0]+40+6));
      SpriteLocation = GETLONGWORD(reinterpret_cast<gs_byte*>(&data[0]+42+6));
      compressed = GETWORD(reinterpret_cast<gs_byte*>(&data[0]+46+6));
  }
  else
  {
      FontTiles = GETWORD(reinterpret_cast<gs_byte*>(&data[0]+16));
      FontLocation = GETLONGWORD(reinterpret_cast<gs_byte*>(&data[0]+18));
      ScreenTiles = GETWORD(reinterpret_cast<gs_byte*>(&data[0]+22));
      ScreenLocation = GETLONGWORD(reinterpret_cast<gs_byte*>(&data[0]+24));
      Num16Tiles = GETWORD(reinterpret_cast<gs_byte*>(&data[0]+28));
      Tiles16Location = GETLONGWORD(reinterpret_cast<gs_byte*>(&data[0]+30));
      NumBitmaps = GETWORD(reinterpret_cast<gs_byte*>(&data[0]+34));
      BitmapLocation = GETLONGWORD(reinterpret_cast<gs_byte*>(&data[0]+36));
      NumSprites = GETWORD(reinterpret_cast<gs_byte*>(&data[0]+40));
      SpriteLocation = GETLONGWORD(reinterpret_cast<gs_byte*>(&data[0]+42));
      compressed = GETWORD(reinterpret_cast<gs_byte*>(&data[0]+46));
  }
  
  // First, retrieve the Tile properties so the tilemap gets properly formatted
  // Important especially for masks, and later in the game for the behaviours
  // of those objects
  
  CTileLoader TileLoader( episode, false, version, p_exedata );
  if(!TileLoader.load(0, Num16Tiles))
    return false;
  
  m_Latch = new CEGALatch(LatchPlaneSize,
			  BitmapTableStart,
			  FontTiles,
			  FontLocation,
			  ScreenTiles,
			  ScreenLocation,
			  Num16Tiles,
			  Tiles16Location,
			  NumBitmaps,
			  BitmapLocation);
  
  m_Latch->loadHead( &data[0], episode );
  
  m_Latch->loadData( gamedir, episode, version, p_exedata, (compressed>>1) ); // The second bit tells, if latch is compressed.
  
  
  m_Sprit = new CEGASprit(SpritePlaneSize,
			  SpriteStart,
			  NumSprites,
			  SpriteLocation,
              gamedir, episode,
              mLoader);
  m_Sprit->loadHead(&data[0]);
  
  struct SpriteLoad: public Action
  {
      std::string buf;
      bool compressed;
      CEGASprit *m_Sprit;
      
      SpriteLoad(CEGASprit *Sprit, const std::string& _buf, bool _compressed):
	buf(_buf), compressed(_compressed), m_Sprit(Sprit) {}
      
      int handle()
      {
	m_Sprit->loadData(buf,compressed);
	return 1;
      }
  };
  
  SpriteLoad sprLoad(m_Sprit,
		     ((gamedir != "") ? gamedir + "/" : "") + 
		     "egasprit.ck" + itoa(episode),
		     (compressed>>1));
  
  sprLoad.handle();
  
  return true;
}

CEGAGraphicsVort::~CEGAGraphicsVort()
{
  if(m_Latch) delete m_Latch;
  if(m_Sprit) delete m_Sprit;
  m_Latch = nullptr;
  m_Sprit = nullptr;
  
  SAFE_SDL_FREE(m_FontSurface);
  SAFE_SDL_FREE(m_BigTileSurface);
  SAFE_SDL_FREE(m_TileSurface);
  SAFE_SDL_FREE(m_BitmapsSurface);
}

}
