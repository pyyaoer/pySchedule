#include "include/message.h"

void Message::PrintMessage() { 
  std::cout << "\tsrc_port_: " << src_port_ << std::endl
            << "\tdst_port_: " << dst_port_ << std::endl
            << "\ttype_: " << type_ << std::endl
            << "\tcreate_time_: " << create_time_ << std::endl
            << "\tdata_.size(): " << data_.size() << std::endl;

  std::ios_base::fmtflags cout_flags( std::cout.flags() );
  std::cout << "\tdata_ :";
  for (auto i : data_) {
    std::cout << std::hex << std::setw(2) << i << " ";
  }
  std::cout << std::endl;
  std::cout.flags(cout_flags);
}

