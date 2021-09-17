// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

static constexpr const char LFS_NAME[] = "spiffs";

#include "vfs_api.h"

extern "C" {
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include "esp_littlefs.h"
}

#include "LittleFS.h"

using namespace fs;

LittleFSFS::LittleFSFS() : FS(FSImplPtr(new VFSImpl()))
{

}

bool LittleFSFS::begin(bool formatOnFail, const char * basePath, uint8_t maxOpenFiles)
{
    if(esp_littlefs_mounted(LFS_NAME)){
        log_w("LittleFS Already Mounted!");
        return true;
    }

    esp_vfs_littlefs_conf_t conf = {
      .base_path = basePath,
      .partition_label = LFS_NAME,
      .format_if_mount_failed = false
    };

    esp_err_t err = esp_vfs_littlefs_register(&conf);
    if(err == ESP_FAIL && formatOnFail){
        if(format()){
            err = esp_vfs_littlefs_register(&conf);
        }
    }
    if(err != ESP_OK){
        log_e("Mounting LittleFS failed! Error: %d", err);
        return false;
    }
    _impl->mountpoint(basePath);
    return true;
}

void LittleFSFS::end()
{
    if(esp_littlefs_mounted(LFS_NAME)){
        esp_err_t err = esp_vfs_littlefs_unregister(LFS_NAME);
        if(err){
            log_e("Unmounting LittleFS failed! Error: %d", err);
            return;
        }
        _impl->mountpoint(NULL);
    }
}

bool LittleFSFS::format()
{
    disableCore0WDT();
    esp_err_t err = esp_littlefs_format(LFS_NAME);
    enableCore0WDT();
    if(err){
        log_e("Formatting LittleFS failed! Error: %d", err);
        return false;
    }
    return true;
}

size_t LittleFSFS::totalBytes()
{
    size_t total,used;
    if(esp_littlefs_info(LFS_NAME, &total, &used)){
        return 0;
    }
    return total;
}

size_t LittleFSFS::usedBytes()
{
    size_t total,used;
    if(esp_littlefs_info(LFS_NAME, &total, &used)){
        return 0;
    }
    return used;
}

LittleFSFS LittleFS;

