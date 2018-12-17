simple_plugin(DetectorClocksServiceStandard "service"
              lardataalg_DetectorInfo
	      lardataobj_RawData
	      art_Utilities
              ${ART_FRAMEWORK_CORE}
	      ${ART_FRAMEWORK_PRINCIPAL}
	      ${ART_FRAMEWORK_IO_ROOT_ROOTDB}
	      art_Persistency_Common
	      art_Persistency_Provenance
	      ${ART_FRAMEWORK_SERVICES_REGISTRY}
	      canvas
	      ${MF_MESSAGELOGGER}
	      
	      ${FHICLCPP}
	      cetlib cetlib_except
	      ${SQLITE3}
	      ${ROOT_BASIC_LIB_LIST}
              
)

simple_plugin(DetectorPropertiesServiceStandard "service"
              lardataalg_DetectorInfo
              larcore_Geometry_Geometry_service
              larcorealg_Geometry
	      ${ART_FRAMEWORK_IO_ROOT_ROOTDB}
	      ${ART_FRAMEWORK_SERVICES_REGISTRY}
	      art_Utilities
	      canvas
	      ${SQLITE3}
	      ${MF_MESSAGELOGGER}
	      
	      cetlib cetlib_except
	      ${ROOT_BASIC_LIB_LIST}
              
)

simple_plugin(LArPropertiesServiceStandard "service"
	      lardataalg_DetectorInfo
              art_Framework_Principal
	      art_Utilities
	      ${ART_FRAMEWORK_SERVICES_REGISTRY}
	      canvas
	      ${MF_MESSAGELOGGER}
	      
	      cetlib cetlib_except
              
)

install_headers()
install_fhicl()
install_source()
