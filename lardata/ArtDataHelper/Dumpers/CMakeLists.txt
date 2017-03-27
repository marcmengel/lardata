#
# Since dumper modules have similar dependencies depending on the data they dump
# we collect them in groups:
#
set(RawDataDumpers
  "DumpOpDetWaveforms"
  )
set(RecoBaseDumpers
  "DumpHits"
  "DumpClusters"
  "DumpSeeds"
  "DumpPCAxes"
  "DumpPFParticles"
  "DumpSpacePoints"
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


art_make(
  NO_PLUGINS
  LIB_LIBRARIES
    lardataobj_RecoBase
    ${ROOT_BASIC_LIB_LIST}
  )


foreach(Dumper IN LISTS RawDataDumpers)
  simple_plugin(${Dumper} "module"
      lardataobj_RawData
      art_Framework_Principal
      canvas_Persistency_Common
      canvas_Persistency_Provenance
      canvas_Utilities
      ${MF_MESSAGELOGGER}
      ${MF_UTILITIES}
      ${CETLIB}
      ${ROOT_BASIC_LIB_LIST}
    )
endforeach(Dumper IN LISTS RawDataDumpers)


foreach(Dumper IN LISTS RecoBaseDumpers)
  simple_plugin(${Dumper} "module"
      lardataobj_RecoBase
      lardata_ArtDataHelper_Dumpers
      art_Framework_Principal
      canvas_Persistency_Common
      canvas_Persistency_Provenance
      canvas_Utilities
      ${MF_MESSAGELOGGER}
      ${MF_UTILITIES}
      ${CETLIB}
      ${ROOT_BASIC_LIB_LIST}
    )
endforeach(Dumper IN LISTS RecoBaseDumpers)


foreach(Dumper IN LISTS SimulationDumpers)
  simple_plugin(${Dumper} "module"
      lardataobj_Simulation
      art_Framework_Principal
      canvas_Persistency_Common
      canvas_Persistency_Provenance
      canvas_Utilities
      ${MF_MESSAGELOGGER}
      ${MF_UTILITIES}
      ${CETLIB}
      ${ROOT_BASIC_LIB_LIST}
    )
endforeach(Dumper IN LISTS SimulationDumpers)


foreach(Dumper IN LISTS MCDumpers)
  simple_plugin(${Dumper} "module"
      lardataobj_MCBase
      art_Framework_Principal
      canvas_Persistency_Common
      canvas_Persistency_Provenance
      canvas_Utilities
      ${MF_MESSAGELOGGER}
      ${MF_UTILITIES}
      ${CETLIB}
      ${ROOT_BASIC_LIB_LIST}
    )
endforeach(Dumper IN LISTS MCDumpers)



install_headers()
install_fhicl()
install_source()
