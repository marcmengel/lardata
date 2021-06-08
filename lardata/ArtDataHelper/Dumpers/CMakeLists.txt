#
# Since dumper modules have similar dependencies depending on the data they dump
# we collect them in groups:
#
set(RawDataDumpers
  "DumpRawDigits"
  "DumpOpDetWaveforms")
set(RecoBaseDumpers
  "DumpWires"
  "DumpHits"
  "DumpClusters"
  "DumpSeeds"
  "DumpPCAxes"
  "DumpPFParticles"
  "DumpSpacePoints"
  "DumpChargedSpacePoints"
  "DumpTracks"
  "DumpVertices")
set(SimulationDumpers
  "DumpSimPhotonsLite"
  "DumpSimEnergyDeposits"
  )
set(MCDumper)


art_make(NO_PLUGINS LIB_LIBRARIES lardataobj_RecoBase)

foreach(Dumper IN LISTS RawDataDumpers)
  simple_plugin(${Dumper} "module"
      lardataobj_RawData
      ${ART_FRAMEWORK_SERVICES_REGISTRY}
      ${MF_MESSAGELOGGER})
endforeach()


foreach(Dumper IN LISTS RecoBaseDumpers)
  simple_plugin(${Dumper} "module"
      lardataobj_RecoBase
      lardata_ArtDataHelper_Dumpers
      lardata_RecoBaseProxy
      ${MF_MESSAGELOGGER})
endforeach()


foreach(Dumper IN LISTS SimulationDumpers)
  simple_plugin(${Dumper} "module"
      lardataalg_MCDumpers
      lardataobj_Simulation
      ${MF_MESSAGELOGGER}
      ROOT::GenVector
      ROOT::Core)
endforeach(Dumper IN LISTS SimulationDumpers)


foreach(Dumper IN LISTS MCDumpers)
  simple_plugin(${Dumper} "module" lardataobj_MCBase)
endforeach()



install_headers()
install_fhicl()
install_source()
