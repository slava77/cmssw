/****************************************************************************
 *
 * This is a part of TOTEM offline software.
 * Authors:
 *  Hubert Niewiadomski
 *  Jan Kašpar (jan.kaspar@gmail.com)
 *
 ****************************************************************************/

#include "TotemAnalysis/TotemNtuplizer/interface/RPNtuplizer.h"

#include "RecoTotemRP/RPRecoDataFormats/interface/RPFittedTrackCollection.h"
#include "DataFormats/TotemRPDataTypes/interface/RPTypes.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPFittedTrack.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RP2DHit.h"
#include "DataFormats/TotemRPDataTypes/interface/RPDigCluster.h"
#include "DataFormats/TotemRPDetId/interface/TotRPDetId.h"
#include "DataFormats/TotemRPDataTypes/interface/RPDetTrigger.h"
#include "DataFormats/TotemRPDetId/interface/TotRPDetId.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPReconstructedProton.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPFittedTrackCollection.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPReconstructedProtonPairCollection.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPReconstructedProtonCollection.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPMulFittedTrackCollection.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPRecognizedPatternsCollection.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPTrackCandidateCollection.h"

#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSet.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/CentralMassInfo.h"

#include <iostream>
#include <string>

#include "TTree.h"

#include <map>

ClassImp(RPRootDumpTrackInfo)
ClassImp(RPRootDumpDigiInfo)
ClassImp(RPRootDumpReconstructedProton)
ClassImp(RPRootDumpReconstructedProtonPair)

RPNtuplizer::RPNtuplizer(const edm::ParameterSet& conf) :
  Ntuplizer(conf), Verbosity_(conf.getUntrackedParameter<unsigned int> ("verbosity", 0))
{
  modulLabelSimu_ = conf.getParameter<std::string> ("ModulLabelSimu");
  productLabelSimu_ = conf.getParameter<std::string> ("ProductLabelSimu");
  primaryProtons = false;
  rpFittedTrackCollectionLabel = conf.getParameter<edm::InputTag>("RPFittedTrackCollectionLabel");
  rpMulFittedTrackCollectionLabel = conf.getParameter<edm::InputTag>("RPMulFittedTrackCollectionLabel");
  rpStripDigiSetLabel = conf.getParameter<edm::InputTag>("RPStripDigiSetLabel");
  rpDigClusterLabel = conf.getParameter<edm::InputTag>("RPDigClusterLabel");
  rpReconstructedProtonCollectionLabel = conf.getParameter<edm::InputTag>("RPReconstructedProtonCollectionLabel");
  rpReconstructedProtonPairCollectionLabel = conf.getParameter<edm::InputTag>("RPReconstructedProtonPairCollectionLabel");

  if (conf.exists("primaryProtons")) //check if "primaryProtons" variable is defined in configuration file
  {
  	primaryProtons = conf.getParameter<bool> ("primaryProtons");
  }
  
  primaryJets_ = false;
  if (conf.exists("primaryJets")) //check if "primaryProtons" variable is defined in configuration file 
  { 
    primaryJets_ = conf.getParameter<bool> ("primaryJets");
    primaryJetsInstance_ = conf.getParameter<std::string> ("primaryJetsInstance");
    primaryJetsLabel_ = conf.getParameter<std::string> ("primaryJetsLabel");
  }

  includeDigi = false;
  if (conf.exists("includeDigi"))
  {
  	includeDigi = conf.getParameter<bool> ("includeDigi");
  }
  
  includePatterns = false;
  if (conf.exists("includePatterns"))
  {
  	includePatterns = conf.getParameter<bool> ("includePatterns");
  }
}


