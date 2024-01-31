#ifndef MR_CLEAN_UTILS_H
#define MR_CLEAN_UTILS_H

#include <vector>
<<<<<<< HEAD
#include "DataContainer.h"
=======
>>>>>>> eb6d5a779d8584000b1c0e0df1c0bd89bf1bafdc

namespace mr_clean_utils { 
 struct SortPairByFirstItemDecreasing
  {
    template<typename T, typename U>
    bool operator()(const std::pair<T, U> &lhs, const std::pair<T, U> &rhs) const
    {
      return lhs.first > rhs.first;
    }
  };

  struct SortPairByFirstItemIncreasing
  {
    template<typename T, typename U>
    bool operator()(const std::pair<T, U> &lhs, const std::pair<T, U> &rhs) const
    {
      return lhs.first < rhs.first;
    }
  };

  struct SortPairBySecondItemDecreasing
  {
    template<typename T, typename U>
    bool operator()(const std::pair<T, U> &lhs, const std::pair<T, U> &rhs) const
    {
      return lhs.second > rhs.second;
    }
  };

  struct SortPairBySecondItemIncreasing
  {
    template<typename T, typename U>
    bool operator()(const std::pair<T, U> &lhs, const std::pair<T, U> &rhs) const
    {
      return lhs.second < rhs.second;
    }
  };
}

#endif