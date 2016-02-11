
art_make( NO_PLUGINS
          
          LIB_LIBRARIES lardata_Utilities_DetectorProperties_service
                        lardata_Utilities_LArProperties_service
                        lardata_Utilities_LArFFT_service
                        lardata_Utilities_TimeService_service
                        lardata_RecoBase
                        larcore_Geometry
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

simple_plugin(TimeService "service"
	      lardata_RawData
              lardata_Utilities_DatabaseUtil_service
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
              
)

simple_plugin(DetectorProperties "service"
              lardata_Utilities_TimeService_service
              lardata_Utilities_LArProperties_service
              lardata_Utilities_DatabaseUtil_service
              larcore_Geometry_Geometry_service
              larcore_Geometry
	      ${ART_PERSISTENCY_ROOTDB}
	      ${SQLITE3}
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      ${CETLIB}
	      ${ROOT_BASIC_LIB_LIST}
              
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
              
)

simple_plugin(LArFFT "service"
	      ${ART_UTILITIES}
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      ${FHICLCPP}
	      ${CETLIB}
	      ${ROOT_FFTW}
	      ${ROOT_BASIC_LIB_LIST}
              
)

simple_plugin(LArProperties "service"
              lardata_Utilities_DatabaseUtil_service
	      ${ART_UTILITIES}
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      ${CETLIB}
              
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
              
)

simple_plugin(ComputePi "module"
              ${ART_FRAMEWORK_CORE}
              ${MF_MESSAGELOGGER}
              ${MF_UTILITIES}
              ${FHICLCPP}
              ${CETLIB}
              
)

install_headers()
install_fhicl()
install_source()

add_subdirectory(test)

