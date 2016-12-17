#ifndef FOR_EACH_ASSOCIATED_GROUP_H
#define FOR_EACH_ASSOCIATED_GROUP_H

/*
 *
 *  @brief  Helper function to access associations in order
 *
 *  This function takes two input arguments, a const ref to 
 *  the association data product itself (Assns), and the function
 *  (func) to be operated on each of the group of associated objects.
 *  This function represents the association data product as 
 *  a range of ranges representing the right hand side in the 
 *  collection, hence the function provided as the second argument
 *  should assume that it will be operating on a range of art::Ptr
 *  to the associated data products grouped by the data product 
 *  they are associated with.
 */


#include "range/v3/all.hpp"

template <class A, class F>
void for_each_associated_group(A const & assns, F & func) {
   ranges::for_each(assns |
                    ranges::view::all |
                    ranges::view::group_by([](auto a1, auto a2) { return a1.first == a2.first;}) |
                    ranges::view::transform([] (auto pairs) {return pairs | ranges::view::values;}),
                    func);
}

#endif
