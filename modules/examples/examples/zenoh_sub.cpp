//=================================================================================================
// Copyright (C) 2023-2024 EOLO Contributors
//=================================================================================================

#include <chrono>
#include <cstdlib>
#include <format>
#include <thread>

#include <fmt/chrono.h>
#include <fmt/core.h>
#include <zenoh.h>
#include <zenohc.hxx>

#include "eolo/examples/types/pose.h"
#include "eolo/examples/types_protobuf/pose.h"
#include "eolo/ipc/subscriber.h"
#include "eolo/ipc/zenoh/session.h"
#include "eolo/ipc/zenoh/subscriber.h"
#include "zenoh_program_options.h"

auto main(int argc, const char* argv[]) -> int {
  try {
    auto desc = getProgramDescription("Periodic publisher example");
    const auto args = std::move(desc).parse(argc, argv);

    auto config = parseArgs(args);

    fmt::println("Opening session...");
    fmt::println("Declaring Subscriber on '{}'", config.topic);

    auto session = eolo::ipc::zenoh::createSession(std::move(config));

    auto cb = [topic = session->config.topic](const eolo::ipc::MessageMetadata& metadata,
                                              const std::shared_ptr<eolo::examples::types::Pose>& pose) {
      fmt::println(">> Time: {}. Topic {}. From: {}. Counter: {}. Received {}",
                   std::chrono::system_clock::time_point{
                       std::chrono::duration_cast<std::chrono::system_clock::duration>(metadata.timestamp) },
                   metadata.topic, metadata.sender_id, metadata.sequence_id, *pose);
    };
    auto subscriber = eolo::ipc::subscribe<eolo::ipc::zenoh::Subscriber, eolo::examples::types::Pose>(
        session, std::move(cb));
    (void)subscriber;

    while (true) {
      std::this_thread::sleep_for(std::chrono::seconds{ 1 });
    }

    return EXIT_SUCCESS;
  } catch (const std::exception& ex) {
    std::ignore = std::fputs(ex.what(), stderr);
    return EXIT_FAILURE;
  }
}
