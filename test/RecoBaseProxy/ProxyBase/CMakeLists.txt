###############################################################################
###  Support utility tests
###
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare -Wno-pedantic")
cet_test(AssnsTraits_test
  USE_BOOST_UNIT
  )


###############################################################################

