#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "RecoTotemRP/RPInverseParameterization/interface/RPInverseParameterization.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPReconstructedProton.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPReconstructedProtonCollection.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPFittedTrack.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPFittedTrackCollection.h"
#include "DataFormats/TotemRPDataTypes/interface/RPTypes.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RP2DHit.h"
#include "RecoTotemRP/RPRomanPotResolutionService/interface/RPFitResolution.h"
#include "TotemCondFormats/BeamOpticsParamsObjects/interface/BeamOpticsParams.h"
#include "TotemCondFormats/DataRecord/interface/BeamOpticsParamsRcd.h"
#include "TotemProtonTransport/TotemRPProtonTransportParametrization/interface/LHCOpticsApproximator.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "HepMC/GenEvent.h"

#include "TFile.h"
#include "TVector3.h"
#include "TRandom2.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>

/**
 \brief Inelastic proton reconstruction.
**/
class RPPrimaryVertexInelasticReconstruction : public edm::EDProducer
{
  public:
    typedef std::map<RPId, RP2DHit> rec_tracks_collection;

    explicit RPPrimaryVertexInelasticReconstruction(const edm::ParameterSet& conf);
    virtual ~RPPrimaryVertexInelasticReconstruction();
    virtual void beginRun(edm::Run const&, edm::EventSetup const&);
    virtual void produce(edm::Event& e, const edm::EventSetup& c);

  private:
    typedef std::vector<int> station_rp_ids_type;

    edm::InputTag rpFittedTrackCollectionLabel;
    edm::EDGetTokenT<RPFittedTrackCollection> rpFittedTrackCollectionToken;

    edm::InputTag HepMCProductLabel;
    edm::EDGetTokenT<edm::HepMCProduct> HepMCProductToken;

    const edm::ParameterSet conf_;
    int verbosity_;
    std::auto_ptr<RPInverseParameterization> inv_param_right_;
    std::auto_ptr<RPInverseParameterization> inv_param_left_;

    std::string param_file_name_220_right_;
    std::string param_file_name_220_left_;
    std::string param_file_name_210_right_;
    std::string param_file_name_210_left_;

    std::string param_prefix_220_right_;
    std::string param_prefix_220_left_;
    std::string param_prefix_210_right_;
    std::string param_prefix_210_left_;

    std::string right_beam_postfix_;
    std::string left_beam_postfix_;

    bool external_primary_vertex_;
    bool set_primary_vertex_to_zero_;

    TVector3 primary_vertex_;
    TVector3 primary_vertex_nom_pos_;
    TVector3 primary_vertex_error_;

    double rp_multiple_scattering_sigma_;

    TRandom2 rand_;
    RPFitResolution resol_degrad_service_;

    BeamOpticsParams BOPar_;

    /// Initialize the optics parameterisation
    void InitInverseParametrizationFitter();

    /// Select hits with given arm id.
    /// Selects only events
    ///    * with two and more units active
    ///    * without top and bottom RPs active at the same time
    /// Returns the number of RPs selected.
    int SelectHits(const RPFittedTrackCollection &tracks, unsigned int armId, rec_tracks_collection &coll);

    /// Increases the hit uncertainties for the contribution(s) from multiple scattering in preceeding RPs
    void AddInStationMultipleScatteringContribution(rec_tracks_collection &col,
          double rp_multiple_scattering_sigma);

    /// Extracts the external primary vertex.
    /// Returns the number of vertices found.
    bool FindPrimaryVertex(edm::Event& e);

    /// Runs the proton reconstruction for one arm.
    void Reconstruct(const rec_tracks_collection &rec_col, RPInverseParameterization &inv_par,
      RPReconstructedProtonCollection & rec_prot_col, double zdirection, bool eternal_prim_vert);
};

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

using namespace std;
using namespace edm;

//----------------------------------------------------------------------------------------------------

