art_make(NO_PLUGINS
         EXCLUDE LArRawInputDriverUBooNE.cxx
         LIB_LIBRARIES lardataobj_RawData
                       larcorealg_Geometry
                       larcoreobj_SummaryData
                       ${ART_FRAMEWORK_CORE}
                       ${ART_FRAMEWORK_IO_SOURCES}
                       ${ART_FRAMEWORK_PRINCIPAL}
                       art_Persistency_Common
                       art_Persistency_Provenance
                       canvas
                       cetlib_except
                       ${PQ}
                       ${Boost_SERIALIZATION_LIBRARY}
                       ${Boost_DATE_TIME_LIBRARY}
                       ROOT::Core
                       ROOT::RIO
                       ROOT::Tree
        )

simple_plugin(LArRawInputSource "source"
  lardataobj_RawData
  lardata_RawData_utils
  ${ART_FRAMEWORK_IO_SOURCES}
)

install_headers()
install_fhicl()
install_source()
