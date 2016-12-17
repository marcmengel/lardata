simple_plugin( AssnProducer1           "module"  NO_INSTALL )
simple_plugin( AssnProducer2           "module"  NO_INSTALL )
simple_plugin( AssnAnalyzer           "module"  NO_INSTALL )

cet_test(test_feag HANDBUILT
  TEST_EXEC lar 
  TEST_ARGS --rethrow-all --config test_feag.fcl
  DATAFILES test_feag.fcl
)

cet_test(test_feag2 HANDBUILT
  TEST_EXEC lar 
  TEST_ARGS --rethrow-all --config test_feag2.fcl
  DATAFILES test_feag2.fcl
  TEST_PROPERTIES DEPENDS test_feag
)

art_dictionary(DICTIONARY_LIBRARIES cetlib NO_INSTALL)
