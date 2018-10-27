#ifndef PYSCHEDULE_MESSAGE_H_
#define PYSCHEDULE_MESSAGE_H_


#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&); \
  void operator=(const TypeName&)

class Message {
 public:
  Message() {};
  ~Message() {};

  std::string ToString() {return std::string("Hello");}

 private:

  DISALLOW_COPY_AND_ASSIGN(Message);
};

#endif // PYSCHEDULE_MESSAGE_H_
