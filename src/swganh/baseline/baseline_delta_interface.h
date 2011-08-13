#ifndef BASELINE_DELTA_INTERFACE_H
#define BASELINE_DELTA_INTERFACE_H

#include <anh/hash_string.h>
#include <anh/byte_buffer.h>
#include <functional>
#include <cstdint>
#include <memory>
#include <map>
#include <set>

namespace anh { namespace component {
	class Entity;
}}

namespace swganh { namespace baseline {
	
	typedef std::function<void(std::shared_ptr<anh::component::Entity> e, anh::ByteBuffer& b)> UpdatableFunctor;
	
	#ifndef UPDATABLES
	#define UPDATABLES
	typedef std::pair<anh::HashString, std::uint16_t> Updatable;

	struct UpdatableComp {
		bool operator() ( Updatable lhs, Updatable rhs) {
			return (lhs.first < rhs.first) || ((lhs.first == rhs.first) && lhs.second < rhs.second);
		}
	};

	typedef std::set<Updatable, UpdatableComp> Updatables;
	#endif

	class BaselineDeltaInterface
	{
	public:
		virtual void attach_updatable(std::uint16_t id, UpdatableFunctor uf) = 0;
		virtual void detach_updatable(std::uint16_t id) = 0;

		virtual anh::ByteBuffer build_baseline(std::shared_ptr<anh::component::Entity> e) = 0;
		virtual anh::ByteBuffer build_delta(std::shared_ptr<anh::component::Entity> e, Updatables::iterator& itr, Updatables::iterator& absolute_end) = 0;

		virtual bool is_private() = 0;

	protected:
		enum constants : std::uint32_t
		{
			BaselinesMessage = 0x0C5FA768,
			DeltasMessage = 0x53218612
		};
	};
};};

#endif