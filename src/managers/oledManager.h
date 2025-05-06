#ifndef OLEDMANAGER_H
#define OLEDMANAGER_H

#include <GyverOLED.h>
#include "GyverEncoder.h"
#include "handlers/settings.h"

#define ITEMS_PER_PAGE 5
extern GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled;
extern Encoder enc1;
extern String imy;
extern int ITEMS;
extern bool flag;

String newParamValues[] = { "по выбору", "по дням", "все профиля",};

void updateDisplay(String names[]) {
    oled.clear();
    oled.home();
    oled.setScale(1);
    for (int i = 0; i < ITEMS_PER_PAGE; i++) {
        int index = deviceSettings.currentPage * ITEMS_PER_PAGE + i;
        if (index < ITEMS - 1) {
            oled.print(F(" "));
            oled.print(names[index]);
            oled.println();
        } else if (index == ITEMS - 1) {
            oled.println(F(" режим работы"));
            oled.print(F(" "));
            oled.println(newParamValues[deviceSettings.workMode]);
            deviceSettings.workMode = deviceSettings.workMode;
            oled.println();
        }
    }
    if (flag) {
        oled.setCursor(0, (deviceSettings.selectedpoint % ITEMS_PER_PAGE));
        oled.print(F(">"));
    } else {
        oled.setCursor(123, (deviceSettings.selectedpoint % ITEMS_PER_PAGE));
        oled.print(F("<"));
    }
    oled.update();
}

void fullOled(String names[], int sizee) {
    enc1.tick();

    if (enc1.isRight()) {
        if (flag) {
            deviceSettings.selectedpoint = constrain(deviceSettings.selectedpoint - 1, 0, ITEMS - 1);
            if (deviceSettings.selectedpoint < deviceSettings.currentPage * ITEMS_PER_PAGE) {
                deviceSettings.currentPage = deviceSettings.selectedpoint / ITEMS_PER_PAGE;
            }
        } else {
            if (deviceSettings.selectedpoint == ITEMS - 1) {
                deviceSettings.workMode = constrain(deviceSettings.workMode - 1, 0, 2);
            }
        }
        updateDisplay(names);
        deviceSettings.selectedProfile = names[deviceSettings.selectedpoint];
        saveSettings();
    }

    if (enc1.isLeft()) {
        if (flag) {
            deviceSettings.selectedpoint = constrain(deviceSettings.selectedpoint + 1, 0, ITEMS - 1);
            if (deviceSettings.selectedpoint >= (deviceSettings.currentPage + 1) * ITEMS_PER_PAGE) {
                deviceSettings.currentPage = deviceSettings.selectedpoint / ITEMS_PER_PAGE;
            }
        } else {
            if (deviceSettings.selectedpoint == ITEMS - 1) {
                deviceSettings.workMode = constrain(deviceSettings.workMode + 1, 0, 2);
            }
        }
        updateDisplay(names);
        deviceSettings.selectedProfile = names[deviceSettings.selectedpoint];
        saveSettings();
    }

    if (enc1.isClick()) {
        flag = !flag;
        updateDisplay(names);
        deviceSettings.selectedProfile = names[deviceSettings.selectedpoint];
        saveSettings();
    }
    delayMicroseconds(10);
}

#endif // OLEDMANAGER_H
