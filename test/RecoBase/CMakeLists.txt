cet_test(Wire_test USE_BOOST_UNIT
  LIBRARIES RecoBase
  )

cet_test(Hit_test USE_BOOST_UNIT
  LIBRARIES RecoBase
  )

cet_test(Cluster_test USE_BOOST_UNIT
  LIBRARIES RecoBase
  )

install_headers()
install_source()
