/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "ESP_Network_Events.h"
#include "IPAddress.h"
#include "WString.h"

class ESP_Network_Manager : public ESP_Network_Events {
public:
	ESP_Network_Manager();

	bool begin();
	int hostByName(const char *aHostname, IPAddress &aResult, bool preferV6=false);
	uint8_t * macAddress(uint8_t * mac);
	String macAddress();
};

extern ESP_Network_Manager Network;
