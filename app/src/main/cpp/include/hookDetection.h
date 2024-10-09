//
// Created by NU on 2024/10/8.
//

#ifndef MONITORDEVICE_HOOKDETECTION_H
#define MONITORDEVICE_HOOKDETECTION_H

#pragma once

#include "../include/elf_util.h"
#include "../include/config.h"
#include "local_dlfcn.h"
#include <jni.h>
#include <list>
#include <string>
#include <set>
#include <zlib.h>
#include <fcntl.h>
#include <vector>
#include <array>
#include <dirent.h>

// 通过 CRC32 比较内存和本地文件数据比较 open 处是否被修改
bool openHookStatus();

// 通过 CRC32 比较内存和本地文件数据比较 .text 和 .plt段 是否被修改
bool segmentHookStatus();

// 通过 CRC32 比较内存和本地文件数据比较 PrettyMethod 是否被修改
bool prettyMethodHookStatus();

// 利用 java 层调用栈检测 hook 工具存在状态，主要是解析 StackTraceElement[] 这个数组来进行检测
int callStackDetection(JNIEnv *env);

// frida 检测
#define MAX_PATH 256
#define MAX_BUFFER 1024

bool frida_check_maps();

bool frida_check_maps_self();

bool frida_check_status();

// xposed 检测
bool xposed_check_maps(char *path);

bool xposed_check_stack(JNIEnv *env);

#endif //MONITORDEVICE_HOOKDETECTION_H
