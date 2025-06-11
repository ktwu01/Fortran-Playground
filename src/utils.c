/**
 * ============================================================================
 * Fortran ZIP炸弹项目 - C系统工具函数实现
 *
 * 功能: 提供跨平台的文件系统操作函数
 * 作者: Fortran-Playground项目
 * 说明: 支持macOS和Linux系统
 * ============================================================================
 */

#include "zipbomb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <errno.h>
#include <time.h>

// macOS和Linux兼容性处理
#ifdef __APPLE__
    #include <sys/mount.h>
    #define STATVFS_AVAIL f_bavail
#else
    #include <sys/statvfs.h>
    #define STATVFS_AVAIL f_bavail
#endif

// ============================================================================
// 文件操作函数
// ============================================================================

/**
 * 获取文件大小
 *
 * @param filename 文件名
 * @return 文件大小(字节)，失败返回-1
 */
int64_t get_file_size(const char* filename) {
    if (!filename) {
        return -1;
    }

    struct stat file_stat;
    if (stat(filename, &file_stat) != 0) {
        // 文件不存在或无法访问
        return -1;
    }

    // 检查是否为常规文件
    if (!S_ISREG(file_stat.st_mode)) {
        return -1;
    }

    return (int64_t)file_stat.st_size;
}

/**
 * 检查文件是否存在
 *
 * @param filename 文件名
 * @return 1存在，0不存在
 */
int file_exists(const char* filename) {
    if (!filename) {
        return 0;
    }

    return (access(filename, F_OK) == 0) ? 1 : 0;
}

/**
 * 删除文件
 *
 * @param filename 文件名
 * @return 0成功，-1失败
 */
int delete_file(const char* filename) {
    if (!filename) {
        return -1;
    }

    if (unlink(filename) == 0) {
        return 0;  // 成功删除
    }

    // 删除失败，检查错误原因
    switch (errno) {
        case ENOENT:
            // 文件不存在，认为是成功
            return 0;
        case EACCES:
        case EPERM:
            fprintf(stderr, "权限不足，无法删除文件: %s\n", filename);
            break;
        case EBUSY:
            fprintf(stderr, "文件正在使用中，无法删除: %s\n", filename);
            break;
        default:
            fprintf(stderr, "删除文件失败: %s (错误: %s)\n", filename, strerror(errno));
            break;
    }

    return -1;
}

/**
 * 创建目录
 *
 * @param dirname 目录名
 * @return 0成功，-1失败
 */
int create_directory(const char* dirname) {
    if (!dirname) {
        return -1;
    }

    // 检查目录是否已存在
    struct stat dir_stat;
    if (stat(dirname, &dir_stat) == 0) {
        if (S_ISDIR(dir_stat.st_mode)) {
            return 0;  // 目录已存在
        } else {
            fprintf(stderr, "路径已存在但不是目录: %s\n", dirname);
            return -1;
        }
    }

    // 创建目录，权限设置为755
    if (mkdir(dirname, 0755) == 0) {
        return 0;
    }

    fprintf(stderr, "创建目录失败: %s (错误: %s)\n", dirname, strerror(errno));
    return -1;
}

/**
 * 获取文件详细信息
 *
 * @param filename 文件名
 * @param info 输出的文件信息结构体
 * @return 0成功，-1失败
 */
int get_file_info(const char* filename, file_info_t* info) {
    if (!filename || !info) {
        return -1;
    }

    // 初始化结构体
    memset(info, 0, sizeof(file_info_t));
    strncpy(info->filename, filename, MAX_FILENAME_LENGTH - 1);
    info->filename[MAX_FILENAME_LENGTH - 1] = '\0';

    struct stat file_stat;
    if (stat(filename, &file_stat) != 0) {
        info->exists = false;
        info->size_bytes = -1;
        info->error_code = errno;
        return -1;
    }

    info->exists = true;
    info->size_bytes = (int64_t)file_stat.st_size;
    info->error_code = 0;

    return 0;
}

// ============================================================================
// 字符串和格式化函数
// ============================================================================

/**
 * 格式化文件大小为人类可读格式
 *
 * @param size_bytes 文件大小(字节)
 * @param buffer 输出缓冲区
 * @param buffer_size 缓冲区大小
 * @return 格式化后的字符串长度
 */
int format_file_size(int64_t size_bytes, char* buffer, int buffer_size) {
    if (!buffer || buffer_size <= 0) {
        return -1;
    }

    if (size_bytes < 0) {
        return snprintf(buffer, buffer_size, "无效大小");
    }

    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    const int num_units = sizeof(units) / sizeof(units[0]);

    double size = (double)size_bytes;
    int unit_index = 0;

    // 找到合适的单位
    while (size >= 1024.0 && unit_index < num_units - 1) {
        size /= 1024.0;
        unit_index++;
    }

    // 格式化输出
    if (unit_index == 0) {
        // 字节，显示整数
        return snprintf(buffer, buffer_size, "%d %s", (int)size, units[unit_index]);
    } else {
        // 其他单位，显示小数
        return snprintf(buffer, buffer_size, "%.2f %s", size, units[unit_index]);
    }
}

// ============================================================================
// 系统资源检查函数
// ============================================================================

/**
 * 检查磁盘空间
 *
 * @param path 路径
 * @param required_bytes 需要的空间(字节)
 * @return 1空间足够，0空间不足，-1检查失败
 */
