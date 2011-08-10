#ifndef BASELINE_SERVICE_INTERFACE_H
#define BASELINE_SERVICE_INTERFACE_H

#include <swganh/base/base_service.h>
#include <anh/hash_string.h>
#include <anh/byte_buffer.h>
#include <memory>
#include <cstdint>
#include <list>

namespace swganh { namespace baseline {

	class baseline_delta_interface;

	class baseline_service_interface : swganh::base::base_service
	{
	public:

		baseline_service_interface(std::shared_ptr<anh::app::KernelInterface> kernel) : base_service(kernel) {}

		virtual void attach_baseline_delta(anh::HashString name, std::shared_ptr<baseline_delta_interface> bdi) = 0;
		virtual void detach_baseline_delta(anh::HashString name) = 0;

		virtual void send_baselines(std::shared_ptr<Entity> e, std::list<std::uint64_t> recieving_entities) = 0;
		virtual void send_deltas(std::shared_ptr<Entity> e, std::list<std::uint64_t> recieving_entities) = 0;
		virtual void send_deltas(std::shared_ptr<Entity> e) = 0;
	};
};};

#endif