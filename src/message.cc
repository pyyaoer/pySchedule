#include "include/message.h"

void Message::PrintMessage() { 
  std::cout << "\tsrc_port_: " << src_port_ << std::endl
            << "\tdst_port_: " << dst_port_ << std::endl
            << "\ttype_: " << type_ << std::endl
            << "\tcreate_time_: " << create_time_ << std::endl
            << "\tdata_size_: " << data_size_ << std::endl;

  std::ios_base::fmtflags cout_flags( std::cout.flags() );
  std::cout << "\tdata_ :";
  for (int i = 0; i < data_size_; i++) {
    std::cout << std::setw(2) << data_[i] << " ";
  }
  std::cout << std::endl;
  std::cout.flags(cout_flags);
}

