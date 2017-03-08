art_make(
  EXCLUDE "DumpOpDetWaveforms_module.cc"
  LIB_LIBRARIES
    lardataobj_RecoBase
    ${ROOT_BASIC_LIB_LIST}
  MODULE_LIBRARIES
    lardataobj_RawData
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

simple_plugin(DumpOpDetWaveforms "module"
  lardataobj_RawData
  art_Framework_Principal
  canvas_Persistency_Common
  canvas_Persistency_Provenance
  canvas_Utilities
  ${MF_MESSAGELOGGER}
  ${MF_UTILITIES}
  ${CETLIB}
  ${ROOT_CORE}
  )


install_headers()
install_fhicl()
install_source()
