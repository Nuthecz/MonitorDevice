//
// Created by NU on 2024/10/8.
//

#ifndef MONITORDEVICE_DEVICEDETECTION_H
#define MONITORDEVICE_DEVICEDETECTION_H
#pragma once

#include "../include/config.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/statfs.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <linux/seccomp.h>
#include <linux/filter.h>
#include <sys/prctl.h>
#include <linux/unistd.h>
#include <dirent.h>
#include <dlfcn.h>
#include <jni.h>

// 集成 root，device，env检测

// root 检测
bool NativeDetected(const char *path, bool usesyscall);

// device 检测

// env 检测
#define AUDIT_ARCH_I386 0x40000003
#define AUDIT_ARCH_X86_64 0xC000003E

// 加载 seccomp 进行检测
void install_check_arch_seccomp();

// 检测架构
std::string check_arch_by_seccomp(JNIEnv *env);

// 检测模拟器
bool simulator_files_check();

#endif //MONITORDEVICE_DEVICEDETECTION_H
