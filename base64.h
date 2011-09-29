#ifndef __BASE_64_H__
#define __BASE_64_H__

#include <string>

std::string base64_encode(const unsigned char * , unsigned int len);
std::string base64_decode(const std::string & s);

#endif
