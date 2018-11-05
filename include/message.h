#ifndef PYSCHEDULE_MESSAGE_H_
#define PYSCHEDULE_MESSAGE_H_

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&); \
  void operator=(const TypeName&)

class Message {
 public:
  explicit Message(const char* data) {
    sscanf(data, "%d %d %d %lld", &src_port_, &dst_port_, &type_, &create_time_);
  };
  explicit Message(short src_port, short dst_port, short type)
    : src_port_(src_port), dst_port_(dst_port), type_(type) {
      create_time_ = time(0);
    }
  virtual ~Message() = default;

  short GetSrcPort() { return src_port_; }
  short GetDstPort() { return dst_port_; }
  short GetType() { return type_; }
  std::string ToString() { 
    std::stringstream ret;
    ret << src_port_ << " " << dst_port_ << " " << type_ 
        << " " << create_time_ << "\0";
    return ret.str();
  }

 protected:
  short src_port_;
  short dst_port_;
  long long create_time_;
  short type_;

  DISALLOW_COPY_AND_ASSIGN(Message);
};

#endif // PYSCHEDULE_MESSAGE_H_
