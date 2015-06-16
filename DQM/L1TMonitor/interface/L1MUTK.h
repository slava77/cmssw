#ifndef L1MUTK_H
#define L1MUTK_H

/*
 * \file L1MUTK.h
 *
 * $Date: 2015/06/16 12:00:00 $
 * $Revision: 0.03 $
 * \author S.Baranov
 *
*/

// system include files
#include <memory>
#include <unistd.h>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/Candidate/interface/LeafCandidate.h"

#include <iostream>
#include <fstream>
#include <vector>

using namespace reco;

//
// class decleration
//

class L1MUTK : public edm::EDAnalyzer {

public:

  // Constructor
  L1MUTK(const edm::ParameterSet& ps);
  
  // Destructor
  virtual ~L1MUTK();

  typedef edm::View<LeafCandidate>      CandView;
 
protected:
  // Analyze
  void analyze(const edm::Event& e, const edm::EventSetup& c);
  
  // BeginJob
  void beginJob(void);
  
  // BeginRun
  void beginRun(const edm::Run& r, const edm::EventSetup& c);
  
  // EndJob
  void endJob(void);
  
private:
  // ----------member data ---------------------------
  DQMStore * dbe;

  MonitorElement *gem_Pt,    *gem_Pt_barrel,    *gem_Pt_endcap;
  MonitorElement *gem_Eta,   *gem_Eta_barrel,   *gem_Eta_endcap;
  MonitorElement *gem_Phi,   *gem_Phi_barrel,   *gem_Phi_endcap;
  MonitorElement *gem_Dxy,   *gem_Dxy_barrel,   *gem_Dxy_endcap,;
  MonitorElement *gem_Z,     *gem_Z_barrel,     *gem_Z_endcap;
  MonitorElement *gem_Q,     *gem_Q_barrel,     *gem_Q_endcap;
  MonitorElement *gem_Pdgid, *gem_Pdgid_barrel, *gem_Pdgid_endcap;

  int nev_; // Number of events processed
  std::string outputFile_; //file name for ROOT ouput
  bool verbose_;
  bool monitorDaemon_;
  ofstream logFile_;
  edm::InputTag gmtSource_ ;

  static const double piconv_;
  double phiconv_(float phi);
  void book_(const edm::EventSetup& c);

  edm::InputTag candInputTag_;

};
#endif
