//=================================================================================================
// Copyright (C) 2023-2024 EOLO Contributors
//=================================================================================================

#pragma once

#include <chrono>
#include <filesystem>
#include <string>
#include <vector>

#include "eolo/bag/proto/bag.pb.h"
#include "eolo/utils/version.h"

namespace eolo::bag {
struct BagHeader {
  uint64_t file_format_version{};  //!< the file format version of this bag as returned by
                                   //!< BagFileFormatVersion()
  utils::Version eolo_version;     //!< the eolo version that generated this bag
  std::string description;         //!< a optional description of this bag
};

struct TopicInfo {
  std::string name;
  std::string type;
  bool qos{};
  bool cache{};

  size_t msgs_count{};
  size_t max_msg_size_bytes{};
  size_t total_msgs_size_bytes{};
  std::chrono::nanoseconds first_msg_timestamp;
  std::chrono::nanoseconds last_msg_timestamp;
};

struct BagInfo {
  BagHeader header{};
  std::vector<TopicInfo> topics;
  std::chrono::nanoseconds start_timestamp{};
  std::chrono::nanoseconds last_msg_timestamp{};
  size_t msgs_count{};
  size_t bag_size_bytes{};  //!< Total number of bytes (records + metadata + header)
};

class IBagIterator;
class IBagRecordFilter;

//! This class provides functionalities to read messages from an bag.
//! Note that this class (and the whole bag design) is agnostic to the type of the message,
//! which is returned as a binary blob together with its metadata.
class IBag {
public:
  virtual ~IBag() = default;

  [[nodiscard]] virtual auto bagInfo() const noexcept -> BagInfo = 0;
  [[nodiscard]] virtual auto getIterator() const -> std::unique_ptr<IBagIterator> = 0;
  /// Returns an iterator only for messages of the specified topic.
  /// If the bag doesn't have message for the required topic the iterator will terminate
  /// immediately.
  [[nodiscard]] virtual auto
  getIterator(std::shared_ptr<const IBagRecordFilter> filter) const -> std::unique_ptr<IBagIterator> = 0;
};

class IBagIterator {
public:
  virtual ~IBagIterator() = default;

  //! This functions read messages sorted by the timestamp when they were created.
  [[nodiscard]] virtual auto
  readNext(std::vector<std::byte>& buffer) -> std::optional<proto::LogRecordMetadata> = 0;
};

//! Provide an utility to skip messages when reading a bag.
class IBagRecordFilter {
public:
  virtual ~IBagRecordFilter() = default;

  [[nodiscard]] virtual auto
  isAcceptable(const proto::LogRecordMetadata& metadata) const noexcept -> bool = 0;
};

//! Return the file format version of the bag. The number is incremented every time a breaking
//! change is introduced.
//! NOTE: the bag is agnostic to the serialized payload, this means that breaking changes in the
//! payload format, don't influence this version number.
[[nodiscard]] auto bagFileFormatVersion() noexcept -> uint64_t;

using BagRecord = std::pair<proto::LogRecordMetadata, std::vector<std::byte>>;
struct InMemoryBagParameters {
  BagHeader header;
  std::vector<BagRecord> records;
};

[[nodiscard]] auto createInMemoryBag(const InMemoryBagParameters& bag_params) -> std::unique_ptr<IBag>;

[[nodiscard]] auto createFileBag(const std::filesystem::path& bag_path) -> std::unique_ptr<IBag>;

[[nodiscard]] auto createTopicNamesFilter(std::string topic_name) -> std::shared_ptr<const IBagRecordFilter>;

[[nodiscard]] auto createTopicNamesFilter(const std::vector<std::string>& topic_names)
    -> std::shared_ptr<const IBagRecordFilter>;

//! Pretty print bag info
auto printBagInfo(const BagInfo& bag_info, std::ostream& out) -> void;

auto printTabulatedBagInfo(const BagInfo& bag_info, std::ostream& out) -> void;

/// Read through the whole bag and serialize the metadata to a comma separated list of values.
/// Each message fill a line, the order being the one returned by the bag iterator.
/// The first line contains the field name. Nested arguments are flattened (e.g. TopicAttributes).
/// NOTE: don't use "," as delim, as types can have a comma as part of the type and will create
/// problem parsing it.
auto exportLogRecordMetadataToCSV(const IBag& bag, std::ostream& output_csv, char delim = '|') -> void;

/// Read through the whole bag and serialize the metadata to a json file.
/// The file format will be a json array where each element is the the protobuf json serialization
/// of proto::LogRecordMetadata.
/// NOTE: for big bag this function can take long time and create massively large files. Prefer the
/// CSV format.
auto exportLogRecordMetadataToJSONLines(const IBag& bag, std::ostream& output_json) -> void;
}  // namespace eolo::bag
