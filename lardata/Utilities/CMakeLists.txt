
art_make( NO_PLUGINS
          LIB_LIBRARIES lardata_Utilities_LArFFT_service
                        lardata_RecoBase
                        larcore_Geometry
                        ${ART_FRAMEWORK_CORE}
                        ${ART_FRAMEWORK_PRINCIPAL}
                        ${ART_FRAMEWORK_SERVICES_REGISTRY}
                        art_Persistency_Common canvas_Persistency_Common
                        art_Persistency_Provenance canvas_Persistency_Provenance
                        art_Utilities canvas_Utilities
                        ${MF_MESSAGELOGGER}
                        ${MF_UTILITIES}
                        ${CETLIB}
                        ${ROOT_BASIC_LIB_LIST}
        )

#simple_plugin(TimeService "service"
#	      lardata_RawData
#             lardata_Utilities_DatabaseUtil_service
# 	      art_Utilities canvas_Utilities
#	      ${ART_FRAMEWORK_CORE}
# 	      ${ART_FRAMEWORK_PRINCIPAL}
# 	      ${ART_PERSISTENCY_ROOTDB}
# 	      art_Persistency_Common canvas_Persistency_Common
# 	      art_Persistency_Provenance canvas_Persistency_Provenance
# 	      ${ART_FRAMEWORK_SERVICES_REGISTRY}
# 	      ${MF_MESSAGELOGGER}
# 	      ${MF_UTILITIES}
# 	      ${FHICLCPP}
# 	      ${CETLIB}
# 	      ${SQLITE3}
# 	      ${ROOT_BASIC_LIB_LIST}
#              
#)

simple_plugin(DatabaseUtil "service"
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      ${FHICLCPP}
	      ${CETLIB}
	      ${PQ}
	      art_Utilities canvas_Utilities
              ${ART_FRAMEWORK_CORE}
              ${ART_FRAMEWORK_PRINCIPAL}
              ${ART_PERSISTENCY_ROOTDB}
              art_Persistency_Common canvas_Persistency_Common
              art_Persistency_Provenance canvas_Persistency_Provenance
              ${ART_FRAMEWORK_SERVICES_REGISTRY}
)
simple_plugin(FileCatalogMetadataExtras "service"
	      art_Utilities canvas_Utilities
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

simple_plugin(LArPropertiesServiceArgoNeuT "service"
              lardata_Utilities_DatabaseUtil_service
              art_Utilities canvas_Utilities
              ${MF_MESSAGELOGGER}
              ${MF_UTILITIES}
              ${CETLIB}
              
)

simple_plugin(DetectorPropertiesServiceArgoNeuT "service"
              lardata_Utilities_LArPropertiesServiceArgoNeuT_service
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

simple_plugin(LArFFT "service"
	      art_Utilities canvas_Utilities
	      ${MF_MESSAGELOGGER}
	      ${MF_UTILITIES}
	      ${FHICLCPP}
	      ${CETLIB}
	      ${ROOT_FFTW}
	      ${ROOT_BASIC_LIB_LIST}
              
)

simple_plugin(MemoryPeakReporter "service"
              art_Utilities canvas_Utilities
              ${ART_FRAMEWORK_SERVICES_REGISTRY}
              ${ART_FRAMEWORK_PRINCIPAL}
              art_Persistency_Provenance canvas_Persistency_Provenance
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

