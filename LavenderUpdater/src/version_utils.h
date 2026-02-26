#pragma once

#include <string>
#include <filesystem>

std::string get_local_dll_version(const std::filesystem::path &dllPath);
