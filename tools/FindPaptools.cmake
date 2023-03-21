message(STATUS "Finding paptools")

set(paptools_DIR "/Users/ird/dev/github/iandinwoodie/paptools/build")
add_library(paptools::paptrace SHARED IMPORTED)
set_target_properties(paptools::paptrace
  PROPERTIES
    IMPORTED_LOCATION "${paptools_DIR}/lib/libpaptrace.a"
)

function(target_enable_paptrace target)
  set_target_properties(${target}
    PROPERTIES
      CXX_COMPILER_LAUNCHER "${paptools_DIR}/bin/pcc"
  )

  target_link_libraries(${target}
    PRIVATE
      paptools::paptrace
  )
endfunction()
