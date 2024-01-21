find_package(PkgConfig)

find_path(OPENAL_INCLUDE_DIR
  "AL/al.h"
  "AL/alc.h"
  PATH_SUFFIXES "AL")

find_library(OPENAL_LIBRARY NAMES openal libopenal)

set(OPENAL_LIBRARIES ${OPENAL_LIBRARY})
set(OPENAL_INCLUDE_DIRS ${OPENAL_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenAL DEFAULT_MSG
  OPENAL_LIBRARY OPENAL_INCLUDE_DIR)

mark_as_advanced(OPENAL_INCLUDE_DIR OPENAL_LIBRARY)
