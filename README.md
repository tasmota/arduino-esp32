# Tasmota Platformio Arduino for ESP32, ESP32-S2, ESP32-S3, ESP32-C2, ESP32-C3, ESP32-C6 and ESP32-H2

### [![GitHub Releases](https://img.shields.io/github/downloads/tasmota/arduino-esp32/total?label=downloads)](https://github.com/tasmota/arduino-esp32/releases/latest)

### Documentation

You can use the [Arduino-ESP32 Online Documentation](https://docs.espressif.com/projects/arduino-esp32/en/latest/) to get all information about this project.

---

**Migration guide from version 2.x to 3.x is available [here](https://docs.espressif.com/projects/arduino-esp32/en/latest/migration_guides/2.x_to_3.0.html).**

---

### Supported Chips

Here are the ESP32 series supported by the Tasmota Arduino-ESP32 project:

| **SoC**  | **Stable** | **Development** |                                           **Datasheet**                                           |
|----------|:----------:|:---------------:|:-------------------------------------------------------------------------------------------------:|
| ESP32    |     Yes    |       Yes       | [ESP32](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf)    |
| ESP32solo1|    Yes    |       Yes       | [ESP32solo1](https://www.espressif.com/sites/default/files/documentation/esp32-solo-1_datasheet_en.pdf)    |
| ESP32-S2 |     Yes    |       Yes       | [ESP32-S2](https://www.espressif.com/sites/default/files/documentation/esp32-s2_datasheet_en.pdf) |
| ESP32-S3 |     Yes    |       Yes       | [ESP32-S3](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf) |
| ESP32-C3 |     Yes    |       Yes       | [ESP32-C3](https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf) |
| ESP32-C2 |     No     |       Yes       | [ESP32-C2](https://www.espressif.com/sites/default/files/documentation/esp8684_datasheet_en.pdf) |
| ESP32-C6 |     No     |       Yes       | [ESP32-C6](https://www.espressif.com/sites/default/files/documentation/esp32-c6_datasheet_en.pdf) |
| ESP32-H2 |     No     |       Yes       | [ESP32-H2](https://www.espressif.com/sites/default/files/documentation/esp32-h2_datasheet_en.pdf) |

> [!NOTE]
> ESP32-C2 is also supported by Arduino-ESP32 but requires rebuilding the static libraries. This is not trivial and requires a good understanding of the ESP-IDF
> build system. For more information, see the [Lib Builder documentation](https://docs.espressif.com/projects/arduino-esp32/en/latest/lib_builder.html).

For more details visit the [supported chips](https://docs.espressif.com/projects/arduino-esp32/en/latest/getting_started.html#supported-soc-s) documentation page.