RPPrimaryVertexInelasticReconstruction::RPPrimaryVertexInelasticReconstruction(const edm::ParameterSet& conf)
 : conf_(conf), resol_degrad_service_(conf)
{
  rpFittedTrackCollectionLabel = conf.getParameter<edm::InputTag>("RPFittedTrackCollectionLabel");
  rpFittedTrackCollectionToken = consumes<RPFittedTrackCollection>(rpFittedTrackCollectionLabel);

  produces< RPReconstructedProtonCollection > ();
}

//----------------------------------------------------------------------------------------------------

void RPPrimaryVertexInelasticReconstruction::InitInverseParametrizationFitter()
{
  typedef std::map<RPId, LHCOpticsApproximator> rp_param_map_type;
  rp_param_map_type rp_param_map_right;
  rp_param_map_type rp_param_map_left;
  std::string par_name;

  string cmsswBase = getenv("CMSSW_BASE");

  // 220 station right
  std::string fileName_220_r = cmsswBase + std::string("/src/") + param_file_name_220_right_;
  TFile *f_220_r = TFile::Open(fileName_220_r.c_str(),"read");
  par_name = param_prefix_220_right_+"_v_1_"+right_beam_postfix_;
  rp_param_map_right[120] = *((LHCOpticsApproximator*) f_220_r->Get(par_name.c_str()));
  rp_param_map_right[121] = *((LHCOpticsApproximator*) f_220_r->Get(par_name.c_str()));

  par_name = param_prefix_220_right_+"_h_1_"+right_beam_postfix_;
  rp_param_map_right[122] = *((LHCOpticsApproximator*) f_220_r->Get(par_name.c_str()));

  par_name = param_prefix_220_right_+"_h_2_"+right_beam_postfix_;
  rp_param_map_right[123] = *((LHCOpticsApproximator*) f_220_r->Get(par_name.c_str()));

  par_name = param_prefix_220_right_+"_v_2_"+right_beam_postfix_;
  rp_param_map_right[124] = *((LHCOpticsApproximator*) f_220_r->Get(par_name.c_str()));
  rp_param_map_right[125] = *((LHCOpticsApproximator*) f_220_r->Get(par_name.c_str()));
  f_220_r->Close();

  // 220 station left
  std::string fileName_220_l = cmsswBase + std::string("/src/") + param_file_name_220_left_;
  TFile *f_220_l = TFile::Open(fileName_220_l.c_str(),"read");
  par_name = param_prefix_220_left_+"_v_1_"+left_beam_postfix_;
  rp_param_map_left[20] = *((LHCOpticsApproximator*) f_220_l->Get(par_name.c_str()));
  rp_param_map_left[21] = *((LHCOpticsApproximator*) f_220_l->Get(par_name.c_str()));

  par_name = param_prefix_220_left_+"_h_1_"+left_beam_postfix_;
  rp_param_map_left[22] = *((LHCOpticsApproximator*) f_220_l->Get(par_name.c_str()));

  par_name = param_prefix_220_left_+"_h_2_"+left_beam_postfix_;
  rp_param_map_left[23] = *((LHCOpticsApproximator*) f_220_l->Get(par_name.c_str()));

  par_name = param_prefix_220_left_+"_v_2_"+left_beam_postfix_;
  rp_param_map_left[24] = *((LHCOpticsApproximator*) f_220_l->Get(par_name.c_str()));
  rp_param_map_left[25] = *((LHCOpticsApproximator*) f_220_l->Get(par_name.c_str()));
  f_220_l->Close();
  
  // 210 station right
  std::string fileName_210_r = cmsswBase + std::string("/src/") + param_file_name_210_right_;
  TFile *f_210_r = TFile::Open(fileName_210_r.c_str(),"read");
  par_name = param_prefix_210_right_+"_v_1_"+right_beam_postfix_;
  rp_param_map_right[100] = *((LHCOpticsApproximator*) f_210_r->Get(par_name.c_str()));
  rp_param_map_right[101] = *((LHCOpticsApproximator*) f_210_r->Get(par_name.c_str()));

  par_name = param_prefix_210_right_+"_h_1_"+right_beam_postfix_;
  rp_param_map_right[102] = *((LHCOpticsApproximator*) f_210_r->Get(par_name.c_str()));

  par_name = param_prefix_210_right_+"_h_2_"+right_beam_postfix_;
  rp_param_map_right[103] = *((LHCOpticsApproximator*) f_210_r->Get(par_name.c_str()));

  par_name = param_prefix_210_right_+"_v_2_"+right_beam_postfix_;
  rp_param_map_right[104] = *((LHCOpticsApproximator*) f_210_r->Get(par_name.c_str()));
  rp_param_map_right[105] = *((LHCOpticsApproximator*) f_210_r->Get(par_name.c_str()));
  f_210_r->Close();

  // 210 station left
  std::string fileName_210_l = cmsswBase + std::string("/src/") + param_file_name_210_left_;
  TFile *f_210_l = TFile::Open(fileName_210_l.c_str(),"read");
  par_name = param_prefix_210_left_+"_v_1_"+left_beam_postfix_;
  rp_param_map_left[0] = *((LHCOpticsApproximator*) f_210_l->Get(par_name.c_str()));
  rp_param_map_left[1] = *((LHCOpticsApproximator*) f_210_l->Get(par_name.c_str()));

  par_name = param_prefix_210_left_+"_h_1_"+left_beam_postfix_;
  rp_param_map_left[2] = *((LHCOpticsApproximator*) f_210_l->Get(par_name.c_str()));

  par_name = param_prefix_210_left_+"_h_2_"+left_beam_postfix_;
  rp_param_map_left[3] = *((LHCOpticsApproximator*) f_210_l->Get(par_name.c_str()));

  par_name = param_prefix_210_left_+"_v_2_"+left_beam_postfix_;
  rp_param_map_left[4] = *((LHCOpticsApproximator*) f_210_l->Get(par_name.c_str()));
  rp_param_map_left[5] = *((LHCOpticsApproximator*) f_210_l->Get(par_name.c_str()));
  f_210_l->Close();
  
  inv_param_right_->SetParameterizations(rp_param_map_right);
  inv_param_left_->SetParameterizations(rp_param_map_left);
  
  if (verbosity_)
  {
    for (rp_param_map_type::iterator it = rp_param_map_right.begin(); it!=rp_param_map_right.end(); ++it)
    {
      std::cout<<"RPId : "<<it->first;
      it->second.PrintOpticalFunctions();
      std::cout<<std::endl;
    }
    
    for(rp_param_map_type::iterator it = rp_param_map_left.begin(); 
      it!=rp_param_map_left.end(); ++it)
    {
      std::cout<<"RPId : "<<it->first;
      it->second.PrintOpticalFunctions();
      std::cout<<std::endl;
    }
  }
}

