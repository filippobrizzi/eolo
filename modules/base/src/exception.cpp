//=================================================================================================
// Copyright (C) 2023-2024 HEPHAESTUS Contributors
// MIT License
//=================================================================================================

#include "hephaestus/base/exception.h"

#include "hephaestus/utils/utils.h"

namespace heph {

Exception::Exception(const std::string& message, std::source_location location)
  : std::runtime_error("[" + std::string(utils::truncate(location.file_name(), "modules")) + ":" +
                       std::to_string(location.line()) + "] " + message) {
}

}  // namespace heph