void RPNtuplizer::CreateBranches(const edm::EventSetup &es, TTree *out_tree_)
{
  for (unsigned int a = 0; a < 2; ++a)
  	for (unsigned int s = 0; s < 3; ++s) 
    {
  	  if (s == 1)
  			continue;
  	  for (unsigned int r = 0; r < 6; r++)
      {
  		unsigned int id = 100 * a + 10 * s + r;
        char br_name[500];

        if (includeDigi)
        {
  		  sprintf(br_name, "digi_rp_%u.", id);
  		  out_tree_->Branch(br_name, &digi_info_[id]);
        }

        if (includePatterns)
        {
  		  sprintf(br_name, "par_patterns_rp_%u.", id); out_tree_->Branch(br_name, &par_patterns_info_[id]);
  		  sprintf(br_name, "nonpar_patterns_rp_%u.", id); out_tree_->Branch(br_name, &nonpar_patterns_info_[id]);
        }
  			
        sprintf(br_name, "track_rp_%u.", id);
  	out_tree_->Branch(br_name, &track_info_[id]);

        sprintf(br_name, "multi_track_rp_%u", id);
        out_tree_->Branch(br_name, &multi_track_info_[id]);
  	}
  }

  // TODO: uncomment
  //out_tree_->Branch("rec_prot_left.", &rec_pr_info_[0]);
  //out_tree_->Branch("rec_prot_right.", &rec_pr_info_[1]);
  //out_tree_->Branch("rec_prot_pair.", &rec_pr_pair_info_);

  if (primaryProtons)
  {
  	out_tree_->Branch("sim_prot_left.", &sim_pr_info_[0]);
  	out_tree_->Branch("sim_prot_right.", &sim_pr_info_[1]);
  }
  
  if(primaryJets_)
  {
    out_tree_->Branch("MCjets.", &MCjets_);
    out_tree_->Branch("DiffMassInfo.", &diff_mass_info_);
  }
}



