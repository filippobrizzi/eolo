//=================================================================================================
// Copyright (C) 2023-2024 EOLO Contributors
//=================================================================================================

#include "eolo/ipc/zenoh/liveliness.h"

#include <zenohc.hxx>

#include <zenoh.h>

#include "eolo/ipc/common.h"
#include "eolo/ipc/zenoh/session.h"
#include "eolo/ipc/zenoh/utils.h"

namespace eolo::ipc::zenoh {

auto getListOfPublishers(const Config& config) -> std::vector<PublisherInfo> {
  static constexpr auto FIFO_BOUND = 100;

  auto session = createSession(config);

  z_owned_reply_channel_t channel = zc_reply_fifo_new(FIFO_BOUND);
  auto keyexpr = z_keyexpr(config.topic.c_str());
  zc_liveliness_get(session->loan(), keyexpr, z_move(channel.send), nullptr);
  z_owned_reply_t reply = z_reply_null();

  std::vector<PublisherInfo> infos;
  for (z_call(channel.recv, &reply); z_check(reply); z_call(channel.recv, &reply)) {
    if (z_reply_is_ok(&reply)) {
      auto sample = static_cast<zenohc::Sample>(z_reply_ok(&reply));
      infos.emplace_back(std::string{ sample.get_keyexpr().as_string_view() });
    } else {
      std::println("Received an error");
    }
  }

  z_drop(z_move(reply));
  z_drop(z_move(channel));

  return infos;
}

void printPublisherInfo(const PublisherInfo& info) {
  std::println("[Publisher] Topic: {}", info.topic);
}
}  // namespace eolo::ipc::zenoh