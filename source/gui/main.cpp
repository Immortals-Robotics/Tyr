#include "drawing/VisualizationRenderer.h"
#include "menu/configMenu.h"

int m_width = 1300;
int m_height = 900;

Protos::SSL_GeometryFieldSize* ssl_field;
Protos::SSL_WrapperPacket* ssl_packet;
Protos::SSL_WrapperPacket* ssl_packet_off;
VisualizationRenderer* visualizationRenderer;
ConfigMenu* configMenu;
Tyr::Common::UdpClient* sslClient;
Protos::Immortals::Debug_Draw* world_state;
Protos::Immortals::Debug_Draw* world_state_off;
Tyr::Common::NetworkAddress updatedAddress;

std::mutex vision_mutex;
std::mutex reality_mutex;

bool isVisionUpdating = false;

void DrawSpeedGraph();


void init()
{
	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(m_width, m_height, "Immortals SSL");
	SetTraceLogLevel(LOG_WARNING);
    SetTargetFPS(60);
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

	static bool opened = true, opened2 = true;
	ImVec2 margin = ImVec2(30,30)*2;
	ImVec2 wSize = ImVec2(900.f, 700.f) + margin;
	// TODO: draw gui
	ImGui::SetNextWindowSize(wSize, ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Field", &opened))
	{
		ImGui::End();
	}
	else {
	visualizationRenderer->DrawField(*ssl_field);
	vision_mutex.lock();
	visualizationRenderer->DrawRobots(ssl_packet->detection().robots_blue(), TEAM_COLOR_BLUE);
	visualizationRenderer->DrawRobots(ssl_packet->detection().robots_yellow(), TEAM_COLOR_YELLOW);
	visualizationRenderer->DrawBalls(ssl_packet->detection().balls());
	if (configMenu->IsNetworkDataUpdated() == VISION_PORT || configMenu->IsNetworkDataUpdated() == VISION_IP) {
		updatedAddress.ip = configMenu->GetNetworkParam(VISION_IP);
		updatedAddress.port = static_cast<unsigned short>(std::stoi(configMenu->GetNetworkParam(VISION_PORT)));
		configMenu->UpdateNetworkData();
		sslClient->Update(updatedAddress);
	}
	vision_mutex.unlock();
	
	visualizationRenderer->ApplyShader();
	rlImGuiImageRenderTextureFit(&visualizationRenderer->shaderVisualizationTexture, true);
	ImGui::End();
	}
	// DrawSpeedGraph();
	// end ImGui Content
	configMenu->Draw();

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
	
	visualizationRenderer = new VisualizationRenderer(ImVec2(900.f, 700.f), 4.);
	configMenu = new ConfigMenu(ImVec2(m_width, m_height));

	ssl_field = new Protos::SSL_GeometryFieldSize();
	ssl_field->set_field_length(12000);
	ssl_field->set_field_width(9000);
	ssl_field->set_goal_width(1800);
	ssl_field->set_goal_depth(180);

	ssl_field->set_boundary_width(300);
	ssl_field->set_center_circle_radius(500);
	ssl_field->set_penalty_area_depth(1800);
	ssl_field->set_penalty_area_width(3600);


	ssl_packet_off = new Protos::SSL_WrapperPacket();
	ssl_packet = new Protos::SSL_WrapperPacket();

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
	
	visualizationRenderer->init();
	while (!WindowShouldClose())
	{
		update();
	}

	running = false;
	shutdown();
	vision_thread.join();
	return 0;
}