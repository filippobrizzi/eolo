//=================================================================================================
// Copyright (C) 2023-2024 EOLO Contributors
//=================================================================================================

#include "eolo/bag/bag.h"

#include <tabulate/table.hpp>

#include "eolo/utils/utils.h"

namespace eolo::bag {
namespace {
constexpr uint64_t BAG_VERSION = 1;
constexpr auto TIME_FORMAT = "{:%Y-%m-%d %Hh:%Mm:%Ss}";

[[nodiscard]] auto
toChronoTimePoint(const std::chrono::nanoseconds& timestamp) -> std::chrono::system_clock::time_point {
  using ClockT = std::chrono::system_clock;
  return ClockT::time_point{ std::chrono::duration_cast<ClockT::duration>(timestamp) };
}

[[nodiscard]] auto getTypes(const BagInfo& bag_info) -> std::vector<std::string> {
  std::set<std::string> types{};
  for (const auto& topic_info : bag_info.topics) {
    types.insert(utils::demangle(topic_info.type));
  }

  return { types.begin(), types.end() };
}

[[nodiscard]] auto bytesToMegaBytes(size_t num_bytes) -> double {
  const auto mega = std::pow(2, 20);
  return static_cast<double>(num_bytes) / mega;
}

[[nodiscard]] auto
getMessagesAverageIntervalForTopic(const TopicInfo& topic_info) -> std::chrono::milliseconds {
  const auto nanoseconds =
      static_cast<std::size_t>((topic_info.last_msg_timestamp - topic_info.first_msg_timestamp).count()) /
      topic_info.msgs_count;

  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::nanoseconds(nanoseconds));
}

[[nodiscard]] auto getMessagesAverageBytesSizeForTopic(const TopicInfo& topic_info) -> size_t {
  return topic_info.total_msgs_size_bytes / topic_info.msgs_count;
}

auto printTabulatedTopicsInfo(const std::vector<TopicInfo>& topics_info, std::ostream& out) -> void {
  using Row = tabulate::Table::Row_t;
  tabulate::Table topics_info_table;

  topics_info_table.add_row(Row{ "Topics info", "" });
  for (const auto& topic_info : topics_info) {
    tabulate::Table topic_info_table;
    topic_info_table.add_row(Row{ "type", topic_info.type });
    topic_info_table.add_row(Row{ "messages", std::to_string(topic_info.msgs_count) });
    topic_info_table.add_row(
        Row{ "avg size", std::format("{} bytes", getMessagesAverageBytesSizeForTopic(topic_info)) });
    topic_info_table.add_row(
        Row{ "interval", std::format("{}", getMessagesAverageIntervalForTopic(topic_info)) });
    topic_info_table.add_row(
        Row{ "start", std::format(TIME_FORMAT, toChronoTimePoint(topic_info.first_msg_timestamp)) });
    topic_info_table.add_row(
        Row{ "end", std::format(TIME_FORMAT, toChronoTimePoint(topic_info.last_msg_timestamp)) });
    topic_info_table.add_row(
        Row{ "Attributes", std::format("QoS: {} - Cache: {}", topic_info.qos, topic_info.cache) });
    topic_info_table.format().hide_border();
    topics_info_table.add_row(Row{ topic_info.name, topic_info_table });
  }

  topics_info_table.format().border_top(" ").border_bottom("").border_left("").border_right("").corner("");
  topics_info_table[0].format().font_style({ tabulate::FontStyle::bold });

  out << topics_info_table << std::endl;
}

}  // namespace

auto bagFileFormatVersion() noexcept -> uint64_t {
  return BAG_VERSION;
}

auto printTabulatedBagInfo(const BagInfo& bag_info, std::ostream& out) -> void {
  using Row = tabulate::Table::Row_t;
  tabulate::Table info;
  const auto& header = bag_info.header;
  info.add_row(Row{ "version", std::format("robocool: {}.{}.{}; file format: {}", header.eolo_version.major,
                                           header.eolo_version.minor, header.eolo_version.patch,
                                           header.file_format_version) });
  info.add_row(Row{ "messages", std::format("{}", bag_info.msgs_count) });
  info.add_row(Row{ "duration",
                    std::format("{:%Hh:%Mm:%Ss}", bag_info.last_msg_timestamp - bag_info.start_timestamp) });
  info.add_row(Row{ "start", std::format(TIME_FORMAT, toChronoTimePoint(bag_info.start_timestamp)) });
  info.add_row(Row{ "end", std::format(TIME_FORMAT, toChronoTimePoint(bag_info.last_msg_timestamp)) });
  info.add_row(Row{ "size", std::format("{:.0f} MB", bytesToMegaBytes(bag_info.bag_size_bytes)) });
  info.add_row(Row{ "topics", std::to_string(bag_info.topics.size()) });
  std::string types;
  for (const auto& type : getTypes(bag_info)) {
    types += std::format("{}\n", type);
  }
  info.add_row(Row{ "types", types });

  info.format().border_top("-").border_bottom("-").border_left("").border_right("").corner("");
  info.column(0).format().font_style({ tabulate::FontStyle::bold });
  out << info << std::endl;

  printTabulatedTopicsInfo(bag_info.topics, out);
}

}  // namespace eolo::bag
