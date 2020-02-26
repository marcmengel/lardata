art_make(LIB_LIBRARIES lardataobj_AnalysisBase
                       lardataobj_RecoBase
                       larcore_Geometry_Geometry_service
                       larcorealg_Geometry
                       ${ART_FRAMEWORK_CORE}
                       ${ART_FRAMEWORK_SERVICES_REGISTRY}
                       ${ART_UTILITIES}
                       canvas
                       ${FHICLCPP}
                       cetlib_except
                       ROOT::Core
                       ROOT::Physics)

install_headers()
install_fhicl()
install_source()
