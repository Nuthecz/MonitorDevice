#pragma once

#include <android/log.h>
#include <linux/elf.h>
#include <unistd.h>
#include <jni.h>
#include <stdio.h>
#include <string>

// 配置日志
#define TAG "nuthecz"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__);
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO , TAG, __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__);

typedef struct ExecSection {
    int sectionNum;
    unsigned long offset[2]; // 用于保存段的起始位置
    unsigned long memSize[2]; // 用于保存段的大小
    char *sectionName[2]; // 用于节区的名称
    unsigned long checkSum[2]; // 计算校验和
    unsigned long startAddrinMem;
    bool isCorrect = false; // 是否完成
} sectionResult;


struct StatFInfo {
    char filePath[256]; // 文件路径
    unsigned long long id; // 文件系统 ID
    unsigned int namelen; // 最大文件名长度
    unsigned int type; // 文件系统类型
    unsigned int blockSize; // 块大小
    unsigned int fundamentalBlockSize; // 基本块大小
    unsigned long long blocksTotal; // 总块数
    unsigned long long blocksFree; // 空闲块数
    unsigned long long blocksAvailable; // 可用块数
    unsigned long long inodesTotal; // 总inode数
    unsigned long long inodesFree; // 空闲inode数
};