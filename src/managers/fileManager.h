#pragma once
#include <LittleFS.h>
#include "debugPrint.h"

// List all files in LittleFS
void listFiles() {
    Dir dir = LittleFS.openDir("/");
    DebugPrintln("Stored files:");
    while (dir.next()) {
        DebugPrintln("    " + dir.fileName() + " (" + String(dir.fileSize()) + " bytes)");
    }
}