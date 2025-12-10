#pragma once

#include "imgui_md_wrapper/imgui_md_wrapper.h"
#include "immapp/immapp.h"
#include "state.h"

const char* s =
#include "markdown.md"
;

void DrawMarkdown(AppState* state)
{
	ImGuiMd::Render(s);
}