/*
 * CVidConfig.cpp
 *
 *  Created on: 04.02.2011
 *      Author: gerhard
 */

#include "CVidConfig.h"

#include <cstring>
#include <cstdio>

CVidConfig::CVidConfig()
{
    reset();
}

bool CVidConfig::operator==(const CVidConfig& target)
{
    return ( memcmp( &target, this, sizeof(CVidConfig)) == 0);
}

bool CVidConfig::operator!=(const CVidConfig& target)
{
    return !(*this == target);
}

void CVidConfig::reset()
{
    // Default values
    mVSync = true;

    mDisplayRect.pos.x = 0;
    mDisplayRect.pos.y = 0;

#if defined(CAANOO) || defined(WIZ) || defined(GP2X) || defined(DINGOO) || defined(NANONOTE)
    mDisplayRect.dim.x=320;
    mDisplayRect.dim.y=240;
#if defined(GP2X) || defined(NANONOTE)
    //m_Resolution.depth=32;
#else
    //m_Resolution.depth=16;
#endif
    Fullscreen=true;
#elif defined(__amigaos__)
    // A 640x400 window on the Workbench: twice the games' 320x200 (an RTG
    // card's desktop rarely has room for 1920x1080, and a 68040 no time).
    mDisplayRect.dim.x = 640;
    mDisplayRect.dim.y = 400;
    mFullscreen = false;
#else
    mDisplayRect.dim.x = 1920;
    mDisplayRect.dim.y = 1080;
    mFullscreen = false;
#endif

    m_ScaleXFilter = VidFilter::NONE;
    m_normal_scale = true;
    Zoom = 1;

#ifdef USE_OPENGL
    mOpengl = true;
#else
    mOpengl = false; // Must stay optional for better compatibility
#endif

    mAspectCorrection.dim.x = 4;
    mAspectCorrection.dim.y = 3;
    m_CameraBounds.reset();

    mGameRect.pos.x = 0;
    mGameRect.pos.y = 0;

#if !TARGET_OS_IOS
    mGameRect.dim.x = 1920;
    mGameRect.dim.y = 1080;
#endif

#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
    mOpengl = false;
    mAspectCorrection.dim.x=0;
    mAspectCorrection.dim.y=0;
#endif

#if defined(ANDROID)
    mAspectCorrection.dim.x=0;
    mAspectCorrection.dim.y=0;
#endif

}

void CVidConfig::setDisplayResolution(const GsVec2D<Uint16>& res)
{
    mDisplayRect.dim = res;
}

void CVidConfig::setGameResolution(const GsVec2D<Uint16>& res)
{
    mGameRect.dim = res;
}
