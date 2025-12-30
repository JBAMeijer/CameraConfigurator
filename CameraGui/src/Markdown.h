#pragma once

#include "imgui_md_wrapper/imgui_md_wrapper.h"
#include "immapp/immapp.h"
#include "opencv2/imgcodecs.hpp"
#include "state.h"

static ImGuiTableFlags table_flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollX;

static void DisplayNode(const MyTreeNode* nodes, int node_count, const char** current_page)
{
    for (int node_i = 0; node_i < node_count; node_i++)
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        const bool is_folder = (nodes[node_i].ChildCount > 0);

        ImGuiTreeNodeFlags node_flags = tree_node_flags_base;

        if (is_folder)
        {
            bool open = ImGui::TreeNodeEx(nodes[node_i].Name, node_flags);
            if (open)
            {
                DisplayNode(nodes[node_i].ChildNodes, nodes[node_i].ChildCount, current_page);
                ImGui::TreePop();
            }
        }
        else
        {
            ImGuiTreeNodeFlags flags = node_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            if (nodes[node_i].page == *current_page)
                flags |= ImGuiTreeNodeFlags_Selected;

            ImGui::TreeNodeEx(nodes[node_i].Name, flags);
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen() && nodes[node_i].page != NULL)
                *current_page = nodes[node_i].page;
        }
    }
}

void DrawPage(const char* page)
{
	ImGui::BeginChild("MarkdownWindow");
	ImGuiMd::Render(page);
	ImGui::EndChild();
}

void TechnicalDocumentsWindow(AppState* state)
{
    static const char* page = test_page;

    ImGui::BeginChild("Yep", ImVec2(ImGui::GetContentRegionAvail().x * 0.25f, 0));

    if (ImGui::BeginTable("3ways", 1, table_flags))
    {
        // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
        ImGui::TableSetupColumn("##Name", ImGuiTableColumnFlags_DefaultHide);
        ImGui::TableHeadersRow();

        DisplayNode(state->Nodes, state->TopNodesCount, &page);

        ImGui::EndTable();
    }
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("applause", ImVec2(0, 0));

    DrawPage(page);

	ImGui::EndChild();
}



