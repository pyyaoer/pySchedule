#ifndef PYSCHEDULE_MESSAGE_H_
#define PYSCHEDULE_MESSAGE_H_

#include "include/lib_include.h"

#define MESSAGE_ROBUST_TAIL 2
#define MESSAGE_DATA_SIZE 50
#define MESSAGE_REAL_DATA_SIZE (MESSAGE_DATA_SIZE + MESSAGE_ROBUST_TAIL)

class Message {
  friend class boost::serialization::access;
  template <class Archive>
//  void serialize(Archive & ar, const unsigned int version);
  void serialize(Archive & ar, const unsigned int version) {
    ar & src_port_;
    ar & dst_port_;
    ar & type_;
    ar & create_time_;
    ar & data_size_;
    ar & boost::serialization::make_array(data_, MESSAGE_DATA_SIZE);
    try {
      for (int i = MESSAGE_DATA_SIZE; i < MESSAGE_REAL_DATA_SIZE; i ++) {
        ar & data_[i];
      }
    } catch(const std::exception& e) {
      // Ignore the exceptions for message tail
    }
  }

 public:
  Message(): Message(-1, -1, -1, 100) {}
  explicit Message(short src_port, short dst_port, short type, int data_size)
    : src_port_(src_port), dst_port_(dst_port), type_(type), data_size_(data_size) {
      create_time_ = (duration_cast< milliseconds >(system_clock::now().time_since_epoch())).count();
    }
  virtual ~Message() = default;

  short GetSrcPort() { return src_port_; }
  short GetDstPort() { return dst_port_; }
  short GetType() { return type_; }
  long long GetCreateTime() { return create_time_; }

  template <class T>
  void SetData(T data) {
    //assert(sizeof(T) == data_size_);
    auto const ptr = reinterpret_cast<unsigned char*>(&data);
    std::memcpy(data_, ptr, sizeof(T));
  }

  template <class T>
  void GetData(T &data) {
    //assert(sizeof(T) == data_size_);
    std::memcpy(&data, data_, sizeof(T));
  }

  // Debug only
  void PrintMessage();

 protected:
  short src_port_;
  short dst_port_;
  short type_;
  long long create_time_;
  int data_size_;
  unsigned char data_[MESSAGE_REAL_DATA_SIZE] = {0};
  //std::vector<char> data_;

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
