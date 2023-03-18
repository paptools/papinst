message(STATUS "Finding paptools")

set(paptools_DIR "/Users/ird/dev/github/iandinwoodie/paptools/build")
add_library(paptools::pathtrace SHARED IMPORTED)
set_target_properties(paptools::pathtrace
  PROPERTIES
    IMPORTED_LOCATION "${paptools_DIR}/lib/libpathtrace.a"
)

function(target_enable_pathtrace target)
  set_target_properties(${target}
    PROPERTIES
      CXX_COMPILER_LAUNCHER "${paptools_DIR}/bin/pcc"
  )

  target_link_libraries(${target}
    PRIVATE
      paptools::pathtrace
  )
endfunction()
