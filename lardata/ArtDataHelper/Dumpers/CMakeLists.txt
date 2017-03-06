art_make(
  LIB_LIBRARIES
    lardataobj_RecoBase
    ${ROOT_BASIC_LIB_LIST}
  MODULE_LIBRARIES
    lardataobj_RecoBase
    lardata_ArtDataHelper_Dumpers
    art_Framework_Principal
    canvas
    ${MF_MESSAGELOGGER}
    ${MF_UTILITIES}
    cetlib cetlib_except
    ${ROOT_BASIC_LIB_LIST}
  )

install_headers()
install_fhicl()
install_source()
