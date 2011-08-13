#ifndef BASELINE_EVENT_H
#define BASELINE_EVENT_H

#include <list>
#include <memory>

namespace anh { namespace component {
	class Entity;
}
}

namespace swganh { namespace baseline {

	class BaselineEvent {
	public:

		BaselineEvent(std::shared_ptr<anh::component::Entity> e, std::list<std::shared_ptr<anh::component::Entity>> recv) 
		{
			entity = e;
			receiving_entities = recv;
		}

		std::shared_ptr<anh::component::Entity> entity;
		std::list<std::shared_ptr<anh::component::Entity>> receiving_entities;
	};

}
}

#endif