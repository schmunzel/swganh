#include "baseline_service.h"

#include <glog/logging.h>
#include <vector>

#include <anh/app/kernel_interface.h>
#include <anh/component/entity.h>
#include <anh/event_dispatcher/event_dispatcher.h>
#include "anh/network/soe/session.h"

#include <swganh/baseline/baseline_delta_interface.h>
#include <swganh/baseline/baseline_event.h>
#include <swganh/baseline/delta_event.h>
#include <swganh/scene/messages/scene_end_baselines.h>
#include <swganh/connection/connection_client.h>

using namespace baseline;
using namespace swganh::baseline;
using namespace anh::component;

BaselineService::BaselineService(std::shared_ptr<anh::app::KernelInterface> kernel) 
	: BaselineServiceInterface(kernel)
{
}

void BaselineService::attach_baseline_delta(anh::HashString name, std::shared_ptr<BaselineDeltaInterface> bdi)
{
	lookup_.insert(std::make_pair<anh::HashString, std::shared_ptr<swganh::baseline::BaselineDeltaInterface>>(name, bdi));
}

void BaselineService::detach_baseline_delta(anh::HashString name)
{
	lookup_.erase(name);
}
void BaselineService::send_baselines_self(std::shared_ptr<anh::component::Entity> e, std::shared_ptr<swganh::connection::ConnectionClient> c) 
{
    std::list<std::shared_ptr<anh::component::Entity>> self;
    self.push_back(e);
    send_baselines(e, c, self);
}
void BaselineService::send_baselines(std::shared_ptr<Entity> e, std::shared_ptr<swganh::connection::ConnectionClient> c, std::list<std::shared_ptr<anh::component::Entity>> recieving_entities)
{
	if(e == nullptr)
		return;

	//Generate Relevant Baselines
	std::vector<std::pair<bool, anh::ByteBuffer>> generated_baselines;

	std::set<anh::HashString> tags = e->Tags();
	auto end = tags.end();
	for(auto itr = tags.begin(); itr != end; ++itr)
	{
		auto search = groups_.find(*itr);
		if(search != groups_.end())
		{
			search->second(e, generated_baselines);
			break;
		}
	}

	if(generated_baselines.size() > 0)
	{
		//Send Baselines to recieving entities
		std::vector<std::pair<bool, anh::ByteBuffer>>::iterator base_itr;
		std::vector<std::pair<bool, anh::ByteBuffer>>::iterator base_end = generated_baselines.end();

		std::list<std::shared_ptr<anh::component::Entity>>::iterator recv_itr;
		std::list<std::shared_ptr<anh::component::Entity>>::iterator recv_end = recieving_entities.end();
		for(recv_itr = recieving_entities.begin(); recv_itr != recv_end; ++recv_itr) {
			for(base_itr = generated_baselines.begin(); base_itr != base_end; ++base_itr) {
				if(!base_itr->first || (base_itr->first && *recv_itr == e))
				{
					c->session->SendMessage((*base_itr).second);
				}
			}
		}
        // send end baseline
        swganh::scene::messages::SceneEndBaselines seb;
        seb.object_id = e->id();
        c->session->SendMessage(seb);
	}
	else
	{
		LOG(WARNING) << "Could not send Baselines for " << e->id() << " because it is missing the proper tag." << std::endl;
	}
}

