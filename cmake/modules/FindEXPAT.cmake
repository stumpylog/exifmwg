# cmake/modules/FindEXPAT.cmake

# ——————————————————————————————————————————————————————————————
# Minimal module to satisfy find_package(EXPAT) for a FetchContent‐built expat
# ——————————————————————————————————————————————————————————————

if(TARGET expat)
  set(EXPAT_FOUND TRUE)
  get_target_property(_expat_includes expat INTERFACE_INCLUDE_DIRECTORIES)
  set(EXPAT_INCLUDE_DIRS ${_expat_includes})
  set(EXPAT_LIBRARIES expat)
  add_library(EXPAT::EXPAT ALIAS expat)
endif()
