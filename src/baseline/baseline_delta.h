#ifndef BASELINE_DELTA_H
#define BASELINE_DELTA_H

#include <swganh/baseline/baseline_delta_interface.h>

namespace baseline {
	class baseline_delta : public swganh::baseline::baseline_delta_interface
	{
	public:

		baseline_delta(const char* type, std::uint8_t id);

		virtual void attach_updatable(std::uint16_t id, swganh::baseline::UpdatableFunctor uf);
		virtual void detach_updatable(std::uint16_t id);

		virtual anh::ByteBuffer build_baseline(std::shared_ptr<anh::component::Entity> e);
		virtual anh::ByteBuffer build_delta	(	
												std::shared_ptr<anh::component::Entity> e, 
												swganh::baseline::Updatables::iterator& itr, 
												swganh::baseline::Updatables::iterator& absolute_end
											);
	private:

		std::map<std::uint16_t, swganh::baseline::UpdatableFunctor> updatables_;

		std::uint32_t type_;
		std::uint8_t id_;
	};
};

#endif