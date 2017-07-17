
art_make( 
          LIB_LIBRARIES lardataobj_RawData
                        lardataobj_RecoBase
                        lardataobj_AnalysisBase
                        larcorealg_Geometry
                        larcore_Geometry_Geometry_service
                        ${ART_FRAMEWORK_CORE}
                        ${ART_FRAMEWORK_PRINCIPAL}
                        ${ART_FRAMEWORK_SERVICES_REGISTRY}
                        art_Persistency_Common
                        art_Persistency_Provenance
                        art_Utilities
			canvas
                        ${MF_MESSAGELOGGER}
                        ${MF_UTILITIES}
                        cetlib cetlib_except
                        ${ROOT_BASIC_LIB_LIST}
                        ${Boost_SYSTEM_LIBRARY}
        )

add_subdirectory(Dumpers)

install_headers()
install_fhicl()
install_source()