void BaselineService::send_deltas(std::shared_ptr<Entity> e, std::list<std::shared_ptr<anh::component::Entity>> recieving_entities)
{
	//Generate Relevant Deltas
	std::vector<std::pair<bool, anh::ByteBuffer>> generated_deltas;

	swganh::baseline::Updatables updates;
	e->swap_updates(updates);

	auto itr = updates.begin();
	auto end = updates.end();

	while(itr != end) 
	{
		auto search = lookup_.find(itr->first);
		if(search != lookup_.end()) 
		{
			generated_deltas.push_back(std::make_pair<bool, anh::ByteBuffer>(search->second->is_private(), search->second->build_delta(e, itr, end)));
		} 
		else 
		{
			LOG(WARNING) << "A delta with CRC " << itr->first.ident() << " was pushed that is not handled." << std::endl;
			++itr;
		}
	}

	//Send Deltas to recieving entities
	std::vector<std::pair<bool, anh::ByteBuffer>>::iterator base_itr;
	std::vector<std::pair<bool, anh::ByteBuffer>>::iterator base_end = generated_deltas.end();

	std::list<std::shared_ptr<anh::component::Entity>>::iterator recv_itr;
	std::list<std::shared_ptr<anh::component::Entity>>::iterator recv_end = recieving_entities.end();
	for(recv_itr = recieving_entities.begin(); recv_itr != recv_end; ++recv_itr) {
		for(base_itr = generated_deltas.begin(); base_itr != base_end; ++base_itr) {
			if(!base_itr->first || (base_itr->first && *recv_itr == e))
			{
				//Send
			}
		}
	}

}

void BaselineService::send_deltas(std::shared_ptr<Entity> e)
{
	//Generate Relevant Deltas
	std::vector<std::pair<bool, anh::ByteBuffer>> generated_deltas;

	swganh::baseline::Updatables updates;
	e->swap_updates(updates);

	auto itr = updates.begin();
	auto end = updates.end();

	while(itr != end)
	{
		auto search = lookup_.find(itr->first);
		if(search != lookup_.end())
		{
			generated_deltas.push_back(std::make_pair<bool, anh::ByteBuffer>(search->second->is_private(), search->second->build_delta(e, itr, end)));
		}
		else
		{
			LOG(WARNING) << "A delta with CRC " << itr->first.ident() << " was pushed that is not handled." << std::endl;
			++itr;
		}
	}

	//Send Deltas to recieving entities
	std::vector<std::pair<bool, anh::ByteBuffer>>::iterator base_itr;
	std::vector<std::pair<bool, anh::ByteBuffer>>::iterator base_end = generated_deltas.end();
}

anh::service::ServiceDescription BaselineService::GetServiceDescription()
{
	anh::service::ServiceDescription service_description(
        "ANH Baseline/Delta Service",
        "baseline",
        "0.1",
        "127.0.0.1", 
        0, 
        0, 
        0);

	return service_description;
}

void BaselineService::DescribeConfigOptions(boost::program_options::options_description& description) {}

void BaselineService::subscribe()
{
	kernel()->GetEventDispatcher()->subscribe(anh::HashString("BaselineEvent"), [this] (std::shared_ptr<anh::event_dispatcher::EventInterface> e) -> bool {
		auto actual_event = std::dynamic_pointer_cast<swganh::baseline::BaselineEvent, anh::event_dispatcher::EventInterface>(e);
		if(actual_event->entity != nullptr)
		{
			//send_baselines(actual_event->entity, actual_event->receiving_entities);
			return true;
		}
		return false;
	});

	kernel()->GetEventDispatcher()->subscribe(anh::HashString("DeltaEvent"), [this] (std::shared_ptr<anh::event_dispatcher::EventInterface> e) -> bool {
		auto actual_event = std::dynamic_pointer_cast<swganh::baseline::DeltaEvent, anh::event_dispatcher::EventInterface>(e);
		if(actual_event->entity != nullptr)
		{
			send_deltas(actual_event->entity);
			return true;
		}
		return false;
	});
}

void BaselineService::create_group(anh::HashString name, swganh::baseline::GroupFunctor functor)
{
	groups_.insert(std::make_pair<anh::HashString, swganh::baseline::GroupFunctor>(name, functor));
}

void BaselineService::remove_group(anh::HashString name)
{
	groups_.erase(name);
}

std::shared_ptr<swganh::baseline::BaselineDeltaInterface> BaselineService::get_baseline_delta(anh::HashString name)
{
	auto itr = lookup_.find(name);
	if(itr != lookup_.end()) {
		return itr->second;
	}
	return nullptr;
}