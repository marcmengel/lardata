# test raw data compression
cet_test(raw_test USE_BOOST_UNIT
  LIBRARIES RawData
  )

# test data products
cet_test(RawDigit_test USE_BOOST_UNIT
  LIBRARIES RawData
  )

install_headers()
install_source()
