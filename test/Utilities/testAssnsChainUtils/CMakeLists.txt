simple_plugin(
  AssnsChainHitMaker "module"
    lardataobj_RecoBase
    art_Framework_Core
    art_Framework_Principal
    ${FHICLCPP}
    cetlib_except
  NO_INSTALL
  )
simple_plugin(
  AssnsChainClusterMaker "module"
    lardataobj_RecoBase
    art_Framework_Core
    art_Framework_Principal
    ${FHICLCPP}
    cetlib_except
  NO_INSTALL
  )
simple_plugin(
  AssnsChainPFParticleMaker "module"
    lardataobj_RecoBase
    art_Framework_Core
    art_Framework_Principal
    ${FHICLCPP}
    cetlib_except
  NO_INSTALL
  )
simple_plugin(
  AssnsChainShowerMaker "module"
    lardataobj_RecoBase
    art_Framework_Core
    art_Framework_Principal
    ${FHICLCPP}
    cetlib_except
  NO_INSTALL
  )
simple_plugin(
  AssnsChainTest "module"
    lardataobj_RecoBase
    art_Framework_Core
    art_Framework_Principal
    ${FHICLCPP}
    cetlib_except
  )

cet_test(AssnsChainUtil_test HANDBUILT
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config assnschainutils_test.fcl
  DATAFILES assnschainutils_test.fcl
  )


# install_headers()
# install_fhicl()
# install_source()
