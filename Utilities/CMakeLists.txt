
art_make( NO_PLUGINS
          LIBRARY_NAME  Utilities
          LIB_LIBRARIES 
                        LArFFT_service
                        RecoBase
                        Geometry
                        ${ART_FRAMEWORK_CORE}
                        ${ART_FRAMEWORK_PRINCIPAL}
                        ${ART_FRAMEWORK_SERVICES_REGISTRY}
                        ${ART_PERSISTENCY_COMMON}
                        ${ART_PERSISTENCY_PROVENANCE}
                        ${ART_UTILITIES}
                        ${MF_MESSAGELOGGER}
                        ${MF_UTILITIES}
                        ${CETLIB}
                        ${ROOT_BASIC_LIB_LIST}
        )

simple_plugin(DetectorClocksServiceStandard "service"
              DataProviders
	      RawData
#              DatabaseUtil_service
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

simple_plugin(DatabaseUtil "service"
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      ${FHICLCPP}
	      ${CETLIB}
	      ${PQ}
	      ${ART_UTILITIES}
              ${ART_FRAMEWORK_CORE}
              ${ART_FRAMEWORK_PRINCIPAL}
              ${ART_PERSISTENCY_ROOTDB}
              ${ART_PERSISTENCY_COMMON}
              ${ART_PERSISTENCY_PROVENANCE}
              ${ART_FRAMEWORK_SERVICES_REGISTRY}
              BASENAME_ONLY
)

simple_plugin(DetectorPropertiesServiceStandard "service"
              DataProviders
#              DatabaseUtil_service
              Geometry_service
              Geometry
	      ${ART_PERSISTENCY_ROOTDB}
	      ${SQLITE3}
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      ${CETLIB}
	      ${ROOT_BASIC_LIB_LIST}
              BASENAME_ONLY
)

simple_plugin(FileCatalogMetadataExtras "service"
	      ${ART_UTILITIES}
	      ${ART_PERSISTENCY_ROOTDB}
	      ${ART_FRAMEWORK_CORE}
	      ${ART_FRAMEWORK_PRINCIPAL}
	      ${ART_FRAMEWORK_SERVICES_SYSTEM_FILECATALOGMETADATA_SERVICE}
	      ${SQLITE3}
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      ${CETLIB}
	      ${ROOT_BASIC_LIB_LIST}
              BASENAME_ONLY
)

simple_plugin(LArFFT "service"
	      ${ART_UTILITIES}
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      ${FHICLCPP}
	      ${CETLIB}
	      ${ROOT_FFTW}
	      ${ROOT_BASIC_LIB_LIST}
              BASENAME_ONLY
)

simple_plugin(LArPropertiesServiceStandard "service"
#              Utilities
	      DataProviders
#              DatabaseUtil_service
	      ${ART_UTILITIES}
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      ${CETLIB}
              BASENAME_ONLY
)

simple_plugin(MemoryPeakReporter "service"
              ${ART_UTILITIES}
              ${ART_FRAMEWORK_SERVICES_REGISTRY}
              ${ART_FRAMEWORK_PRINCIPAL}
              ${ART_PERSISTENCY_PROVENANCE}
              ${MF_MESSAGELOGGER}
              ${MF_UTILITIES}
              ${FHICLCPP}
              ${CETLIB}
              BASENAME_ONLY
)

simple_plugin(ComputePi "module"
              ${ART_FRAMEWORK_CORE}
              ${MF_MESSAGELOGGER}
              ${MF_UTILITIES}
              ${FHICLCPP}
              ${CETLIB}
              BASENAME_ONLY
)

install_headers()
install_fhicl()
install_source()

add_subdirectory(test)

