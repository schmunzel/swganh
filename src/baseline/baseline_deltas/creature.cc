#include "creature.h"
#include <baseline/baseline_delta.h>

namespace baseline { namespace baseline_deltas {

	void init_creo1(baseline::BaselineService* serv)
	{
		auto bd = std::make_shared<baseline::BaselineDelta>("CREO", 0x1, true);

		bd->attach_updatable(0, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Bank Credits
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(1, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Cash Credits
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(2, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Ham Value List
			b.write<std::uint32_t>(0);
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(3, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Skill List
			b.write<std::uint32_t>(0);
			b.write<std::uint32_t>(0);
		});

		serv->attach_baseline_delta("CREO1", bd);
	}

	void init_creo4(baseline::BaselineService* serv)
	{
		auto bd = std::make_shared<baseline::BaselineDelta>("CREO", 0x4, true);

		bd->attach_updatable(0, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Acceleration Multiplier Base
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(1, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Acceleration Multiplier Mod
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(2, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Encumberance HAM List
			b.write<std::uint32_t>(0);
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(3, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Skill Mod List
			b.write<std::uint32_t>(0);
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(4, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Speed Multipier Base
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(5, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Speed Multiplier Mod
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(6, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//ListenToID
			b.write<std::uint64_t>(0);
		});

		bd->attach_updatable(7, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//RunSpeed
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(8, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Slope Mod Angle
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(9, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Slope Mod Percent
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(10, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Turn Radius
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(11, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Walk Speed
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(12, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//WaterModPercent
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(13, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//GroupMissionCriticalObjects
			b.write<std::uint32_t>(0);
			b.write<std::uint32_t>(0);
		});

		serv->attach_baseline_delta("CREO4", bd);
	}

	void init_creo3(baseline::BaselineService* serv)
	{
		auto bd = std::make_shared<baseline::BaselineDelta>("CREO", 0x3, false);
		
		bd->attach_updatable(0, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Complexity
			b.write<float>(0.0f);
		});

		bd->attach_updatable(1, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Default Name
			b.write<std::string>("Kronos Powerking");
			b.write<std::uint32_t>(0);
			b.write<std::string>("");
		});

		bd->attach_updatable(2, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Custom Name
			b.write<std::wstring>(L"Kronos Powerking");
		});

		bd->attach_updatable(3, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Volume
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(4, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Customization
			b.write<std::string>("");
		});

		bd->attach_updatable(5, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//ComponentCustomization
			b.write<std::uint32_t>(0);
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(6, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//OptionsBitmask
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(7, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//IncapTimer
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(8, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//ConditionDamage
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(9, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Max Condition
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(10, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//??
			b.write<std::uint8_t>(0);
		});

		bd->attach_updatable(11, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Posture
			b.write<std::uint8_t>(0);
		});

		bd->attach_updatable(12, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Faction Rank
			b.write<std::uint8_t>(0);
		});

		bd->attach_updatable(13, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//OwnerID
			b.write<std::uint64_t>(0);
		});

		bd->attach_updatable(14, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Height
			b.write<float>(0.0f);
		});

		bd->attach_updatable(15, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Battle Fatigue
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(16, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//StatesBitmask
			b.write<std::uint64_t>(0);
		});

		bd->attach_updatable(17, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Wound HAM LIST
			b.write<std::uint32_t>(0);
			b.write<std::uint32_t>(0);
		});

		serv->attach_baseline_delta("CREO3", bd);
	}

	void init_creo6(baseline::BaselineService* serv)
	{
		auto bd = std::make_shared<baseline::BaselineDelta>("CREO", 0x6, true);

		bd->attach_updatable(0, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//??
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(1, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Combat Defender List
			b.write<std::uint32_t>(0);
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(2, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Difficulty Con
			b.write<std::uint16_t>(0);
		});

		bd->attach_updatable(3, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Current Animation
			b.write<std::string>("");
		});

		bd->attach_updatable(4, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//MoodAnimation
			b.write<std::string>("");
		});

		bd->attach_updatable(5, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//WeaponID
			b.write<std::uint64_t>(0);
		});

		bd->attach_updatable(6, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//GroupID
			b.write<std::uint64_t>(0);
		});

		bd->attach_updatable(7, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Invite Sender
			b.write<std::uint64_t>(0);

			//Invite Counter
			b.write<std::uint64_t>(0);
		});

		bd->attach_updatable(8, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Guild ID
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(9, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Target ID
			b.write<std::uint64_t>(0);
		});

		bd->attach_updatable(10, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Mood ID
			b.write<std::uint8_t>(0);
		});

		bd->attach_updatable(11, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Performance Start Time
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(12, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Performance ID
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(13, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Current Ham List
			b.write<std::uint32_t>(0);
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(14, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Max HAM List
			b.write<std::uint32_t>(0);
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(15, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Equipment List
			b.write<std::uint32_t>(0);
			b.write<std::uint32_t>(0);
		});

		bd->attach_updatable(16, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//SetObjectTemplateString
			b.write<std::string>("");
		});

		bd->attach_updatable(17, [] (std::shared_ptr<anh::component::Entity>& e, anh::ByteBuffer& b) {
			//Stationary Flag
			b.write<std::uint8_t>(0);
		});

		serv->attach_baseline_delta("CREO6", bd);
	}

	void init_creature(baseline::BaselineService* serv)
	{
		init_creo1(serv);
		init_creo4(serv);
		init_creo3(serv);
		init_creo6(serv);

		serv->create_group("creature", [serv] (std::shared_ptr<anh::component::Entity> e, std::vector<std::pair<bool, anh::ByteBuffer>>& bs) {
			auto bd = serv->get_baseline_delta("CREO1");
			if(bd != nullptr)
				bs.push_back(std::make_pair<bool, anh::ByteBuffer>(bd->is_private(), bd->build_baseline(e)));

			bd = serv->get_baseline_delta("CREO3");
			if(bd != nullptr)
				bs.push_back(std::make_pair<bool, anh::ByteBuffer>(bd->is_private(), bd->build_baseline(e)));
			
			bd = serv->get_baseline_delta("CREO4");
			if(bd != nullptr)
				bs.push_back(std::make_pair<bool, anh::ByteBuffer>(bd->is_private(), bd->build_baseline(e)));

			bd = serv->get_baseline_delta("CREO6");
			if(bd != nullptr)
				bs.push_back(std::make_pair<bool, anh::ByteBuffer>(bd->is_private(), bd->build_baseline(e)));

		});
	}

}
}