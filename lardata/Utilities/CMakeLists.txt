include_directories(${FFTW_INCLUDE_DIR})
cet_find_library(FFTW_LIBRARY NAMES fftw3 fftw3-3 PATHS $ENV{FFTW_DIR}/$ENV{FFTW_FQ}/lib )
set(FFTW_LIBRARIES ${FFTW_LIBRARY})

art_make(NO_PLUGINS
         LIB_LIBRARIES lardata_Utilities_LArFFT_service
                       lardataobj_RecoBase
                       larcorealg_Geometry
                       larreco_HitFinder
                       canvas
                       ${MF_MESSAGELOGGER}
                       cetlib_except
                       ${FFTW_LIBRARIES})

simple_plugin(DatabaseUtil "service"
              ${MF_MESSAGELOGGER}
              ${PQ}
              ${ART_FRAMEWORK_CORE}
              ${ART_FRAMEWORK_PRINCIPAL}
              ${ART_ROOT_IO_ROOTDB}
              art_Persistency_Provenance)

simple_plugin(LArPropertiesServiceArgoNeuT "service"
              lardata_Utilities_DatabaseUtil_service
              ${MF_MESSAGELOGGER})

simple_plugin(DetectorPropertiesServiceArgoNeuT "service"
              lardata_Utilities_LArPropertiesServiceArgoNeuT_service
              lardata_Utilities_DatabaseUtil_service
              larcore_Geometry_Geometry_service
              larcorealg_Geometry
              ${ART_ROOT_IO_ROOTDB}
              ${SQLITE3}
              ${MF_MESSAGELOGGER}
              ${ROOT_CORE}
              ${ROOT_RIO})

simple_plugin(LArFFT "service"
              ${MF_MESSAGELOGGER}
              ${ROOT_CORE}
              ${ROOT_FFTW}
              ${ROOT_HIST}
              ${ROOT_MATHCORE}
              ${ROOT_PHYSICS})

simple_plugin(ComputePi "module"
              ${MF_MESSAGELOGGER})

install_headers()
install_fhicl()
install_source()

add_subdirectory(test)
