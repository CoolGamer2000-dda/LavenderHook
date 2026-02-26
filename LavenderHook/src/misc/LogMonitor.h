#pragma once

#include <string>
#include <atomic>

namespace LavenderHook {
    namespace LogMonitor {

        void Start();
        void Stop();

        bool LatestLineHasAbort();
    }
}
