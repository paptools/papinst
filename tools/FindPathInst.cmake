message(STATUS "Finding PathInst")

set(PathInst_DIR "/Users/ird/dev/github/iandinwoodie/pathinst/build")
add_library(pathinst SHARED IMPORTED)
set_target_properties(pathinst
  PROPERTIES
    IMPORTED_LOCATION "${PathInst_DIR}/lib/libpathinst.a"
)

function(target_enable_pathinst target)
  set_target_properties(${target}
    PROPERTIES
      CXX_COMPILER_LAUNCHER "${PathInst_DIR}/bin/pcc"
  )

  target_link_libraries(${target}
    PRIVATE
      pathinst
  )
endfunction()
