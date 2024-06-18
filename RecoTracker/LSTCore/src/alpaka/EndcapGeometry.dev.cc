#include "EndcapGeometry.h"

template <typename TDev>
SDL::EndcapGeometry<TDev>::EndcapGeometry(TDev const& devAccIn)
    : geoMapDetId_buf(allocBufWrapper<unsigned int>(devAccIn, 0)), geoMapPhi_buf(allocBufWrapper<float>(devAccIn, 0)) {}

template <typename TDev>
template <typename TQueue>
SDL::EndcapGeometry<TDev>::EndcapGeometry(TQueue& queue, SDL::EndcapGeometry<SDL::DevHost> const& endcapGeometrySrc)
    : dxdy_slope_(endcapGeometrySrc.dxdy_slope_),
      centroid_phis_(endcapGeometrySrc.centroid_phis_),
      geoMapDetId_buf(allocBufWrapper<unsigned int>(alpaka::getDev(queue), endcapGeometrySrc.nEndCapMap)),
      geoMapPhi_buf(allocBufWrapper<float>(alpaka::getDev(queue), endcapGeometrySrc.nEndCapMap)),
      nEndCapMap(endcapGeometrySrc.nEndCapMap) {
  alpaka::memcpy(queue, geoMapPhi_buf, endcapGeometrySrc.geoMapPhi_buf);
  alpaka::memcpy(queue, geoMapDetId_buf, endcapGeometrySrc.geoMapDetId_buf);
}

template <typename TDev>
void SDL::EndcapGeometry<TDev>::load(std::string filename) {
  dxdy_slope_.clear();
  centroid_phis_.clear();

  std::ifstream ifile(filename, std::ios::binary);
  if (!ifile.is_open()) {
    throw std::runtime_error("Unable to open file: " + filename);
  }

  while (!ifile.eof()) {
    unsigned int detid;
    float dxdy_slope, centroid_phi;

    // Read the detid, dxdy_slope, and centroid_phi from binary file
    ifile.read(reinterpret_cast<char*>(&detid), sizeof(detid));
    ifile.read(reinterpret_cast<char*>(&dxdy_slope), sizeof(dxdy_slope));
    ifile.read(reinterpret_cast<char*>(&centroid_phi), sizeof(centroid_phi));

    if (ifile) {
      dxdy_slope_[detid] = dxdy_slope;
      centroid_phis_[detid] = centroid_phi;
    } else {
      // End of file or read failed
      if (!ifile.eof()) {
        throw std::runtime_error("Failed to read Endcap Geometry binary data.");
      }
    }
  }

  fillGeoMapArraysExplicitHost();
}

template <typename TDev>
void SDL::EndcapGeometry<TDev>::fillGeoMapArraysExplicitHost() {
  unsigned int phi_size = centroid_phis_.size();

  // Allocate buffers on host
  SDL::DevHost const& devHost = cms::alpakatools::host();
  geoMapPhi_buf = allocBufWrapper<float>(devHost, phi_size);
  geoMapDetId_buf = allocBufWrapper<unsigned int>(devHost, phi_size);

  // Access the raw pointers of the buffers
  float* mapPhi = alpaka::getPtrNative(geoMapPhi_buf);
  unsigned int* mapDetId = alpaka::getPtrNative(geoMapDetId_buf);

  unsigned int counter = 0;
  for (auto it = centroid_phis_.begin(); it != centroid_phis_.end(); ++it) {
    unsigned int detId = it->first;
    float Phi = it->second;
    mapPhi[counter] = Phi;
    mapDetId[counter] = detId;
    counter++;
  }

  nEndCapMap = counter;
}

template <typename TDev>
float SDL::EndcapGeometry<TDev>::getdxdy_slope(unsigned int detid) const {
  if (dxdy_slope_.find(detid) != dxdy_slope_.end()) {
    return dxdy_slope_.at(detid);
  } else {
    return 0;
  }
}

// We need to instantiate these to ensure that the compiler generates them
template class SDL::EndcapGeometry<SDL::DevHost>;
template SDL::EndcapGeometry<SDL::Dev>::EndcapGeometry(SDL::QueueAcc& queue,
                                                       SDL::EndcapGeometry<SDL::DevHost> const& endcapGeometrySrc);