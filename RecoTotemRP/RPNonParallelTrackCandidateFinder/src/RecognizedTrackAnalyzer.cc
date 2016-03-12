#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/TotemRPDetId/interface/TotRPDetId.h"
#include "DataFormats/TotemRPDataTypes/interface/RPRecoHit.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSet.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPTrackCandidateCollection.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPTrackCandidate.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/TotemRPGeometryBuilder/interface/TotemRPGeometry.h"
#include "Geometry/TotemRecords/interface/RealGeometryRecord.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "TH1D.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TFile.h"

using namespace std;
using namespace edm;

// ====================================================================================================

///\brief To visualize results of track recognition (mainly to debug class RPNonParallelTrackCandidateFinder)
class RecognizedTrackAnalyzer : public edm::EDAnalyzer
{
	public:
		RecognizedTrackAnalyzer(const edm::ParameterSet&);
		~RecognizedTrackAnalyzer();

	private:
		edm::InputTag RPTrackCandidateCollectionLabel;
		edm::InputTag detSetVectorRPRecoHitLabel;
		edm::EDGetTokenT<RPTrackCandidateCollection> RPTrackCandidateCollectionToken;
		edm::EDGetTokenT<edm::DetSetVector<RPRecoHit>> detSetVectorRPRecoHitToken;
		unsigned char verbosity;
		std::string outputFile;
		TFile *of;

		TCanvas *can;
		TGraph *allHits_u, *allHits_v, *selHits_u, *selHits_v;

		virtual void beginJob();
		virtual void analyze(const edm::Event&, const edm::EventSetup&);
		virtual void endJob();
};

// ====================================================================================================

RecognizedTrackAnalyzer::RecognizedTrackAnalyzer(const edm::ParameterSet& conf) : of(NULL)
{
	RPTrackCandidateCollectionLabel = conf.getParameter<edm::InputTag>("RPTrackCandidateCollectionLabel");
	detSetVectorRPRecoHitLabel = conf.getParameter<edm::InputTag>("DetSetVectorRPRecoHitLabel");
	verbosity = conf.getUntrackedParameter<unsigned int>("verbosity", 0);
	outputFile = conf.getParameter<std::string>("outputFile");
}

//----------------------------------------------------------------------------------------------------

RecognizedTrackAnalyzer::~RecognizedTrackAnalyzer()
{
}

//----------------------------------------------------------------------------------------------------

void RecognizedTrackAnalyzer::beginJob()
{
	of = new TFile(outputFile.c_str(), "recreate");

	allHits_u = new TGraph(); allHits_u->SetMarkerStyle(20); allHits_u->SetMarkerColor(1); allHits_u->SetMarkerSize(0.9);
	allHits_v = new TGraph(); allHits_v->SetMarkerStyle(20); allHits_v->SetMarkerColor(1); allHits_v->SetMarkerSize(0.9);
	selHits_u = new TGraph(); selHits_u->SetMarkerStyle(20); selHits_u->SetMarkerColor(2); selHits_u->SetMarkerSize(0.7);
	selHits_v = new TGraph(); selHits_v->SetMarkerStyle(20); selHits_v->SetMarkerColor(2); selHits_v->SetMarkerSize(0.7);
	can = new TCanvas();
}

//----------------------------------------------------------------------------------------------------

void RecognizedTrackAnalyzer::endJob()
{
	delete of;
	delete allHits_u;
	delete allHits_v;
	delete selHits_u;
	delete selHits_v;
	delete can;
}

//----------------------------------------------------------------------------------------------------

