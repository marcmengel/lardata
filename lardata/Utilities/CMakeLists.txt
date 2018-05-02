
art_make( NO_PLUGINS
          LIB_LIBRARIES lardata_Utilities_LArFFT_service
                        lardataobj_RecoBase
                        larcorealg_Geometry
                        ${ART_FRAMEWORK_CORE}
                        ${ART_FRAMEWORK_PRINCIPAL}
                        ${ART_FRAMEWORK_SERVICES_REGISTRY}
                        art_Persistency_Common
                        art_Persistency_Provenance
                        art_Utilities
                        canvas
                        ${MF_MESSAGELOGGER}
                        
                        cetlib cetlib_except
                        ${ROOT_GEOM}
                        ${ROOT_BASIC_LIB_LIST}
        )

simple_plugin(DatabaseUtil "service"
	      ${MF_MESSAGELOGGER}
	      
	      ${FHICLCPP}
	      cetlib cetlib_except
	      ${PQ}
	      art_Utilities
              ${ART_FRAMEWORK_CORE}
              ${ART_FRAMEWORK_PRINCIPAL}
              ${ART_FRAMEWORK_IO_ROOT_ROOTDB}
              art_Persistency_Common
              art_Persistency_Provenance
              ${ART_FRAMEWORK_SERVICES_REGISTRY}
	      canvas
	      ${ROOT_CORE}
)
simple_plugin(FileCatalogMetadataExtras "service"
	      art_Utilities
	      ${ART_FRAMEWORK_IO_ROOT_ROOTDB}
	      ${ART_FRAMEWORK_CORE}
	      ${ART_FRAMEWORK_PRINCIPAL}
	      ${ART_FRAMEWORK_SERVICES_SYSTEM_FILECATALOGMETADATA_SERVICE}
	      canvas
	      ${SQLITE3}
	      ${MF_MESSAGELOGGER}
	      
	      cetlib cetlib_except
	      ${ROOT_BASIC_LIB_LIST}
)

simple_plugin(LArPropertiesServiceArgoNeuT "service"
              lardata_Utilities_DatabaseUtil_service
              art_Utilities
	      canvas
              ${MF_MESSAGELOGGER}
              
              cetlib cetlib_except
	      ${ROOT_CORE}
)

simple_plugin(DetectorPropertiesServiceArgoNeuT "service"
              lardata_Utilities_LArPropertiesServiceArgoNeuT_service
              lardata_Utilities_DatabaseUtil_service
              larcore_Geometry_Geometry_service
              larcorealg_Geometry
              ${ART_FRAMEWORK_IO_ROOT_ROOTDB}
              ${SQLITE3}
              ${MF_MESSAGELOGGER}
              
              cetlib cetlib_except
              ${ROOT_BASIC_LIB_LIST}
)

simple_plugin(LArFFT "service"
	      art_Utilities
	      canvas
	      ${MF_MESSAGELOGGER}
	      
	      ${FHICLCPP}
	      cetlib cetlib_except
	      ${ROOT_FFTW}
	      ${ROOT_BASIC_LIB_LIST}
)

simple_plugin(ComputePi "module"
              ${ART_FRAMEWORK_CORE}
              ${MF_MESSAGELOGGER}
              
              ${FHICLCPP}
              cetlib cetlib_except
	      ${ROOT_CORE}
)

install_headers()
install_fhicl()
install_source()

add_subdirectory(test)

