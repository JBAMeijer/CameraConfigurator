#pragma once
#include <imgui.h>

namespace ImGui {
	void BeginReadOnly(void) {
		PushItemFlag(ImGuiItemFlags_Disabled, true);
		PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	void EndReadOnly(void) {

	}
}