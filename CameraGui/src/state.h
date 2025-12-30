#pragma once
#include <cstdint>
#include "config.h"

static const char* test_page =
#include "markdown.md"
;

static const char* wd_page =
#include "Working_distance.md"
;

static const char* ME2P_1230_9GMC_page =
#include "ME2P-1230-9GMC.md"
;

static const char* FS_1600D_10GE_page =
#include "FS-1600D-10GE.md"
;

static const char* MER2_160_227U3MC_page =
#include "MER2-160-227U3MC.md"
;

static const char* MER2_2000_6GMC_page =
#include "MER2-2000-6GMC.md"
;


struct CameraProperties
{
	float pixelSize;

	float sensorDiagonal;
	float sensorWidth;
	float sensorHeight;

	int pixelWidth;
	int pixelHeight;
};

struct Camera
{
    uint8_t itemsStored = 0;
    char name[MAX_ITEMS_ALLOWED][CAMERA_NAME_STRING_SIZE];
	CameraProperties cProps[MAX_ITEMS_ALLOWED] = { 0 };
};

static ImGuiTreeNodeFlags tree_node_flags_base = ImGuiTreeNodeFlags_SpanAllColumns | ImGuiTreeNodeFlags_DrawLinesFull;

struct MyTreeNode
{
    const char* Name;
    MyTreeNode* ChildNodes;
    int         ChildCount;
    const char* page = NULL;
};

struct AppState
{
	MyTreeNode* Nodes = NULL;
	int TopNodesCount = 0;

	Camera cameras;

	ImGuiID ActiveViewPorts[30] = { 0 };
	float FontScaleFactorPerViewPort = { 1.f };
	int ActiveViewPortsCount = 0;

	float wd_to_precision_sync_value = 0;

	bool cameraModalEnabled = false;
	bool technicalDocumentsWindowEnabled = false;
};