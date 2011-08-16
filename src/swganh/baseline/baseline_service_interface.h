#ifndef BASELINE_SERVICE_INTERFACE_H
#define BASELINE_SERVICE_INTERFACE_H

#include <swganh/base/base_service.h>
#include <anh/hash_string.h>
#include <anh/byte_buffer.h>
#include <memory>
#include <cstdint>
#include <list>

namespace anh { namespace component {
	class Entity;
}} // namespace anh::component
namespace swganh{
namespace connection{
    struct ConnectionClient;
}} // namespace swganh::connection

namespace swganh { namespace baseline {

	class BaselineDeltaInterface;
	typedef std::function<void(std::shared_ptr<anh::component::Entity>, std::vector<std::pair<bool, anh::ByteBuffer>>&)> GroupFunctor;

	class BaselineServiceInterface : public swganh::base::BaseService
	{
	public:

		BaselineServiceInterface(std::shared_ptr<anh::app::KernelInterface> kernel) : BaseService(kernel) {}

		virtual void create_group(anh::HashString name, GroupFunctor functor) = 0;
		virtual void remove_group(anh::HashString name) = 0;

		virtual void attach_baseline_delta(anh::HashString name, std::shared_ptr<BaselineDeltaInterface> bdi) = 0;
		virtual std::shared_ptr<swganh::baseline::BaselineDeltaInterface> get_baseline_delta(anh::HashString name) = 0;
		virtual void detach_baseline_delta(anh::HashString name) = 0;

        virtual void send_baselines_self(std::shared_ptr<anh::component::Entity> e) = 0;
		virtual void send_baselines(std::shared_ptr<anh::component::Entity> e, std::list<std::shared_ptr<anh::component::Entity>> recieving_entities) = 0;
		virtual void send_deltas(std::shared_ptr<anh::component::Entity> e, std::list<std::shared_ptr<anh::component::Entity>> recieving_entities) = 0;
		virtual void send_deltas(std::shared_ptr<anh::component::Entity> e) = 0;

		virtual anh::service::ServiceDescription GetServiceDescription() = 0;

		virtual void onStart() = 0;
		virtual void onStop() = 0;
		virtual void subscribe() = 0;

		virtual void DescribeConfigOptions(boost::program_options::options_description& description) = 0;
	};
};};

#endif