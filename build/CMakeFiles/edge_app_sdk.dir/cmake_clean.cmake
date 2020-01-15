file(REMOVE_RECURSE
  "lib/libedge_app_sdk.pdb"
  "lib/libedge_app_sdk.a"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/edge_app_sdk.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
