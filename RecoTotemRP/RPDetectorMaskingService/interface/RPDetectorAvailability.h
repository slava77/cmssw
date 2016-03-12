#ifndef RecoTotemRP_RPDetectorMaskingService_RPDetectorAvailability_h
#define RecoTotemRP_RPDetectorMaskingService_RPDetectorAvailability_h

class RPDetectorAvailability
{
  public:
    inline bool IsRPDetAvailableForReconstruction(unsigned int det_id) {return true;}
    inline bool IsRPDetAvailableForTrackFitting(unsigned int det_id) {return true;}
    inline bool IsRPDetStripDead(unsigned int det_id) {return false;}
    inline bool IsRPDetStripNoisy(unsigned int det_id) {return false;}
    
  private:
};

#endif
