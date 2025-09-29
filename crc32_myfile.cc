// Copyright (c) 2025 渟雲. All rights reserved.
//
// Licensed under the TOSSUCU 2025.9 License (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  https://raw.githubusercontent.com/M3351AN/M3351AN/1ee25fbd5318d178d15924046fa2060e765b2f66/TOSSUCU.txt
//
// -----------------------------------------------------------------------------
// File: crc32_myfile.cc
// Author: 渟雲(quq[at]outlook.it)
// Date: 2025-09-28
//
// Description:
//   Embeds a CRC32 checksum into a file by replacing the first
//   placeholder with a self-consistent value.
//
// CRC32 of this file: 479786b4
// -----------------------------------------------------------------------------
#include <cstdio>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include "miniz.h"

namespace build {
constexpr char kMarker[] = "CRC32_HASH_MARK";
}

std::string ToHex(uint32_t value) {
    char buf[9];
    snprintf(buf, sizeof(buf), "%08x", value);
    return std::string(buf);
}

struct Result {
    std::atomic<bool> found{false};
    std::atomic<uint32_t> value{0};
    std::string trial;
};

std::atomic<uint64_t> attempts{0};

void Worker(const std::string& content, size_t pos,
            uint32_t start, uint32_t step, Result* result) {
    // Precompute prefix and suffix CRC
    uint32_t crc_prefix = mz_crc32(0,
        reinterpret_cast<const unsigned char*>(content.data()), pos);

    const char* suffix_ptr = content.data() + pos + sizeof(build::kMarker) - 1;
    size_t suffix_len = content.size() - (pos + sizeof(build::kMarker) - 1);

    uint32_t candidate = start;
    while (!result->found.load(std::memory_order_relaxed)) {
        // Convert candidate to hex string
        char hex[9];
        snprintf(hex, sizeof(hex), "%08x", candidate);

        // Incremental CRC: prefix -> hex -> suffix
        uint32_t crc_mid = mz_crc32(crc_prefix,
            reinterpret_cast<const unsigned char*>(hex), 8);
        uint32_t crc = mz_crc32(crc_mid,
            reinterpret_cast<const unsigned char*>(suffix_ptr), suffix_len);

        attempts.fetch_add(1, std::memory_order_relaxed);

        if (crc == candidate) {
            result->value.store(candidate, std::memory_order_relaxed);

            // Build final trial string only once
            result->trial = content;
            result->trial.replace(pos, sizeof(build::kMarker) - 1, hex);

            result->found.store(true, std::memory_order_relaxed);
            return;
        }

        candidate += step;
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        printf("Usage: %s <file_path>\n", argv[0]);
        return 1;
    }

    std::string file_path = argv[1];

    // Read file
    std::ifstream fin(file_path, std::ios::binary);
    if (!fin) {
        printf("Failed to open file: %s\n", file_path.c_str());
        return 1;
    }
    std::string content((std::istreambuf_iterator<char>(fin)),
                        std::istreambuf_iterator<char>());
    fin.close();

    // Find placeholder
    size_t pos = content.find(build::kMarker);
    if (pos == std::string::npos) {
        printf("Placeholder not found: %s\n", build::kMarker);
        return 1;
    }

    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4;

    printf("Starting search with %u threads...\n", num_threads);

    Result result;
    auto start_time = std::chrono::steady_clock::now();

    // Launch worker threads
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < num_threads; i++) {
        threads.emplace_back(Worker, std::cref(content), pos, i, num_threads, &result);
    }

    // Launch monitor thread
    std::thread monitor([&] {
        while (!result.found.load()) {
            printf("Total tried: %llu\n",
                   static_cast<unsigned long long>(attempts.load()));
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    // Join workers
    for (auto& t : threads) {
        t.join();
    }
    monitor.join();

    auto end_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();

    if (result.found.load()) {
        uint32_t crc = result.value.load();
        printf("Fixed point found!\n");
        printf("CRC32 = %s\n", ToHex(crc).c_str());

        // Write back to file
        std::ofstream fout(file_path, std::ios::binary | std::ios::trunc);
        fout.write(result.trial.data(), result.trial.size());
        fout.close();

        printf("File updated: %s\n", file_path.c_str());
        printf("Elapsed time: %lld seconds\n", static_cast<long long>(elapsed));
        printf("Total attempts: %llu\n",
               static_cast<unsigned long long>(attempts.load()));
    } else {
        printf("No fixed point found.\n");
    }

    system("pause");
    return 0;
}
