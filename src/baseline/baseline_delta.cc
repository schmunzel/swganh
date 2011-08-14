#include "baseline_delta.h"
#include <anh/component/entity.h>
#include <anh/utilities.h>

using namespace baseline;
using namespace swganh::baseline;
using namespace anh::component;

BaselineDelta::BaselineDelta(const char* type, std::uint8_t id, bool is_private)
	: id_(id), is_private_(is_private)
{
	memcpy(&type_, type, 4);
}

void BaselineDelta::attach_updatable(std::uint16_t id, UpdatableFunctor uf)
{
	updatables_.insert(std::make_pair<std::uint16_t, UpdatableFunctor>(id, uf));
}

void BaselineDelta::detach_updatable(std::uint16_t id)
{
	updatables_.erase(id);
}

anh::ByteBuffer BaselineDelta::build_baseline(std::shared_ptr<Entity> e)
{
	anh::ByteBuffer b;

	b.write<uint16_t>(5);
	b.write<uint32_t>(anh::hostToLittle(BaselinesMessage));
	b.write<uint64_t>(e->id());
	b.write<uint32_t>(type_);
	b.write<uint8_t>(id_);

	b.write<uint32_t>(0xCAFEBABE); //Size Placeholder
	b.write<uint16_t>(updatables_.size());

	std::for_each(updatables_.begin(), updatables_.end(), [&] (const std::pair<std::uint16_t, UpdatableFunctor>& p) {
		p.second(e, b);
	});

	b.writeAt<uint32_t>(19, b.write_position()-23);

	return b;
}

anh::ByteBuffer BaselineDelta::build_delta(std::shared_ptr<Entity> e, swganh::baseline::Updatables::iterator& itr, swganh::baseline::Updatables::iterator& absolute_end)
{
	anh::ByteBuffer b;

	b.write<uint16_t>(5);
	b.write<uint32_t>(DeltasMessage);
	b.write<uint64_t>(e->id());
	b.write<uint32_t>(type_);
	b.write<uint8_t>(id_);

	b.write<uint32_t>(0xCAFEBABE); //Size Placeholder
	b.write<uint16_t>(0xABCD); //Count Placeholder

	std::uint16_t count = 0;
	anh::HashString our_hash = itr->first;
	while(itr != absolute_end)
	{
		if(itr->first != our_hash) {
			break;
		}
		
		auto search = updatables_.find(itr->second);
		if(search != updatables_.end())
		{
			b.write<std::uint16_t>(itr->second);
			search->second(e, b);
			++count;
		}
		++itr;
	}

	//Overwrite placeholders
	b.writeAt<std::uint32_t>(18, b.write_position()-22);
	b.writeAt<std::uint16_t>(22, count);

	return b;
}