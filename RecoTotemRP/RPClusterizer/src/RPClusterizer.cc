// File: RPClusterizer.cc
// Description:  see RPClusterizer.h
// Author:  Hubert Niewiadomski, CERN
// Creation Date:  OGU Aug. 1 2005 Initial version.
//
//--------------------------------------------
 
#include "RecoTotemRP/RPClusterizer/interface/RPClusterizer.h"
#include "FWCore/Framework/interface/MakerMacros.h"

RPClusterizer::RPClusterizer(edm::ParameterSet const& conf) :
  conf_(conf), RPClusterizerAlgorithm_(conf)
{
  edm::LogInfo("RPClusterizer") << "[RPClusterizer::RPClusterizer] Constructing object...";
  verbosity_ = conf.getParameter<int>("Verbosity");
//  digiProducer_ = conf.getParameter<std::string>("DigiProducer");
  if(!conf.exists("DigiLabel")){
	  std::cout<<"expecting DigiLabel with data type RPStripDigi"<<std::endl;
  }
  digiInputTag_ = conf.getParameter<edm::InputTag>("DigiLabel");
//  clusterLabel_ = conf.getParameter<std::string>("ClusterLabel");
  //produces< edm::DetSetVector<RPDigCluster> > (clusterLabel_);
  produces< edm::DetSetVector<RPDigCluster> > ();
  digiInputTagToken_ = consumes<edm::DetSetVector<RPStripDigi> >(digiInputTag_);


}
 
// Virtual destructor needed.
RPClusterizer::~RPClusterizer()
{
  edm::LogInfo("RPClusterizer") << "[RPClusterizer::~RPClusterizer] Destructing object...";
}
 
//Get at the beginning
void RPClusterizer::beginJob()
{
  if(verbosity_)
  {
    edm::LogInfo("RPClusterizer") << "[RPClusterizer::beginJob]";
  }
}
 
// Functions that gets called by framework every event
void RPClusterizer::produce(edm::Event& e, const edm::EventSetup& es)
{
  // Step B: Get Inputs
  edm::Handle< edm::DetSetVector<RPStripDigi> >  input;
 
  // Step C: produce output product
  std::vector< edm::DetSet<RPDigCluster> > vRPStripCluster;
  vRPStripCluster.reserve(240);
  
//  e.getByLabel(digiInputTag_, input);  //FIXME: fix this label
   e.getByToken(digiInputTagToken_, input);

//  e.getByType(input);  //FIXME: fix this label
 
  if(verbosity_)
    std::cout << " Reading " << input->size() << " of RPStripDigi" << std::endl;
 
  if(input->size())
    run(*input,vRPStripCluster);
   
  // Step D: create and fill output collection
  std::auto_ptr< edm::DetSetVector<RPDigCluster> > output(new edm::DetSetVector<RPDigCluster>(vRPStripCluster) );
 
  if(verbosity_)
    std::cout << " Saving " << output->size() << " of RPDigCluster" << std::endl;

  // Step D: write output to file
  e.put(output);
}


void RPClusterizer::run(const edm::DetSetVector<RPStripDigi>& input,std::vector<edm::DetSet<RPDigCluster> > & output)
{
    int number_detunits = 0;
    int total_cluster_no = 0;
 
    //loop on all detset inside the input collection
    edm::DetSetVector<RPStripDigi>::const_iterator DSViter=input.begin();
    for (; DSViter!=input.end();DSViter++)
    {
      ++number_detunits;
      if(verbosity_)
        LogDebug("RPClusterizer")  << "[RPClusterizer::run] DetID " << DSViter->id;
 
      edm::DetSet<RPDigCluster> ssc(DSViter->id);
      RPClusterizerAlgorithm_.BuildClusters(DSViter->data, ssc.data);
      total_cluster_no += ssc.data.size();
       
      if (ssc.data.size())
        output.push_back(ssc);  // insert the DetSet<RPDigCluster> in the  DetSetVec<RPDigCluster> only if there is at least a digi
    }
    if(verbosity_)
      LogDebug("RPClusterizer") << "[RPClusterizer] generating " << total_cluster_no << " SiStripClusters in " << number_detunits << " DetUnits.";
}

DEFINE_FWK_MODULE(RPClusterizer);
