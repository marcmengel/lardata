cet_enable_asserts()

simple_plugin(LArPropTest "module"
	      LArProperties_service
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

