# test raw data compression
cet_test(raw_test USE_BOOST_UNIT
  LIBRARIES RawData
  )

install_headers()
install_fhicl()
install_source()
