# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/main/*.*)

set(COMPONENT_ADD_INCLUDEDIRS "." "./includes" "./tflite/micro_features" "./tflite")
set(COMPONENT_REQUIRES 
    "nvs_flash" 
    "esp-aws-iot" 
    "esp-cryptoauthlib" 
    "core2forAWS" 
    "tfmicro" 
    # "esp_http_client" 
    # "esp_https_ota"
    # "ota-for-aws-iot-embedded-sdk"
    "app_update"
    # "antenna-sdk"
    # "sdmmc"
)
idf_component_register(SRCS ${app_sources})

# Custom Addition
target_add_binary_data(${COMPONENT_TARGET} "certs/aws-root-ca.pem" TEXT)