void RPNtuplizer::FillEvent(const edm::Event& e, const edm::EventSetup& es)
{
#ifdef DEBUG
  printf(">> RPNtuplizer::FillEvent(%u:%u)\n", e.id().run(), e.id().event());
#endif

  // initialize objects with default values
  for (unsigned int a = 0; a < 2; ++a)
  {
  	for (unsigned int s = 0; s < 3; ++s)
    {
  		if (s == 1)
  			continue;

  		for (unsigned int r = 0; r < 6; r++)
        {
  			unsigned int id = 100 * a + 10 * s + r;
  			track_info_[id] = RPRootDumpTrackInfo();
  
            if (includeDigi)
    		  digi_info_[id] = RPRootDumpDigiInfo();

            if (includePatterns)
            {
  			  par_patterns_info_[id].Reset();
  			  nonpar_patterns_info_[id].Reset();
            }

  			multi_track_info_[id].clear();
  		}
  	}
  }

  rec_pr_info_[0] = RPRootDumpReconstructedProton();
  rec_pr_info_[1] = RPRootDumpReconstructedProton();
  rec_pr_pair_info_ = RPRootDumpReconstructedProtonPair();
  
  if (primaryProtons)
  {
  	sim_pr_info_[0] = RPRootDumpReconstructedProton();
  	sim_pr_info_[1] = RPRootDumpReconstructedProton();
  }

  // single-RP track fits
  edm::Handle < RPFittedTrackCollection > fitted_tracks;
  e.getByLabel(rpFittedTrackCollectionLabel, fitted_tracks);
  if (fitted_tracks.isValid())
  {
  	for (RPFittedTrackCollection::const_iterator it = fitted_tracks->begin(); it != fitted_tracks->end(); ++it)
    {
  	  track_info_[it->first].valid = it->second.IsValid();
  	  track_info_[it->first].chi2 = it->second.ChiSquared();
  	  track_info_[it->first].chi2ndf = it->second.ChiSquaredOverN();
  	  track_info_[it->first].x = it->second.X0();
  	  track_info_[it->first].y = it->second.Y0();
  	  track_info_[it->first].z = it->second.Z0();
          track_info_[it->first].thx = it->second.GetTx();
          track_info_[it->first].thy = it->second.GetTy();
  	  track_info_[it->first].entries = it->second.GetHitEntries();
          track_info_[it->first].u_id = it->second.GetUid();
          track_info_[it->first].v_id = it->second.GetVid();
  	}
  }

  // save the multi-track fits, if present
  try {
  	edm::Handle < RPMulFittedTrackCollection > multi_fitted_tracks;
  	e.getByLabel(rpMulFittedTrackCollectionLabel, multi_fitted_tracks);

  	if (multi_fitted_tracks.isValid())
    {
  	  for (RPMulFittedTrackCollection::const_iterator rit = multi_fitted_tracks->begin(); rit
  		        != multi_fitted_tracks->end(); ++rit)
      {
  	    std::vector < RPRootDumpTrackInfo > &tiv = multi_track_info_[rit->first];
  		for (std::vector<RPFittedTrack>::const_iterator it = rit->second.begin(); it
  			        != rit->second.end(); ++it)
        {
  	      RPRootDumpTrackInfo ti;
  		  ti.valid = it->IsValid();
  		  ti.chi2 = it->ChiSquared();
  		  ti.chi2ndf = it->ChiSquaredOverN();
  		  ti.x = it->X0();
  		  ti.y = it->Y0();
  		  ti.z = it->Z0();
                  ti.thx = it->GetTx();
                  ti.thy = it->GetTy();
  		  ti.entries = it->GetHitEntries();
                  ti.u_id = it->GetUid();
                  ti.v_id = it->GetVid();
  		  tiv.push_back(ti);
  		}
  	  }
  	}
  } catch (const std::exception& e) { std::cout << "Exception: " << e.what() << std::endl;  }

  if (includeDigi)
  {
  	edm::Handle < edm::DetSetVector<RPDigCluster> > clusters;
  	e.getByLabel(rpDigClusterLabel, clusters);
  
  	edm::DetSetVector<RPDigCluster>::const_iterator inputIteratorCl = clusters->begin();
  	for (; inputIteratorCl != clusters->end(); inputIteratorCl++)
    {
  	  TotRPDetId detectorId(inputIteratorCl->id);
  	  
      //inputIterator->data : vector< RPDigCluster>
      //(inputIterator->data)[i] : RPDigCluster
      for (unsigned int i = 0; i < (inputIteratorCl->data).size(); ++i)
      {
        unsigned int detNo = TotRPDetId::RawToDecId((inputIteratorCl->data)[i].DetId());
  	  	unsigned int planeNo = detNo % 10;
  	  	unsigned int RPNo = detNo / 10;

  	  	double centralStrip = (inputIteratorCl->data)[i].CentreStripPos();
  	  	int clusterSize = (inputIteratorCl->data)[i].GetNumberOfStrips();
  
  	  	// if up to now no clusters in this plane -> increase no of planes counter
  	  	if (digi_info_[RPNo].numberOfClusters[planeNo] == 0)
        {
  	  		digi_info_[RPNo].numberOfPlanesOn++;
            if (TotRPDetId::IsStripsCoordinateUDirection(planeNo))
  	  		  digi_info_[RPNo].uPlanesOn++;
            else
  	  		  digi_info_[RPNo].vPlanesOn++;
        }
  
  	  	digi_info_[RPNo].numberOfClusters[planeNo]++;
  
  	  	digi_info_[RPNo].planeId.push_back(planeNo);
  	  	digi_info_[RPNo].clusterSize.push_back(clusterSize);
  	  	digi_info_[RPNo].centralStrip.push_back((int)centralStrip);
      }
  	}
  }

  // fill in pattern-recognition results (parallel)
  try {
    edm::Handle< RPRecognizedPatternsCollection > patterns;
    e.getByLabel("RPSinglTrackCandFind", "", patterns);

    edm::Handle< RPTrackCandidateCollection > trCand;
    e.getByLabel("RPSinglTrackCandFind", "", trCand);

    for (RPRecognizedPatternsCollection::const_iterator rpit = patterns->begin(); rpit != patterns->end(); ++rpit) {
      unsigned int rp = rpit->first;

      for (std::vector<RPRecognizedPatterns::Line>::const_iterator lit = rpit->second.uLines.begin(); lit != rpit->second.uLines.end(); ++lit)
      {
        par_patterns_info_[rp].u.push_back(RPRootDumpPattern(lit->a, lit->b, lit->w));
        par_patterns_info_[rp].u_no = par_patterns_info_[rp].u.size(); 
      }
      
      for (std::vector<RPRecognizedPatterns::Line>::const_iterator lit = rpit->second.vLines.begin(); lit != rpit->second.vLines.end(); ++lit)
      {
        par_patterns_info_[rp].v.push_back(RPRootDumpPattern(lit->a, lit->b, lit->w));
        par_patterns_info_[rp].v_no = par_patterns_info_[rp].v.size();
      }

      RPTrackCandidateCollection::const_iterator sr = trCand->find(rp);
      par_patterns_info_[rp].fittable = (sr != trCand->end()) ? sr->second.Fittable() : false;
    }
  } catch (const std::exception& e) { std::cout << "Exception: " << e.what() << std::endl; }

  // fill in pattern-recognition results (non-parallel)
  try {
    edm::Handle< RPRecognizedPatternsCollection > patterns;
    e.getByLabel("NonParallelTrackFinder", patterns);

    edm::Handle< RPTrackCandidateCollection > trCand;
    e.getByLabel("NonParallelTrackFinder", trCand);

    for (RPRecognizedPatternsCollection::const_iterator rpit = patterns->begin(); rpit != patterns->end(); ++rpit)
    {
      unsigned int rp = rpit->first;

      for (std::vector<RPRecognizedPatterns::Line>::const_iterator lit = rpit->second.uLines.begin(); lit != rpit->second.uLines.end(); ++lit)
      {
        nonpar_patterns_info_[rp].u.push_back(RPRootDumpPattern(lit->a, lit->b, lit->w));
        nonpar_patterns_info_[rp].u_no = nonpar_patterns_info_[rp].u.size(); 
      }
      
      for (std::vector<RPRecognizedPatterns::Line>::const_iterator lit = rpit->second.vLines.begin(); lit != rpit->second.vLines.end(); ++lit)
      {
        nonpar_patterns_info_[rp].v.push_back(RPRootDumpPattern(lit->a, lit->b, lit->w));
        nonpar_patterns_info_[rp].v_no = nonpar_patterns_info_[rp].v.size();
      }

      RPTrackCandidateCollection::const_iterator sr = trCand->find(rp);
      nonpar_patterns_info_[rp].fittable = (sr != trCand->end()) ? sr->second.Fittable() : false;
    }
  } catch (...) {}

  // fill coincidence chip data
  edm::Handle < std::vector<RPCCBits> > cc_chip_bits;
  e.getByLabel(modulLabelSimu_, productLabelSimu_, cc_chip_bits);
  if (cc_chip_bits.isValid())
  {
  	std::vector<RPCCBits>::const_iterator itc;
  	for (itc = cc_chip_bits->begin(); itc != cc_chip_bits->end(); ++itc)
    {
  	  RPCCId rawid(itc->getId());
  	  RPId decid = rawid.Arm() * 100 + rawid.Station() * 10 + rawid.RomanPot();

  	  const bool *bits = itc->getRawBS();

      if (rawid.IsStripsCoordinateUDirection())
      {
  		for (int i = 0; i < 16; ++i)
        {
  		  if (bits[i])
  		    track_info_[decid].u_sect.push_back(i);
  		}
  		track_info_[decid].u_sect_no = track_info_[decid].u_sect.size();
  	  } else {
  		for (int i = 0; i < 16; ++i)
        {
  		  if (bits[i])
  		    track_info_[decid].v_sect.push_back(i);
  		}
  		track_info_[decid].v_sect_no = track_info_[decid].v_sect.size();
  	  }
  	}
  }

  // fill in primary (simulated) proton data
  if (primaryProtons && FindSimulatedProtons(e) && FindSimulatedProtonsVertex(e))
  {
  	if (left_prim_prot_.found)
  	  FindParametersOfSimulatedProtons(left_prim_prot_, 0);

  	if (right_prim_prot_.found)
  	  FindParametersOfSimulatedProtons(right_prim_prot_, 1);
  }

  // fill in reconstructed proton data 
  if (FindReconstrucedProtons(e))
  {
  	if (left_rec_prot_fund_)
    {
      rec_pr_info_[0].valid = reconstructed_protons_[0]->Valid();
      RPRecoProtMADXVariables mad_var = reconstructed_protons_[0]->GetMADXVariables();
  
      rec_pr_info_[0].thx = reconstructed_protons_[0]->Theta_x_angle();
      rec_pr_info_[0].thy = reconstructed_protons_[0]->Theta_y_angle();
      double reconstructed_phi = BOPar_.MADXCanonicalVariablesToCrossingAngleCorrectedPhi(mad_var);
      rec_pr_info_[0].phi = reconstructed_phi;
      double sin_phi = TMath::Sin(reconstructed_phi);
      double cos_phi = TMath::Cos(reconstructed_phi);
      double reconstructed_t = BOPar_.MADXCanonicalVariablesTot(mad_var);
  
      rec_pr_info_[0].t = reconstructed_t;
      rec_pr_info_[0].tx = reconstructed_t * cos_phi * cos_phi;
      rec_pr_info_[0].ty = reconstructed_t * sin_phi * sin_phi;
      rec_pr_info_[0].xi = reconstructed_protons_[0]->Ksi();
      rec_pr_info_[0].x0 = reconstructed_protons_[0]->X();
      rec_pr_info_[0].y0 = reconstructed_protons_[0]->Y();
      rec_pr_info_[0].chi2 = reconstructed_protons_[0]->Chi2();
      rec_pr_info_[0].chindf = reconstructed_protons_[0]->Chi2Norm();
  	}

  	if (right_rec_prot_fund_)
    {
      rec_pr_info_[1].valid = reconstructed_protons_[1]->Valid();
      RPRecoProtMADXVariables mad_var = reconstructed_protons_[1]->GetMADXVariables();

      rec_pr_info_[1].thx = reconstructed_protons_[1]->Theta_x_angle();
      rec_pr_info_[1].thy = reconstructed_protons_[1]->Theta_y_angle();
      double reconstructed_phi = BOPar_.MADXCanonicalVariablesToCrossingAngleCorrectedPhi(mad_var);
      rec_pr_info_[1].phi = reconstructed_phi;
      double sin_phi = TMath::Sin(reconstructed_phi);
      double cos_phi = TMath::Cos(reconstructed_phi);
      double reconstructed_t = BOPar_.MADXCanonicalVariablesTot(mad_var);
      //      std::cout<<"reconstructed_t="<<reconstructed_t<<"  rec_pr_info_[1].thx"<<rec_pr_info_[1].thx<<std::endl;

      rec_pr_info_[1].t = reconstructed_t;
      rec_pr_info_[1].tx = reconstructed_t * cos_phi * cos_phi;
      rec_pr_info_[1].ty = reconstructed_t * sin_phi * sin_phi;
      rec_pr_info_[1].xi = reconstructed_protons_[1]->Ksi();
      rec_pr_info_[1].x0 = reconstructed_protons_[1]->X();
      rec_pr_info_[1].y0 = reconstructed_protons_[1]->Y();
      rec_pr_info_[1].chi2 = reconstructed_protons_[1]->Chi2();
      rec_pr_info_[1].chindf = reconstructed_protons_[1]->Chi2Norm();
  	}
  }

  if (FindReconstrucedProtonPair(e))
  {
  	rec_pr_pair_info_.valid = reconstructed_proton_pair_.Valid();
  	rec_pr_pair_info_.x0 = reconstructed_proton_pair_.X3D();
  	rec_pr_pair_info_.y0 = reconstructed_proton_pair_.Y3D();
  	rec_pr_pair_info_.z0 = reconstructed_proton_pair_.Z3D();
  	rec_pr_pair_info_.thxl = reconstructed_proton_pair_.ThetaXAngleLeft();
  	rec_pr_pair_info_.thyl = reconstructed_proton_pair_.ThetaYAngleLeft();
  	rec_pr_pair_info_.thxr = reconstructed_proton_pair_.ThetaXAngleRight();
  	rec_pr_pair_info_.thyr = reconstructed_proton_pair_.ThetaYAngleRight();
  	rec_pr_pair_info_.xil = reconstructed_proton_pair_.KsiLeft();
  	rec_pr_pair_info_.xir = reconstructed_proton_pair_.KsiRight();
  	rec_pr_pair_info_.chi2 = reconstructed_proton_pair_.Chi2();
  	rec_pr_pair_info_.chindf = reconstructed_proton_pair_.Chi2Norm();

  	RPRecoProtMADXVariables mad_var_right = reconstructed_proton_pair_.GetMADXVariablesRight();
  	RPRecoProtMADXVariables mad_var_left = reconstructed_proton_pair_.GetMADXVariablesLeft();

  	double phi_reconst_right = BOPar_.MADXCanonicalVariablesToCrossingAngleCorrectedPhi(mad_var_right);
  	double phi_reconst_left = BOPar_.MADXCanonicalVariablesToCrossingAngleCorrectedPhi(mad_var_left);
  	double cos_rec_phir = TMath::Cos(phi_reconst_right);
  	double sin_rec_phir = TMath::Sin(phi_reconst_right);
  	double cos_rec_phil = TMath::Cos(phi_reconst_left);
  	double sin_rec_phil = TMath::Sin(phi_reconst_left);

  	rec_pr_pair_info_.phir = phi_reconst_right;
  	rec_pr_pair_info_.phil = phi_reconst_left;

  	double t_reconst_right = BOPar_.MADXCanonicalVariablesTot(mad_var_right);
  	double t_reconst_left = BOPar_.MADXCanonicalVariablesTot(mad_var_left);

  	rec_pr_pair_info_.tr = t_reconst_right;
  	rec_pr_pair_info_.tl = t_reconst_left;

  	rec_pr_pair_info_.txr = t_reconst_right * cos_rec_phir * cos_rec_phir;
  	rec_pr_pair_info_.tyr = t_reconst_right * sin_rec_phir * sin_rec_phir;
  	rec_pr_pair_info_.txl = t_reconst_left * cos_rec_phil * cos_rec_phil;
  	rec_pr_pair_info_.tyl = t_reconst_left * sin_rec_phil * sin_rec_phil;

  	rec_pr_pair_info_.t = BOPar_.MADXCanonicalVariablesToElasticPhysics_t(mad_var_right, mad_var_left);
  }
  
  // fill in other primary (simulated) object data
  if(primaryJets_)
  {
    edm::Handle < std::vector<fastjet::PseudoJet> > MCjets_handle;
    e.getByLabel(primaryJetsInstance_, primaryJetsLabel_, MCjets_handle);
    if(MCjets_handle.isValid())
    {
      if (Verbosity_)
        std::cout << "MC jets found" << std::endl;
      MCjets_.AddJets( (*MCjets_handle) );
    }
    else
    {
      if (Verbosity_)
        std::cout << "MC jets not found" << std::endl;
    }
    
    edm::Handle < CentralMassInfo > central_mass_info_handle;
    e.getByLabel(primaryJetsInstance_, primaryJetsLabel_, central_mass_info_handle);
    if(central_mass_info_handle.isValid())
    {
      if (Verbosity_)
        std::cout << "Central mass info found" << std::endl;
      diff_mass_info_.SetVariables( (*central_mass_info_handle) );
    }
    else
    {
      if (Verbosity_)
        std::cout << "Central mass info not found" << std::endl;
    }
  }
}


