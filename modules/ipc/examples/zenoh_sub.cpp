//=================================================================================================
// Copyright (C) 2023-2024 EOLO Contributors
//=================================================================================================

#include <chrono>
#include <cstdlib>
#include <print>
#include <thread>
#include <zenohc.hxx>

#include <zenoh.h>

#include "eolo/ipc/zenoh/subscriber.h"
#include "eolo/serdes/serdes.h"
#include "eolo/types/pose.h"
#include "eolo/types_protobuf/pose.h"
#include "zenoh_program_options.h"

auto main(int argc, const char* argv[]) -> int {
  try {
    auto desc = getProgramDescription("Periodic publisher example");
    const auto args = std::move(desc).parse(argc, argv);

    auto config = parseArgs(args);

    std::println("Opening session...");
    std::println("Declaring Subscriber on '{}'", config.topic);
    const auto cb = [topic = config.topic](const eolo::ipc::zenoh::MessageMetadata& metadata,
                                           std::span<const std::byte> data) {
      eolo::types::Pose pose;
      eolo::serdes::deserialize(data, pose);
      std::println(">> Time: {}. Topic {}. From: {}. Counter: {}. Received {}",
                   std::chrono::system_clock::time_point{
                       std::chrono::duration_cast<std::chrono::system_clock::duration>(metadata.timestamp) },
                   topic, metadata.sender_id, metadata.sequence_id, pose.position.transpose());
    };

    eolo::ipc::zenoh::Subscriber sub{ std::move(config), std::move(cb) };

    while (true) {
      std::this_thread::sleep_for(std::chrono::seconds{ 1 });
    }

    return EXIT_SUCCESS;
  } catch (const std::exception& ex) {
    std::ignore = std::fputs(ex.what(), stderr);
    return EXIT_FAILURE;
  }
}
