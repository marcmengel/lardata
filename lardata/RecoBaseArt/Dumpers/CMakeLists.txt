art_make(
  LIB_LIBRARIES
    lardata_RecoBase
  MODULE_LIBRARIES
    lardata_RecoBase
    lardata_RecoBaseArt_Dumpers
    ${ART_FRAMEWORK_PRINCIPAL}
    ${ART_PERSISTENCY_COMMON}
    ${ART_PERSISTENCY_PROVENANCE}
    ${ART_UTILITIES}
    ${MF_MESSAGELOGGER}
    ${MF_UTILITIES}
    ${CETLIB}
  )

install_headers()
install_fhicl()
install_source()
