# Check ESP-IDF version and error out if it is not in the supported range.
#
# Note for arduino-esp32 developers: to bypass the version check locally,
# set ARDUINO_SKIP_IDF_VERSION_CHECK environment variable to 1. For example:
#   export ARDUINO_SKIP_IDF_VERSION_CHECK=1
#   idf.py build

set(min_supported_idf_version "5.3.0")
set(max_supported_idf_version "5.5.99")
set(idf_version "${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}.${IDF_VERSION_PATCH}")

if ("${idf_version}" AND NOT "$ENV{ARDUINO_SKIP_IDF_VERSION_CHECK}")
  if (idf_version VERSION_LESS min_supported_idf_version)
    message(FATAL_ERROR "Arduino-esp32 can be used with ESP-IDF versions "
                        "between ${min_supported_idf_version} and ${max_supported_idf_version}, "
                        "but an older version is detected: ${idf_version}.")
  endif()
  if (idf_version VERSION_GREATER max_supported_idf_version)
    message(FATAL_ERROR "Arduino-esp32 can be used with ESP-IDF versions "
                        "between ${min_supported_idf_version} and ${max_supported_idf_version}, "
                        "but a newer version is detected: ${idf_version}.")
  endif()
endif()

set(CORE_SRCS
  cores/esp32/base64.cpp
  cores/esp32/cbuf.cpp
  cores/esp32/ColorFormat.c
  cores/esp32/esp32-hal-adc.c
  cores/esp32/esp32-hal-bt.c
  cores/esp32/esp32-hal-cpu.c
  cores/esp32/esp32-hal-dac.c
  cores/esp32/esp32-hal-gpio.c
  cores/esp32/esp32-hal-i2c.c
  cores/esp32/esp32-hal-i2c-ng.c
  cores/esp32/esp32-hal-i2c-slave.c
  cores/esp32/esp32-hal-ledc.c
  cores/esp32/esp32-hal-matrix.c
  cores/esp32/esp32-hal-misc.c
  cores/esp32/esp32-hal-periman.c
  cores/esp32/esp32-hal-psram.c
  cores/esp32/esp32-hal-rgb-led.c
  cores/esp32/esp32-hal-sigmadelta.c
  cores/esp32/esp32-hal-spi.c
  cores/esp32/esp32-hal-time.c
  cores/esp32/esp32-hal-timer.c 
  cores/esp32/esp32-hal-touch.c
  cores/esp32/esp32-hal-touch-ng.c
  cores/esp32/esp32-hal-uart.c
  cores/esp32/esp32-hal-rmt.c
  cores/esp32/Esp.cpp
  cores/esp32/freertos_stats.cpp
  cores/esp32/FunctionalInterrupt.cpp
  cores/esp32/HardwareSerial.cpp
  cores/esp32/HEXBuilder.cpp
  cores/esp32/IPAddress.cpp
  cores/esp32/libb64/cdecode.c
  cores/esp32/libb64/cencode.c
  cores/esp32/MacAddress.cpp
  cores/esp32/main.cpp
  cores/esp32/MD5Builder.cpp
  cores/esp32/Print.cpp
  cores/esp32/SHA1Builder.cpp
  cores/esp32/stdlib_noniso.c
  cores/esp32/Stream.cpp
  cores/esp32/StreamString.cpp
  cores/esp32/Tone.cpp
  cores/esp32/HWCDC.cpp
  cores/esp32/FirmwareMSC.cpp
  cores/esp32/firmware_msc_fat.c
  cores/esp32/wiring_pulse.c
  cores/esp32/wiring_shift.c
  cores/esp32/WMath.cpp
  cores/esp32/WString.cpp
  )
if(IDF_TARGET MATCHES "esp32s2|esp32s3|esp32p4" AND CONFIG_TINYUSB_ENABLED)
  list(APPEND CORE_SRCS
    cores/esp32/esp32-hal-tinyusb.c
    cores/esp32/USB.cpp
    cores/esp32/USBCDC.cpp
    cores/esp32/USBMSC.cpp)
endif()

set(ARDUINO_ALL_LIBRARIES
  ArduinoOTA
  AsyncUDP
  DNSServer
  EEPROM
  ESPmDNS
  Ethernet
  FFat
  FS
  HTTPClient
  HTTPUpdate
  LittleFS
  NetBIOS
  Network
  PPP
  Preferences
  SD_MMC
  SD
  SPI
  Ticker
  Update
  WebServer
  WiFi
  Wire
  )
if(IDF_TARGET MATCHES "esp32s2|esp32s3|esp32p4" AND CONFIG_TINYUSB_ENABLED)
  list(APPEND ARDUINO_ALL_LIBRARIES USB)
endif()

set(ARDUINO_LIBRARY_ArduinoOTA_SRCS libraries/ArduinoOTA/src/ArduinoOTA.cpp)

set(ARDUINO_LIBRARY_AsyncUDP_SRCS libraries/AsyncUDP/src/AsyncUDP.cpp)

set(ARDUINO_LIBRARY_DNSServer_SRCS libraries/DNSServer/src/DNSServer.cpp)

