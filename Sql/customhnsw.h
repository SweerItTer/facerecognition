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

#include <typeinfo>

#include "hnswlib.h"
#include "dataitem.h" // 结构体

#include <cstddef>  // for size_t

struct IndexData {
    std::string name;
    std::vector<std::vector<float>> vectors;
};

class CustomHNSW {
private:
    // HNSW索引
    hnswlib::HierarchicalNSW<float>* alg;
    // 存储原始数据
    std::vector<DataItem> data;
    // 映射HNSW索引到原始数据
    std::unordered_map<size_t, IndexData> indexMap;
    // 空间
    hnswlib::L2Space *space = nullptr;


    // 添加计算余弦相似度的辅助函数
    float cosine_similarity(const std::vector<float>& a, const std::vector<float>& b) {
        float dot = 0.0, denom_a = 0.0, denom_b = 0.0;
        for (size_t i = 0; i < a.size(); i++) {
            dot += a[i] * b[i];
            denom_a += a[i] * a[i];
            denom_b += b[i] * b[i];
        }
        return dot / (sqrt(denom_a) * sqrt(denom_b));
    }

public:
    CustomHNSW(size_t maxElements) {
        space = new hnswlib::L2Space(128);
        alg = new hnswlib::HierarchicalNSW<float>(space, maxElements);
    } // 构建

    ~CustomHNSW() {
        delete alg;
        delete space;
    }

    void addItem(const DataItem& item) {
        size_t id = data.size();
        data.push_back(item);  // 先保存原始数据
        
        try {
            std::cout << "--------------------------------" << std::endl;   

            // 打印调试信息
            std::cout << "Adding item: " << item.name << " with ID: " << id << std::endl;
            
            // // 打印所有向量的位置
            // std::cout << "Vector positions: ";
            // for(int i = 0; i < 10; ++i) {
            //     std::cout << id*10 + i << " ";
            // }
            // std::cout << std::endl;
            
            // 检查所有向量
            for(const auto& vec : item.vectors) {
                if(vec.empty()) {
                    throw std::runtime_error("++Empty feature vector detected");
                }
                if(vec.size() != 128) {
                    throw std::runtime_error("++Invalid vector dimension");
                }
            }
            
            // 添加所有特征向量
            for(size_t i = 0; i < item.vectors.size(); ++i) {
                alg->addPoint(item.vectors[i].data(), id * 10 + i);
            }
            
            // 存储映射关系
            indexMap[id] = {
                item.name,
                item.vectors  // 现在直接存储vector数组
            };
            
            std::cout << "Successfully added item: " << item.name << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "Error adding item: " << e.what() << std::endl;
            // 如果添加失败，需要回滚
            data.pop_back();
            throw;
        }
    }

