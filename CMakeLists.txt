if(IDF_TARGET STREQUAL "esp32" OR IDF_TARGET STREQUAL "esp32s2" OR IDF_TARGET STREQUAL "esp32s3")
  set(COMPONENT_SRCS
   # ble/ble_main.c
   # ble/gatt_svr.c
   # ble/misc.c
   # ble/scli.c
    webserver.c
    base64.c
    adc_demo.c
    key.c
    http_demo.c
    socket.c
    driver/esp_camera.c
    driver/cam_hal.c
    driver/sccb.c
    driver/sensor.c
    sensors/ov2640.c
    sensors/ov3660.c
    sensors/ov5640.c
    sensors/ov7725.c
    sensors/ov7670.c
    sensors/nt99141.c
    sensors/gc0308.c
    sensors/gc2145.c
    sensors/gc032a.c
    sensors/esp32_ov5640_af.c
    conversions/yuv.c
    conversions/to_jpg.cpp
    conversions/to_bmp.c
    conversions/jpge.cpp
    conversions/esp_jpg_decode.c
    )

  set(COMPONENT_ADD_INCLUDEDIRS
    driver/include
    conversions/include
    ble
    )

  set(COMPONENT_PRIV_INCLUDEDIRS
    driver/private_include
    sensors/private_include
    conversions/private_include
    target/private_include
    )

  if(IDF_TARGET STREQUAL "esp32")
    list(APPEND COMPONENT_SRCS
      target/xclk.c
      target/esp32/ll_cam.c
      )
  endif()

  if(IDF_TARGET STREQUAL "esp32s2")
    list(APPEND COMPONENT_SRCS
      target/xclk.c
      target/esp32s2/ll_cam.c
      target/esp32s2/tjpgd.c
      )

    list(APPEND COMPONENT_PRIV_INCLUDEDIRS
      target/esp32s2/private_include
      )
  endif()

  if(IDF_TARGET STREQUAL "esp32s3")
    list(APPEND COMPONENT_SRCS
      target/esp32s3/ll_cam.c
      )
  endif()

  set(COMPONENT_REQUIRES driver esp_adc_cal esp_http_client esp-tls json console bt esp_http_server)
  set(COMPONENT_PRIV_REQUIRES freertos nvs_flash)


  register_component()
endif()
