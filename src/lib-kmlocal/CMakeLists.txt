file(
  GLOB_RECURSE
  source_files
  ./*
  )

add_library(
  kmlocal
  SHARED
  ${source_files}
  )
