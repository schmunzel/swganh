#ifndef BASELINE_SERVICE_H
#define BASELINE_SERVICE_H

#include <swganh/baseline/baseline_service_interface.h>
#include <map>

namespace baseline {
	class baseline_service : public swganh::baseline::baseline_service_interface
	{
	public:

		baseline_service(std::shared_ptr<anh::app::KernelInterface> kernel);

		virtual void attach_baseline_delta(anh::HashString name, std::shared_ptr<swganh::baseline::baseline_delta_interface> bdi);
		virtual void detach_baseline_delta(anh::HashString name);

		virtual void send_baselines(std::shared_ptr<Entity> e, std::list<std::uint64_t> recieving_entities);

		virtual void send_deltas(std::shared_ptr<Entity> e, std::list<std::uint64_t> recieving_entities);
		virtual void send_deltas(std::shared_ptr<Entity> e);

	private:
		std::map<anh::HashString, std::shared_ptr<swganh::baseline::baseline_delta_interface>> lookup_;
	};
};

#endif