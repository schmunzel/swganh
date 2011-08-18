#ifndef BASELINE_SERVICE_H
#define BASELINE_SERVICE_H

#include <swganh/baseline/baseline_service_interface.h>
#include <map>

namespace baseline {

    class BaselineService : public swganh::baseline::BaselineServiceInterface
	{
	public:

		BaselineService(std::shared_ptr<anh::app::KernelInterface> kernel);

		virtual void create_group(anh::HashString name, swganh::baseline::GroupFunctor functor);
		virtual void remove_group(anh::HashString name);

		virtual void attach_baseline_delta(anh::HashString name, std::shared_ptr<swganh::baseline::BaselineDeltaInterface> bdi);
		virtual std::shared_ptr<swganh::baseline::BaselineDeltaInterface> get_baseline_delta(anh::HashString name);
		virtual void detach_baseline_delta(anh::HashString name);
        // simply a helper function
		virtual void send_baselines(std::shared_ptr<anh::component::Entity> e, std::list<std::shared_ptr<anh::component::Entity>> recieving_entities, bool duplex);
		virtual void send_baselines(std::shared_ptr<anh::component::Entity> e, std::shared_ptr<anh::component::Entity> recieving_entity);
		
		virtual void send_deltas(std::shared_ptr<anh::component::Entity> e, std::list<std::shared_ptr<anh::component::Entity>> recieving_entities);
		virtual void send_deltas(std::shared_ptr<anh::component::Entity> e);

		virtual anh::service::ServiceDescription GetServiceDescription();
		virtual void DescribeConfigOptions(boost::program_options::options_description& description);
		virtual void subscribe();

		virtual void onStart() {}
		virtual void onStop() {}
		
	private:
		std::map<anh::HashString, std::shared_ptr<swganh::baseline::BaselineDeltaInterface>> lookup_;
		std::map<anh::HashString, swganh::baseline::GroupFunctor> groups_;
	};
};

#endif