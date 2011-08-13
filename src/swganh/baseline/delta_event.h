#ifndef DELTA_EVENT_H
#define DELTA_EVENT_H

namespace anh { namespace component {
	class Entity;
}
}

namespace swganh { namespace baseline {

	class DeltaEvent {
	public:

		DeltaEvent(std::shared_ptr<anh::component::Entity> e) {
			entity = e;
		}

		std::shared_ptr<anh::component::Entity> entity;
	};

}
}

#endif