/**
 * This function computes the parameters of simulated proton that will be saved in root file.
 * proton - can be left_prim_prot_ or right_prim_prot_
 * simulatedProtonNumber - left_prim_prot_ - 0, right_prim_prot_ - 1
 */
void RPNtuplizer::FindParametersOfSimulatedProtons(PrimaryProton proton, int simulatedProtonNumber)
{
  double phi = BOPar_.ComputeNonSmearedProtonPhi(proton.momentum);
  double sin_phi = TMath::Sin(phi);
  double cos_phi = TMath::Cos(phi);
  sim_pr_info_[simulatedProtonNumber].phi = phi;
  sim_pr_info_[simulatedProtonNumber].x0 = proton.vertex.x();
  sim_pr_info_[simulatedProtonNumber].y0 = proton.vertex.y();
  sim_pr_info_[simulatedProtonNumber].thx = BOPar_.IPNonSmearedProtonMomentumToThetaX(proton.momentum);
  sim_pr_info_[simulatedProtonNumber].thy = BOPar_.IPNonSmearedProtonMomentumToThetaY(proton.momentum);
  sim_pr_info_[simulatedProtonNumber].xi = BOPar_.IPNonSmearedProtonMomentumToXi(proton.momentum.px(),
          proton.momentum.py(), proton.momentum.pz());
  sim_pr_info_[simulatedProtonNumber].t = BOPar_.IPNonSmearedProtonMomentumTot(proton.momentum.px(),
          proton.momentum.py(), proton.momentum.pz());
  sim_pr_info_[simulatedProtonNumber].tx = sim_pr_info_[simulatedProtonNumber].t * cos_phi * cos_phi;
  sim_pr_info_[simulatedProtonNumber].ty = sim_pr_info_[simulatedProtonNumber].t * sin_phi * sin_phi;
  sim_pr_info_[simulatedProtonNumber].valid = true;
}


