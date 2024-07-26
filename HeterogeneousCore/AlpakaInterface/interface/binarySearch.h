#ifndef HeterogeneousCore_AlpakaInterface_interface_binarySearch_h
#define HeterogeneousCore_AlpakaInterface_interface_binarySearch_h

namespace cms::alpakatools {

  ALPAKA_FN_HOST_ACC ALPAKA_FN_INLINE int binary_search(const unsigned int* data,  // Array that we are searching over
                                                        const unsigned int search_val,  // Value we want to find in data array
                                                        const unsigned int ndata)       // Number of elements in data array
  {
    unsigned int low = 0;
    unsigned int high = ndata - 1;

    while (low <= high) {
      unsigned int mid = (low + high) / 2;
      unsigned int test_val = data[mid];
      if (test_val == search_val)
        return mid;
      else if (test_val > search_val)
        high = mid - 1;
      else
        low = mid + 1;
    }
    // Couldn't find search value in array.
    return -1;
  };

}  // namespace cms::alpakatools

#endif  // HeterogeneousCore_AlpakaInterface_interface_binarySearch_h