int check_disk_space(const char* path, int64_t required_bytes) {
    if (!path || required_bytes < 0) {
        return -1;
    }

    struct statvfs disk_info;
    if (statvfs(path, &disk_info) != 0) {
        fprintf(stderr, "获取磁盘信息失败: %s (错误: %s)\n", path, strerror(errno));
        return -1;
    }

    // 计算可用空间
    int64_t available_bytes = (int64_t)disk_info.STATVFS_AVAIL * (int64_t)disk_info.f_frsize;

    if (available_bytes >= required_bytes) {
        return 1;  // 空间足够
    } else {
        char required_str[64], available_str[64];
        format_file_size(required_bytes, required_str, sizeof(required_str));
        format_file_size(available_bytes, available_str, sizeof(available_str));

        fprintf(stderr, "磁盘空间不足:\n");
        fprintf(stderr, "  需要: %s\n", required_str);
        fprintf(stderr, "  可用: %s\n", available_str);

        return 0;  // 空间不足
    }
}

/**
 * 获取当前工作目录
 *
 * @param buffer 输出缓冲区
 * @param buffer_size 缓冲区大小
 * @return 成功返回缓冲区指针，失败返回NULL
 */
char* get_current_directory(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) {
        return NULL;
    }

    return getcwd(buffer, buffer_size);
}

/**
 * 检查文件权限
 *
 * @param filename 文件名
 * @param mode 权限模式 (R_OK, W_OK, X_OK, F_OK)
 * @return 1有权限，0无权限
 */
int check_file_permission(const char* filename, int mode) {
    if (!filename) {
        return 0;
    }

    return (access(filename, mode) == 0) ? 1 : 0;
}

// ============================================================================
// 错误处理和诊断函数
// ============================================================================

/**
 * 获取错误描述
 *
 * @param error_code 错误代码
 * @return 错误描述字符串
 */
const char* get_error_description(int error_code) {
    switch (error_code) {
        case ZIPBOMB_SUCCESS:
            return "操作成功";
        case ZIPBOMB_ERROR_FILE_CREATE:
            return "文件创建失败";
        case ZIPBOMB_ERROR_WRITE_FAILED:
            return "文件写入失败";
        case ZIPBOMB_ERROR_COMPRESS_FAIL:
            return "压缩操作失败";
        case ZIPBOMB_ERROR_INVALID_PARAM:
            return "参数无效";
        case ZIPBOMB_ERROR_MEMORY_ALLOC:
            return "内存分配失败";
        default:
            return "未知错误";
    }
}

/**
 * 打印系统信息
 */
void print_system_info(void) {
    printf("=== 系统信息 ===\n");

    // 获取当前目录
    char cwd[1024];
    if (get_current_directory(cwd, sizeof(cwd))) {
        printf("当前目录: %s\n", cwd);
    }

    // 检查磁盘空间
    struct statvfs disk_info;
    if (statvfs(".", &disk_info) == 0) {
        int64_t total_space = (int64_t)disk_info.f_blocks * (int64_t)disk_info.f_frsize;
        int64_t available_space = (int64_t)disk_info.STATVFS_AVAIL * (int64_t)disk_info.f_frsize;

        char total_str[64], available_str[64];
        format_file_size(total_space, total_str, sizeof(total_str));
        format_file_size(available_space, available_str, sizeof(available_str));

        printf("磁盘总空间: %s\n", total_str);
        printf("磁盘可用空间: %s\n", available_str);
    }

    printf("================\n");
}

// ============================================================================
// 参数验证函数
// ============================================================================

/**
 * 验证输入参数
 *
 * @param filename 文件名
 * @param config 配置参数
 * @return 1有效，0无效
 */
int validate_parameters(const char* filename, const zipbomb_config_t* config) {
    if (!filename || !config) {
        fprintf(stderr, "参数验证失败: 空指针\n");
        return 0;
    }

    // 检查文件名
    if (strlen(filename) == 0) {
        fprintf(stderr, "参数验证失败: 文件名为空\n");
        return 0;
    }

    if (strlen(filename) >= MAX_FILENAME_LENGTH) {
        fprintf(stderr, "参数验证失败: 文件名过长\n");
        return 0;
    }

    // 检查配置参数
    if (config->target_size_mb <= 0 || config->target_size_mb > 100000) {
        fprintf(stderr, "参数验证失败: 目标大小无效 (%d MB)\n", config->target_size_mb);
        return 0;
    }

    if (config->compression_level < 1 || config->compression_level > 9) {
        fprintf(stderr, "参数验证失败: 压缩级别无效 (%d)\n", config->compression_level);
        return 0;
    }

    if (config->pattern_size <= 0 || config->pattern_size > 100*1024*1024) {
        fprintf(stderr, "参数验证失败: 模式大小无效 (%d 字节)\n", config->pattern_size);
        return 0;
    }

    // 检查磁盘空间（估算需要的临时空间）
    int64_t estimated_temp_space = (int64_t)config->target_size_mb * 1024 * 1024 / 1000; // 估算1/1000
    if (!check_disk_space(".", estimated_temp_space)) {
        fprintf(stderr, "参数验证失败: 磁盘空间不足\n");
        return 0;
    }

    return 1;  // 所有验证通过
}

/**
 * 检查文件名是否安全
 *
 * @param filename 文件名
 * @return 1安全，0不安全
 */
int is_safe_filename(const char* filename) {
    if (!filename) {
        return 0;
    }

    // 检查危险字符
    const char* dangerous_chars = "../\\:*?\"<>|";
    for (const char* p = dangerous_chars; *p; p++) {
        if (strchr(filename, *p)) {
            fprintf(stderr, "文件名包含危险字符: %c\n", *p);
            return 0;
        }
    }

    // 检查是否以点开头（隐藏文件）
    if (filename[0] == '.') {
        fprintf(stderr, "警告: 文件名以点开头，将创建隐藏文件\n");
    }

    return 1;
}

// ============================================================================
// 性能监控辅助函数
// ============================================================================

/**
 * 获取当前时间戳（毫秒）
 */
int64_t get_timestamp_ms(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
        return (int64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    }
    return 0;
}
