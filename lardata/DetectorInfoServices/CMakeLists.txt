
simple_plugin(DetectorClocksServiceStandard "service"
              lardata_DetectorInfo
	      lardata_RawData
	      ${ART_UTILITIES}
              ${ART_FRAMEWORK_CORE}
	      ${ART_FRAMEWORK_PRINCIPAL}
	      ${ART_PERSISTENCY_ROOTDB}
	      ${ART_PERSISTENCY_COMMON}
	      ${ART_PERSISTENCY_PROVENANCE}
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
	      ${ART_UTILITIES}
	      ${SQLITE3}
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      ${CETLIB}
	      ${ROOT_BASIC_LIB_LIST}
              
)

simple_plugin(LArPropertiesServiceStandard "service"
	      lardata_DetectorInfo
	      ${ART_UTILITIES}
	      ${ART_FRAMEWORK_SERVICES_REGISTRY}
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      ${CETLIB}
              
)

install_headers()
install_fhicl()
install_source()

