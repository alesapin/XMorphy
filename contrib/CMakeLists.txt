set(catboost_model_lib ${CMAKE_CURRENT_SOURCE_DIR}/catboost/catboost/libs/model_interface/libcatboostmodel.so PARENT_SCOPE)
set(catboost_model_headers ${CMAKE_CURRENT_SOURCE_DIR}/catboost PARENT_SCOPE)
add_custom_target(catboost_build ALL DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/catboost/catboost/libs/model_interface/libcatboostmodel.so)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/catboost/catboost/libs/model_interface/libcatboostmodel.so
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/catboost/ya make -r ${CMAKE_CURRENT_SOURCE_DIR}/catboost/catboost/libs/model_interface)
