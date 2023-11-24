#pragma once

// global
#define _WIN32_DCOM

#include <string>
#include <vector>
#include <future>
#include <windows.h>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <psapi.h>
#include <mscat.h>
#include <stdlib.h>
#include <SoftPub.h>
#include <atlbase.h>
#include <regex>
#include <sstream>
#include <TlHelp32.h>
#include <chrono>
#include <inttypes.h>
#include <stdio.h>
#include <Assert.h>
#include <intrin.h>
#include <cstdint>
#include <comdef.h>
#include <nmmintrin.h>
#include <signal.h>

#include <Wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")
#include <VersionHelpers.h>
#pragma comment(lib, "version.lib" )
#include <wintrust.h>
#pragma comment (lib, "wintrust")
#include <wincrypt.h>
#pragma comment(lib, "crypt32.lib")
#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#include "third-party/libcurl/include/curl.h"
#pragma comment (lib, "third-party/libcurl/lib/libcurl.lib")

// local
#include "third-party/XorString.h"
#include "third-party/CRC.h"
#include "third-party/lazy_importer.hpp"
#include "third-party/md5.h"
#include "third-party/mine.h"
#include "third-party/json.hpp"

#include "src/game/cleo_check.h"
#include "src/game/game.h"
#include "src/game/modules.h"
#include "src/game/sampfuncs.h"

#include "src/util/core.h"
#include "src/util/longpool.h"
#include "src/util/network.h"
#include "src/util/report.h"
#include "src/util/session.h"
#include "src/util/log.h"

#include "src/system/antidebug.h"
#include "src/system/hardware.h"
#include "src/system/memory_integrity.h"
#include "src/system/virtual_machine.h"
#include "src/system/window_check.h"

#include "src/anticheat.h"
#include "src/config.h"