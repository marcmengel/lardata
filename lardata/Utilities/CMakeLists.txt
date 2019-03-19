art_make(NO_PLUGINS
         LIB_LIBRARIES lardata_Utilities_LArFFT_service
                       lardataobj_RecoBase
                       larcorealg_Geometry
                       canvas
                       ${MF_MESSAGELOGGER}
                       cetlib_except)

simple_plugin(DatabaseUtil "service"
              ${MF_MESSAGELOGGER}
              ${PQ}
              ${ART_FRAMEWORK_CORE}
              ${ART_FRAMEWORK_PRINCIPAL}
              ${ART_FRAMEWORK_IO_ROOT_ROOTDB}
              art_Persistency_Provenance)

simple_plugin(FileCatalogMetadataExtras "service"
              ${ART_FRAMEWORK_IO_ROOT_ROOTDB}
              ${ART_FRAMEWORK_CORE}
              ${ART_FRAMEWORK_PRINCIPAL}
              ${ART_FRAMEWORK_SERVICES_SYSTEM_FILECATALOGMETADATA_SERVICE}
              ${SQLITE3}
              ${MF_MESSAGELOGGER}
              ${ROOT_BASIC_LIB_LIST})

simple_plugin(LArPropertiesServiceArgoNeuT "service"
              lardata_Utilities_DatabaseUtil_service
              ${MF_MESSAGELOGGER})

simple_plugin(DetectorPropertiesServiceArgoNeuT "service"
              lardata_Utilities_LArPropertiesServiceArgoNeuT_service
              lardata_Utilities_DatabaseUtil_service
              larcore_Geometry_Geometry_service
              larcorealg_Geometry
              ${ART_FRAMEWORK_IO_ROOT_ROOTDB}
              ${SQLITE3}
              ${MF_MESSAGELOGGER}
              ${ROOT_CORE}
              ${ROOT_RIO})

simple_plugin(LArFFT "service"
              ${MF_MESSAGELOGGER}
              ${ROOT_FFTW}
              ${ROOT_BASIC_LIB_LIST})

simple_plugin(ComputePi "module"
              ${MF_MESSAGELOGGER})

install_headers()
install_fhicl()
install_source()

add_subdirectory(test)