//----------------------------------------------------------------------------------------------------

RPPrimaryVertexInelasticReconstruction::~RPPrimaryVertexInelasticReconstruction()
{
}

//----------------------------------------------------------------------------------------------------

void RPPrimaryVertexInelasticReconstruction::beginRun(edm::Run const&, edm::EventSetup const& es)
{
  edm::ESHandle<BeamOpticsParams> BOParH;
  es.get<BeamOpticsParamsRcd>().get(BOParH);
  if (!BOParH.isValid())
    throw cms::Exception("RPPrimaryVertexInelasticReconstruction::beginRun") << " edm::ESHandle<BeamOpticsParams> is invalid" << endl;
  BOPar_ = *BOParH;
  
  inv_param_right_ = std::auto_ptr<RPInverseParameterization>(new RPInverseParameterization(1.0, conf_, BOPar_));
  inv_param_left_ = std::auto_ptr<RPInverseParameterization>(new RPInverseParameterization(-1.0, conf_, BOPar_));

  verbosity_ = conf_.getParameter<int>("Verbosity");
  inv_param_right_->Verbosity(verbosity_);
  inv_param_left_->Verbosity(verbosity_);

  external_primary_vertex_ = conf_.getParameter<bool>("ExternalPrimaryVertex");
  set_primary_vertex_to_zero_ = conf_.getParameter<bool>("ConstrainPrimaryVertex");
  external_primary_vertex_ = external_primary_vertex_ && !set_primary_vertex_to_zero_;
  
  param_file_name_210_right_ = conf_.getParameter<std::string>("ParameterizationFileName210Right");
  param_file_name_210_left_ = conf_.getParameter<std::string>("ParameterizationFileName210Left");
  param_prefix_210_right_ = conf_.getParameter<std::string>("ParameterizationNamePrefix210Right");
  param_prefix_210_left_ = conf_.getParameter<std::string>("ParameterizationNamePrefix210Left");
  
  param_file_name_220_right_ = conf_.getParameter<std::string>("ParameterizationFileName220Right");
  param_file_name_220_left_ = conf_.getParameter<std::string>("ParameterizationFileName220Left");
  param_prefix_220_right_ = conf_.getParameter<std::string>("ParameterizationNamePrefix220Right");
  param_prefix_220_left_ = conf_.getParameter<std::string>("ParameterizationNamePrefix220Left");
  
  if (external_primary_vertex_)
  {
    HepMCProductLabel = conf_.getParameter<edm::InputTag>("HepMCProductLabel");
    HepMCProductToken = consumes<edm::HepMCProduct>(HepMCProductLabel);
    primary_vertex_error_.SetX(conf_.getParameter<double>("PrimaryVertexXSigma"));
    primary_vertex_error_.SetY(conf_.getParameter<double>("PrimaryVertexYSigma"));
    primary_vertex_error_.SetZ(conf_.getParameter<double>("PrimaryVertexZSigma"));
  }
  
  if (set_primary_vertex_to_zero_)
  {
    // conversion from meters to mm
    primary_vertex_error_.SetX(BOPar_.GetPrimVertSizeX()*1000.0);
    primary_vertex_error_.SetY(BOPar_.GetPrimVertSizeY()*1000.0);
    primary_vertex_error_.SetZ(BOPar_.GetPrimVertSizeZ()*1000.0);
    primary_vertex_nom_pos_.SetX(BOPar_.GetBeamDisplacementX()*1000.0);
    primary_vertex_nom_pos_.SetY(BOPar_.GetBeamDisplacementY()*1000.0);
    primary_vertex_nom_pos_.SetZ(BOPar_.GetBeamDisplacementZ()*1000.0);
  }
  
  right_beam_postfix_ = conf_.getParameter<std::string>("RightBeamPostfix");
  left_beam_postfix_ = conf_.getParameter<std::string>("LeftBeamPostfix");
  
  rp_multiple_scattering_sigma_ = conf_.getParameter<double>("RPMultipleScatteringSigma");

  InitInverseParametrizationFitter();
}

