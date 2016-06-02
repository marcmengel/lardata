
art_make( NO_PLUGINS
          
	  EXCLUDE LArRawInputDriverUBooNE.cxx
          LIB_LIBRARIES lardataobj_RawData
                        larcoreobj_SummaryData
                        ${ART_FRAMEWORK_CORE}
                        ${ART_FRAMEWORK_IO_SOURCES}
			${ART_FRAMEWORK_PRINCIPAL}
                        ${ART_FRAMEWORK_SERVICES_REGISTRY}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
 			art_Persistency_Common canvas_Persistency_Common
			art_Persistency_Provenance canvas_Persistency_Provenance
			art_Utilities canvas_Utilities
			${MF_MESSAGELOGGER}
			${MF_UTILITIES}
			${CETLIB}
                        ${Boost_SYSTEM_LIBRARY}
           	        ${PQ}
                        ${Boost_SERIALIZATION_LIBRARY}
                        ${Boost_DATE_TIME_LIBRARY}
	                ${ROOT_CORE}
                        ${ROOT_CINT} 
                        ${ROOT_RIO}
                        ${ROOT_NET}
                        ${ROOT_HIST} 
                        ${ROOT_GEOM}
                        ${ROOT_XMLIO}
                        ${ROOT_GDML}
                        ${ROOT_GRAF}
                        ${ROOT_GRAF3D}
                        ${ROOT_GPAD}
                        ${ROOT_TREE}
                        ${ROOT_RINT}
                        ${ROOT_POSTSCRIPT}
                        ${ROOT_MATRIX}
                        ${ROOT_PHYSICS}
                        ${ROOT_MATHCORE}
                        ${ROOT_THREAD}
                        ${ROOT_EG}
        )

simple_plugin(LArRawInputSource "source"
  lardataobj_RawData
  lardata_RawData_utils
  ${ART_FRAMEWORK_CORE}
  ${ART_FRAMEWORK_IO_SOURCES}
  ${ART_FRAMEWORK_PRINCIPAL}
  art_Persistency_Provenance canvas_Persistency_Provenance
  art_Utilities canvas_Utilities
  ${MF_MESSAGELOGGER}
  
)

install_headers()
install_fhicl()
install_source()

