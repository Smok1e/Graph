#include <ImGuiExtra.hpp>

//========================================

bool ImGui::ColorEdit3(const char* label, sf::Color* color, ImGuiColorEditFlags flags /*= 0*/)
{
	float channels[3] = {
		static_cast<float>(color->r) / 0xFF,
		static_cast<float>(color->g) / 0xFF,
		static_cast<float>(color->b) / 0xFF
	};

	if (ImGui::ColorEdit3(label, channels, flags))
	{
		color->r = 0xFF * channels[0];
		color->g = 0xFF * channels[1];
		color->b = 0xFF * channels[2];

		return true;
	}

	return false;
}

//========================================