/**
 * ============================================================================
 * Fortran ZIP炸弹项目 - C/C++头文件
 *
 * 功能: 定义C/C++函数接口，供Fortran调用
 * 作者: Fortran-Playground项目
 * 警告: 仅用于教学目的，请勿恶意使用！
 * ============================================================================
 */

#ifndef ZIPBOMB_H
#define ZIPBOMB_H

#include <stdint.h>

#ifdef __cplusplus
#include <iostream>
extern "C" {
#else
#include <stdbool.h>
#endif

// ============================================================================
// 常量定义
// ============================================================================

/** 默认压缩配置 */
#define DEFAULT_TARGET_SIZE_MB      10240    // 默认目标大小: 10GB
#define DEFAULT_COMPRESSION_LEVEL   6        // 默认压缩级别: 中等
#define DEFAULT_PATTERN_SIZE        1048576  // 默认模式大小: 1MB
#define MAX_FILENAME_LENGTH         512      // 最大文件名长度

/** 错误代码 */
#define ZIPBOMB_SUCCESS             0        // 成功
#define ZIPBOMB_ERROR_FILE_CREATE   -1       // 文件创建失败
#define ZIPBOMB_ERROR_WRITE_FAILED  -2       // 写入失败
#define ZIPBOMB_ERROR_COMPRESS_FAIL -3       // 压缩失败
#define ZIPBOMB_ERROR_INVALID_PARAM -4       // 参数无效
#define ZIPBOMB_ERROR_MEMORY_ALLOC  -5       // 内存分配失败

// ============================================================================
// 结构体定义
// ============================================================================

/**
 * ZIP炸弹配置结构体
 */
typedef struct {
    int target_size_mb;           // 目标解压大小(MB)
    int compression_level;        // 压缩级别(1-9)
    int pattern_size;             // 重复模式大小(字节)
    char pattern_char;            // 重复字符
    bool use_nested_compression;  // 是否使用嵌套压缩
    int nested_levels;            // 嵌套层数
} zipbomb_config_t;

/**
 * 文件信息结构体
 */
typedef struct {
    char filename[MAX_FILENAME_LENGTH];
    int64_t size_bytes;
    bool exists;
    int error_code;
} file_info_t;

// ============================================================================
// C++函数声明 (ZIP生成核心)
// ============================================================================

/**
 * 创建ZIP炸弹文件 (主函数)
 *
 * @param filename 输出文件名
 * @return 成功返回0，失败返回错误代码
 */
void create_zipbomb(const char* filename);

/**
 * 使用自定义配置创建ZIP炸弹
 *
 * @param filename 输出文件名
 * @param config 压缩配置
 * @return 成功返回0，失败返回错误代码
 */
int create_zipbomb_with_config(const char* filename, const zipbomb_config_t* config);

/**
 * 设置压缩参数
 *
 * @param target_size 目标解压大小(MB)
 * @param compression_level 压缩级别(1-9)
 */
void set_compression_params(int target_size, int compression_level);

/**
 * 获取默认配置
 *
 * @return 默认配置结构体
 */
zipbomb_config_t get_default_config(void);

/**
 * 清理C++分配的资源
 */
void cleanup_resources(void);

// ============================================================================
// C函数声明 (系统工具函数)
// ============================================================================

/**
 * 获取文件大小
 *
 * @param filename 文件名
 * @return 文件大小(字节)，失败返回-1
 */
int64_t get_file_size(const char* filename);

/**
 * 检查文件是否存在
 *
 * @param filename 文件名
 * @return 1存在，0不存在
 */
int file_exists(const char* filename);

/**
 * 删除文件
 *
 * @param filename 文件名
 * @return 0成功，-1失败
 */
int delete_file(const char* filename);

/**
 * 创建目录
 *
 * @param dirname 目录名
 * @return 0成功，-1失败
 */
int create_directory(const char* dirname);

/**
 * 获取文件详细信息
 *
 * @param filename 文件名
 * @param info 输出的文件信息结构体
 * @return 0成功，-1失败
 */
int get_file_info(const char* filename, file_info_t* info);

/**
 * 格式化文件大小为人类可读格式
 *
 * @param size_bytes 文件大小(字节)
 * @param buffer 输出缓冲区
 * @param buffer_size 缓冲区大小
 * @return 格式化后的字符串长度
 */
int format_file_size(int64_t size_bytes, char* buffer, int buffer_size);

// ============================================================================
// 调试和日志函数
// ============================================================================

/**
 * 启用/禁用详细日志输出
 *
 * @param enable 1启用，0禁用
 */
void set_verbose_logging(int enable);

/**
 * 输出调试信息
 *
 * @param message 调试消息
 */
void debug_log(const char* message);

/**
 * 输出错误信息
 *
 * @param error_code 错误代码
 * @param message 错误消息
 */
void error_log(int error_code, const char* message);

// ============================================================================
// 安全和验证函数
// ============================================================================

/**
 * 验证输入参数
 *
 * @param filename 文件名
 * @param config 配置参数
 * @return 1有效，0无效
 */
int validate_parameters(const char* filename, const zipbomb_config_t* config);

/**
 * 检查磁盘空间
 *
 * @param path 路径
 * @param required_bytes 需要的空间(字节)
 * @return 1空间足够，0空间不足
 */
int check_disk_space(const char* path, int64_t required_bytes);

/**
 * 获取错误描述
 *
 * @param error_code 错误代码
 * @return 错误描述字符串
 */
const char* get_error_description(int error_code);

// ============================================================================
// 统计和性能监控
// ============================================================================

/**
 * 重置性能计数器
 */
void reset_performance_counters(void);

/**
 * 获取压缩比率
 *
 * @return 压缩比率 (压缩后大小/原始大小)
 */
double get_compression_ratio(void);

/**
 * 获取处理时间(秒)
 *
 * @return 处理时间
 */
double get_processing_time(void);

/**
 * 打印性能统计
 */
void print_performance_stats(void);

#ifdef __cplusplus
}
#endif

#endif /* ZIPBOMB_H */
