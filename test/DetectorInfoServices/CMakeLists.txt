simple_plugin(LArPropTest "module"
	      ${ART_FRAMEWORK_SERVICES_REGISTRY}
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      ${FHICLCPP}
	      ${CETLIB}
	      ${PQ}
              BASENAME_ONLY
)

cet_test( LarProp HANDBUILT
  DATAFILES lartest.fcl
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config ./lartest.fcl
  )

install_headers()
install_fhicl()
install_source()

