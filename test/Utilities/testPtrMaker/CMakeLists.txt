simple_plugin(IntVectorProducer           "module"  NO_INSTALL )
simple_plugin(PtrMakerProducer           "module"  NO_INSTALL )
simple_plugin(PtrMakerProducer2           "module"  NO_INSTALL )
simple_plugin(PtrMakerAnalyzer           "module"  NO_INSTALL )

cet_test(test_ptrmaker HANDBUILT
  TEST_EXEC lar 
  TEST_ARGS --rethrow-all --config test_ptrmaker.fcl
  DATAFILES test_ptrmaker.fcl
)

cet_test(test_ptrmaker2 HANDBUILT
  TEST_EXEC lar 
  TEST_ARGS --rethrow-all --config test_ptrmaker2.fcl
  DATAFILES test_ptrmaker2.fcl
)

