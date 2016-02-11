cet_enable_asserts()

simple_plugin(LArPropTest "module"
	      lardata_Utilities_LArProperties_service
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      ${FHICLCPP}
	      ${CETLIB}
	      ${PQ}
              
)

install_headers()
install_fhicl()
install_source()