//----------------------------------------------------------------------------------------------------

void RPPrimaryVertexInelasticReconstruction::produce(edm::Event& e, const edm::EventSetup& c)
{
  //printf("--------------------------------- event %u ----------------------------------------\n", e.id().event());

  // get input
  edm::Handle< RPFittedTrackCollection > input;
  RPReconstructedProtonCollection reconstructed_proton_collection;

  e.getByToken(rpFittedTrackCollectionToken, input);

  // process primary vertex
  if (external_primary_vertex_)
  {
    if(!FindPrimaryVertex(e))
      throw cms::Exception("RPPrimaryVertexInelasticReconstruction::produce") << "Primary vertex not found." << endl;
  }
  
  if (set_primary_vertex_to_zero_)
  {
    primary_vertex_= primary_vertex_nom_pos_;
  }
  
  // split input hits per station
  rec_tracks_collection hits_l;
  SelectHits(*input, 0, hits_l);

  rec_tracks_collection hits_r;
  SelectHits(*input, 1, hits_r);
  
  // can proton in an arm be reconstructed?
  bool left_reconstructable = (hits_l.size() >= 2);
  bool right_reconstructable = (hits_r.size() >= 2);

  /*
  printf("hits_l.size = %lu\n", hits_l.size());
  printf("hits_r.size = %lu\n", hits_r.size());

  printf("hits_l: ");
  for (const auto &h : hits_l)
    printf("%u, ", h.first);
  printf("\n");

  printf("hits_r: ");
  for (const auto &h : hits_r)
    printf("%u, ", h.first);
  printf("\n");

  printf("left_reconstructable = %u\n", left_reconstructable);
  printf("right_reconstructable = %u\n", right_reconstructable);
  */
  
  // run the reconstruction
  if (left_reconstructable)
  {
    AddInStationMultipleScatteringContribution(hits_l, rp_multiple_scattering_sigma_);
    //printf("* reconstruction left\n");
    Reconstruct(hits_l, *inv_param_left_, reconstructed_proton_collection, -1.0,
      external_primary_vertex_ || set_primary_vertex_to_zero_);
  }

  if (right_reconstructable)
  {
    AddInStationMultipleScatteringContribution(hits_r, rp_multiple_scattering_sigma_);
    //printf("* reconstruction right\n");
    Reconstruct(hits_r, *inv_param_right_, reconstructed_proton_collection, 1.0,
      external_primary_vertex_ || set_primary_vertex_to_zero_);
  }

  auto_ptr<RPReconstructedProtonCollection> output(new RPReconstructedProtonCollection(reconstructed_proton_collection));
  e.put(output);
}

