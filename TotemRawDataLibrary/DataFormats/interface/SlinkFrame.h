/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors: 
*  Jan Kašpar (jan.kaspar@gmail.com) 
*  Leszek Grzanka    
*  Hubert Niewiadomski
*
****************************************************************************/


#ifndef _Totem_SLINK_FRAME__
#define _Totem_SLINK_FRAME__

#include <vector>
#include "TotemRawDataLibrary/DataFormats/interface/SimpleVFATFrameCollection.h"

namespace Totem {

class VFATFrameCollection;

// in bytes
#define SLINK_HEADER_SIZE 0
#define SLINK_FOOTER_SIZE 0
// in bits
#define VFAT_FRAME_BIT_SIZE 192
#define NUMBER_OF_VFATS 64
#define SLINK_FRAME_SIZE (SLINK_HEADER_SIZE+VFAT_FRAME_BIT_SIZE*NUMBER_OF_VFATS+SLINK_FOOTER_SIZE)

/**
 * Structure for a Slink frame.
 * 
 * Decodes a S-link frame into VFAT frames, see DecodeVFATFrames() method.
**/
class SlinkFrame
{
  public:
    SlinkFrame() {};
    SlinkFrame(char *data_) {SetData(data_);}
    char *GetSlinkFrame() {return frame;}
    void SetData(char *data_);

    /// Converts a Slink frame into array of 64 VFAT frames
    void DecodeVFATFrames(SimpleVFATFrameCollection *);
    
    unsigned long long GetTag() { return tag; }
    void SetTag(unsigned long long _t) { tag = _t; }

  private:
    /// buffer, MS Word of 64 bits at lowest address, little endian 64-bit numbers stored
    char frame[SLINK_FRAME_SIZE]; 
    unsigned long long tag;
};

} // namespace

#endif

