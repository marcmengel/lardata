simple_plugin( AssnProducer1          "module"  NO_INSTALL cetlib_except )
simple_plugin( AssnProducer2          "module"  NO_INSTALL cetlib_except )
simple_plugin( AssnAnalyzer           "module"  NO_INSTALL cetlib_except )

cet_test(associated_groups_test USE_BOOST_UNIT
  LIBRARIES
    canvas
    cetlib cetlib_except
    ${ROOT_CORE}
  )

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

cet_test(test_associatedgroups_full HANDBUILT
  TEST_EXEC lar 
  TEST_ARGS --rethrow-all --config test_associatedgroups_full.fcl
  DATAFILES test_associatedgroups_full.fcl
)

cet_test(test_associatedgroups HANDBUILT
  TEST_EXEC lar 
  TEST_ARGS --rethrow-all --config test_associatedgroups.fcl
  DATAFILES test_associatedgroups.fcl
  TEST_PROPERTIES DEPENDS test_associatedgroups_full
)

art_dictionary(DICTIONARY_LIBRARIES cetlib cetlib_except NO_INSTALL )
