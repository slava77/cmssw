#ifndef EventFilter_RPCRawToDigi_RPCTwinMuxRecord_h
#define EventFilter_RPCRawToDigi_RPCTwinMuxRecord_h

#include <stdint.h>

namespace rpctwinmux {

class TwinMuxRecord
{
public:
    /** Some of the types as defined in TwinMux_uROS_payload_v12.xlsx  **/
    static unsigned int const rpc_first_type_  = 0;
    static unsigned int const rpc_second_type_ = 1;
    static unsigned int const error_type_      = 2;
    static unsigned int const unknown_type_    = 3;

    static ::uint64_t const twinmux_header_identifier_mask_ = 0xf000000000000000;
    static ::uint64_t const twinmux_header_identifier_      = 0x0000000000000000;
    static ::uint64_t const rpc_first_identifier_mask_      = 0xf000000000000000;
    static ::uint64_t const rpc_first_identifier_           = 0x9000000000000000;
    static ::uint64_t const rpc_second_identifier_mask_     = 0xf000000000000000;
    static ::uint64_t const rpc_second_identifier_          = 0xe000000000000000;
    static ::uint64_t const error_identifier_mask_          = 0xf000000000000000;
    static ::uint64_t const error_identifier_               = 0xf000000000000000;

public:
    TwinMuxRecord(::uint64_t const _record = 0x0);

    static unsigned int getType(::uint64_t const _record);
    unsigned int getType() const;

    void set(::uint64_t const _record);
    void reset();

    ::uint64_t const & getRecord() const;

protected:
    ::uint64_t record_;
};

class BlockHeader
{
public:
    static ::uint64_t const ufov_mask_          = 0xf000000000000000;
    static ::uint64_t const n_amc_mask_         = 0x00f0000000000000;
    static ::uint64_t const orbit_counter_mask_ = 0x0000000ffffffff0;

    static unsigned int const ufov_offset_          = 60;
    static unsigned int const n_amc_offset_         = 52;
    static unsigned int const orbit_counter_offset_ = 4;

public:
    BlockHeader(::uint64_t const _record = 0x0);
    BlockHeader(unsigned int _ufov, unsigned int _n_amc, unsigned int _orbit_counter);

    void set(::uint64_t const _record);
    void reset();

    ::uint64_t const & getRecord() const;

    unsigned int getFirmwareVersion() const;
    unsigned int getNAMC() const;
    unsigned int getOrbitCounter() const;

    void setFirmwareVersion(unsigned int _ufov);
    void setNAMC(unsigned int _n_amc);
    void setOrbitCounter(unsigned int _orbit_counter);

protected:
    ::uint64_t record_;
};

class BlockTrailer
{
public:
    static ::uint64_t const crc_mask_           = 0xffffffff00000000;
    static ::uint64_t const block_number_mask_  = 0x000000000ff00000;
    static ::uint64_t const event_counter_mask_ = 0x00000000000ff000;
    static ::uint64_t const bx_counter_mask_    = 0x0000000000000fff;

    static unsigned int const crc_offset_           = 32;
    static unsigned int const block_number_offset_  = 20;
    static unsigned int const event_counter_offset_ = 12;
    static unsigned int const bx_counter_offset_    = 0;

public:
    BlockTrailer(::uint64_t const _record = 0x0);
    BlockTrailer(::uint32_t _crc
                 , unsigned int _block_number
                 , unsigned int _event_counter
                 , unsigned int _bx_counter);

    void set(::uint64_t const _record);
    void reset();

    ::uint64_t const & getRecord() const;

    ::uint32_t getCRC() const;
    unsigned int getBlockNumber() const;
    unsigned int getEventCounter() const;
    unsigned int getBXCounter() const;

    void setCRC(::uint32_t _crc);
    void setBlockNumber(unsigned int _block_number);
    void setEventCounter(unsigned int _event_counter);
    void setBXCounter(unsigned int _bx_counter);

protected:
    ::uint64_t record_;
};

class BlockAMCContent
{
public:
    static ::uint64_t const length_correct_mask_ = 0x4000000000000000;
    static ::uint64_t const more_blocks_mask_    = 0x2000000000000000; // is not last block
    static ::uint64_t const segmented_mask_      = 0x1000000000000000; // is not first block
    static ::uint64_t const enabled_mask_        = 0x0800000000000000;
    static ::uint64_t const present_mask_        = 0x0400000000000000;
    static ::uint64_t const valid_mask_          = 0x0200000000000000; // evn, bcn match
    static ::uint64_t const crc_ok_mask_         = 0x0100000000000000;
    static ::uint64_t const size_mask_           = 0x00ffffff00000000;
    static ::uint64_t const block_number_mask_   = 0x000000000ff00000;
    static ::uint64_t const amc_number_mask_     = 0x00000000000f0000;
    static ::uint64_t const board_id_mask_       = 0x000000000000ffff;

