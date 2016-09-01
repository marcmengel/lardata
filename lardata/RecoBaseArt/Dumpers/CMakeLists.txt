art_make(
  LIB_LIBRARIES
    lardataobj_RecoBase
  MODULE_LIBRARIES
    lardataobj_RecoBase
    lardata_RecoBaseArt_Dumpers
    ${ART_FRAMEWORK_PRINCIPAL}
    art_Persistency_Common canvas_Persistency_Common
    art_Persistency_Provenance canvas_Persistency_Provenance
    art_Utilities canvas_Utilities
    ${MF_MESSAGELOGGER}
    ${MF_UTILITIES}
    ${CETLIB}
  )

install_headers()
install_fhicl()
install_source()
