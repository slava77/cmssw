/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors:
* 	Hubert Niewiadomski
*   Jan Kašpar (jan.kaspar@gmail.com)
*
****************************************************************************/

#ifndef RecoTotemRP_RPRecoDataFormats_RPTrackCandidate_h
#define RecoTotemRP_RPRecoDataFormats_RPTrackCandidate_h

#include "DataFormats/TotemRPDataTypes/interface/RPRecoHit.h"
#include "DataFormats/TotemRPDataTypes/interface/RPTypes.h"

#include <vector>
#include <set>

/**
 *\brief A candidate for a track in RP (quite generally, a result of pattern recognition).
 **/
class RPTrackCandidate
{
  public:
    typedef std::vector<RPRecoHit> RecHitContainer;
    typedef RecHitContainer::const_iterator const_iterator;
    typedef std::pair<const_iterator,const_iterator> range;
    
    RPTrackCandidate(const RecHitContainer &rh, bool _f = true) : rh_(rh), fittable_(_f), weight_(0.0), u_id_(0), v_id_(0) {}
    RPTrackCandidate() : fittable_(true), weight_(0.0), u_id_(0), v_id_(0) {}

    void InsertHits(const RecHitContainer &vect, double weight)
    {
      rh_.insert(rh_.end(), vect.begin(), vect.end());
      weight_ += weight;
    }
    
    void InsertHits(const std::vector<const RPRecoHit *> &vect, double weight)
    {
      for (unsigned int i = 0; i < vect.size(); i++)
        rh_.push_back(* vect[i]);
      weight_ += weight;
    }
    
    void InsertHits(const std::set<const RPRecoHit *> &data, double weight)
    {
      for (std::set<const RPRecoHit *>::iterator it = data.begin(); it != data.end(); ++it)
        rh_.push_back(* (*it));
      weight_ += weight;
    }
    
    virtual ~RPTrackCandidate(){}

    inline bool Fittable() const {return fittable_;}
    inline void Fittable(bool fittable) {fittable_ = fittable;}

    inline void Weight(double weight) {weight_ = weight;}
    inline double Weight() const {return weight_;}
    inline unsigned int Size() const {return rh_.size();}
    
    range recHits() const {return std::make_pair(rh_.begin(), rh_.end());}
    inline const std::vector<RPRecoHit> &TrackRecoHits() const {return rh_;}
    
    inline void SetUVid(unsigned int u, unsigned int v) {u_id_=u; v_id_ = v;}
    inline unsigned int GetUid() const {return u_id_;}
    inline unsigned int GetVid() const {return v_id_;}
    
  private:
    RecHitContainer rh_;		///< selected hits, i.e. those that shall form the track
    bool fittable_;
    double weight_;
    unsigned int u_id_, v_id_;  ///<if there are multiple tracks the uv combination id is preserved here
};


#endif  //RecoTotemRP_RPRecoDataFormats_RPTrackCandidate_h

