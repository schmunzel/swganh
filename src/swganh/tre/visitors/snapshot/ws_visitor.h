// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef VISITORS_WS_H
#define VISITORS_WS_H

#include "../visitor_interface.h"
#include <cstdint> //for the std::uint32_t and std::uint64_t, ect
#include <vector> //for storing names
#include <map> //For storing chunks
#include <glm/glm.hpp> //For vec3
#include <glm/gtx/quaternion.hpp> //for the quaternions

namespace swganh
{
namespace tre
{
	/**
		@brief An IFFInterpreter for ws files.
	*/
	class WsVisitor : public VisitorInterface
	{
	public:
		static const VisitorType Type = WS_VISITOR;

		/**
			@brief Constructor for a ws interpreter
		*/
		WsVisitor();

		/**
			@brief interprets a IFF::FileNode associated with this interpreter.
			This should only be called by the IFFFile code.
		*/
		virtual void visit_data(uint32_t depth, std::string name, uint32_t size, anh::ByteBuffer& data);

		/**
			@brief interprets a IFF::FolderNode associated with this interpreter.
			This should only be called by the IFFFile code.
		*/
		virtual void visit_folder(uint32_t depth, std::string name, uint32_t size);

		/**
			@brief A single chunk object in the world 
		*/
		struct CHUNK
		{
			std::uint64_t id;
			std::uint64_t parent_id;
			std::uint32_t name_id;
			float scale;
			glm::quat orientation;
			glm::vec3 location;
			float object_priority; //could also be type, but I don't buy it.
			std::uint32_t pob_crc;
		};

		const std::map<std::uint64_t, CHUNK>& chunk_map() { return chunks; }

		//This interface should be satisfactory
		size_t name_count() { return names.size(); }
		std::string name(size_t id) { return names[id];}

	private:

		std::map<std::uint64_t, CHUNK> chunks;
		std::vector<std::string> names;

		//Internal Node Handling functions
		void _handleOTNL(anh::ByteBuffer& buffer);
		void _handle0000DATA(anh::ByteBuffer& buffer);
	};
}
}
#endif