void RecognizedTrackAnalyzer::analyze(const edm::Event& event, const edm::EventSetup& eSetup)
{
	ESHandle<TotemRPGeometry> geometry;
	eSetup.get<RealGeometryRecord>().get(geometry);

	Handle< edm::DetSetVector<RPRecoHit> > allHits;
	event.getByToken(detSetVectorRPRecoHitToken, allHits);
	Handle<RPTrackCandidateCollection> selHits;
	event.getByToken(RPTrackCandidateCollectionToken, selHits);

	if (verbosity) printf("\nEVENT %llu\n", event.id().event());

	// process all hits collection
	map< unsigned int, pair< vector<const RPRecoHit *>, vector<const RPRecoHit *> > > allHitsMap;
	for (DetSetVector<RPRecoHit>::const_iterator dit = allHits->begin(); dit != allHits->end(); ++dit) {
		unsigned int detId = TotRPDetId::RawToDecId(dit->detId());
		unsigned int RPId = TotRPDetId::RPOfDet(detId);
		bool uDir = TotRPDetId::IsStripsCoordinateUDirection(detId);

		for (DetSet<RPRecoHit>::const_iterator hit = dit->begin(); hit != dit->end(); ++hit) {
			if (uDir) allHitsMap[RPId].first.push_back(& (*hit));
			else allHitsMap[RPId].second.push_back(& (*hit));
		}
	}

	// process selected hits collection
	map< unsigned int, pair< vector<const RPRecoHit *>, vector<const RPRecoHit *> > > selHitsMap;
	for (RPTrackCandidateCollection::const_iterator dit = selHits->begin(); dit != selHits->end(); ++dit) {
		if (!dit->second.Fittable())
			continue;

		unsigned int RPId = dit->first;
		const vector<RPRecoHit> &rhs = dit->second.TrackRecoHits();
		for (vector<RPRecoHit>::const_iterator hit = rhs.begin(); hit != rhs.end(); ++hit) {
			unsigned int detId = TotRPDetId::RawToDecId(hit->DetId());
			bool uDir = TotRPDetId::IsStripsCoordinateUDirection(detId);

			if (uDir) selHitsMap[RPId].first.push_back(& (*hit));
			else selHitsMap[RPId].second.push_back(& (*hit));
		}
	}

	// print out data and fill graphs
	for (map< unsigned int, pair< vector<const RPRecoHit *>, vector<const RPRecoHit *> > >::iterator it = allHitsMap.begin();
			it != allHitsMap.end(); ++it) {
		unsigned int RPId = it->first;

		// reset graphs and canvas
		allHits_u->Set(0);
		allHits_v->Set(0);
		selHits_u->Set(0);
		selHits_v->Set(0);
		can->Clear();

		if (verbosity > 5) printf(">> RP %i\n>> all u hits:\n", RPId);
		for (vector<const RPRecoHit *>::iterator hit = it->second.first.begin(); hit != it->second.first.end(); ++hit) {
			double z = geometry->GetDetector((*hit)->DetId())->translation().z();
			if (verbosity > 5) printf("\t%.3f\t%.3f\n", z, (*hit)->Position());
			allHits_u->SetPoint(allHits_u->GetN(), z, (*hit)->Position());
		}
		if (verbosity > 5) printf(">> all v hits:\n");
		for (vector<const RPRecoHit *>::iterator hit = it->second.second.begin(); hit != it->second.second.end(); ++hit) {
			double z = geometry->GetDetector((*hit)->DetId())->translation().z();
			if (verbosity > 5) printf("\t%.3f\t%.3f\n", z, (*hit)->Position());
			allHits_v->SetPoint(allHits_v->GetN(), z, (*hit)->Position());
		}
		
		map< unsigned int, pair< vector<const RPRecoHit *>, vector<const RPRecoHit *> > >::iterator sit = selHitsMap.find(RPId);
		if (sit == selHitsMap.end()) {
			if (verbosity > 5) printf(">> no selected hits\n");
		} else {
			if (verbosity > 5) printf(">> RP %i\n>> sel u hits:\n", RPId);
			for (vector<const RPRecoHit *>::iterator hit = sit->second.first.begin(); hit != sit->second.first.end(); ++hit) {
				double z = geometry->GetDetector((*hit)->DetId())->translation().z();
				if (verbosity > 5) printf("\t%.3f\t%.3f\n", z, (*hit)->Position());
				selHits_u->SetPoint(selHits_u->GetN(), z, (*hit)->Position());
			}
			if (verbosity > 5) printf(">> sel v hits:\n");
			for (vector<const RPRecoHit *>::iterator hit = sit->second.second.begin(); hit != sit->second.second.end(); ++hit) {
				double z = geometry->GetDetector((*hit)->DetId())->translation().z();
				if (verbosity > 5) printf("\t%.3f\t%.3f\n", z, (*hit)->Position());
				selHits_v->SetPoint(selHits_v->GetN(), z, (*hit)->Position());
			}
		}

		// make and save canvas
		can->Divide(2, 1);
		can->cd(1);
		if (allHits_u->GetN() > 0) {
			allHits_u->Draw("AP");	
			if (selHits_v->GetN() > 0) {
				selHits_u->Draw("P");
				selHits_u->Fit("pol1", "Q");	
			}
		}
		can->cd(2);
		if (allHits_v->GetN() > 0) {
			allHits_v->Draw("AP");	
			if (selHits_v->GetN() > 0) {
				selHits_v->Draw("P");
				selHits_v->Fit("pol1", "Q");	
			}
		}
	
		char buf[25];
		sprintf(buf, "%i can %llu", RPId, event.id().event());
		if (sit != selHitsMap.end()) strcat(buf, ", reco");
		RPTrackCandidateCollection::const_iterator tcit = selHits->find(RPId);
		if (tcit != selHits->end() && tcit->second.Fittable()) strcat(buf, ", fit");
		can->Write(buf);
	}
}

DEFINE_FWK_MODULE(RecognizedTrackAnalyzer);
