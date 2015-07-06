#ifndef DQM_L1TMonitor_L1TkMuonMonitor_h
#define DQM_L1TMonitor_L1TkMuonMonitor_h

/*
 * \file L1TkMuonMonitor.h
 *
 * Date: 2015/06/16 12:00:00
 * Revision: 0.03
 * \author S.Baranov
 *
*/

// system include files
#include <memory>

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

//
// class decleration
//

class L1TkMuonMonitor : public edm::EDAnalyzer {

public:

  // Constructor
  L1TkMuonMonitor(const edm::ParameterSet& ps);
  
  // Destructor
  virtual ~L1TkMuonMonitor();

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

  MonitorElement *l1tkmu_Pt,    *l1tkmu_Pt_barrel,    *l1tkmu_Pt_endcap;
  MonitorElement *l1tkmu_Eta,   *l1tkmu_Eta_barrel,   *l1tkmu_Eta_endcap;
  MonitorElement *l1tkmu_Phi,   *l1tkmu_Phi_barrel,   *l1tkmu_Phi_endcap;
  MonitorElement *l1tkmu_Dxy,   *l1tkmu_Dxy_barrel,   *l1tkmu_Dxy_endcap,;
  MonitorElement *l1tkmu_Z,     *l1tkmu_Z_barrel,     *l1tkmu_Z_endcap;
  MonitorElement *l1tkmu_Q,     *l1tkmu_Q_barrel,     *l1tkmu_Q_endcap;
  MonitorElement *l1tkmu_Pdgid, *l1tkmu_Pdgid_barrel, *l1tkmu_Pdgid_endcap;

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
