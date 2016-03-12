/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*   Jan Kašpar (jan.kaspar@gmail.com) 
*
****************************************************************************/

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSet.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/ESWatcher.h"

#include "DataFormats/TotemRPDetId/interface/TotRPDetId.h"
#include "DataFormats/TotemRPDataTypes/interface/RPRecoHit.h"
#include "Geometry/TotemRecords/interface/RealGeometryRecord.h"
#include "Geometry/TotemRPGeometryBuilder/interface/TotemRPGeometry.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPTrackCandidateCollection.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPTrackCandidate.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPRecognizedPatternsCollection.h"
#include "RecoTotemRP/RPNonParallelTrackCandidateFinder/interface/FastLineRecognition.h"

#include <iostream>


/**
 * \brief Class to recognize straight line tracks, based on optimized Hough trasform.
 *
 * The search is perfomed in global U,V coordinates (wrt. beam). In this way (some of)
 * the alignment corrections can be taken into account.
**/
class RPNonParallelTrackCandidateFinder : public edm::EDProducer
{
  public:
    RPNonParallelTrackCandidateFinder(const edm::ParameterSet& conf);
    virtual ~RPNonParallelTrackCandidateFinder();
    virtual void produce(edm::Event& e, const edm::EventSetup& c);
  
  private:
    edm::InputTag detSetVectorRPRecoHitLabel;
    edm::EDGetTokenT<edm::DetSetVector<RPRecoHit> > detSetVectorRPRecoHitToken;

    unsigned int verbosity;

    /// minimal required number of active planes per projection to even start track recognition
    unsigned char minPlanesPerProjectionToSearch;

    /// minimal required number of active planes per projection to mark track candidate as fittable
    unsigned char minPlanesPerProjectionToFit;

    /// above this limit, planes are considered noisy
    unsigned int maxHitsPerPlaneToSearch;

    /// the line recognition algorithm
    FastLineRecognition *lrcgn;

    /// minimal weight of (Hough) cluster to accept it as candidate
    double threshold;

    /// maximal angle (in any projection) to mark candidate as fittable - controls track parallelity
    double max_a_toFit;

    /// whether to allow to combine most significant U and V pattern, in case there several of them
    bool allowAmbiguousCombination;

    /// exceptional settings, per RP and per projection
    std::vector<edm::ParameterSet> exceptionalSettings;

    edm::ESWatcher<RealGeometryRecord> geometryWatcher;

    /// executes line recognition in a projection
    void RecognizeAndSelect(unsigned int RPId, double z0, double threshold,
      const std::vector<const RPRecoHit *> &hits, std::vector<RPRecognizedPatterns::Line> &);
};

//----------------------------------------------------------------------------------------------------

using namespace std;
using namespace edm;

//----------------------------------------------------------------------------------------------------

RPNonParallelTrackCandidateFinder::RPNonParallelTrackCandidateFinder(const edm::ParameterSet& conf) :
  detSetVectorRPRecoHitLabel(conf.getParameter<edm::InputTag>("DetSetVectorRPRecoHitLabel")),
  verbosity(conf.getUntrackedParameter<unsigned int>("verbosity", 0)),
  minPlanesPerProjectionToSearch(conf.getParameter<unsigned int>("minPlanesPerProjectionToSearch")),
  minPlanesPerProjectionToFit(conf.getParameter<unsigned int>("minPlanesPerProjectionToFit")),
  maxHitsPerPlaneToSearch(conf.getParameter<unsigned int>("maxHitsPerPlaneToSearch")),
  lrcgn(new FastLineRecognition(conf.getParameter<double>("clusterSize_a"), conf.getParameter<double>("clusterSize_b"))),
  threshold(conf.getParameter<double>("threshold")),
  max_a_toFit(conf.getParameter<double>("max_a_toFit")),
  allowAmbiguousCombination(conf.getParameter<bool>("allowAmbiguousCombination")),
  exceptionalSettings(conf.getParameter< vector<ParameterSet> >("exceptionalSettings"))
{
  detSetVectorRPRecoHitToken = consumes<edm::DetSetVector<RPRecoHit> >(detSetVectorRPRecoHitLabel);

  produces<RPRecognizedPatternsCollection> ();
  produces<RPTrackCandidateCollection> ();
}

//----------------------------------------------------------------------------------------------------

