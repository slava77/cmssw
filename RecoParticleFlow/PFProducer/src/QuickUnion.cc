#include "RecoParticleFlow/PFProducer/interface/QuickUnion.h"

#include <vector>

QuickUnion::QuickUnion(const unsigned NBranches) {
  count_ = NBranches;
  id_.resize(NBranches);
  size_.resize(NBranches);
  for (unsigned i = 0; i < NBranches; ++i) {
    id_[i] = i;
    size_[i] = 1;
  }
}

int QuickUnion::count() const { return count_; }

unsigned QuickUnion::find(unsigned p) {
  while (p != id_[p]) {
    id_[p] = id_[id_[p]];
    p = id_[p];
  }
  return p;
}

bool QuickUnion::connected(unsigned p, unsigned q) { return find(p) == find(q); }

void QuickUnion::unite(unsigned p, unsigned q) {
  unsigned rootP = find(p);
  unsigned rootQ = find(q);
  id_[p] = q;

  if (size_[rootP] < size_[rootQ]) {
    id_[rootP] = rootQ;
    size_[rootQ] += size_[rootP];
  } else {
    id_[rootQ] = rootP;
    size_[rootP] += size_[rootQ];
  }
  --count_;
}
