cet_enable_asserts()

simple_plugin(LArPropTest "module"
              DataProviders
	      ${ART_FRAMEWORK_SERVICES_REGISTRY}
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      ${FHICLCPP}
	      ${CETLIB}
	      ${PQ}
              BASENAME_ONLY
)

install_headers()
install_fhicl()
install_source()