bool RPNtuplizer::FindReconstrucedProtons(const edm::Event& e)
{
  reconstructed_protons_.clear();

  if (Verbosity_)
  	std::cout << "Finding the reconstructed protons" << std::endl;
  edm::Handle < RPReconstructedProtonCollection > input;
  e.getByLabel(rpReconstructedProtonCollectionLabel, input);

  right_rec_prot_fund_ = 0;
  left_rec_prot_fund_ = 0;
  if (input.isValid()) {
  	for (RPReconstructedProtonCollection::const_iterator it = input->begin(); it != input->end(); ++it) {
  		if (Verbosity_)
  			std::cout << "Reconstructed proton ksi" << it->Ksi() << "zdirection:" << it->ZDirection()
  			        << std::endl;
  		if (it->ZDirection() > 0) {
  			++right_rec_prot_fund_;
  			reconstructed_protons_[1] = &(*it);
  			if (Verbosity_)
  				std::cout << "Setting the reconstructed proton right" << std::endl;
  		}
  		if (it->ZDirection() < 0) {
  			++left_rec_prot_fund_;
  			reconstructed_protons_[0] = &(*it);
  			if (Verbosity_)
  				std::cout << "Setting the reconstructed proton left" << std::endl;
  		}
  	}
  }
  bool result = right_rec_prot_fund_ <= 1 && left_rec_prot_fund_ <= 1 && (right_rec_prot_fund_ > 0
          || left_rec_prot_fund_ > 0);
  return result;
}

