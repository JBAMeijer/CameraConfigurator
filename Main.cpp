#define IMGUI_BUNDLE_WITH_IMGUI_NODE_EDITOR
#define _CRT_SECURE_NO_WARNINGS

#include "immapp/immapp.h"
#include "imgui.h"

#include <fstream>
#include <math.h>
#include "state.h"
#include "IO.h"
#include "Markdown.h"
//#include "glad/glad.h"

#define M_PI   3.14159265358979323846264338327950288
#define DegToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)
#define RadToDeg(angleInRadians) ((angleInRadians) * 180.0 / M_PI)

struct InputData {
	uint32_t string_size = 0;
};

static int CharCallback(ImGuiInputTextCallbackData* data) {
	InputData* d = (InputData*)data->UserData;
	d->string_size = data->BufTextLen;
	return 0;
}

void CameraWindow(AppState* state) {
	static uint8_t prop_selected = 0;
	static uint8_t delete_prop_selected = 0;

	static bool ButtonOk = false;

	static char cameraName[CAMERA_NAME_STRING_SIZE] = "";
	static CameraProperties props;
	static InputData inputData;

	if (state->cameras.itemsStored > 0)
	{
		if (ImGui::TreeNodeEx("Existing camera configurations", ImGuiTreeNodeFlags_DefaultOpen))
		{
			const char* combo_val = state->cameras.name[prop_selected];
			if (ImGui::BeginCombo("Camera", combo_val))
			{
				for (int n = 0; n < state->cameras.itemsStored; n++)
				{
					const bool is_selected = (prop_selected == n);
					if (ImGui::Selectable(state->cameras.name[n], is_selected))
						prop_selected = n;

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			ImGui::InputFloat("Pixel size (um)", &state->cameras.cProps[prop_selected].pixelSize, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat("Sensor diagonal (mm)", &state->cameras.cProps[prop_selected].sensorDiagonal, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat("Sensor width (mm)", &state->cameras.cProps[prop_selected].sensorWidth, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat("Sensor height (mm)", &state->cameras.cProps[prop_selected].sensorHeight, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputInt("Pixel width", &state->cameras.cProps[prop_selected].pixelWidth, 0, 100, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputInt("Pixel height", &state->cameras.cProps[prop_selected].pixelHeight, 0, 100, ImGuiInputTextFlags_ReadOnly);

			ImGui::TreePop();
		}
	}

	if (ImGui::TreeNode("Create new camera"))
	{
		ImGui::InputText("Camera name", cameraName, CAMERA_NAME_STRING_SIZE-1, ImGuiInputTextFlags_CallbackEdit, CharCallback, (void*)(&inputData));
		ImGui::InputFloat("Pixel size (um)", &props.pixelSize);
		ImGui::InputInt("Pixel width", &props.pixelWidth, 0);
		ImGui::InputInt("Pixel height", &props.pixelHeight, 0);

		ButtonOk = ImGui::Button("Ok");

		ImGui::TreePop();
	}

	if (state->cameras.itemsStored > 0)
	{
		if (ImGui::TreeNode("Delete camera"))
		{
			const char* combo_val = state->cameras.name[delete_prop_selected];
			if (ImGui::BeginCombo("Camera##delete", combo_val))
			{
				for (int n = 0; n < state->cameras.itemsStored; n++)
				{
					const bool is_selected = (delete_prop_selected == n);
					if (ImGui::Selectable(state->cameras.name[n], is_selected))
						delete_prop_selected = n;

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("Delete##cammera"))
				DeleteCamera(&state->cameras, delete_prop_selected);

			ImGui::TreePop();
		}
	}

	if (ButtonOk) {
		props.sensorWidth = props.pixelWidth * (props.pixelSize / 1000.f);
		props.sensorHeight = props.pixelHeight * (props.pixelSize / 1000.f);
		props.sensorDiagonal = sqrtf(props.sensorWidth * props.sensorWidth + props.sensorHeight * props.sensorHeight);

		if (inputData.string_size == CAMERA_NAME_STRING_SIZE)
			cameraName[CAMERA_NAME_STRING_SIZE-1] = '\n';
		else
			cameraName[inputData.string_size] = '\n';

		AddOrUpdateCamera(&state->cameras, cameraName, &props);
		memset(cameraName, '\0', CAMERA_NAME_STRING_SIZE);
	}
}

void CameraDistanceCalculationView(AppState* state)
{
	static const float FOCAL_DISTANCES[] = { 4.0, 6.0, 8.0, 12.0, 16.0, 25.0, 35.0 };
	static const char* FOCAL_DISTANCES_STR[] = { "4.0", "6.0", "8.0", "12.0", "16.0", "25.0", "35.0" };

	static uint8_t propSelectedCamera = 0;
	static uint8_t propSelectedFocal = 0;
	static int32_t objectLength = 400;
	static int32_t objectWidth = 300;
	static int32_t objectDepth = 50;
	static float focalDistance = 12.f;
	static float openingAngle = 10.f;
	static bool useDiagonal = true;

	static CameraProperties cprop = {0};

	static bool cameraManualInput = false;
	static bool lensManualInput = false;

	ImGui::SeparatorText("Object information");
	ImGui::InputInt("Object length (mm)", &objectLength, 0);
	ImGui::InputInt("Object width (mm)", &objectWidth, 0);
	ImGui::InputInt("Object depth (mm)", &objectDepth, 0);

	if (ImGui::CollapsingHeader("Working distance based on camera and lens", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SeparatorText("Camera and lens settings");
		ImGui::Text("Camera settings");
		if (state->cameras.itemsStored == 0)
			ImGui::Text("No camera's configured");
		else {
			if (cameraManualInput)
				ImGui::BeginDisabled();

			const char* combo_val = state->cameras.name[propSelectedCamera];
			if (ImGui::BeginCombo("Camera", combo_val))
			{
				for (int n = 0; n < state->cameras.itemsStored; n++)
				{
					const bool is_selected = (propSelectedCamera == n);
					if (ImGui::Selectable(state->cameras.name[n], is_selected))
						propSelectedCamera = n;

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (cameraManualInput)
				ImGui::EndDisabled();

			//ImGui::SameLine();
			ImGui::Checkbox("Manual input##camera", &cameraManualInput);

			if (!cameraManualInput) {
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.6f);
				ImGui::InputFloat("Pixel size (um)", &state->cameras.cProps[propSelectedCamera].pixelSize, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
				ImGui::InputFloat("Sensor diagonal (mm)", &state->cameras.cProps[propSelectedCamera].sensorDiagonal, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
				ImGui::InputFloat("Sensor width (mm)", &state->cameras.cProps[propSelectedCamera].sensorWidth, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
				ImGui::InputFloat("Sensor height (mm)", &state->cameras.cProps[propSelectedCamera].sensorHeight, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
				ImGui::InputInt("Pixel width", &state->cameras.cProps[propSelectedCamera].pixelWidth, 0, 100, ImGuiInputTextFlags_ReadOnly);
				ImGui::InputInt("Pixel height", &state->cameras.cProps[propSelectedCamera].pixelHeight, 0, 100, ImGuiInputTextFlags_ReadOnly);
				ImGui::PopStyleVar();
				cprop = state->cameras.cProps[propSelectedCamera];
			}
			else {
				bool changed = false;
				changed = ImGui::InputFloat("Pixel size (um)", &cprop.pixelSize, 0.0f, 0.0f, "%.3f") ? !changed : changed;
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.6f);
				ImGui::InputFloat("Sensor diagonal (mm)", &cprop.sensorDiagonal, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
				ImGui::InputFloat("Sensor width (mm)", &cprop.sensorWidth, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
				ImGui::InputFloat("Sensor height (mm)", &cprop.sensorHeight, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
				ImGui::PopStyleVar();
				changed = ImGui::InputInt("Pixel width", &cprop.pixelWidth, 0, 100) ? !changed : changed;
				changed = ImGui::InputInt("Pixel height", &cprop.pixelHeight, 0, 100) ? !changed : changed;

				if (changed) {
					cprop.sensorWidth = (cprop.pixelSize / 1000.f) * cprop.pixelWidth;
					cprop.sensorHeight = (cprop.pixelSize / 1000.f) * cprop.pixelHeight;
					cprop.sensorDiagonal = sqrtf(cprop.sensorWidth * cprop.sensorWidth + cprop.sensorHeight * cprop.sensorHeight);
				}
			}

			ImGui::Text("Lens settings");

			if (lensManualInput)
				ImGui::BeginDisabled();

			const char* combo_val_dist = FOCAL_DISTANCES_STR[propSelectedFocal];
			if (ImGui::BeginCombo("Focal distance (mm)", combo_val_dist))
			{
				for (int n = 0; n < sizeof(FOCAL_DISTANCES) / sizeof(FOCAL_DISTANCES[0]); n++)
				{
					const bool is_selected = (propSelectedFocal == n);
					if (ImGui::Selectable(FOCAL_DISTANCES_STR[n], is_selected))
						propSelectedFocal = n;

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (lensManualInput)
				ImGui::EndDisabled();

			//ImGui::SameLine();
			ImGui::Checkbox("Manual input##lens", &lensManualInput);

			if (lensManualInput)
				ImGui::InputFloat("##fd_input", &focalDistance);
			else
				focalDistance = FOCAL_DISTANCES[propSelectedFocal];

			ImGui::SeparatorText("Calculated working distance");


			float selectedCameraSensorWidth = 0.f;
			float selectedCameraSensorHeight = 0.f;

			if (!cameraManualInput) {
				selectedCameraSensorWidth = state->cameras.cProps[propSelectedCamera].sensorWidth;
				selectedCameraSensorHeight = state->cameras.cProps[propSelectedCamera].sensorHeight;
			}
			else {
				selectedCameraSensorWidth = cprop.sensorWidth;
				selectedCameraSensorHeight = cprop.sensorHeight;
			}

			float dist1 = focalDistance * objectLength / selectedCameraSensorWidth;
			float dist2 = focalDistance * objectWidth / selectedCameraSensorHeight;

			float dist3 = focalDistance * objectWidth / selectedCameraSensorWidth;
			float dist4 = focalDistance * objectLength / selectedCameraSensorHeight;

			float dist12 = fmaxf(dist1, dist2);
			float dist34 = fmaxf(dist3, dist4);

			float workingDist = fminf(dist12, dist34) + objectDepth;

			ImGui::InputFloat("Working distance (mm)##sensor", &workingDist, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
		}
	}
	if (ImGui::CollapsingHeader("Working distance based on opening angle of lens")) {
		ImGui::InputFloat("Opening angle of lens", &openingAngle);
		if (openingAngle < 1.0)
			openingAngle = 1.0;

		ImGui::Checkbox("Use object diagonal as longest edge", &useDiagonal);

		float longestObjectLength = 0;
		if (useDiagonal)
			longestObjectLength = sqrtf(objectLength * objectLength + objectWidth * objectWidth);
		else
			longestObjectLength = fmaxf(objectLength, objectWidth);

		float workingDist = (longestObjectLength / 2) / tan(DegToRad(openingAngle / 2));

		ImGui::InputFloat("Working distance (mm)##opening angle", &workingDist, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
	}
}

void Gui(AppState* state)
{
    if (ImGui::BeginTabBar("TabBarItems", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Camera"))
        {
			CameraDistanceCalculationView(state);
            ImGui::EndTabItem();
        }
		if (ImGui::BeginTabItem("Markdown"))
		{
			DrawMarkdown(state);
			ImGui::EndTabItem();
		}
        if (ImGui::BeginTabItem("Debug"))
        {
            ImGui::Text("CameraModalEnabled: %d", state->cameraModalEnabled);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    if (state->cameraModalEnabled)
    {
        if (ImGui::Begin("Camera config", &state->cameraModalEnabled)) CameraWindow(state);
        ImGui::End();
    }
}

void ShowMenuGui(AppState* state)
{
    if (ImGui::BeginMenu("Settings")) {
        if (ImGui::MenuItem("Camera's")) state->cameraModalEnabled = true;

        ImGui::EndMenu();
    }
}

void OnNewFrame(AppState* state)
{
	if (ImGui::IsKeyChordPressed(ImGuiKey::ImGuiMod_Shift | ImGuiKey::ImGuiKey_Equal))
		if (ImGui::GetStyle().FontScaleMain + 0.2 < 5)
			ImGui::GetStyle().FontScaleMain += 0.2;

	if (ImGui::IsKeyChordPressed(ImGuiKey::ImGuiMod_Shift | ImGuiKey::ImGuiKey_Minus))
		if (ImGui::GetStyle().FontScaleMain - 0.2 > 0.5)
			ImGui::GetStyle().FontScaleMain -= 0.2;
}

void ShowFontScaleInStatusBar()
{
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x / 10);
	ImGui::SliderFloat("Font scale", &ImGui::GetStyle().FontScaleMain, 0.5, 5);
}

int main(int, char**)
{
    AppState state;
    if (read_camera_properties("cameras.txt", &state.cameras))
    {
        printf("File does not exists.\n Creating new file!\n");
		FILE *handle = fopen("cameras.txt", "w");
		if (handle == NULL)
		{
			fprintf(stderr, "Failed to create new file!\n");
			return -1;
		}
		fclose(handle);
    }

    //write_camera_properties("cameras2.txt", &state.CProps);
	//HelloImGui::SetAssetsFolder("C:\\Users\\joeym\\imgui_bundle\\Debug\\assets");

    HelloImGui::RunnerParams runner_params;

    runner_params.appWindowParams.windowTitle = "Camera tool";
    runner_params.appWindowParams.resizable = true;
    runner_params.appWindowParams.windowGeometry.size = { 1280, 720 };
    runner_params.appWindowParams.restorePreviousGeometry = true;

    runner_params.imGuiWindowParams.showMenuBar = true;
    runner_params.imGuiWindowParams.showMenu_View = true;
    runner_params.imGuiWindowParams.showMenu_App = false;

    runner_params.callbacks.ShowGui = [&state]() { Gui(&state); };
    runner_params.callbacks.ShowMenus = [&state]() { ShowMenuGui(&state); };
    runner_params.callbacks.PreNewFrame = [&state]() { OnNewFrame(&state); };
    runner_params.callbacks.ShowStatus = [&state]() { ShowFontScaleInStatusBar(); };

    runner_params.callbacks.defaultIconFont = HelloImGui::DefaultIconFont::FontAwesome4;

    ImmApp::AddOnsParams add_ons;
    add_ons.withMarkdown = true;

    ImmApp::Run(runner_params, add_ons);

    return 0;
}