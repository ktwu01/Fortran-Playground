/**
 * ============================================================================
 * Fortran ZIP炸弹项目 - C++ ZIP生成核心实现
 *
 * 功能: 实现ZIP炸弹的生成算法
 * 原理: 利用ZIP格式的重复数据压缩特性，创建小文件解压成大文件
 * 作者: Fortran-Playground项目
 * 警告: 仅用于教学目的，请勿恶意使用！
 * ============================================================================
 */

#include "zipbomb.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <iomanip>

// 简化的ZIP文件结构实现（教学版本）
namespace ZipBombGenerator {

// ============================================================================
// 全局变量和配置
// ============================================================================

static zipbomb_config_t g_config = {
    DEFAULT_TARGET_SIZE_MB,      // 10GB目标大小
    DEFAULT_COMPRESSION_LEVEL,   // 压缩级别6
    DEFAULT_PATTERN_SIZE,        // 1MB模式大小
    'A',                         // 默认重复字符
    false,                       // 不使用嵌套压缩
    1                            // 嵌套层数
};

static bool g_verbose_logging = false;
static std::chrono::steady_clock::time_point g_start_time;
static std::chrono::steady_clock::time_point g_end_time;
static double g_compression_ratio = 0.0;

// ============================================================================
// ZIP文件格式结构体 (简化版本)
// ============================================================================

#pragma pack(push, 1)  // 确保结构体紧密打包

/** ZIP文件本地文件头 */
struct ZipLocalFileHeader {
    uint32_t signature;          // 0x04034b50
    uint16_t version;            // 版本
    uint16_t flags;              // 通用标志
    uint16_t compression;        // 压缩方法
    uint16_t mod_time;           // 修改时间
    uint16_t mod_date;           // 修改日期
    uint32_t crc32;              // CRC-32
    uint32_t compressed_size;    // 压缩后大小
    uint32_t uncompressed_size;  // 原始大小
    uint16_t filename_length;    // 文件名长度
    uint16_t extra_length;       // 额外字段长度
};

/** ZIP中央目录文件头 */
struct ZipCentralDirHeader {
    uint32_t signature;          // 0x02014b50
    uint16_t version_made;       // 制作版本
    uint16_t version_needed;     // 需要版本
    uint16_t flags;              // 通用标志
    uint16_t compression;        // 压缩方法
    uint16_t mod_time;           // 修改时间
    uint16_t mod_date;           // 修改日期
    uint32_t crc32;              // CRC-32
    uint32_t compressed_size;    // 压缩后大小
    uint32_t uncompressed_size;  // 原始大小
    uint16_t filename_length;    // 文件名长度
    uint16_t extra_length;       // 额外字段长度
    uint16_t comment_length;     // 注释长度
    uint16_t disk_start;         // 开始磁盘号
    uint16_t internal_attr;      // 内部属性
    uint32_t external_attr;      // 外部属性
    uint32_t local_header_offset; // 本地头偏移
};

/** ZIP文件结束记录 */
struct ZipEndOfCentralDir {
    uint32_t signature;          // 0x06054b50
    uint16_t disk_number;        // 磁盘号
    uint16_t disk_start;         // 中央目录开始磁盘
    uint16_t entries_on_disk;    // 本磁盘条目数
    uint16_t total_entries;      // 总条目数
    uint32_t central_dir_size;   // 中央目录大小
    uint32_t central_dir_offset; // 中央目录偏移
    uint16_t comment_length;     // 注释长度
};

#pragma pack(pop)

// ============================================================================
// 工具函数
// ============================================================================

/**
 * 计算CRC32校验和
 */
uint32_t calculate_crc32(const std::vector<uint8_t>& data) {
    // 简化的CRC32实现（教学版本）
    // 实际应用中应使用zlib的crc32函数
    uint32_t crc = 0xFFFFFFFF;
    static const uint32_t crc_table[256] = {
        // CRC32查找表（部分，完整版本需要256个条目）
        0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
        0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3
        // ... 更多条目
    };

    for (uint8_t byte : data) {
        crc = crc_table[(crc ^ byte) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
}

/**
 * 简单的压缩函数（模拟压缩效果）
 */
std::vector<uint8_t> simple_compress(const std::vector<uint8_t>& input) {
    if (input.empty()) return {};

    std::vector<uint8_t> compressed;

    // 检测重复模式并进行简单的行程编码
    size_t i = 0;
    while (i < input.size()) {
        uint8_t current_byte = input[i];
        size_t count = 1;

        // 计算连续相同字节的数量
        while (i + count < input.size() &&
               input[i + count] == current_byte &&
               count < 255) {
            count++;
        }

        // 如果重复次数大于3，使用压缩
        if (count > 3) {
            compressed.push_back(0xFF);  // 压缩标记
            compressed.push_back(static_cast<uint8_t>(count));
            compressed.push_back(current_byte);
        } else {
            // 直接存储
            for (size_t j = 0; j < count; j++) {
                compressed.push_back(current_byte);
            }
        }

        i += count;
    }

    return compressed;
}

/**
 * 生成重复数据模式
 */
std::vector<uint8_t> generate_pattern_data(size_t size, char pattern_char) {
    std::vector<uint8_t> data(size, static_cast<uint8_t>(pattern_char));

    // 添加一些变化以避免过于明显的模式
    for (size_t i = 0; i < size; i += 1024) {
        if (i + 3 < size) {
            data[i] = 'Z';
            data[i+1] = 'I';
            data[i+2] = 'P';
        }
    }

    return data;
}

/**
 * 日志函数
 */
void log_message(const std::string& message) {
    if (g_verbose_logging) {
        std::cout << "[ZIP炸弹生成器] " << message << std::endl;
    }
}

// ============================================================================
// ZIP文件生成核心函数
// ============================================================================

/**
 * 创建ZIP文件的本地文件条目
 */
bool write_zip_file_entry(std::ofstream& file,
                         const std::string& filename,
                         const std::vector<uint8_t>& data) {

    std::vector<uint8_t> compressed_data = simple_compress(data);
    uint32_t crc = calculate_crc32(data);

    // 写入本地文件头
    ZipLocalFileHeader header = {};
    header.signature = 0x04034b50;
    header.version = 20;
    header.flags = 0;
    header.compression = (compressed_data.size() < data.size()) ? 8 : 0; // 8=deflate, 0=store
    header.mod_time = 0;
    header.mod_date = 0;
    header.crc32 = crc;
    header.compressed_size = static_cast<uint32_t>(compressed_data.size());
    header.uncompressed_size = static_cast<uint32_t>(data.size());
    header.filename_length = static_cast<uint16_t>(filename.length());
    header.extra_length = 0;

    file.write(reinterpret_cast<const char*>(&header), sizeof(header));
    file.write(filename.c_str(), filename.length());
    file.write(reinterpret_cast<const char*>(compressed_data.data()), compressed_data.size());

    return file.good();
}

/**
 * 创建中央目录
 */
bool write_central_directory(std::ofstream& file,
                            const std::vector<std::string>& filenames,
                            const std::vector<uint32_t>& file_offsets,
                            const std::vector<uint32_t>& compressed_sizes,
                            const std::vector<uint32_t>& uncompressed_sizes,
                            const std::vector<uint32_t>& crcs) {

    uint32_t central_dir_start = static_cast<uint32_t>(file.tellp());

    // 写入每个文件的中央目录条目
    for (size_t i = 0; i < filenames.size(); i++) {
        ZipCentralDirHeader central_header = {};
        central_header.signature = 0x02014b50;
        central_header.version_made = 20;
        central_header.version_needed = 20;
        central_header.flags = 0;
        central_header.compression = (compressed_sizes[i] < uncompressed_sizes[i]) ? 8 : 0;
        central_header.mod_time = 0;
        central_header.mod_date = 0;
        central_header.crc32 = crcs[i];
        central_header.compressed_size = compressed_sizes[i];
        central_header.uncompressed_size = uncompressed_sizes[i];
        central_header.filename_length = static_cast<uint16_t>(filenames[i].length());
        central_header.extra_length = 0;
        central_header.comment_length = 0;
        central_header.disk_start = 0;
        central_header.internal_attr = 0;
        central_header.external_attr = 0;
        central_header.local_header_offset = file_offsets[i];

        file.write(reinterpret_cast<const char*>(&central_header), sizeof(central_header));
        file.write(filenames[i].c_str(), filenames[i].length());
    }

    uint32_t central_dir_size = static_cast<uint32_t>(file.tellp()) - central_dir_start;

    // 写入目录结束记录
    ZipEndOfCentralDir end_record = {};
    end_record.signature = 0x06054b50;
    end_record.disk_number = 0;
    end_record.disk_start = 0;
    end_record.entries_on_disk = static_cast<uint16_t>(filenames.size());
    end_record.total_entries = static_cast<uint16_t>(filenames.size());
    end_record.central_dir_size = central_dir_size;
    end_record.central_dir_offset = central_dir_start;
    end_record.comment_length = 0;

    file.write(reinterpret_cast<const char*>(&end_record), sizeof(end_record));

    return file.good();
}

/**
 * 核心ZIP炸弹生成函数
 */
int create_zipbomb_internal(const std::string& filename, const zipbomb_config_t& config) {
    g_start_time = std::chrono::steady_clock::now();

    log_message("开始生成ZIP炸弹: " + filename);
    log_message("目标大小: " + std::to_string(config.target_size_mb) + " MB");

    std::ofstream zip_file(filename, std::ios::binary);
    if (!zip_file) {
        error_log(ZIPBOMB_ERROR_FILE_CREATE, "无法创建输出文件");
        return ZIPBOMB_ERROR_FILE_CREATE;
    }

    // 生成大量重复数据
    size_t pattern_size = static_cast<size_t>(config.pattern_size);
    std::vector<uint8_t> pattern_data = generate_pattern_data(pattern_size, config.pattern_char);

    // 计算需要多少个文件来达到目标大小
    size_t target_bytes = static_cast<size_t>(config.target_size_mb) * 1024 * 1024;
    size_t num_files = target_bytes / pattern_size;
    if (num_files == 0) num_files = 1;

    // 限制文件数量，避免生成过多小文件
    if (num_files > 1000) {
        num_files = 1000;
        pattern_size = target_bytes / num_files;
        pattern_data = generate_pattern_data(pattern_size, config.pattern_char);
    }

    log_message("将生成 " + std::to_string(num_files) + " 个文件");
    log_message("每个文件大小: " + std::to_string(pattern_size) + " 字节");

    // 存储元数据
    std::vector<std::string> filenames;
    std::vector<uint32_t> file_offsets;
    std::vector<uint32_t> compressed_sizes;
    std::vector<uint32_t> uncompressed_sizes;
    std::vector<uint32_t> crcs;

    // 写入文件条目
    for (size_t i = 0; i < num_files; i++) {
        std::string internal_filename = "bomb_data_" + std::to_string(i) + ".txt";
        uint32_t offset = static_cast<uint32_t>(zip_file.tellp());

        std::vector<uint8_t> compressed = simple_compress(pattern_data);
        uint32_t crc = calculate_crc32(pattern_data);

        if (!write_zip_file_entry(zip_file, internal_filename, pattern_data)) {
            error_log(ZIPBOMB_ERROR_WRITE_FAILED, "写入文件条目失败");
            return ZIPBOMB_ERROR_WRITE_FAILED;
        }

        filenames.push_back(internal_filename);
        file_offsets.push_back(offset);
        compressed_sizes.push_back(static_cast<uint32_t>(compressed.size()));
        uncompressed_sizes.push_back(static_cast<uint32_t>(pattern_data.size()));
        crcs.push_back(crc);

        // 进度报告
        if ((i + 1) % 100 == 0 || i == num_files - 1) {
            log_message("进度: " + std::to_string(i + 1) + "/" + std::to_string(num_files));
        }
    }

    // 写入中央目录
    if (!write_central_directory(zip_file, filenames, file_offsets,
                                compressed_sizes, uncompressed_sizes, crcs)) {
        error_log(ZIPBOMB_ERROR_WRITE_FAILED, "写入中央目录失败");
        return ZIPBOMB_ERROR_WRITE_FAILED;
    }

    zip_file.close();

    g_end_time = std::chrono::steady_clock::now();

    // 计算压缩比
    int64_t file_size = get_file_size(filename.c_str());
    if (file_size > 0) {
        g_compression_ratio = static_cast<double>(file_size) / static_cast<double>(target_bytes);
    }

    log_message("ZIP炸弹生成完成!");
    log_message("文件大小: " + std::to_string(file_size) + " 字节");
    log_message("压缩比: " + std::to_string(g_compression_ratio * 100.0) + "%");

    return ZIPBOMB_SUCCESS;
}

} // namespace ZipBombGenerator

// ============================================================================
// C接口实现 (供Fortran调用)
// ============================================================================

extern "C" {

void create_zipbomb(const char* filename) {
    if (!filename) {
        error_log(ZIPBOMB_ERROR_INVALID_PARAM, "文件名为空");
        return;
    }

    int result = ZipBombGenerator::create_zipbomb_internal(filename, g_config);
    if (result != ZIPBOMB_SUCCESS) {
        std::cerr << "ZIP炸弹生成失败，错误代码: " << result << std::endl;
    }
}

int create_zipbomb_with_config(const char* filename, const zipbomb_config_t* config) {
    if (!filename || !config) {
        return ZIPBOMB_ERROR_INVALID_PARAM;
    }

    return ZipBombGenerator::create_zipbomb_internal(filename, *config);
}

void set_compression_params(int target_size, int compression_level) {
    g_config.target_size_mb = target_size;
    g_config.compression_level = compression_level;
    ZipBombGenerator::log_message("压缩参数已更新");
}

zipbomb_config_t get_default_config(void) {
    return g_config;
}

void cleanup_resources(void) {
    ZipBombGenerator::log_message("清理资源完成");
}

void set_verbose_logging(int enable) {
    g_verbose_logging = (enable != 0);
}

void debug_log(const char* message) {
    if (message && g_verbose_logging) {
        std::cout << "[调试] " << message << std::endl;
    }
}

void error_log(int error_code, const char* message) {
    std::cerr << "[错误 " << error_code << "] " << (message ? message : "未知错误") << std::endl;
}

double get_compression_ratio(void) {
    return g_compression_ratio;
}

double get_processing_time(void) {
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(g_end_time - g_start_time);
    return duration.count() / 1000.0;
}

void print_performance_stats(void) {
    std::cout << "=== 性能统计 ===" << std::endl;
    std::cout << "处理时间: " << get_processing_time() << " 秒" << std::endl;
    std::cout << "压缩比: " << std::fixed << std::setprecision(2)
              << get_compression_ratio() * 100.0 << "%" << std::endl;
}

} // extern "C"