/**
 * The purpose of this function is to read most forward primary protons for given event
 */
bool RPNtuplizer::FindSimulatedProtons(const edm::Event& e)
{
  edm::Handle < edm::HepMCProduct > input;
  e.getByLabel("SmearingGenerator", "original", input);

  if (input.isValid())
  {
  	const HepMC::GenEvent *evt = input->GetEvent();
  	right_prim_prot_.found = false;
  	left_prim_prot_.found = false;

  	for (HepMC::GenEvent::particle_const_iterator it = evt->particles_begin(); it != evt->particles_end(); ++it)
    {
      HepMC::GenParticle * g = (*it);
      int g_status = g->status();
      int pdg_id = g->pdg_id();

      // scanning only for particles with status == 1
      if (g_status == 1 && pdg_id == 2212)
      {
        const HepMC::FourVector &vtx = g->production_vertex()->position();
        const HepMC::FourVector &mom = g->momentum();

        if (mom.z() > 0)
        {
          if (!right_prim_prot_.found || ( fabs(mom.z()) > fabs(right_prim_prot_.momentum.z()) ) )
          {
            right_prim_prot_.vertex = vtx; //[mm]
            right_prim_prot_.momentum = mom; //[GeV]
            right_prim_prot_.found = true;
          }
        }

        if (mom.z() < 0)
        {
          if (!left_prim_prot_.found || (fabs(mom.z()) > fabs(left_prim_prot_.momentum.z())) )
          {
            left_prim_prot_.vertex = vtx; //[mm]
            left_prim_prot_.momentum = mom; //[GeV]
            left_prim_prot_.found = true;
          }
        }
      }
  	}

  	return true;
  }

  return false;
}

