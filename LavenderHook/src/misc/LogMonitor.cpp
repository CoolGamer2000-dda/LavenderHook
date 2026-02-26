#include "LogMonitor.h"
#include "Globals.h"
#include "../ui/components/Console.h"

#include <thread>
#include <atomic>
#include <fstream>
#include <algorithm>
#include <chrono>

using namespace std::chrono_literals;

namespace LavenderHook {
    namespace LogMonitor {

        static std::thread s_thread;
        static std::atomic_bool s_running{ false };
        static std::atomic_bool s_hasNewAbort{ false };
        static std::string s_lastAbortLine;
        static std::chrono::steady_clock::time_point s_lastAbortTime{};

        static std::string GetLatestAbortLineFromFile()
        {
            const char* pathTemplate = "C:\\Users\\%USERNAME%\\AppData\\Local\\DDS\\Saved\\Logs\\DDS.log";

            char username[256]; size_t outlen = 0;
            getenv_s(&outlen, username, sizeof(username), "USERNAME");

            std::string path(pathTemplate);
            size_t pos = path.find("%USERNAME%");
            if (pos != std::string::npos) path.replace(pos, strlen("%USERNAME%"), username);

            std::ifstream f(path, std::ios::in | std::ios::binary);
            if (!f) return {};

            f.seekg(0, std::ios::end);
            std::streamoff size = f.tellg();
            if (size <= 0) return {};

            std::streamoff posSeek = size - 1;
            std::string buf;

            while (posSeek >= 0) {
                f.seekg(posSeek);
                char c = f.get();
                if (c == '\n' || c == '\r') {
                    if (!buf.empty()) {
                        std::reverse(buf.begin(), buf.end());
                        if (buf.find("has been Aborted") != std::string::npos)
                            return buf;
                        buf.clear();
                    }
                }
                else {
                    buf.push_back(c);
                }
                --posSeek;
            }

            if (!buf.empty()) {
                std::reverse(buf.begin(), buf.end());
                if (buf.find("has been Aborted") != std::string::npos)
                    return buf;
            }

            return {};
        }

        static void ThreadFunc()
        {
            while (s_running.load()) {
                if (LavenderHook::Globals::stop_on_fail) {
                    try {
                        auto found = GetLatestAbortLineFromFile();
                        if (!found.empty() && found != s_lastAbortLine) {
                            s_lastAbortLine = found;
                            s_hasNewAbort.store(true);
                            s_lastAbortTime = std::chrono::steady_clock::now();
                            try {
                                LavenderConsole::GetInstance().Log("LogMonitor: stopped actions");
                            }
                            catch (...) {}
                        }
                    }
                    catch (...) {
                        // swallow
                    }
                }

                std::this_thread::sleep_for(250ms);
            }
        }

        void Start()
        {
            if (s_running.load()) return;
            s_running.store(true);
            s_thread = std::thread(ThreadFunc);
        }

        void Stop()
        {
            if (!s_running.load()) return;
            s_running.store(false);
            if (s_thread.joinable()) s_thread.join();
        }

        bool LatestLineHasAbort()
        {
			// Return the current flag state to stop actions, but only reset it if it's been more than 1 second since the last detected abort line.
            if (!s_hasNewAbort.load()) return false;

            using namespace std::chrono;
            auto now = steady_clock::now();
            if (s_lastAbortTime.time_since_epoch().count() == 0) {
                return true;
            }

            if ((now - s_lastAbortTime) <= milliseconds(1000))
                return true;

            s_hasNewAbort.store(false);
            return false;
        }

    }
}
