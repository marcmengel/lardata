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

cet_test( SurfaceTest SOURCES SurfaceTest.cxx LIBRARIES RecoObjects )

install_headers()
install_fhicl()
install_source()
