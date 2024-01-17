find_package(PkgConfig)

find_path(TOMLPLUSPLUS_INCLUDE_DIR
  "toml++/toml.hpp"
  PATH_SUFFIXES "toml++")

find_library(TOMLPLUSPLUS_LIBRARY NAMES tomlplusplus libtomlplusplus)

set(TOMLPLUSPLUS_LIBRARIES ${TOMLPLUSPLUS_LIBRARY})
set(TOMLPLUSPLUS_INCLUDE_DIRS ${TOMLPLUSPLUS_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TomlPlusPlus DEFAULT_MSG
  TOMLPLUSPLUS_LIBRARY TOMLPLUSPLUS_INCLUDE_DIR)

mark_as_advanced(TOMLPLUSPLUS_INCLUDE_DIR TOMLPLUSPLUS_LIBRARY)

