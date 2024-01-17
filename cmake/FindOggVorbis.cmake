find_package(PkgConfig)

# -logg -lvorbis -lvorbisfile
# vorbis/vorbisfile.h

find_path(OGG_INCLUDE_DIR
  "ogg/config_types.h"
  "ogg/ogg.h"
  "ogg/os_types.h"
  PATH_SUFFIXES "ogg")
find_library(OGG_LIBRARY NAMES ogg libogg)

find_path(VORBIS_INCLUDE_DIR
  "vorbis/codec.h"
  "vorbis/vorbisenc.h"
  PATH_SUFFIXES "vorbis")
find_library(VORBIS_LIBRARY NAMES vorbis libvorbis)

find_path(VORBISFILE_INCLUDE_DIR
  "vorbis/vorbisfile.h"
  PATH_SUFFIXES "vorbis")
find_library(VORBISFILE_LIBRARY NAMES vorbisfile libvorbisfile)

set(OGGVORBIS_LIBRARIES ${OGG_LIBRARY} ${VORBIS_LIBRARY} ${VORBISFILE_LIBRARY})
set(OGGVORBIS_INCLUDE_DIRS ${OGG_INCLUDE_DIR} ${VORBIS_INCLUDE_DIR} ${VORBISFILE_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OggVorbis DEFAULT_MSG
  OGGVORBIS_LIBRARIES OGGVORBIS_INCLUDE_DIRS)

mark_as_advanced(OGGVORBIS_INCLUDE_DIRS OGGVORBIS_LIBRARIES)

