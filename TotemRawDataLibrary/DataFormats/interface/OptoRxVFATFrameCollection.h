/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors: 
*  Jan Kašpar (jan.kaspar@gmail.com) 
*    
****************************************************************************/


#ifndef _Totem_OptoRxVFATFrameCollection_h_
#define _Totem_OptoRxVFATFrameCollection_h_

#include "TotemRawDataLibrary/DataFormats/interface/VFATFrameCollection.h"
#include "TotemRawDataLibrary/DataFormats/interface/VFATFrame.h"

#include <map>

namespace Totem {

/**
 * An implementation of the VFAT frame collection optimized for OptoRx-formatted data.
**/
class OptoRxVFATFrameCollection : public VFATFrameCollection
{
  public:
    /// a block of VFATs corresponding to one GOH
    struct GOHBlock {
      bool valid;                 ///< whether this block contains valid data
      static unsigned int size;   ///< the size
      VFATFrame *buffer;          ///< pointer to the VFATFrame array of size `size'
      VFATFrame::word** dataPtrs; ///< array of pointers to the data blocks within the VFAT frames

      GOHBlock();
      ~GOHBlock()
        { delete [] buffer; delete [] dataPtrs; }
    };

    /// a block of GOH blocks corresponding to one OptoRx
    struct OptoRxBlock {
      static unsigned int size;   ///< the size
      GOHBlock *buffer;           ///< pointer to the VFATFrame array of size `size'

      OptoRxBlock() : buffer(new GOHBlock[size]) {}
      ~OptoRxBlock()
        { delete [] buffer; }
    };

  protected:
    /// map: OptoRx ID --> OptoRx data block
    typedef std::map<unsigned int, OptoRxBlock*> MapType;

    /// the collection data
    MapType data;

    virtual value_type BeginIterator() const;
    virtual value_type NextIterator(const value_type&) const;
    virtual bool IsEndIterator(const value_type&) const;

  public:
    typedef MapType::iterator IteratorType;

    OptoRxVFATFrameCollection();
    ~OptoRxVFATFrameCollection();

    virtual std::string GetClassName() const
      { return "OptoRxVFATFrameCollection"; }
    
    const VFATFrame* GetFrameByID(unsigned int ID) const;
    const VFATFrame* GetFrameByIndex(FramePosition index) const;
    
    virtual unsigned int Size() const;

    virtual bool Empty() const
      { return (Size() == 0); }
    
    /// Empties the data collection.
    /// use rather the Invalidate method unless you really wish to free the memory
    void Clear()
      { data.clear(); }

    /// sets valid flags of all blocks to false
    void Invalidate();

    /// inserts (if needed)  an OptoRx block with the given id
    IteratorType InsertOptoRxBlock(unsigned int OptoRxId);

    /// sets the given GOH block as valid and returns the array of VFAT data pointers
    VFATFrame::word** ValidateGOHBlock(const IteratorType &it, unsigned int GOHId);
};

} // namespace
#endif