    static unsigned int const size_offset_         = 32;
    static unsigned int const block_number_offset_ = 20;
    static unsigned int const amc_number_offset_   = 16;
    static unsigned int const board_id_offset_     = 0;

public:
    BlockAMCContent(::uint64_t const _record = 0x0);
    BlockAMCContent(bool _length_correct
                    , bool _last_block
                    , bool _first_block
                    , bool _enabled
                    , bool _present
                    , bool _valid
                    , bool _crc_ok
                    , unsigned int _size
                    , unsigned int _block_number
                    , unsigned int _amc_number
                    , unsigned int _board_id);

    void set(::uint64_t const _record);
    void reset();

    ::uint64_t const & getRecord() const;

    bool isLengthCorrect() const;
    bool isLastBlock() const;
    bool isFirstBlock() const;
    bool isEnabled() const;
    bool isPresent() const;
    bool isValid() const;
    bool isCRCOk() const;
    unsigned int getSize() const;
    unsigned int getBlockNumber() const;
    unsigned int getAMCNumber() const;
    unsigned int getBoardId() const;

    void setLengthCorrect(bool _length_correct);
    void setLastBlock(bool _last_block);
    void setFirstBlock(bool _first_block);
    void setEnabled(bool _enabled);
    void setPresent(bool _present);
    void setValid(bool _valid);
    void setCRCOk(bool _crc_ok);
    void setSize(unsigned int _size);
    void setBlockNumber(unsigned int _block_number);
    void setAMCNumber(unsigned int _amc_number);
    void setBoardId(unsigned int _board_id);

protected:
    ::uint64_t record_;
};

class TwinMuxHeader
{
public:
    // First Word
    static ::uint64_t const amc_number_mask_    = 0x0f00000000000000;
    static ::uint64_t const event_counter_mask_ = 0x00ffffff00000000;
    static ::uint64_t const bx_counter_mask_    = 0x00000000fff00000;
    static ::uint64_t const data_length_mask_   = 0x00000000000fffff;

    static unsigned int const amc_number_offset_    = 56;
    static unsigned int const event_counter_offset_ = 32;
    static unsigned int const bx_counter_offset_    = 20;
    static unsigned int const data_length_offset_   = 0;

    // Second word
    static ::uint64_t const orbit_counter_mask_ = 0x00000000ffff0000;
    static ::uint64_t const board_id_mask_      = 0x000000000000ffff;

    static unsigned int const orbit_counter_offset_ = 16;
    static unsigned int const board_id_offset_      = 0;

    static ::uint64_t const dt_bx_window_mask_  = 0x0000001f00000000;
    static ::uint64_t const rpc_bx_window_mask_ = 0x000003e000000000;
    static ::uint64_t const ho_bx_window_mask_  = 0x00001c0000000000;

    static unsigned int const dt_bx_window_offset_  = 32;
    static unsigned int const rpc_bx_window_offset_ = 37;
    static unsigned int const ho_bx_window_offset_  = 42;

public:
    TwinMuxHeader();
    TwinMuxHeader(::uint64_t const _record[2]);
    TwinMuxHeader(unsigned int _amc_number
                  , unsigned int _event_counter
                  , unsigned int _bx_counter
                  , unsigned int _data_length
                  , unsigned int _orbit_counter
                  , unsigned int _board_id
                  , unsigned int _dt_bx_window = 0x1f
                  , int _rpc_bx_min = 10, int _rpc_bx_max = 5
                  , unsigned int _ho_bx_window = 0x7);

    void set(unsigned int _nword, ::uint64_t const _word);
    void reset();

    ::uint64_t const * getRecord() const;

    unsigned int getAMCNumber() const;
    unsigned int getEventCounter() const;
    unsigned int getBXCounter() const;
    unsigned int getDataLength() const;

    unsigned int getOrbitCounter() const;
    unsigned int getBoardId() const;

    bool hasDTBXWindow() const;
    unsigned int getDTBXWindow() const;
    bool hasRPCBXWindow() const;
    int getRPCBXMin() const;
    int getRPCBXMax() const;
    bool hasHOBXWindow() const;
    unsigned int getHOBXWindow() const;

    void setAMCNumber(unsigned int _amc_number);
    void setEventCounter(unsigned int _event_counter);
    void setBXCounter(unsigned int _bx_counter);
    void setDataLength(unsigned int _data_length);

    void setOrbitCounter(unsigned int _orbit_counter);
    void setBoardId(unsigned int _board_id);

    void setDTBXWindow(unsigned int _bx_window = 0x1f);
    void setRPCBXWindow(int _bx_min = 10, int _bx_max = 5);
    void setHOBXWindow(unsigned int _bx_window = 0x7);

protected:
    ::uint64_t record_[2];
};

class TwinMuxTrailer
{
public:
    static ::uint64_t const crc_mask_           = 0xffffffff00000000;
    static ::uint64_t const event_counter_mask_ = 0x00000000ff000000;
    static ::uint64_t const data_length_mask_   = 0x00000000000fffff;

