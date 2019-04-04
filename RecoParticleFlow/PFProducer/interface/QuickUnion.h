#ifndef RecoParticleFlow_PFProducer_QuickUnion_h
#define RecoParticleFlow_PFProducer_QuickUnion_h 

#include <vector>

class QuickUnion {
  std::vector<unsigned> id_;
  std::vector<unsigned> size_;
  int count_;

public:
  QuickUnion(const unsigned NBranches);

  int count() const;

  unsigned find(unsigned p);

  bool connected(unsigned p, unsigned q);

  void unite(unsigned p, unsigned q);
};

#endif
