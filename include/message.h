#ifndef PYSCHEDULE_MESSAGE_H_
#define PYSCHEDULE_MESSAGE_H_

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&); \
  void operator=(const TypeName&)

enum eMessageType{ BASIC = 0 };

class Message {
 public:
  explicit Message(const char* data) {
    sscanf(data, "%d %d %d", &src_port_, &dst_port_, &type_);
  };
  explicit Message(short src_port, short dst_port, eMessageType type)
    : src_port_(src_port), dst_port_(dst_port), type_(type) {}
  ~Message() {};

  short GetSrcPort() { return src_port_; }
  short GetDstPort() { return dst_port_; }
  std::string ToString() { 
    std::stringstream ret;
    ret << src_port_ << " " << dst_port_ << " " << type_ << "\0";
    return ret.str();
  }

 private:
  short src_port_;
  short dst_port_;
  eMessageType type_;

  DISALLOW_COPY_AND_ASSIGN(Message);
};

#endif // PYSCHEDULE_MESSAGE_H_
