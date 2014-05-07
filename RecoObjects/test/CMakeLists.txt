cet_enable_asserts()

simple_plugin(KalmanFilterTest "module"
              RecoObjects
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      ${FHICLCPP}
	      ${CETLIB}
	      ${PQ}
              BASENAME_ONLY
             )

simple_plugin(PropTest "module"
              RecoObjects
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

