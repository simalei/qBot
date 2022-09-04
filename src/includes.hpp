#ifndef __INCLUDES_HPP

#define __INCLUDES_HPP

#ifdef _MSC_VER
#pragma warning(disable: 4251) // shut up cocos
#endif

// Windows
#define WIN32_LEAN_AND_MEAN
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

// Native File Dialog
#include <nfd.hpp>

// ImGui Notify
#include <imgui_notify.h>
#include <tahoma.h>

using namespace cocos2d;
using namespace cocos2d::extension;

#endif