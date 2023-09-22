add_subdirectory(ap)
add_subdirectory(ep)
add_subdirectory(msg_hand)
add_subdirectory(sm)
#add_subdirectory(e2sm_common_ie)

add_library(pending_events_obj OBJECT 
                                pending_events.c 
                                pending_event_ric.c
                                ind_event.c
                                $<TARGET_OBJECTS:e2ap_types_obj> 
                                $<TARGET_OBJECTS:e2ap_ap_obj>
                                )

