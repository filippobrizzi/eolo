//=================================================================================================
// Copyright (C) 2023-2024 EOLO Contributors
// MIT License
//=================================================================================================

#pragma once

#include <cstdint>
#include <string_view>

namespace eolo::utils
{

// These are autogenerated by the build system from version.in

static constexpr std::uint8_t VERSION_MAJOR = 0;
static constexpr std::uint8_t VERSION_MINOR = 0;
static constexpr std::uint16_t VERSION_PATCH = 0;

static constexpr std::string_view REPO_BRANCH = "zenoh_liveliness";
static constexpr std::string_view BUILD_PROFILE = "Release";
static constexpr std::string_view REPO_HASH = "8f5391d";

} // namespace eolo::utils
