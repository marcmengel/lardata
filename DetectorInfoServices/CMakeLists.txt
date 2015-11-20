
simple_plugin(DetectorClocksServiceStandard "service"
              DetectorInfo
	      RawData
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
              BASENAME_ONLY
)

simple_plugin(DetectorPropertiesServiceStandard "service"
              DetectorInfo
              Geometry_service
              Geometry
	      ${ART_PERSISTENCY_ROOTDB}
	      ${ART_FRAMEWORK_SERVICES_REGISTRY}
	      ${SQLITE3}
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      ${CETLIB}
	      ${ROOT_BASIC_LIB_LIST}
              BASENAME_ONLY
)

simple_plugin(LArPropertiesServiceStandard "service"
	      DetectorInfo
	      ${ART_UTILITIES}
	      ${ART_FRAMEWORK_SERVICES_REGISTRY}
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      ${CETLIB}
              BASENAME_ONLY
)

install_headers()
install_fhicl()
install_source()

