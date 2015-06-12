
art_make( NO_PLUGINS
          LIBRARY_NAME  RawData_utils
	  EXCLUDE LArRawInputDriverUBooNE.cxx
          LIB_LIBRARIES RawData
                        SummaryData
                        ${ART_FRAMEWORK_CORE}
                        ${ART_FRAMEWORK_IO_SOURCES}
			${ART_FRAMEWORK_PRINCIPAL}
                        ${ART_FRAMEWORK_SERVICES_REGISTRY}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
 			${ART_PERSISTENCY_COMMON}
			${ART_PERSISTENCY_PROVENANCE}
			${ART_UTILITIES}
			${MF_MESSAGELOGGER}
			${MF_UTILITIES}
			${CETLIB}
                        ${Boost_SYSTEM_LIBRARY}
           	        ${PQ}
                        ${BOOST_SERIALIZATION}
                        ${BOOST_DATE_TIME}
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
  RawData
  RawData_utils
  ${ART_FRAMEWORK_CORE}
  ${ART_FRAMEWORK_IO_SOURCES}
  ${ART_FRAMEWORK_PRINCIPAL}
  ${ART_PERSISTENCY_PROVENANCE}
  ${ART_UTILITIES}
  ${MF_MESSAGELOGGER}
  BASENAME_ONLY
)

install_headers()
install_fhicl()
install_source()