set(ARDUINO_LIBRARY_EEPROM_SRCS libraries/EEPROM/src/EEPROM.cpp)

set(ARDUINO_LIBRARY_ESPmDNS_SRCS libraries/ESPmDNS/src/ESPmDNS.cpp)

set(ARDUINO_LIBRARY_Ethernet_SRCS libraries/Ethernet/src/ETH.cpp)

set(ARDUINO_LIBRARY_FFat_SRCS libraries/FFat/src/FFat.cpp)

set(ARDUINO_LIBRARY_FS_SRCS
  libraries/FS/src/FS.cpp
  libraries/FS/src/vfs_api.cpp)

set(ARDUINO_LIBRARY_HTTPClient_SRCS libraries/HTTPClient/src/HTTPClient.cpp)

set(ARDUINO_LIBRARY_HTTPUpdate_SRCS libraries/HTTPUpdate/src/HTTPUpdate.cpp)

set(ARDUINO_LIBRARY_LittleFS_SRCS libraries/LittleFS/src/LittleFS.cpp)

set(ARDUINO_LIBRARY_NetBIOS_SRCS libraries/NetBIOS/src/NetBIOS.cpp)

set(ARDUINO_LIBRARY_PPP_SRCS
  libraries/PPP/src/PPP.cpp
  libraries/PPP/src/ppp.c)

set(ARDUINO_LIBRARY_Preferences_SRCS libraries/Preferences/src/Preferences.cpp)

set(ARDUINO_LIBRARY_SD_MMC_SRCS libraries/SD_MMC/src/SD_MMC.cpp)

set(ARDUINO_LIBRARY_SD_SRCS
  libraries/SD/src/SD.cpp
  libraries/SD/src/sd_diskio.cpp
  libraries/SD/src/sd_diskio_crc.c)

set(ARDUINO_LIBRARY_SPI_SRCS libraries/SPI/src/SPI.cpp)

set(ARDUINO_LIBRARY_Ticker_SRCS libraries/Ticker/src/Ticker.cpp)

set(ARDUINO_LIBRARY_Update_SRCS
  libraries/Update/src/Updater.cpp
  libraries/Update/src/HttpsOTAUpdate.cpp)

if(IDF_TARGET MATCHES "esp32s2|esp32s3|esp32p4" AND CONFIG_TINYUSB_ENABLED)
  set(ARDUINO_LIBRARY_USB_SRCS
    libraries/USB/src/USBHID.cpp
    libraries/USB/src/USBMIDI.cpp
    libraries/USB/src/USBHIDMouse.cpp
    libraries/USB/src/USBHIDKeyboard.cpp
    libraries/USB/src/keyboardLayout/KeyboardLayout_da_DK.cpp
    libraries/USB/src/keyboardLayout/KeyboardLayout_de_DE.cpp
    libraries/USB/src/keyboardLayout/KeyboardLayout_en_US.cpp
    libraries/USB/src/keyboardLayout/KeyboardLayout_es_ES.cpp
    libraries/USB/src/keyboardLayout/KeyboardLayout_fr_FR.cpp
    libraries/USB/src/keyboardLayout/KeyboardLayout_hu_HU.cpp
    libraries/USB/src/keyboardLayout/KeyboardLayout_it_IT.cpp
    libraries/USB/src/keyboardLayout/KeyboardLayout_pt_BR.cpp
    libraries/USB/src/keyboardLayout/KeyboardLayout_pt_PT.cpp
    libraries/USB/src/keyboardLayout/KeyboardLayout_sv_SE.cpp
    libraries/USB/src/USBHIDGamepad.cpp
    libraries/USB/src/USBHIDConsumerControl.cpp
    libraries/USB/src/USBHIDSystemControl.cpp
    libraries/USB/src/USBHIDVendor.cpp
    libraries/USB/src/USBVendor.cpp)
endif()

set(ARDUINO_LIBRARY_WebServer_SRCS
  libraries/WebServer/src/WebServer.cpp
  libraries/WebServer/src/Parsing.cpp
  libraries/WebServer/src/detail/mimetable.cpp
  libraries/WebServer/src/middleware/MiddlewareChain.cpp
  libraries/WebServer/src/middleware/AuthenticationMiddleware.cpp
  libraries/WebServer/src/middleware/CorsMiddleware.cpp
  libraries/WebServer/src/middleware/LoggingMiddleware.cpp)

set(ARDUINO_LIBRARY_Network_SRCS
  libraries/Network/src/NetworkInterface.cpp
  libraries/Network/src/NetworkEvents.cpp
  libraries/Network/src/NetworkManager.cpp
  libraries/Network/src/NetworkClient.cpp
  libraries/Network/src/NetworkServer.cpp
  libraries/Network/src/NetworkUdp.cpp)

set(ARDUINO_LIBRARY_WiFi_SRCS
  libraries/WiFi/src/WiFiAP.cpp
  libraries/WiFi/src/WiFi.cpp
  libraries/WiFi/src/WiFiGeneric.cpp
  libraries/WiFi/src/WiFiMulti.cpp
  libraries/WiFi/src/WiFiScan.cpp
  libraries/WiFi/src/WiFiSTA.cpp
  libraries/WiFi/src/STA.cpp
  libraries/WiFi/src/AP.cpp)

