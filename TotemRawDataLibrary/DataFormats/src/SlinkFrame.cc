/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors: 
*  Hubert Niewiadomski
*    
****************************************************************************/


#include "TotemRawDataLibrary/DataFormats/interface/SlinkFrame.h"

#include <iostream>
#include <vector>
#include <cstring>


namespace Totem {

void SlinkFrame::SetData(char *data_)
{
  if(data_) {
    memcpy(frame, data_, SLINK_FRAME_SIZE);
  }
}

//----------------------------------------------------------------------------------------------------

/**
 * This method assumes that collection \c fc has at least \c NUMBER_OF_VFATS elements.
 * It decodes one Slink frame to the first \c NUMBER_OF_VFATS frames in the collection.
**/
void SlinkFrame::DecodeVFATFrames(SimpleVFATFrameCollection *fc)
{
  // zero the frames and create vector of VFATFrame data pointers
  // TODO: this may not be really efficient - it frees memory and then allocate it again
  fc->Clear();  

  std::vector<short unsigned int*> datavector;
  for(unsigned int i = 0; i < NUMBER_OF_VFATS; ++i) {
    datavector.push_back((short unsigned int*) fc->InsertEmptyFrame(i)->getData());
  }

  // compute payload address
  unsigned int *p = (unsigned int *) (frame + SLINK_FOOTER_SIZE);
  
  for(int vfat_bit=VFAT_FRAME_BIT_SIZE-1; vfat_bit>=0; --vfat_bit)
  {
    unsigned int frame_short_word_num = vfat_bit/16;
    unsigned int sh_word_bit_no = vfat_bit%16;
    
    unsigned bit;
    for(int bit_range=0; bit_range<=1; ++bit_range)
    {
      unsigned int word = *(p++);
      unsigned int up_bit=32*bit_range+32;
      unsigned int lo_bit=32*bit_range;
      for(bit=lo_bit; bit<up_bit; ++bit)
      {
        unsigned short bit_value = (unsigned short) (word & 0x1);
        unsigned short *vfat_frame_word = datavector.at(bit) + frame_short_word_num;
        //unsigned short *vfat_frame_word = (short unsigned int*)fc->GetFrameByIndex(bit)->getData() + frame_short_word_num;
          
        *vfat_frame_word = (*vfat_frame_word) | bit_value<<sh_word_bit_no;

        word = word>>1;
      }
    }
  }
}

} // namespace
