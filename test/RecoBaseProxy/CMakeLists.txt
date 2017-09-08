simple_plugin(ProxyBaseTest "module"
  lardata_RecoBaseProxy
  lardataobj_RecoBase
  ${MF_MESSAGELOGGER}
  ${MF_UTILITIES}
  ${ROOT_GENVECTOR}
  USE_BOOST_UNIT
  )

simple_plugin(TrackProxyTest "module"
  lardata_RecoBaseProxy
  lardataobj_RecoBase
  ${MF_MESSAGELOGGER}
  ${MF_UTILITIES}
  ${ROOT_GENVECTOR}
  USE_BOOST_UNIT
  )

simple_plugin(TrackProxyHitMaker "module"
  lardataobj_RecoBase
  ${MF_MESSAGELOGGER}
  ${MF_UTILITIES}
  )

simple_plugin(TrackProxyTrackMaker "module"
  lardataobj_RecoBase
  ${MF_MESSAGELOGGER}
  ${MF_UTILITIES}
  )

cet_test(ProxyBase_test
  HANDBUILT
  DATAFILES test_proxybase.fcl
  TEST_EXEC lar_ut
  TEST_ARGS -- --rethrow-all -c ./test_proxybase.fcl
  USE_BOOST_UNIT
  )

cet_test(Track_test
  HANDBUILT
  DATAFILES test_trackproxy.fcl
  TEST_EXEC lar_ut
  TEST_ARGS -- --rethrow-all -c ./test_trackproxy.fcl
  USE_BOOST_UNIT
  )