RPNonParallelTrackCandidateFinder::~RPNonParallelTrackCandidateFinder()
{
  delete lrcgn;
}

//----------------------------------------------------------------------------------------------------

void RPNonParallelTrackCandidateFinder::RecognizeAndSelect(unsigned int RPId, double z0, double threshold_loc,
    const vector<const RPRecoHit *> &hits, vector<RPRecognizedPatterns::Line> &lines)
{
  lrcgn->GetLines(hits, z0, threshold_loc, lines);

  if (verbosity > 5)
  {
    for (unsigned int i = 0; i < lines.size(); i++)
      printf("\t\t\ta = %.3f, b = %.3f, w = %.3f\n", lines[i].a, lines[i].b, lines[i].w);
  }
}

//----------------------------------------------------------------------------------------------------

void RPNonParallelTrackCandidateFinder::produce(edm::Event& event, const edm::EventSetup& es)
{
  if (verbosity > 5)
    printf(">> RPNonParallelTrackCandidateFinder::produce (%u:%llu)\n", event.id().run(), event.id().event());

  // geometry
  ESHandle<TotemRPGeometry> geometry;
  es.get<RealGeometryRecord>().get(geometry);
  if (geometryWatcher.check(es))
    lrcgn->ResetGeometry(geometry.product());
  
  // get input and prepare output
  edm::Handle< edm::DetSetVector<RPRecoHit> > input;
  event.getByToken(detSetVectorRPRecoHitToken, input);

  auto_ptr<RPTrackCandidateCollection> output(new RPTrackCandidateCollection());
  auto_ptr<RPRecognizedPatternsCollection> patternsCollection(new RPRecognizedPatternsCollection());
  
  // map hits to RP ids
  map< unsigned int, pair< vector<const RPRecoHit*>, vector<const RPRecoHit*> > > hitMap;
  map< unsigned int, pair< map<unsigned char, unsigned int>, map<unsigned char, unsigned int> > > planeOccupancyMap;

  for (DetSetVector<RPRecoHit>::const_iterator dit = input->begin(); dit != input->end(); ++dit)
  {
    unsigned int detId = TotRPDetId::RawToDecId(dit->detId());
    unsigned int RPId = TotRPDetId::RPOfDet(detId);
    unsigned int plane = detId % 10;
    bool uDir = TotRPDetId::IsStripsCoordinateUDirection(detId);

    for (DetSet<RPRecoHit>::const_iterator hit = dit->begin(); hit != dit->end(); ++hit)
    {
      if (uDir)
      {
        hitMap[RPId].first.push_back(& (*hit));
        planeOccupancyMap[RPId].first[plane]++;
      } else {
        hitMap[RPId].second.push_back(& (*hit));
        planeOccupancyMap[RPId].second[plane]++;
      }
    }
  }

  // track recognition pot by pot
  for (map< unsigned int, pair< vector<const RPRecoHit*>, vector<const RPRecoHit*> > >::iterator it = hitMap.begin();
      it != hitMap.end(); ++it)
  {
    unsigned int rpId = it->first;

    // merge default and exceptional settings (if available)
    unsigned int minPlanesPerProjectionToFit_U = minPlanesPerProjectionToFit;
    unsigned int minPlanesPerProjectionToFit_V = minPlanesPerProjectionToFit;
    double threshold_U = threshold;
    double threshold_V = threshold;

    for (auto ps : exceptionalSettings)
    {
      unsigned int setId = ps.getParameter<unsigned int>("rpId");
      if (setId == rpId)
      {
        minPlanesPerProjectionToFit_U = ps.getParameter<unsigned int>("minPlanesPerProjectionToFit_U");
        minPlanesPerProjectionToFit_V = ps.getParameter<unsigned int>("minPlanesPerProjectionToFit_V");
        threshold_U = ps.getParameter<double>("threshold_U");
        threshold_V = ps.getParameter<double>("threshold_V");
      }
    }

    const map<unsigned char, unsigned int> &uColl = planeOccupancyMap[rpId].first;
    const map<unsigned char, unsigned int> &vColl = planeOccupancyMap[rpId].second;

    if (verbosity > 5)
    {
      printf("\tRP %u\n", rpId);
      printf("\t\tall hits: u = %lu, v = %lu\n", it->second.first.size(), it->second.second.size());
      printf("\t\tall planes: u = %lu, v = %lu\n", uColl.size(), vColl.size());
    }

    // count planes with clean data (no showers, noise, ...)
    unsigned int uPlanes = 0, vPlanes = 0;
    for (map<unsigned char, unsigned int>::const_iterator pit = uColl.begin(); pit != uColl.end(); ++pit)
      if (pit->second <= maxHitsPerPlaneToSearch)
        uPlanes++;
    for (map<unsigned char, unsigned int>::const_iterator pit = vColl.begin(); pit != vColl.end(); ++pit)
      if (pit->second <= maxHitsPerPlaneToSearch)
        vPlanes++;

    if (verbosity > 5)
      printf("\t\tplanes with clean data: u = %u, v = %i\n", uPlanes, vPlanes);

    // discard RPs with too few reasonable planes
    if (uPlanes < minPlanesPerProjectionToSearch || vPlanes < minPlanesPerProjectionToSearch)
      continue;

    // prepare data containers
    RPTrackCandidate &tc = (*output)[rpId];
    RPRecognizedPatterns &patterns = (*patternsCollection)[rpId];
    patterns.source = RPRecognizedPatterns::sNonParallel;

    // "typical" z0 for the RP
    double z0 = geometry->GetRPDevice(rpId)->translation().z();

    // u then v recognition
    if (verbosity > 5)
      printf("\t\tu recognition\n");
    RecognizeAndSelect(rpId, z0, threshold_U, it->second.first, patterns.uLines);

    if (verbosity > 5)
      printf("\t\tv recognition\n");
    RecognizeAndSelect(rpId, z0, threshold_V, it->second.second, patterns.vLines);

    if (patterns.uLines.size() == 0 || patterns.vLines.size() == 0)
    {
      // no lines
      tc.Fittable(false);
      if (verbosity)
        LogPrint("RPNonParallelTrackCandidateFinder") <<
          ">> RPNonParallelTrackCandidateFinder::produce > No line recognized in RP " << rpId << ".";
    } else {
      if ((patterns.uLines.size() > 1 || patterns.vLines.size() > 1) && !allowAmbiguousCombination)
      {
        // too many lines
        tc.Fittable(false);
        if (verbosity)
          LogPrint("RPNonParallelTrackCandidateFinder") <<
            ">> RPNonParallelTrackCandidateFinder::produce > Too many U and/or V patterns in RP " << rpId << ".";
      } else {
        // at least one line in each projection
        const RPRecognizedPatterns::Line &uLine = patterns.uLines[0];
        const RPRecognizedPatterns::Line &vLine = patterns.vLines[0];
      
        set<unsigned int> planes_u, planes_v;
        for (RPRecognizedPatterns::Line::HitCollection::const_iterator hit = uLine.hits.begin(); hit != uLine.hits.end(); ++hit)
          planes_u.insert(hit->DetId() % 10);
        for (RPRecognizedPatterns::Line::HitCollection::const_iterator hit = vLine.hits.begin(); hit != vLine.hits.end(); ++hit)
          planes_v.insert(hit->DetId() % 10);
    
        // compile the RPTrackCandidate from u and v projections
        tc.InsertHits(uLine.hits, 0.5);
        tc.InsertHits(vLine.hits, 0.5);
    
        // decide whether this candidate is worth fitting
        tc.Fittable(true);
    
        if (planes_u.size() < minPlanesPerProjectionToFit_U || planes_v.size() < minPlanesPerProjectionToFit_V)
          tc.Fittable(false);
    
        if (fabs(uLine.a) > max_a_toFit || fabs(vLine.a) > max_a_toFit)
          tc.Fittable(false);
    
        if (verbosity > 5)
        {
          printf("\t\tall hits_u = %lu, hits_v = %lu\n", it->second.first.size(), it->second.second.size());
          printf("\t\tselected hits_u = %lu, hits_v = %lu\n", uLine.hits.size(), vLine.hits.size());
          printf("\t\tselected planes_u = %lu, planes_v = %lu\n", planes_u.size(), planes_v.size());
          printf("\t\ta_u = %E, a_v = %E\n", uLine.a, vLine.a);
        }
      }
    }

    if (verbosity > 5)
      printf("\t\t--> fittable : %i\n", tc.Fittable());
  }
 
  // save output
  event.put(output);
  event.put(patternsCollection);
}
 

DEFINE_FWK_MODULE(RPNonParallelTrackCandidateFinder);

