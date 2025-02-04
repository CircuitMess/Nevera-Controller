file(GLOB_RECURSE LIBS "../lib/*/src/**.cpp" "../lib/*/src/**.c")
set(LIBS_INCL "")

idf_component_register(SRCS "main.cpp" ${LIBS} INCLUDE_DIRS "" ${LIBS_INCL})
