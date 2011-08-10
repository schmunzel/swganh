#include "baseline_service.h"

#include <vector>

using namespace baseline;
using namespace swganh::baseline;

baseline_service::baseline_service(std::shared_ptr<anh::app::KernelInterface> kernel) 
	: baseline_service_interface(kernel)
{
}

void baseline_service::attach_baseline_delta(anh::HashString name, std::shared_ptr<baseline_delta_interface> bdi)
{
	lookup_.insert(std::make_pair<anh::HashString, std::shared_ptr<swganh::baseline::baseline_delta_interface>>(name, bdi));
}

void baseline_service::detach_baseline_delta(anh::HashString name)
{
	lookup_.erase(name);
}

void baseline_service::send_baselines(std::shared_ptr<Entity> e, std::list<std::uint64_t> recieving_entities)
{
	//Generate Relevant Baselines
	std::vector<anh::ByteBuffer> generated_baselines;



	//Send Baselines to recieving entities
	std::vector<anh::ByteBuffer>::iterator base_itr;
	std::vector<anh::ByteBuffer>::iterator base_end = generated_baselines.end();

	std::list<std::uint64_t>::iterator recv_iter;
	std::list<std::uint64_t>::iterator recv_end = recieving_entities.end();
	for(recv_iter = recieving_entities.begin(); recv_iter != recv_end; ++recv_iter) {
		for(base_iter = generated_baselines.begin(); base_iter != base_end; ++base_itr) {
			//Send
		}
	}
}

void baseline_service::send_deltas(std::shared_ptr<Entity> e, std::list<std::uint64_t> recieving_entities)
{
	//Generate Relevant Deltas
	std::vector<anh::ByteBuffer> generated_deltas;

	//Send Deltas to recieving entities
	std::vector<anh::ByteBuffer>::iterator base_itr;
	std::vector<anh::ByteBuffer>::iterator base_end = generated_deltas.end();

	std::list<std::uint64_t>::iterator recv_iter;
	std::list<std::uint64_t>::iterator recv_end = recieving_entities.end();
	for(recv_iter = recieving_entities.begin(); recv_iter != recv_end; ++recv_iter) {
		for(base_iter = generated_deltas.begin(); base_iter != base_end; ++base_itr) {
			//Send
		}
	}

}

void baseline_service::send_deltas(std::shared_ptr<Entity> e)
{
	//Generate Relevant Deltas
	std::vector<anh::ByteBuffer> generated_deltas;

	//Send Deltas to recieving entities
	std::vector<anh::ByteBuffer>::iterator base_itr;
	std::vector<anh::ByteBuffer>::iterator base_end = generated_deltas.end();

	///////Container System Magic////////

}