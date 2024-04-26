#include "drawing/FieldRenderer.h"

int m_width = 1600;
int m_height = 900;

Protos::RoboCup2014Legacy::Geometry::SSL_GeometryFieldSize* ssl_field;
Protos::SSL_WrapperPacket* ssl_packet;
Protos::SSL_WrapperPacket* ssl_packet_off;
FieldRenderer* field_renderer;
Tyr::Common::UdpClient* sslClient;
Protos::Immortals::Debug_Draw* world_state;
Protos::Immortals::Debug_Draw* world_state_off;

std::mutex vision_mutex;
std::mutex reality_mutex;

void DrawSpeedGraph();

void init()
{
	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(m_width, m_height, "Immortals SSL");
    SetTargetFPS(144);
    rlImGuiSetup(true);
}

void shutdown()
{
	rlImGuiShutdown();

    // Close window and OpenGL context
    CloseWindow();
}

void update()
{
	BeginDrawing();
	ClearBackground(DARKGRAY);

	// start ImGui Conent
	rlImGuiBegin();

	static bool opened = true;
	ImVec2 margin = ImVec2(30,30)*2;
	ImVec2 wSize = ImVec2(900.f, 600.f)+margin;
	// TODO: draw gui
	ImGui::SetNextWindowSize(wSize, ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Field", &opened))
	{
		ImGui::End();
	}
	else {
		ImDrawList *draw_list = ImGui::GetWindowDrawList();

		field_renderer->SetWidgetProperties(ImGui::GetWindowPos() + ImGui::GetCursorPos(), ImGui::GetWindowSize() - ImGui::GetCursorPos() * 2.f);
		field_renderer->SetDrawList(draw_list);
		field_renderer->DrawFieldLegacy(*ssl_field);

		vision_mutex.lock();
		field_renderer->DrawBalls(ssl_packet->detection().balls());
		field_renderer->DrawRobots(ssl_packet->detection().robots_blue(), Blue);
		field_renderer->DrawRobots(ssl_packet->detection().robots_yellow(), Yellow);

		DrawSpeedGraph();

		vision_mutex.unlock();
		reality_mutex.lock();
		field_renderer->DrawBalls(world_state->circle());
		reality_mutex.unlock();

		ImGui::End();
	}

	// end ImGui Content
	rlImGuiEnd();

	EndDrawing();

}

std::vector<float> plot_data[12 * 2];
void DrawSpeedGraph()
{
	static bool p_open = true;

	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Speed", &p_open))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	//ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);    // 2/3 of the space for widget and 1/3 for labels
	ImGui::PushItemWidth(800);                                 // Right align, keep 140 pixels for labels

	ImGui::Text("Dear ImGui says hello.");

	if (ImGui::CollapsingHeader("Graphs widgets"))
	{
		ImGui::PlotLines("Lines", plot_data[0].data(), plot_data[0].size(), 0, "x", 0, 4500, ImVec2(0, 250));
		ImGui::PlotLines("Lines", plot_data[5].data(), plot_data[5].size(), 0, "omega", -1024.0f, 1024.0f, ImVec2(0, 250));
	}

	ImGui::End();
}

int main(int argc, char *argv[])
{
	for (auto & data : plot_data)
		data.resize(300, 0.0f);

	init();
	
	field_renderer = new FieldRenderer();

	ssl_field = new Protos::RoboCup2014Legacy::Geometry::SSL_GeometryFieldSize();
	ssl_field->set_field_length(9000);
	ssl_field->set_field_width(6000);
	ssl_field->set_boundary_width(700);
	ssl_field->set_referee_width(300);
	ssl_field->set_center_circle_radius(500);
	ssl_field->set_defense_radius(1000);
	ssl_field->set_defense_stretch(500);
	ssl_field->set_goal_width(1000);
	ssl_field->set_goal_depth(180);

	field_renderer->SetFieldSizeLegacy(*ssl_field);

	ssl_packet_off = new Protos::SSL_WrapperPacket();
	ssl_packet = new Protos::SSL_WrapperPacket();
	auto ball = ssl_packet->mutable_detection()->add_balls();
	ball->set_x(0.f);
	ball->set_y(0.f);
	

	auto robot = ssl_packet->mutable_detection()->add_robots_blue();
	robot->set_x(1500);
	robot->set_confidence(0.7);
	robot->set_orientation(1.5);
	robot = ssl_packet->mutable_detection()->add_robots_yellow();
	robot->set_y(2460);
	robot->set_confidence(0.95);

	sslClient = new Tyr::Common::UdpClient({"224.5.23.2", 10006});

	std::atomic<bool> running(true);

	auto vision_func = [&]()
	{
		std::map<uint32_t, Protos::SSL_DetectionFrame> frame_map;
		Protos::SSL_WrapperPacket tmp_ssl_packet;

		while (running) {
			sslClient->receive(&tmp_ssl_packet);

			if (tmp_ssl_packet.has_detection())
			{
				auto detection = tmp_ssl_packet.detection();
				frame_map[detection.camera_id()] = detection;

				for (auto &robot :  detection.robots_blue())
				{
					if (robot.robot_id() != 1)
						continue;
					plot_data[0].erase(plot_data[0].begin());
					plot_data[0].push_back(robot.x());
					break;
				}
			}

			ssl_packet_off->clear_detection();
			for(auto detection: frame_map)
			{
				ssl_packet_off->mutable_detection()->MergeFrom(detection.second);
			}

			vision_mutex.lock();
			std::swap(ssl_packet, ssl_packet_off);
			vision_mutex.unlock();
		}
	};

	world_state = new Protos::Immortals::Debug_Draw();
	world_state_off = new Protos::Immortals::Debug_Draw();

	std::thread vision_thread(vision_func);

	while (!WindowShouldClose())
	{
		update();
	}

	running = false;
	shutdown();
	vision_thread.join();
	return 0;
}