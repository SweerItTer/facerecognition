#ifndef CUSTOMHNSW_H
#define CUSTOMHNSW_H

#include "hnswlib.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include "dataitem.h"

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
};

#endif // CUSTOMHNSW