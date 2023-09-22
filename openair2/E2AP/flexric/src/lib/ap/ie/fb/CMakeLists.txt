add_library(e2ap_fb_obj INTERFACE)
#Header files library
target_include_directories(e2ap_fb_obj INTERFACE .)
target_include_directories(e2ap_fb_obj INTERFACE ${FlatCC_INCLUDE_DIR})

