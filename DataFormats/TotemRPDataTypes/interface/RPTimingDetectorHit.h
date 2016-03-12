#ifndef DataFormats_TotemRPDataTypes_RPTimingDetectorHit_H
#define DataFormats_TotemRPDataTypes_RPTimingDetectorHit_H

#include "DataFormats/TotemRPDataTypes/interface/RPTypes.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"

class RPTimingDetectorHit {
public:
    RPTimingDetectorHit(const RPDetId &det_id = 0, unsigned short electrode_id = 0,
                        const Local2DPoint position = Local2DPoint(0, 0)) : det_id(det_id),
                                                        electrode_id(electrode_id),
                                                        position(position) { }

    RPDetId GetDetId() const { return det_id; }
    unsigned short GetElectrodeId() const { return electrode_id; }
    const Local2DPoint &GetPosition() const {
        return position;
    }

private:
    RPDetId det_id;
    unsigned short electrode_id;
    Local2DPoint position;
};


inline bool operator<(const RPTimingDetectorHit & a, const RPTimingDetectorHit & b) {
    if (a.GetDetId() == b.GetDetId()) {
        return a.GetElectrodeId() < b.GetElectrodeId();
    }

    return a.GetDetId() < b.GetDetId();
}

#endif