#ifndef EventFilter_RPCRawToDigi_RPCLBRecord_h
#define EventFilter_RPCRawToDigi_RPCLBRecord_h

#include <stdint.h>

class RPCLBRecord
{
public:
    /* https://twiki.cern.ch/twiki/bin/viewauth/CMS/DtUpgradeTwinMux#RPC_payload RPC_optical_links_data_format.pdf */
    /* 4b partition is split in 3b connector + 1b partition */
    static ::uint32_t const bcn_mask_            = 0xfe000000;
    static ::uint32_t const bc0_mask_            = 0x01000000;
    static ::uint32_t const link_board_mask_     = 0x00060000;
    static ::uint32_t const eod_mask_            = 0x00008000;
    static ::uint32_t const delay_mask_          = 0x00007000;
    static ::uint32_t const connector_mask_      = 0x00000e00;
    static ::uint32_t const partition_mask_      = 0x00000100;
    static ::uint32_t const partition_data_mask_ = 0x000000ff;

    static unsigned int const bcn_offset_            = 25;
    static unsigned int const bc0_offset_            = 24;
    static unsigned int const link_board_offset_     = 17;
    static unsigned int const eod_offset_            = 15;
    static unsigned int const delay_offset_          = 12;
    static unsigned int const connector_offset_      = 9;
    static unsigned int const partition_offset_      = 8;
    static unsigned int const partition_data_offset_ = 0;

public:
    RPCLBRecord(::uint32_t const _record = 0x0);
    RPCLBRecord(unsigned int _bcn
                , bool _bc0
                , unsigned int _link_board
                , bool _eod
                , unsigned int _delay
                , unsigned int _connector
                , unsigned int _partition
                , ::uint8_t _data);

    void set(::uint32_t const _record = 0x0);
    void reset();

    ::uint32_t const & getRecord() const;

    unsigned int getBCN() const;
    bool         isBC0() const;
    unsigned int getLinkBoard() const;
    bool         isEOD() const;
    unsigned int getDelay() const;
    unsigned int getConnector() const;
    unsigned int getPartition() const;
    ::uint8_t    getPartitionData() const;

    void setBCN(unsigned int _bcn);
    void setBC0(bool _bc0);
    void setLinkBoard(unsigned int _link_board);
    void setEOD(bool _eod);
    void setDelay(unsigned int _delay);
    void setConnector(unsigned int _connector);
    void setPartition(unsigned int _partition);
    void setPartitionData(::uint8_t _data);

    bool operator<(RPCLBRecord const & _rhs) const;

protected:
    ::uint32_t record_;
};

#include "EventFilter/RPCRawToDigi/interface/RPCLBRecord.icc"

#endif // EventFilter_RPCRawToDigi_RPCLBRecord_h