/**
 * The purpose of this function is to read most forward primary protons' vertices for given event
 */
bool RPNtuplizer::FindSimulatedProtonsVertex(const edm::Event& e) {
  edm::Handle < edm::HepMCProduct > input;
  e.getByLabel("generator", input);
  
  bool left_found = false;
  bool right_found = false;
  HepMC::FourVector mom_left;
  HepMC::FourVector mom_right;
  
  if (input.isValid()) {
    const HepMC::GenEvent *evt = input->GetEvent();

    for (HepMC::GenEvent::particle_const_iterator it = evt->particles_begin(); it != evt->particles_end(); ++it) {
      HepMC::GenParticle * g = (*it);
      int g_status = g->status();
      int pdg_id = g->pdg_id();

      // scanning only for particles with status == 1
      if (g_status == 1 && pdg_id == 2212) {
        const HepMC::FourVector &vtx = g->production_vertex()->position();
        const HepMC::FourVector &mom = g->momentum();

        if (mom.z() > 0) {
          if (!right_found || (fabs(mom.z()) > fabs(mom_right.z())) ) {
            right_prim_prot_.vertex = vtx; //[mm]
            mom_right = mom; //[GeV]
            right_found = true;
          }
        }
        if (mom.z() < 0) {
          if (!left_found || (fabs(mom.z()) > fabs(mom_left.z())) ) {
            left_prim_prot_.vertex = vtx; //[mm]
            mom_left = mom; //[GeV]
            left_found = true;
          }
        }
      }
    }
    
//    if(!result)
//      std::cout<<"FindSimulatedProtonsVertex:: Vertex not found, 1"<<std::endl;
//    else
//      std::cout<<"FindSimulatedProtonsVertex:: Vertex found"<<std::endl; 

    return true;
  }
  
//  std::cout<<"FindSimulatedProtonsVertex:: Vertex not found, 2"<<std::endl;
  return false;
}


bool RPNtuplizer::FindReconstrucedProtonPair(const edm::Event& e) {
  if (Verbosity_)
  	std::cout << "Finding the reconstructed protons" << std::endl;
  edm::Handle < RPReconstructedProtonPairCollection > input;
  e.getByLabel(rpReconstructedProtonPairCollectionLabel, input);

  if (!input.isValid() || input->size() != 1) {
  	if (Verbosity_)
  		std::cout << "reconstructed protons not found" << std::endl;
  	return false;
  } else {
  	if (Verbosity_)
  		std::cout << "reconstructed protons found" << std::endl;
  	reconstructed_proton_pair_ = (*input)[0];
  	return true;
  }
}
