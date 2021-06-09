art_make(LIB_LIBRARIES lardataobj_RawData
                       lardataobj_RecoBase
                       lardataobj_AnalysisBase
                       larcorealg_Geometry
                       larcore_Geometry_Geometry_service
                       art::Framwork_Core
                       art::Framework_Principal
                       art::Framework_Services_Registry
                       art_Persistency_Common
                       art_Persistency_Provenance
                       art_Utilities
                       canvas
                       cetlib_except
                       ROOT::Core
                       ROOT::GenVector)

add_subdirectory(Dumpers)

install_headers()
install_fhicl()
install_source()
