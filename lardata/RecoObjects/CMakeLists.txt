art_make(LIB_LIBRARIES
           lardataobj_AnalysisBase
           lardataobj_RecoBase
           lardataalg_DetectorInfo
           larcore_Geometry_Geometry_service
           larcorealg_Geometry
           ${ART_FRAMEWORK_CORE}
           ${ART_FRAMEWORK_SERVICES_REGISTRY}
           ${ART_UTILITIES}
           canvas
           fhiclcpp::fhiclcpp fhiclcpp::types
           cetlib_except
           ROOT::Core
           ROOT::Physics)

install_headers()
install_fhicl()
install_source()
