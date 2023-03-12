#include <hooks/hooks.hpp>
#include <menu/menu.hpp>
#include <game/gt.hpp>
#include <external/minhook/include/MinHook.h>
#include <external/proton/variant2.hpp>
#include <utils/console.hpp>
#include <utils/memory.hpp>

#include <stdexcept>
#include <format>
#include <sstream>

void hook_function(auto& function, const auto& hook_function) {
	void* address = function;
	void* original = nullptr;

	if (MH_CreateHook(address, hook_function, &original) != MH_OK)
		throw std::runtime_error("failed to create hook");

	if (MH_EnableHook(address) != MH_OK)
		throw std::runtime_error("failed to enable hook");

	function = reinterpret_cast<decltype(function)>(original);
}

void hooks::install() {
	if (MH_Initialize() != MH_OK)
		throw std::runtime_error("failed to initialize minhook");

	hook_function(gt::send_packet, send_packet_hook);
	hook_function(gt::send_packet_raw, send_packet_raw_hook);
	hook_function(gt::on_text_game_message, on_text_game_message_hook);
	hook_function(gt::process_tank_update_packet, process_tank_update_packet_hook);
	hook_function(gt::handle_track_packet, handle_track_packet_hook);
	hook_function(gt::on_punched, on_punched_hook);
	hook_function(gt::is_darkened, is_darkened_hook);
	hook_function(gt::is_anzu_platform, is_anzu_platform_hook);
	hook_function(gt::collide, collide_hook);

	hook_function(gt::end_scene, end_scene_hook);
	original_wnd_proc = reinterpret_cast<WNDPROC>(SetWindowLongPtrA(gt::hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&wnd_proc_hook)));

	console::print_good("installed hooks");
}

void hooks::send_packet_hook(net_message_type type, const std::string& packet, void* peer) {
	using enum net_message_type;

	switch (type) {
	case generic_text:
		console::println<console::color::cyan>(std::format("sending generic text: {}", packet));
		break;

	case game_message:
		console::println<console::color::blue>(std::format("sending game message: {}", packet));
		break;

	default:
		console::println<console::color::yellow>(std::format("sending unknown packet: {}", packet));
		break;
	}

	gt::send_packet(type, packet, peer);
}


void hooks::send_packet_raw_hook(net_message_type type, const void* data, std::int32_t data_size, std::uint8_t* unk1, void* peer, std::uint32_t flags) {
	if (type != net_message_type::game_packet || data_size != sizeof(game_packet_t)) // this should never happen
		return;

	game_packet_t* packet = const_cast<game_packet_t*>(static_cast<const game_packet_t*>(data));

	switch (packet->type) {
	case game_packet_type::state:
		if (cheats::anti_damage) {
			memory::remove_bit(packet->flags, visual_state::on_fire_damage);
			memory::remove_bit(packet->flags, visual_state::on_damage);
			memory::remove_bit(packet->flags, visual_state::on_acid_damage);
		}

		if (cheats::fake_lag)
			memory::set_bit(packet->flags, visual_state::on_spawn);

		if (cheats::super_punch && memory::has_bit(packet->flags, visual_state::on_punched))
			memory::set_bit(packet->flags, visual_state::on_power_up_punch_end);

		// if you send walk packets in air you will get banned so lets set player velocity as 0 so server thinks we are not walking
		if (cheats::walk_in_air)
			packet->vec2 = {0.f, 0.f};

		break;

	case game_packet_type::got_punched:
		// this also blocks rapier effect and game generator game damage
		if (cheats::anti_zombie)
			return;
		break;

	case game_packet_type::app_integrity_fail:
		gt::log_to_console(std::format("`4venom:`` `oblocking client hack report`` `5(type {})``", packet->int3));
		return;
		break;

	default:
		break;
	}

	console::println<console::color::magenta>(std::format("sending game packet, type: {}", static_cast<int>(packet->type)));

	gt::send_packet_raw(type, data, data_size, unk1, peer, flags);
}

void hooks::process_tank_update_packet_hook(game_logic_component_t* _this, game_packet_t* packet) {

	switch (packet->type)
	{
		case game_packet_type::call_function:
		{
			variantlist_t varlist{};
			if (varlist.serialize_from_mem(packet->get_extra_data()))
			{
				console::println<console::color::dark_yellow>(std::format("varlist: {}", varlist.print()));
			}
		}break;
	}
	gt::process_tank_update_packet(_this, packet);
}

void hooks::on_text_game_message_hook(game_logic_component_t* _this, const char* packet) {

	gt::on_text_game_message(_this, packet);
}

void hooks::handle_track_packet_hook(track_handler_component_t* _this, const char* packet) {

	gt::handle_track_packet(_this, packet);
}

void hooks::on_punched_hook(net_avatar_t* _this, vec2f_t unk1, net_avatar_t* puncher) {
	if (cheats::anti_knockback)
		return;

	gt::on_punched(_this, unk1, puncher);
}

std::int32_t hooks::is_darkened_hook(std::int32_t item_id) {
	if (cheats::night_vision)
		return 0;

	return gt::is_darkened(item_id);
}

bool hooks::is_anzu_platform_hook() {
	if (cheats::block_ads)
		return false;

	return gt::is_anzu_platform();
}

bool hooks::collide_hook(world_tile_map_t* _this, float unk1, float unk2, float unk3, float unk4, int unk5, bool unk6) {
	if (cheats::walk_in_air)
		return true;

	return gt::collide(_this, unk1, unk2, unk3, unk4, unk5, unk6);
}

HRESULT hooks::end_scene_hook(IDirect3DDevice9* _this) {
	menu::render();
	return gt::end_scene(_this);
}

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

LRESULT hooks::wnd_proc_hook(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
		return TRUE;

	if (msg == WM_KEYUP && wparam == VK_INSERT)
		menu::show = !menu::show;

	const ImGuiIO& io = ImGui::GetIO();

	bool mouse =
		msg == WM_LBUTTONDOWN ||
		msg == WM_RBUTTONDOWN ||
		msg == WM_LBUTTONDBLCLK ||
		msg == WM_RBUTTONDBLCLK ||
		msg == WM_MOUSEWHEEL;

	if (io.WantTextInput || (io.WantCaptureMouse && mouse))
		return TRUE;

	return original_wnd_proc(hwnd, msg, wparam, lparam);
}