//----------------------------------------------------------------------------------------------------

void RPPrimaryVertexInelasticReconstruction::AddInStationMultipleScatteringContribution(rec_tracks_collection &col,
    double rp_multiple_scattering_sigma)
{
  if (verbosity_)
  {
    rec_tracks_collection::iterator it = col.begin();
    std::cout<<"RPPrimaryVertexInelasticReconstruction::AddInStationMultipleScatteringContribution"<<std::endl;
    for(; it!=col.end(); ++it)
    {
      std::cout<<"rp:"<<it->first<<" sigma=("<<it->second.Sx()<<","<<it->second.Sy()<<")"<<std::endl;
      std::cout<<"\tpos=("<<it->second.X()<<","<<it->second.Y()<<","<<it->second.Z()<<")"<<std::endl;
    }
  }

  double sig2 = rp_multiple_scattering_sigma*rp_multiple_scattering_sigma;

  // traverse all hit pairs
  for (auto &h_dest : col)
  {
    //printf("RP %u: multiple scattering contributions from ", h_dest.first);

    double scat_var_contrib = 0.0;

    for (const auto &h_src : col)
    {
      // skip combinations of twice the same hit
      if (h_src.first == h_dest.first)
        continue;

      // calculate distance from src to dest, with Z increasing from the IP
      double dist = fabs(h_dest.second.Z()) - fabs(h_src.second.Z());

      // ensure causality: dist > 0
      if (dist < 0.)
        continue;

      //printf("%u, ", h_src.first);

      scat_var_contrib += sig2 * dist*dist;
    }

    double vx = h_dest.second.Vx() + scat_var_contrib;
    double vy = h_dest.second.Vy() + scat_var_contrib;

    //printf("\n    multiple_scat_position_sigma=%.3E, total sigma x=%.3E, y=%.3E\n", sqrt(scat_var_contrib), sqrt(vx), sqrt(vy));

    h_dest.second.Vx(vx);
    h_dest.second.Vy(vy);
  }
}

//----------------------------------------------------------------------------------------------------

