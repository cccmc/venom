#pragma once
#include <cstdint>
#include <string>

#pragma pack(push, 1)
struct enet_client_t {
	void* vftable;
	std::uint8_t boost_signals[152]; // 4 boost signals
	void* host; // 0xa0
	void* peer; // 0xa8
	std::uint32_t timed_out_timestamp;
	std::uint32_t tracker_tick;
	std::uint32_t network_message_type;
	std::uint32_t unknown_timer;
	std::string server_ip; // 0xc0
	std::uint32_t server_port; // 0xe0
	std::uint32_t unk;
	std::int32_t token; // 0xe8
	std::int32_t user; // 0xec
	std::string door_id; // 0xf0
	std::int32_t lmode; // 0x110
	std::uint32_t unk2;
	std::string uuid_token; // 0x118
};
#pragma pack(pop)