    std::string search(const std::vector<float>& query, size_t K) {
        // 查询最近邻
        std::string name = "";
        auto result = alg->searchKnn(query.data(), K);
        
        std::unordered_map<std::string, int> candidates;
        float best_similarity = 0.0f;
        
        // 处理 priority_queue 结果
        while (!result.empty()) {
            auto item = result.top();  // 获取队列的顶部元素
            result.pop();  // 弹出顶部元素
            
            // 第一重判断：L2距离阈值
            if(item.first > 1.1f) {
                // std::cout << "item.first " << item.first;
                continue;
            }
            // 获取原始ID和向量索引
            size_t originalId = item.second / 10;  // 现在除以10
            int vectorIndex = item.second % 10;    // 现在对10取余
            
            // 获取对应的特征向量
            const auto& storedData = indexMap[originalId];
            const auto& storedVector = storedData.vectors[vectorIndex];
            
            // 第二重判断：余弦相似度
            float cos_sim = cosine_similarity(query, storedVector);
            
            if(cos_sim > 0.85f) {
                std::cout << "Checking " << storedData.name 
                         << " (Vector " << vectorIndex + 1 
                         << ") Cosine Similarity: " << cos_sim << std::endl;
                continue;
            }
            
            
            // 记录这个候选人
            candidates[storedData.name]++;
            if (cos_sim > best_similarity) {
                best_similarity = cos_sim;
                name = storedData.name;
            }
        }
        
        if(candidates.empty()) return "";
        // 检查候选人的投票数
        // std::cout << "\nCandidate votes:" << std::endl;
        // for (const auto& pair : candidates) {
        //     std::cout << pair.first << ": " << pair.second << " votes" << std::endl;
        // }
        
        // 只有当某个候选人得票超过阈值时才返回结果
        if (!candidates.empty()) {
            auto best_candidate = std::max_element(
                candidates.begin(), 
                candidates.end(),
                [](const auto& p1, const auto& p2) {
                    return p1.second < p2.second;
                }
            );
            
            // 增加投票阈值，因为现在有更多的特征向量
            if (best_candidate->second >= 1) {  // 要求至少1个向量匹配
                name = best_candidate->first;
                std::cout << "\nSelected candidate: " << name 
                         << " with " << best_candidate->second << " votes" 
                         << " and best similarity: " << best_similarity << std::endl;
            } else {
                std::cout << "No candidate has enough votes" << std::endl;
                name = "";
            }
        }
        
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

        // 保存 indexMap 到附加文件
        std::ofstream mapFile(filename + ".map", std::ios::binary);
        if (!mapFile) {
            throw std::runtime_error("Failed to open map file for saving.");
        }

        // 保存 indexMap 的大小
        size_t mapSize = indexMap.size();
        mapFile.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));

        // 保存 indexMap 的数据
        for (const auto& pair : indexMap) {
            size_t index = pair.first;
            const IndexData& data = pair.second;
            
            // 写入索引
            mapFile.write(reinterpret_cast<const char*>(&index), sizeof(index));
            
            // 写入名字长度和名字
            size_t nameLength = data.name.length();
            mapFile.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
            mapFile.write(data.name.c_str(), nameLength);
            
            // 写入向量数量
            size_t vectorCount = data.vectors.size();
            mapFile.write(reinterpret_cast<const char*>(&vectorCount), sizeof(vectorCount));
            
            // 写入所有特征向量
            for(const auto& vector : data.vectors) {
                size_t vectorSize = vector.size();
                mapFile.write(reinterpret_cast<const char*>(&vectorSize), sizeof(vectorSize));
                mapFile.write(reinterpret_cast<const char*>(vector.data()), sizeof(float) * vectorSize);
            }
        }
    }

    void loadFromFile(const std::string& filename) {
        // 加载索引
        alg->loadIndex(filename, space);

        // 加载 indexMap
        std::ifstream mapFile(filename + ".map", std::ios::binary);
        if (!mapFile) {
            throw std::runtime_error("Failed to open map file for loading.");
        }

        // 加载 indexMap 的大小
        size_t mapSize;
        mapFile.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));

        // 清空原有 indexMap
        indexMap.clear();

        // 重新加载 indexMap 的数据
        for (size_t i = 0; i < mapSize; ++i) {
            size_t index;
            mapFile.read(reinterpret_cast<char*>(&index), sizeof(index));
            
            // 读取名字
            size_t nameLength;
            mapFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
            std::string name(nameLength, '\0');
            mapFile.read(&name[0], nameLength);
            
            // 读取向量数量
            size_t vectorCount;
            mapFile.read(reinterpret_cast<char*>(&vectorCount), sizeof(vectorCount));
            
            // 读取所有特征向量
            std::vector<std::vector<float>> vectors;
            vectors.reserve(vectorCount);
            
            for(size_t j = 0; j < vectorCount; ++j) {
                size_t vectorSize;
                mapFile.read(reinterpret_cast<char*>(&vectorSize), sizeof(vectorSize));
                
                std::vector<float> vector(vectorSize);
                mapFile.read(reinterpret_cast<char*>(vector.data()), sizeof(float) * vectorSize);
                vectors.push_back(std::move(vector));
            }
            
            // 构建 IndexData 并存储
            indexMap[index] = {name, std::move(vectors)};
        }
    }
};

#endif // CUSTOMHNSW