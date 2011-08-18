#include "slot_arrangement_interface.h"

namespace swganh { namespace slots {

	class SlotArrangementComponent : public SlotArrangementInterface 
	{
	public:

		SlotArrangementComponent() : SlotArrangementInterface("SlotArrangement") {}

		virtual void add_arrangement(const std::set<anh::HashString>& slot);
		virtual void remove_arrangement(size_t position);

		virtual size_t arrangements();
		virtual const std::set<anh::HashString>& arrangement(unsigned int arrangement_id);

	private:
		std::vector<std::set<anh::HashString>> slots_;
	};

}
}