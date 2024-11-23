/****************************************************************************
 *
 *   @copyright Copyright (c) 2024 Dave Hylands     <dhylands@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the MIT License version as described in the
 *   LICENSE file in the root of this repository.
 *
 ****************************************************************************/
/**
 *   @file   DuinoEsp32LittleFS.ino
 *
 *   @brief  Sample program for copy files to/from LittleFS
 *
 ****************************************************************************/

#include <Arduino.h>

#include "ArduinoSerialBus.h"
#include "CorePacketHandler.h"
#include "duino_led.h"
#include "duino_util.h"
#include "FS.h"
#include "LittleFS.h"
#include "LittleFsPacketHandler.h"
#include "Log.h"

#if !defined(LED_BUILTIN)
#error No definition for LED_BUILTIN
#endif

//  You only need to format LittleFS the first time you run a
//  test or else use the LITTLEFS plugin to create a partition
//  https://github.com/lorol/arduino-esp32littlefs-plugin

#define FORMAT_LITTLEFS_IF_FAILED true

static ArduinoLed led{LED_BUILTIN, ArduinoLed::Active::HIGH};

static TimedActionSequence::Action heartbeat_list[] = {
    {std::bind(&Led::toggle, led), 100},
    {std::bind(&Led::toggle, led), 100},
    {std::bind(&Led::toggle, led), 100},
    {std::bind(&Led::toggle, led), 700},
};
static TimedActionSequence heartbeat{heartbeat_list, LEN(heartbeat_list)};

static TimedActionSequence::Action activity_list[] = {
    {std::bind(&Led::toggle, led), 20},
    {std::bind(&Led::toggle, led), 20},
};
static TimedActionSequence activity{
    activity_list, LEN(activity_list), TimedActionSequence::Mode::ONE_SHOT};

static uint8_t cmdPacketData[256];
static uint8_t rspPacketData[256];
static Packet cmdPacket(LEN(cmdPacketData), cmdPacketData);
static Packet rspPacket(LEN(rspPacketData), rspPacketData);

static ArduinoSerialBus serialBus(&Serial, &cmdPacket, &rspPacket);

// The core packet handler deals with PING requests
static CorePacketHandler corePacketHandler;

static LittleFsPacketHandler littleFsPacketHandler;

void setup() {
    Serial.begin(115200);
    led.init();
    heartbeat.init();
    activity.init();
    serialBus.add(corePacketHandler);

    if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
        Log::error("LittleFS Mount Failed");
        return;
    }

    serialBus.add(littleFsPacketHandler);
}

uint32_t lastMillis = 0;

void loop() {
    if (serialBus.processByte() == Packet::Error::NONE) {
        activity.start();
        serialBus.handlePacket();
    }
    heartbeat.run();
    activity.run();
}
