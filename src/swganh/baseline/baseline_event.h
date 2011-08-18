#ifndef BASELINE_EVENT_H
#define BASELINE_EVENT_H

#include <list>
#include <memory>

namespace anh { namespace component {
	class Entity;
}
}

namespace swganh { namespace baseline {

	class BaselineEvent  {
	public:

		BaselineEvent(std::shared_ptr<anh::component::Entity> entity, bool duplex = true) 
		{
			entity_ = entity;
			duplex_ = true;
		}

		std::shared_ptr<anh::component::Entity> entity_;
		std::list<std::shared_ptr<anh::component::Entity>> receiving_entities_;
		bool duplex_;
	};

}
}

#endif