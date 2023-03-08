#include <game/gt.hpp>
#include <utils/memory.hpp>
#include <utils/console.hpp>

#include <stdexcept>
#include <string>
#include <format>

bool patch_integrity_check() noexcept {
	std::uintptr_t integrity_check = memory::find_pattern("00 3b c1 75 08 85 c9", 3);

	if (integrity_check == 0)
		return memory::find_pattern("00 3b c1 90 90 85 c9") != 0; // check if it's already patched

	return memory::patch_bytes(integrity_check, "90 90");
}

enum class find_mode {
	normal,
	call,
	load
};

void find_address(auto& dest, std::string_view pattern, find_mode mode = find_mode::normal, std::intptr_t offset = 0) {
	std::uintptr_t address = memory::find_pattern(pattern, offset);

	switch (mode) {
	case find_mode::call:
		address = memory::get_address_from_call(address);
		break;

	case find_mode::load:
		address = memory::get_address_from_load(address);
		break;

	default:
		break;
	}

	if (address == 0)
		throw std::runtime_error(std::format("pattern '{}' not found", pattern));

	dest = reinterpret_cast<decltype(dest)>(address);
}

void find_addresses() {
	find_address(gt::get_app, "c3 e8 ? ? ? ? 48 8b c8 33 d2", find_mode::call, 1);
	find_address(gt::get_client, "75 ? e8 ? ? ? ? 48 83 b8", find_mode::call, 2);
	find_address(gt::get_game_logic, "e8 ? ? ? ? 8b 17 48 8d ? ? ? ? ? e8 ? ? ? ? 48 8b", find_mode::call);

	find_address(gt::set_fps_limit, "e8 ? ? ? ? e8 ? ? ? ? 83 e8", find_mode::call);
	find_address(gt::log_to_console, "e8 ? ? ? ? 48 8b c8 e8 ? ? ? ? 90 48 8d 4d ? e8 ? ? ? ? e8", find_mode::call, 8);
	find_address(gt::send_packet, "02 00 00 00 e8 ? ? ? ? 90 48 8d 4c 24 50", find_mode::call, 4);
	find_address(gt::send_packet_raw, "e8 ? ? ? ? 48 83 c4 ? c3 cc 4c 8b dc 48 83 ec", find_mode::call);
	find_address(gt::on_text_game_message, "48 8b d3 e8 ? ? ? ? eb ? 49 8b 4f", find_mode::call, 3);
	find_address(gt::process_tank_update_packet, "48 8b d3 e8 ? ? ? ? eb ? 48 8d 0d", find_mode::call, 3);
	find_address(gt::handle_track_packet, "48 8b 88 ? ? ? ? e8 ? ? ? ? eb ? 48 8d 0d", find_mode::call, 7);

	find_address(gt::renderer, "48 8b 05 ? ? ? ? 75 ? c6 43", find_mode::load);

	// get end_scene from d3d9 device's vftable
	gt::end_scene = reinterpret_cast<decltype(gt::end_scene)>((*reinterpret_cast<void***>(gt::renderer->device_ex))[42]);
}

void gt::setup() {
	using namespace console;

	MODULEINFO module_info = {};
	if (!GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &module_info, sizeof(MODULEINFO)))
		throw std::runtime_error("failed to get module information");

	base_address = reinterpret_cast<std::uintptr_t>(module_info.lpBaseOfDll);
	end_address = base_address + module_info.SizeOfImage;
	if (base_address == 0 || end_address == 0)
		throw std::runtime_error("invalid module address");

	hwnd = FindWindow(nullptr, "Growtopia");
	if (hwnd == nullptr)
		throw std::runtime_error("growtopia window not found");

	if (!patch_integrity_check())
		throw std::runtime_error("failed to patch integrity check");
	
	console::setup();
	console::print<console::color::red>("venom, growtopia cheat by cmc\n\n");
	
	print_good("patched integrity check");

	find_addresses();
	print_good("found addresses");

	set_fps_limit(get_app(), 0.f);
	print_good("unlocked fps limit");
}

void gt::send_generic_text(const std::string& packet) noexcept {
	enet_client_t* client = gt::get_client();

	if (client == nullptr || client->peer == nullptr)
		return;

	send_packet(net_message_type::generic_text, packet, client->peer);
}

void gt::send_game_message(const std::string& packet) noexcept {
	enet_client_t* client = gt::get_client();

	if (client == nullptr || client->peer == nullptr)
		return;

	send_packet(net_message_type::game_message, packet, client->peer);
}

void gt::send_game_packet(const game_packet_t& packet) noexcept {
	enet_client_t* client = gt::get_client();

	if (client == nullptr || client->peer == nullptr)
		return;

	send_packet_raw(net_message_type::game_packet, &packet, sizeof(game_packet_t), nullptr, client->peer, 0x1);
}

void gt::process_game_message(const std::string& packet) noexcept {
	game_logic_component_t* game_logic = gt::get_game_logic();

	if (game_logic == nullptr)
		return;

	on_text_game_message(game_logic, packet.c_str());
}

void gt::process_game_packet(const game_packet_t& packet) noexcept {
	game_logic_component_t* game_logic = gt::get_game_logic();

	if (game_logic == nullptr)
		return;

	process_tank_update_packet(game_logic, const_cast<game_packet_t*>(&packet));
}
