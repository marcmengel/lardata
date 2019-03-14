simple_plugin(DetectorClocksServiceStandard "service"
              lardataalg_DetectorInfo
              lardataobj_RawData
              ${ART_FRAMEWORK_CORE}
              ${ART_FRAMEWORK_PRINCIPAL}
              ${ART_FRAMEWORK_IO_ROOT_ROOTDB}
              art_Persistency_Provenance
              ${SQLITE3})

simple_plugin(DetectorPropertiesServiceStandard "service"
              lardataalg_DetectorInfo
              larcore_Geometry_Geometry_service
              larcorealg_Geometry
              ${ART_FRAMEWORK_IO_ROOT_ROOTDB}
              ${SQLITE3}
              ${MF_MESSAGELOGGER})

simple_plugin(LArPropertiesServiceStandard "service"
              lardataalg_DetectorInfo
              art_Framework_Principal)

install_headers()
install_fhicl()
install_source()
