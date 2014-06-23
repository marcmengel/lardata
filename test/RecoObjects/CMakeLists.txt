cet_test( SurfYZTest USE_BOOST_UNIT LIBRARIES RecoObjects )
cet_test( SurfXYZTest USE_BOOST_UNIT LIBRARIES RecoObjects )
cet_test( TrackTest LIBRARIES RecoObjects )
cet_test( LATest LIBRARIES RecoObjects )

install_headers()
install_fhicl()
install_source()
