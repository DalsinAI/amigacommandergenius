#include "TypeDefinitions.h"
#include <SDL.h>


auto GETBYTE(const gs_byte *offset) -> gs_byte
{
    return *((gs_byte *)(offset));
}



auto GETWORD(const gs_byte *ptr) -> word
{
    // Keen's files are little-endian on every host: read byte by byte,
    // the same on a big-endian machine (the Amiga) as on a PC.
    return (ptr[0] | ptr[1] << 8);
}

/**
 * @brief READWORD Like GETWORD but the pointer will also be modified, by
 *                 going two bytes forward
 * @param ptr      reference to pointer (*&) which will be modified
 * @return         the value
 */
auto READWORD(gs_byte *&ptr) -> word
{
    const auto val = GETWORD(ptr);
    ptr += sizeof(word);
    return val;
}


/**
 * @brief GETLONGWORD   Like GETWORD but 4 bytes.
 * @param ptr           Pointer to data
 * @return              The value
 */
auto GETLONGWORD(const gs_byte *ptr) -> longword
{
    // little-endian on every host, as GETWORD
    longword val = ptr[0] | ptr[1] << 8 | ptr[2] << 16 | (longword)ptr[3] << 24;
    return val;
}


/**
 * @brief READLONGWORD  Like READWORD but 4 bytes
 * @param ptr      reference to pointer (*&) which will be modified
 * @return         the value
 */
auto READLONGWORD(gs_byte *&ptr) -> longword
{
    const auto val = GETLONGWORD(ptr);
    ptr += 4;
    return val;
}

/**
 * @brief MERGERLOFFSET Strange offset merger.
 *                      For some offsets, it helps detecting the pointer correctly.
 *                      It is used for level names basically
 * @param levelLongWord word to work on
 * @return tranformed value
 */
auto MERGERLOFFSET(const unsigned long levelLongWord) ->  unsigned long
{
    const unsigned long part1 = levelLongWord >> 12;
    const unsigned long part2 = levelLongWord & 0xFFF;
    return part1 + part2;
};