set(ARDUINO_LIBRARY_Wire_SRCS libraries/Wire/src/Wire.cpp)

set(ARDUINO_LIBRARIES_SRCS)
set(ARDUINO_LIBRARIES_REQUIRES)
set(ARDUINO_LIBRARIES_INCLUDEDIRS)
foreach(libname IN LISTS ARDUINO_ALL_LIBRARIES)
  if(NOT CONFIG_ARDUINO_SELECTIVE_COMPILATION OR CONFIG_ARDUINO_SELECTIVE_${libname})
    if(ARDUINO_LIBRARY_${libname}_SRCS)
      list(APPEND ARDUINO_LIBRARIES_SRCS ${ARDUINO_LIBRARY_${libname}_SRCS})
    endif()
    if(ARDUINO_LIBRARY_${libname}_REQUIRES)
      list(APPEND ARDUINO_LIBRARIES_REQUIRES ${ARDUINO_LIBRARY_${libname}_REQUIRES})
    endif()
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/libraries/${libname}/src)
      list(APPEND ARDUINO_LIBRARIES_INCLUDEDIRS libraries/${libname}/src)
    endif()
  endif()
endforeach()

set(includedirs variants/${CONFIG_ARDUINO_VARIANT}/ cores/esp32/ ${ARDUINO_LIBRARIES_INCLUDEDIRS})
set(srcs ${CORE_SRCS} ${ARDUINO_LIBRARIES_SRCS})
set(priv_includes cores/esp32/libb64)
set(requires spi_flash esp_partition mbedtls wpa_supplicant esp_adc esp_eth http_parser esp_ringbuf esp_driver_gptimer esp_driver_usb_serial_jtag driver esp_http_client esp_https_ota)
set(priv_requires fatfs nvs_flash app_update bootloader_support bt esp_hid usb esp_psram ${ARDUINO_LIBRARIES_REQUIRES})

if(IDF_TARGET STREQUAL "esp32p4")
  list(APPEND requires esp_driver_touch_sens)
endif()

idf_component_register(INCLUDE_DIRS ${includedirs} PRIV_INCLUDE_DIRS ${priv_includes} SRCS ${srcs} REQUIRES ${requires} PRIV_REQUIRES ${priv_requires})

if(NOT CONFIG_FREERTOS_HZ EQUAL 1000 AND NOT "$ENV{ARDUINO_SKIP_TICK_CHECK}")
    # See delay() in cores/esp32/esp32-hal-misc.c.
    message(FATAL_ERROR "esp32-arduino requires CONFIG_FREERTOS_HZ=1000 "
                        "(currently ${CONFIG_FREERTOS_HZ})")
endif()

string(TOUPPER ${CONFIG_ARDUINO_VARIANT} idf_target_caps)
string(REPLACE "-" "_" idf_target_for_macro "${idf_target_caps}")
target_compile_options(${COMPONENT_TARGET} PUBLIC
    -DARDUINO=10812
    -DARDUINO_${idf_target_for_macro}_DEV
    -DARDUINO_ARCH_ESP32
    -DARDUINO_BOARD="${idf_target_caps}_DEV"
    -DARDUINO_VARIANT="${CONFIG_ARDUINO_VARIANT}"
    -DESP32=ESP32)

if(CONFIG_AUTOSTART_ARDUINO)
    # in autostart mode, arduino-esp32 contains app_main() function and needs to
    # reference setup() and loop() in the main component. If we add main
    # component to priv_requires then we create a large circular dependency
    # (arduino-esp32 -> main -> arduino-esp32) and can get linker errors, so
    # instead we add setup() and loop() to the undefined symbols list so the
    # linker will always include them.
    #
    # (As they are C++ symbol, we need to add the C++ mangled names.)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u _Z5setupv -u _Z4loopv")
endif()

# This function adds a dependency on the given component if the component is included into the build.
function(maybe_add_component component_name)
    idf_build_get_property(components BUILD_COMPONENTS)
    if (${component_name} IN_LIST components)
        idf_component_get_property(lib_name ${component_name} COMPONENT_LIB)
        target_link_libraries(${COMPONENT_LIB} PUBLIC ${lib_name})
    endif()
endfunction()

if(IDF_TARGET MATCHES "esp32s2|esp32s3|esp32p4" AND CONFIG_TINYUSB_ENABLED)
    maybe_add_component(arduino_tinyusb)
endif()
if(NOT CONFIG_ARDUINO_SELECTIVE_COMPILATION OR CONFIG_ARDUINO_SELECTIVE_ArduinoOTA)
    maybe_add_component(esp_https_ota)
endif()
if(NOT CONFIG_ARDUINO_SELECTIVE_COMPILATION OR CONFIG_ARDUINO_SELECTIVE_LittleFS)
    maybe_add_component(joltwallet__littlefs)
endif()
