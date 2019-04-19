#
# Since dumper modules have similar dependencies depending on the data they dump
# we collect them in groups:
#
set(RawDataDumpers
  "DumpRawDigits"
  "DumpOpDetWaveforms"
  )
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
  "DumpVertices"
  )
set(SimulationDumpers
  "DumpSimPhotonsLite"
  )
set(MCDumpers
  )

string(REPLACE ";" "_module.cc;" DumperSources
  "${RawDataDumpers};${RecoBaseDumpers};${SimulationDumpers};${MCDumpers}"
  )


art_make(NO_PLUGINS LIB_LIBRARIES lardataobj_RecoBase)

foreach(Dumper IN LISTS RawDataDumpers)
  simple_plugin(${Dumper} "module"
      lardataobj_RawData
      art_Persistency_Provenance
      ${MF_MESSAGELOGGER})
endforeach(Dumper IN LISTS RawDataDumpers)


foreach(Dumper IN LISTS RecoBaseDumpers)
  simple_plugin(${Dumper} "module"
      lardataobj_RecoBase
      lardata_ArtDataHelper_Dumpers
      lardata_RecoBaseProxy
      art_Persistency_Provenance
      ${MF_MESSAGELOGGER})
endforeach(Dumper IN LISTS RecoBaseDumpers)


foreach(Dumper IN LISTS SimulationDumpers)
  simple_plugin(${Dumper} "module"
      lardataobj_Simulation
      art_Persistency_Provenance
      ${MF_MESSAGELOGGER}
      ${ROOT_CORE})
endforeach(Dumper IN LISTS SimulationDumpers)


foreach(Dumper IN LISTS MCDumpers)
  simple_plugin(${Dumper} "module"
      lardataobj_MCBase
      art_Persistency_Provenance)
endforeach(Dumper IN LISTS MCDumpers)



install_headers()
install_fhicl()
install_source()
