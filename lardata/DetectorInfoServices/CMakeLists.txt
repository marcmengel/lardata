
simple_plugin(DetectorClocksServiceStandard "service"
              lardata_DetectorInfo
	      lardataobj_RawData
	      art_Utilities
              ${ART_FRAMEWORK_CORE}
	      ${ART_FRAMEWORK_PRINCIPAL}
	      ${ART_PERSISTENCY_ROOTDB}
	      art_Persistency_Common
	      art_Persistency_Provenance
	      ${ART_FRAMEWORK_SERVICES_REGISTRY}
	      canvas
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      ${FHICLCPP}
	      cetlib cetlib_except
	      ${SQLITE3}
	      ${ROOT_BASIC_LIB_LIST}
              
)

simple_plugin(DetectorPropertiesServiceStandard "service"
              lardata_DetectorInfo
              larcore_Geometry_Geometry_service
              larcorealg_Geometry
	      ${ART_PERSISTENCY_ROOTDB}
	      ${ART_FRAMEWORK_SERVICES_REGISTRY}
	      art_Utilities
	      canvas
	      ${SQLITE3}
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      cetlib cetlib_except
	      ${ROOT_BASIC_LIB_LIST}
              
)

simple_plugin(LArPropertiesServiceStandard "service"
	      lardata_DetectorInfo
	      art_Utilities
	      ${ART_FRAMEWORK_SERVICES_REGISTRY}
	      canvas
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      cetlib cetlib_except
              
)

install_headers()
install_fhicl()
install_source()