bool RPPrimaryVertexInelasticReconstruction::FindPrimaryVertex(edm::Event& e)
{
  edm::Handle<edm::HepMCProduct> HepMCEvt;
  e.getByToken(HepMCProductToken, HepMCEvt);
  
  if (!HepMCEvt.isValid())
  {
    throw cms::Exception("RPPrimaryVertexInelasticReconstruction::FindPrimaryVertex") <<
      "Handle<edm::HepMCProduct> invalid." << endl;
  }
  
  const HepMC::GenEvent *evt = HepMCEvt->GetEvent();
  int vertex_number = 0;
  for (HepMC::GenEvent::vertex_const_iterator vitr = evt->vertices_begin(); vitr != evt->vertices_end(); ++vitr)
  {
    ++vertex_number;
    const HepMC::FourVector &xvtx = (*vitr)->position();
    primary_vertex_.SetX(xvtx.x() + rand_.Gaus(0, primary_vertex_error_.X()) );
    primary_vertex_.SetY(xvtx.y() + rand_.Gaus(0, primary_vertex_error_.Y()) );
    primary_vertex_.SetZ(xvtx.z() + rand_.Gaus(0, primary_vertex_error_.Z()) );

    if (verbosity_)
    {
      std::cout<<"Vertex found:[" << xvtx.x() << ", " << xvtx.y() << ", " << xvtx.z() << ", " << xvtx.t()
    		<< "]  smeared:("<<primary_vertex_.X()<<", " <<primary_vertex_.Y()
    		<<", "<<primary_vertex_.Z()<<")"<<std::endl;
    }
  }

  return (vertex_number >= 1);
}

//----------------------------------------------------------------------------------------------------

// return the number of pots through which the proton goes
int RPPrimaryVertexInelasticReconstruction::SelectHits(const RPFittedTrackCollection &tracks, 
    unsigned int armId_request, rec_tracks_collection &coll)
{
  // filter hits with selected arm id
  bool top = false;
  bool bottom = false;
  set<unsigned int> units;
  for (const auto &p : tracks)
  {
    //printf("%u, ", p.first);

    if (!p.second.IsValid())
      continue;

    const unsigned int &rpId = p.first;
    unsigned int armId = rpId / 100;
    if (armId != armId_request)
      continue;

    coll[rpId] = resol_degrad_service_.Create2DHit(rpId, p.second);

    // for quality checks
    unsigned int rpNum = rpId % 10;
    if (rpNum == 0 || rpNum == 4)
      top = true;
    if (rpNum == 1 || rpNum == 5)
      bottom = true;

    unsigned int unitId = (rpId / 10) * 10;
    if (rpNum > 2)
      unitId++;
    units.insert(unitId);
  }

  //printf("\n");

  // quality check
  bool collection_accepted = (units.size() >= 2) && ( !(top && bottom) );
  
  if (!collection_accepted)
    coll.clear();
  
  return coll.size();
}

//----------------------------------------------------------------------------------------------------

void RPPrimaryVertexInelasticReconstruction::Reconstruct(const rec_tracks_collection &rec_col,
  RPInverseParameterization &inv_par,
  RPReconstructedProtonCollection & rec_prot_col, double zdirection, bool external_prim_vertex)
{
  inv_par.ClearEvent();
  inv_par.AddProtonAtRPCollection(rec_col);

  if (external_prim_vertex)
    inv_par.SetPrimaryVertex(primary_vertex_, primary_vertex_error_);

  RPReconstructedProton rec_prot;
  rec_prot.Fitted(RPReconstructedProton::nx, true);
  rec_prot.Fitted(RPReconstructedProton::ntheta_x, true);
  rec_prot.Fitted(RPReconstructedProton::ny, true);
  rec_prot.Fitted(RPReconstructedProton::ntheta_y, true);
  rec_prot.Fitted(RPReconstructedProton::nksi, true);
  if(verbosity_)
    inv_par.PrintFittedHitsInfo(std::cout);

  rec_prot.ZDirection(zdirection);
  inv_par.Fit(rec_prot);

  //printf("rec_prot.Valid = %u, xi=%.4f, th_x=%.2E, th_y=%.2E\n", rec_prot.Valid(), rec_prot.Ksi(), rec_prot.Theta_x(), rec_prot.Theta_y());

  rec_prot_col.push_back(rec_prot);
}

DEFINE_FWK_MODULE(RPPrimaryVertexInelasticReconstruction);
