
simple_plugin(DetectorClocksServiceStandard "service"
              lardata_DetectorInfo
	      lardata_RawData
	      art_Utilities canvas_Utilities
              ${ART_FRAMEWORK_CORE}
	      ${ART_FRAMEWORK_PRINCIPAL}
	      ${ART_PERSISTENCY_ROOTDB}
	      art_Persistency_Common canvas_Persistency_Common
	      art_Persistency_Provenance canvas_Persistency_Provenance
	      ${ART_FRAMEWORK_SERVICES_REGISTRY}
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      ${FHICLCPP}
	      ${CETLIB}
	      ${SQLITE3}
	      ${ROOT_BASIC_LIB_LIST}
              
)

simple_plugin(DetectorPropertiesServiceStandard "service"
              lardata_DetectorInfo
              larcore_Geometry_Geometry_service
              larcore_Geometry
	      ${ART_PERSISTENCY_ROOTDB}
	      ${ART_FRAMEWORK_SERVICES_REGISTRY}
	      art_Utilities canvas_Utilities
	      ${SQLITE3}
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      ${CETLIB}
	      ${ROOT_BASIC_LIB_LIST}
              
)

simple_plugin(LArPropertiesServiceStandard "service"
	      lardata_DetectorInfo
	      art_Utilities canvas_Utilities
	      ${ART_FRAMEWORK_SERVICES_REGISTRY}
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      ${CETLIB}
              
)

install_headers()
install_fhicl()
install_source()

