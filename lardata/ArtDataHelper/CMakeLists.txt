
art_make( 
          LIB_LIBRARIES lardataobj_RawData
                        lardataobj_RecoBase
                        lardataobj_AnalysisBase
                        larcore_Geometry
                        larcore_Geometry_Geometry_service
                        ${ART_FRAMEWORK_CORE}
                        ${ART_FRAMEWORK_PRINCIPAL}
                        ${ART_FRAMEWORK_SERVICES_REGISTRY}
                        art_Persistency_Common canvas_Persistency_Common
                        art_Persistency_Provenance canvas_Persistency_Provenance
                        art_Utilities canvas_Utilities
                        ${MF_MESSAGELOGGER}
                        ${MF_UTILITIES}
                        ${CETLIB}
                        ${ROOT_BASIC_LIB_LIST}
                        ${Boost_SYSTEM_LIBRARY}
        )

# add_subdirectory(Dumpers)

install_headers()
install_fhicl()
install_source()