    static unsigned int const crc_offset_           = 32;
    static unsigned int const event_counter_offset_ = 24;
    static unsigned int const data_length_offset_   = 0;

public:
    TwinMuxTrailer(::uint64_t const _record = 0x0);
    TwinMuxTrailer(::uint32_t _crc
                   , unsigned int _event_counter
                   , unsigned int _data_length);

    void set(::uint64_t const _record);
    void reset();

    ::uint64_t const & getRecord() const;

    ::uint32_t getCRC() const;
    unsigned int getEventCounter() const;
    unsigned int getDataLength() const;

    void setCRC(::uint32_t _crc);
    void setEventCounter(unsigned int _event_counter);
    void setDataLength(unsigned int _data_length);

protected:
    ::uint64_t record_;
};

class RPCLinkRecord
{
public:
    static ::uint32_t const da_mask_             = 0x080000;
    static ::uint32_t const de_mask_             = 0x040000;
    static ::uint32_t const eod_mask_            = 0x020000;
    static ::uint32_t const delay_mask_          = 0x01c000;
    static ::uint32_t const link_board_mask_     = 0x003000;
    static ::uint32_t const connector_mask_      = 0x000e00;
    static ::uint32_t const partition_mask_      = 0x000100;
    static ::uint32_t const partition_data_mask_ = 0x0000ff;

    static unsigned int const delay_offset_          = 14;
    static unsigned int const link_board_offset_     = 12;
    static unsigned int const connector_offset_      = 9;
    static unsigned int const partition_offset_      = 8;
    static unsigned int const partition_data_offset_ = 0;

public:
    RPCLinkRecord(::uint32_t const _record = da_mask_);

    void set(::uint32_t const _record = da_mask_);
    void reset();

    ::uint32_t const & getRecord() const;

    bool isAcknowledge() const;
    bool isError() const;
    bool isEOD() const;
    unsigned int getDelay() const;
    unsigned int getLinkBoard() const;
    unsigned int getConnector() const;
    unsigned int getPartition() const;
    ::uint8_t    getPartitionData() const;

    void setAcknowledge(bool _da);
    void setError(bool _de);
    void setEOD(bool _eod);
    void setDelay(unsigned int _delay);
    void setLinkBoard(unsigned int _link_board);
    void setConnector(unsigned int _connector);
    void setPartition(unsigned int _partition);
    void setPartitionData(::uint8_t _data);

protected:
    ::uint32_t record_;
};

class RPCBXRecord
{
public:
    static ::uint8_t const bc0_mask_ = 0x04;
    static ::uint8_t const bcn_mask_ = 0x03;
    static unsigned int const bcn_offset_ = 0;

public:
    RPCBXRecord(::uint8_t const _record = 0x0);

    void set(::uint8_t const _record = 0x0);
    void reset();

    ::uint8_t const & getRecord() const;

    bool isBC0() const;
    unsigned int getBXCounter() const;

    void setBC0(bool _bc0);
    void setBXCounter(unsigned int _bcn);

protected:
    ::uint8_t record_;
};

class RPCRecord
{
public:
    // first word
    static ::uint64_t const bx_offset_mask_ = 0x0f00000000000000;
    static ::uint64_t const overflow_mask_  = 0x0080000000000000;

    static unsigned int const bx_offset_offset_ = 56;

    static ::uint64_t const link_record_mask_ = 0x0fffff;
    static unsigned int const link_record_0_offset_ = 20;
    static unsigned int const link_record_1_offset_ = 0;

    static ::uint64_t const bx_record_mask_ = 0x07;
    static unsigned int const bx_record_0_offset_ = 52;
    static unsigned int const bx_record_1_offset_ = 49;
    static unsigned int const bx_record_2_offset_ = 46;
    static unsigned int const bx_record_3_offset_ = 43;
    static unsigned int const bx_record_4_offset_ = 40;

    // second word
    static unsigned int const link_record_2_offset_ = 40;
    static unsigned int const link_record_3_offset_ = 20;
    static unsigned int const link_record_4_offset_ = 0;

public:
    RPCRecord();
    RPCRecord(::uint64_t const _record[2]);

    void set(unsigned int _word, ::uint64_t const _record);
    void reset();

    ::uint64_t const * getRecord() const;

    int getBXOffset() const;
    bool hasOverflow() const;
    RPCBXRecord getRPCBXRecord(unsigned int _link) const;
    RPCLinkRecord getRPCLinkRecord(unsigned int _link) const;

    void setBXOffset(int _bx_offset);
    void setOverflow (bool _overflow);
    void setRPCBXRecord(unsigned int _link, RPCBXRecord const & _bx_record);
    void setRPCLinkRecord(unsigned int _link, RPCLinkRecord const & _link_record);

protected:
    ::uint64_t record_[2];
};

} // namespace rpctwinmux

#include "EventFilter/RPCRawToDigi/interface/RPCTwinMuxRecord.icc"

#endif // EventFilter_RPCRawToDigi_RPCTwinMuxRecord_h
