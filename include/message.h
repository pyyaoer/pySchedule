#ifndef PYSCHEDULE_MESSAGE_H_
#define PYSCHEDULE_MESSAGE_H_

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&); \
  void operator=(const TypeName&)

class Message {
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & src_port_;
    ar & dst_port_;
    ar & type_;
    ar & create_time_;
    ar & data;
  }

 public:
  Message() {}
  explicit Message(short src_port, short dst_port, short type)
    : src_port_(src_port), dst_port_(dst_port), type_(type) {
      create_time_ = time(0);
    }
  virtual ~Message() = default;

  short GetSrcPort() { return src_port_; }
  short GetDstPort() { return dst_port_; }
  short GetType() { return type_; }

  // Debug only
  void PrintMessage() { 
    std::cout << "\tsrc_port_: " << src_port_ << std::endl
              << "\tdst_port_: " << dst_port_ << std::endl
              << "\ttype_: " << type_ << std::endl
              << "\tcreate_time_: " << create_time_ << std::endl
              << "\tdata.size(): " << data.size() << std::endl;
  }

 protected:
  short src_port_;
  short dst_port_;
  short type_;
  long long create_time_;
  std::vector<char> data;

  DISALLOW_COPY_AND_ASSIGN(Message);
};

#endif // PYSCHEDULE_MESSAGE_H_
