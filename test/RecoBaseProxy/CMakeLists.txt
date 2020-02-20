###############################################################################
###  ProxyBase tests
###
add_subdirectory(ProxyBase)

simple_plugin(ProxyBaseTest "module"
  lardata_RecoBaseProxy
  lardataobj_RecoBase
  art_Persistency_Provenance
  ${MF_MESSAGELOGGER}

  ROOT::GenVector
  USE_BOOST_UNIT
  )

cet_test(ProxyBase_test
  HANDBUILT
  DATAFILES test_proxybase.fcl
  TEST_EXEC lar_ut
  TEST_ARGS -- --rethrow-all -c ./test_proxybase.fcl
  USE_BOOST_UNIT
  )


###############################################################################
###  TrackProxy tests
###
simple_plugin(TrackProxyTest "module"
  lardata_RecoBaseProxy
  lardataobj_RecoBase
  art_Persistency_Provenance
  ${MF_MESSAGELOGGER}

  ROOT::GenVector
  USE_BOOST_UNIT
  )

simple_plugin(TrackProxyHitMaker "module"
  lardataobj_RecoBase
  art_Persistency_Provenance
  ${MF_MESSAGELOGGER}

  )

simple_plugin(TrackProxyTrackMaker "module"
  lardataobj_RecoBase
  art_Persistency_Provenance
  ${MF_MESSAGELOGGER}
  ROOT::Core
  )

cet_test(TrackProxy_test
  HANDBUILT
  DATAFILES test_trackproxy.fcl
  TEST_EXEC lar_ut
  TEST_ARGS -- --rethrow-all -c ./test_trackproxy.fcl
  USE_BOOST_UNIT
  )

###############################################################################
###  ChargedSpacePointProxy tests
###
simple_plugin(ChargedSpacePointProxyTest "module"
  lardata_RecoBaseProxy
  lardataobj_RecoBase
  art_Persistency_Provenance
  ${MF_MESSAGELOGGER}

  ROOT::GenVector
  USE_BOOST_UNIT
  )

simple_plugin(ChargedSpacePointProxyInputMaker "module"
  lardataobj_RecoBase
  lardata_ArtDataHelper
  ${MF_MESSAGELOGGER}

  USE_BOOST_UNIT
  )

cet_test(ChargedSpacePointProxy_test
  HANDBUILT
  DATAFILES test_chargedspacepointproxy.fcl
  TEST_EXEC lar_ut
  TEST_ARGS -- --rethrow-all -c ./test_chargedspacepointproxy.fcl
  USE_BOOST_UNIT
  )

###############################################################################
