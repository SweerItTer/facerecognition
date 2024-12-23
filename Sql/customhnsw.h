#ifndef CUSTOMHNSW_H
#define CUSTOMHNSW_H

#include <vector>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <ratio>
#include <sys/stat.h>
#include <ctime>

#include "hnswlib.h"
#include "dataitem.h" // 结构体

class CustomHNSW {
private:
    // HNSW索引
    hnswlib::HierarchicalNSW<float>* alg;
    // 存储原始数据
    std::vector<DataItem> data;
    // 映射HNSW索引到原始数据
    std::unordered_map<size_t, size_t> indexMap;
    // 空间
    hnswlib::L2Space *space = nullptr;


    // 计算三个向量的平均值
    std::vector<float> calculateAverageVector(const DataItem& item) {
        std::vector<float> avg(128, 0.0f);
        for (size_t i = 0; i < 128; ++i) {
            avg[i] = (item.vector1[i] + item.vector2[i] + item.vector3[i]) / 3.0f;
        }
        return avg;
    }

public:
    CustomHNSW(size_t maxElements){//, size_t M = 16, size_t efConstruction = 200) {
        space = new hnswlib::L2Space(128);
        alg = new hnswlib::HierarchicalNSW<float>(space, maxElements);
    } // 构建

    ~CustomHNSW() {
        delete alg;
        delete space;
    }

    void addItem(const DataItem& item) {
        std::vector<float> avgVector = calculateAverageVector(item);
        size_t id = data.size();
        alg->addPoint(avgVector.data(), id);
        data.push_back(item);
        indexMap[id] = id;
    }   // 添加数据

    std::string search(const std::vector<float>& query, size_t K) {
        // 查询最近邻
        std::string name = "";
        auto result = alg->searchKnn(query.data(), K);
        // 处理 priority_queue 结果
        while (!result.empty()) {
            auto item = result.top();  // 获取队列的顶部元素
            result.pop();  // 弹出顶部元素
            if(item.first < 1.1){// 阈值
                name = indexMap[item.second];
                std::cout << "ID: " << name  << " Distance: " << item.first << std::endl;
                break;
            } else {
                continue;
            }
        }
        if (name == "")
            printf("No result found!\n");
        
        return name;
    }

    bool shouldUpdateIndex(const std::string& indexFilePath) {
        struct stat fileInfo;

        // 获取文件信息
        if (stat(indexFilePath.c_str(), &fileInfo) != 0) {
            // 文件不存在，返回 true 以创建并更新
            return true;
        }

        // 获取最后修改时间
        std::time_t lastWriteTime = fileInfo.st_mtime;

        // 获取当前时间
        std::time_t now = std::time(nullptr);

        // 计算时间差（以秒为单位）
        double secondsSinceLastWrite = std::difftime(now, lastWriteTime);
        
        // 判断是否超过7天（7天 = 7 * 24 * 60 * 60 秒）
        if (secondsSinceLastWrite > (7 * 24 * 60 * 60)) {
            return true; // 超过7天需要更新
        }

        return false; // 没有超过7天，不需要更新
    }

    void saveToFile(const std::string& filename) {
        // 保存索引
        alg->saveIndex(filename);

        // 保存 indexMap 到附加文件 (比如 filename + ".map")
        std::ofstream mapFile(filename + ".map", std::ios::binary);
        if (!mapFile) {
            throw std::runtime_error("Failed to open map file for saving.");
        }

        // 保存 indexMap 的大小
        size_t mapSize = indexMap.size();
        // reinterpret_cast<const char*> 将指针转换为 const char*(按照字节写入)
        mapFile.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));

        // 保存 indexMap 的键值对
        for (const auto& pair : indexMap) {
            auto index = pair.first;  // 获取键
            auto id = pair.second;    // 获取值
            mapFile.write(reinterpret_cast<const char*>(&index), sizeof(index));
            mapFile.write(reinterpret_cast<const char*>(&id), sizeof(id));
        }
    }

    void loadFromFile(const std::string& filename) {
        // 加载索引
        alg->loadIndex(filename, space);
        // 加载map
        // 从附加文件中加载 indexMap
        std::ifstream mapFile(filename + ".map", std::ios::binary);
        if (!mapFile) {
            throw std::runtime_error("Failed to open map file for loading.");
        }

        // 加载 indexMap 的大小
        size_t mapSize;
        mapFile.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));

        // 清空原有 indexMap
        indexMap.clear();

        // 重新加载 indexMap 的键值对
        for (size_t i = 0; i < mapSize; ++i) {
            size_t index, id;
            mapFile.read(reinterpret_cast<char*>(&index), sizeof(index));
            mapFile.read(reinterpret_cast<char*>(&id), sizeof(id));
            indexMap[index] = id;
        }
    }
};

#endif // CUSTOMHNSW