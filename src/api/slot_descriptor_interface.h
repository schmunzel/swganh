#ifndef SLOT_DESCRIPTOR_INTERFACE_H
#define SLOT_DESCRIPTOR_INTERFACE_H

#include <map>
#include <set>

namespace anh
{
namespace api
{

class slot_descriptor_interface
{
public:

	bool can_attach(std::set<std::string> slots);

private:
	std::map<std::string, unsigned int> slots_contained_;
};

};
};

#endif