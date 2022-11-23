#ifndef __INCLUDES_HPP

#define __INCLUDES_HPP
#define IMGUI_DEFINE_MATH_OPERATORS
#ifdef _MSC_VER
#pragma warning(disable: 4251) // shut up cocos
#pragma warning( push )
#endif
#include <algorithm>
// Windows
#include <Windows.h>

// Cocos2d-x
#include <cocos2d.h>
#include <cocos-ext.h>

// Minhook
#include <MinHook.h>

// GD.h
#include <gd.h>

// ImGui
#include <imgui.h>
#include <imgui-hook.hpp>

// Rapidjson
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/istreamwrapper.h>

using namespace cocos2d;
#pragma warning( pop )
#endif