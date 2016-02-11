cet_test( SurfYZTest USE_BOOST_UNIT LIBRARIES lardata_RecoObjects )
cet_test( SurfXYZTest USE_BOOST_UNIT LIBRARIES lardata_RecoObjects )
cet_test( SurfYZLineTest USE_BOOST_UNIT LIBRARIES lardata_RecoObjects )
cet_test( TrackTest LIBRARIES lardata_RecoObjects )
cet_test( LATest LIBRARIES lardata_RecoObjects )

install_headers()
install_fhicl()
install_source()
