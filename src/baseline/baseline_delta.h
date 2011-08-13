#ifndef BASELINE_DELTA_H
#define BASELINE_DELTA_H

#include <swganh/baseline/baseline_delta_interface.h>

namespace baseline {
	class BaselineDelta : public swganh::baseline::BaselineDeltaInterface
	{
	public:

		BaselineDelta(const char* type, std::uint8_t id, bool is_private);

		virtual void attach_updatable(std::uint16_t id, swganh::baseline::UpdatableFunctor uf);
		virtual void detach_updatable(std::uint16_t id);

		virtual anh::ByteBuffer build_baseline(std::shared_ptr<anh::component::Entity> e);
		virtual anh::ByteBuffer build_delta	(	
												std::shared_ptr<anh::component::Entity> e, 
												swganh::baseline::Updatables::iterator& itr, 
												swganh::baseline::Updatables::iterator& absolute_end
											);

		virtual bool is_private() { return is_private_; }

	private:

		std::map<std::uint16_t, swganh::baseline::UpdatableFunctor> updatables_;

		std::uint32_t type_;
		std::uint8_t id_;

		bool is_private_;
	};
};

#endif