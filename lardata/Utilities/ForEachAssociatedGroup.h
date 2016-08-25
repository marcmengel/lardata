#ifndef FOR_EACH_ASSOCIATED_GROUP_H
#define FOR_EACH_ASSOCIATED_GROUP_H

/*
 *
 *  @brief  Helper function to access associations in order
 *
 */


#include "range/v3/all.hpp"

template <class A, class F>
void for_each_associated_group(A const & assns, F & func) {
   ranges::for_each(assns |
                    ranges::view::all |
                    ranges::view::group_by([](auto a1, auto a2) { return a1.first==a2.first;}) |
                    ranges::view::transform([] (auto pairs) {return pairs |
      ranges::view::values;}),
                    func);
}

#endif
