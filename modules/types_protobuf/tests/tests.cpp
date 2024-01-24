//=================================================================================================
// Copyright (C) 2023-2024 EOLO Contributors
//=================================================================================================

#include <random>

#include <gtest/gtest.h>

#include "eolo/serdes/protobuf/buffers.h"
#include "eolo/serdes/serdes.h"
#include "eolo/types/pose.h"
#include "eolo/types/proto/geometry.pb.h"
#include "eolo/types_protobuf/geometry.h"
#include "eolo/types_protobuf/pose.h"

// NOLINTNEXTLINE(google-build-using-namespace)
using namespace ::testing;

namespace eolo::types::tests {

auto randomPose(std::mt19937_64& mt) -> Pose {
  static constexpr auto RANDOM_TRANSLATION_RANGE = 100.0;

  using DistT = std::uniform_real_distribution<double>;
  DistT t_distribution(RANDOM_TRANSLATION_RANGE);
  DistT r_distribution(-M_PI, M_PI);

  return { .orientation = Eigen::Quaterniond{ r_distribution(mt), r_distribution(mt),
                                              r_distribution(mt), r_distribution(mt) }
                              .normalized(),
           .position =
               Eigen::Vector3d{ t_distribution(mt), t_distribution(mt), t_distribution(mt) } };
}

TEST(Geometry, StaticMatrix) {
  Eigen::Matrix4d matrix4d = Eigen::Matrix4d::Random();
  proto::MatrixXd proto_matrix;
  toProto(proto_matrix, matrix4d);

  Eigen::Matrix4d matrix4d_des;
  fromProto(proto_matrix, matrix4d_des);

  EXPECT_EQ(matrix4d, matrix4d_des);
}

TEST(Geometry, DynamicMatrix1d) {
  static constexpr Eigen::Index MAX_SIZE = 1000;
  std::mt19937_64 mt{ std::random_device{}() };
  std::uniform_int_distribution<Eigen::Index> size_dist{ 1, MAX_SIZE };
  auto size = size_dist(mt);

  Eigen::Matrix<float, 2, -1> matrixf = Eigen::Matrix<float, 2, -1>::Random(2, size);
  EXPECT_EQ(matrixf.cols(), size);

  proto::MatrixXf proto_matrix;
  toProto(proto_matrix, matrixf);

  Eigen::Matrix<float, 2, -1> matrixf_des;
  fromProto(proto_matrix, matrixf_des);
  EXPECT_EQ(matrixf, matrixf_des);
}

TEST(Geometry, DynamicMatrix2d) {
  static constexpr Eigen::Index MAX_SIZE = 1000;
  std::mt19937_64 mt{ std::random_device{}() };
  std::uniform_int_distribution<Eigen::Index> size_dist{ 1, MAX_SIZE };
  auto rows = size_dist(mt);
  auto cols = size_dist(mt);

  Eigen::Matrix<float, -1, -1> matrixf = Eigen::Matrix<float, -1, -1>::Random(rows, cols);

  proto::MatrixXf proto_matrix;
  toProto(proto_matrix, matrixf);

  Eigen::Matrix<float, -1, -1> matrixf_des;
  fromProto(proto_matrix, matrixf_des);
  EXPECT_EQ(matrixf, matrixf_des);
}

TEST(Geometry, DynamicVector) {
  static constexpr Eigen::Index MAX_SIZE = 1000;
  std::mt19937_64 mt{ std::random_device{}() };
  std::uniform_int_distribution<Eigen::Index> size_dist{ 1, MAX_SIZE };
  auto size = size_dist(mt);

  Eigen::VectorXf vector = Eigen::VectorX<float>::Random(size);

  proto::VectorXf proto_vector;
  toProto(proto_vector, vector);

  Eigen::VectorXf vector_des;
  fromProto(proto_vector, vector_des);
  EXPECT_EQ(vector, vector_des);
}

TEST(Geometry, StaticVector2) {
  Eigen::Vector2f vector = Eigen::Vector2f::Random();
  proto::Vector2f proto_vector;
  toProto(proto_vector, vector);

  Eigen::Vector2f vector_des;
  fromProto(proto_vector, vector_des);
  EXPECT_EQ(vector, vector_des);
}

TEST(Geometry, StaticVector3) {
  Eigen::Vector3d vector = Eigen::Vector3d::Random();
  proto::Vector3d proto_vector;
  toProto(proto_vector, vector);

  Eigen::Vector3d vector_des;
  fromProto(proto_vector, vector_des);
  EXPECT_EQ(vector, vector_des);
}

TEST(Pose, Pose) {
  std::mt19937_64 mt{ std::random_device{}() };
  auto pose = randomPose(mt);

  proto::Pose proto_pose;
  toProto(proto_pose, pose);

  Pose pose_des;
  fromProto(proto_pose, pose_des);
  EXPECT_EQ(pose, pose_des);
}

TEST(Pose, PoseBuffer) {
  std::mt19937_64 mt{ std::random_device{}() };
  auto pose = randomPose(mt);

  serdes::protobuf::SerializerBuffer ser_buffer;
  toProtobuf(ser_buffer, pose);

  auto buffer = std::move(ser_buffer).exctractSerializedData();

  serdes::protobuf::DeserializerBuffer des_buffer{ buffer };
  Pose pose_des;
  fromProtobuf(des_buffer, pose_des);

  EXPECT_EQ(pose, pose_des);
}

TEST(Pose, PoseSerdes) {
  std::mt19937_64 mt{ std::random_device{}() };
  auto pose = randomPose(mt);

  auto buffer = serdes::serialize(pose);

  Pose pose_des;
  serdes::deserialize(buffer, pose_des);
  EXPECT_EQ(pose, pose_des);
}

}  // namespace eolo::types::tests