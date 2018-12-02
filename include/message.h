#ifndef PYSCHEDULE_MESSAGE_H_
#define PYSCHEDULE_MESSAGE_H_

#include "include/lib_include.h"

class Message {
  friend class boost::serialization::access;
  template <class Archive>
//  void serialize(Archive & ar, const unsigned int version);
  void serialize(Archive & ar, const unsigned int version) {
    ar & src_port_;
    ar & dst_port_;
    ar & type_;
    ar & create_time_;
    ar & data_;
  }

 public:
  Message() {}
  explicit Message(short src_port, short dst_port, short type, int data_size)
    : src_port_(src_port), dst_port_(dst_port), type_(type), data_(data_size, '\0') {
      create_time_ = time(0);
    }
  virtual ~Message() = default;

  short GetSrcPort() { return src_port_; }
  short GetDstPort() { return dst_port_; }
  short GetType() { return type_; }

  template <class T>
  void SetData(T data) {
    assert(sizeof(T) == data_.size());
    auto const ptr = reinterpret_cast<unsigned char*>(&data);
    data_.assign(ptr, ptr + sizeof(T));
  }

  template <class T>
  void GetData(T &data) {
    std::memcpy(&data, data_.data(), sizeof(T));
  }

  // Debug only
  void PrintMessage();

 protected:
  short src_port_;
  short dst_port_;
  short type_;
  long long create_time_;
  std::vector<char> data_;

  DISALLOW_COPY_AND_ASSIGN(Message);
};

#define DERIVED_CLASS_SERIALIZATION \
  friend class boost::serialization::access; \
  template <class Archive> \
  void serialize(Archive & ar, const unsigned int version) { \
    ar & boost::serialization::base_object<Message>(*this); \
  }

#define ToClassName(MessageName) MessageName ## Message
#define ToEnumName(MessageName) e ## MessageName
#define ToDataName(MessageName) MessageName ## Data
#define ToHandleName(MessageName) HandleMessage_ ## MessageName

#define DERIVED_CLASS_CONSTRUCTORS_RAW(ClassName, EnumName, DataName) \
  ClassName(){} \
  explicit ClassName(short src_port, short dst_port) \
    : Message(src_port, dst_port, EnumName, sizeof(DataName)) {}

#define DERIVED_CLASS_CONSTRUCTORS(MessageName) \
  DERIVED_CLASS_CONSTRUCTORS_RAW(ToClassName(MessageName), \
                                 ToEnumName(MessageName), \
                                 ToDataName(MessageName))

#define DERIVED_CLASS_PREREQUISITES(MessageName) \
 private: DERIVED_CLASS_SERIALIZATION \
 public: DERIVED_CLASS_CONSTRUCTORS(MessageName) \
 public: DISALLOW_COPY_AND_ASSIGN(ToClassName(MessageName))

#define HandleMessageCase(MessageName, Var) \
  case ToEnumName(MessageName): \
    ToHandleName(MessageName)(std::dynamic_pointer_cast<ToClassName(MessageName)>(Var)); \
    break

#define HandleMessageDefault \
  default: \
    std::cout << "Error in HandleMessage: message type undefined" << std::endl

#endif // PYSCHEDULE_MESSAGE_H_
