art_make(
  MODULE_LIBRARIES
    Geometry
    ${ART_FRAMEWORK_SERVICES_REGISTRY}
    ${MF_MESSAGELOGGER}
    ${MF_UTILITIES}
    ${FHICLCPP}
    ${CETLIB}
)

cet_test(LarProp HANDBUILT
  DATAFILES lartest.fcl
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config ./lartest.fcl
  )

cet_test(ServicePackTest HANDBUILT
  DATAFILES servicepacktest.fcl
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config ./servicepacktest.fcl
  )

install_headers()
install_fhicl()
install_source()

