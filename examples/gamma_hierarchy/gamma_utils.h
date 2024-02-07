#ifndef BAYESMIX_EXAMPLES_UTILS_H_
#define BAYESMIX_EXAMPLES_UTILS_H_

#include <google/protobuf/any.h>

namespace bayesmix {

// Unpack google::protobuf::Any message
template <typename T>
T unpack_protobuf_any(const google::protobuf::Any& _packed_msg) {
  T unpacked_state;
  if (_packed_msg.Is<T>()) {
    _packed_msg.UnpackTo(&unpacked_state);
  } else {
    throw std::runtime_error("Unable to unpack google::protobuf::Any type.");
  }
  return unpacked_state;
};

}  // namespace bayesmix

#endif  // BAYESMIX_EXAMPLES_UTILS_H_
