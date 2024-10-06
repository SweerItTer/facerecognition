#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <thread>
#include <future>
#include <mutex>
#include <condition_variable> // 流光法检测运动
#include <memory>
#include <functional>

#include <QPixmap>
#include <QImage>
#include <QDebug>

#include "./Yolov8/YOLO/yolo.h"
#include "./Sql/facedatabase.h"
#include "./Sql/customhnsw.h"
#include "./Facenet/facenet.h"
// #include "./pool.h"

class Yolo;

// 处理图像的线程类
class ImageProcessor {
public:
    ImageProcessor(Yolo *y, FaceNet *f, FaceDatabase *db) 
        : stopFlag(false), yolo(y), facenet(f), database(db) {
        // pool = new Pool(std::thread::hardware_concurrency());
        // 创建线程
        processingThread = std::thread(&ImageProcessor::process, this);
        hnsw = new CustomHNSW(1000); // 最大元素数为1000

        struct stat fileInfo;
        if(stat(hnswconfg.c_str(), &fileInfo) == 0){
            hnsw->loadFromFile(hnswconfg); // 从文件加载HNSW索引
            std::cout << "HNSW index loaded." << std::endl;
        } else {
            buildHNSWIndex();  // 构建HNSW索引
        }
    }

    ~ImageProcessor() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            stopFlag = true;
        }
        cv.notify_all();
        if (processingThread.joinable()) {
            processingThread.join();
        }
        if(hnsw->shouldUpdateIndex(hnswconfg)){
            std::cout << "HNSW index updated." << std::endl;
            hnsw->saveToFile(hnswconfg); // 保存HNSW索引到文件
        }
        delete hnsw; // HNSW索引
        delete yolo;
        delete facenet;
        delete database;
    }

    void setImage(const cv::Mat& img) {
        std::lock_guard<std::mutex> lock(mutex);
        image = img; // 直接引用
        cv.notify_all();
    }

    void setCallback(std::function<void(const QPixmap&)> cb) {
        std::lock_guard<std::mutex> lock(mutex);
        callback = cb;
    }

private:
    // 对比特征
    /*
    bool compareFeatures(const std::vector<float>& features,
                        const std::vector<std::vector<float>>& stored_features, const std::string& user_id, std::string& matched_user) {
            // 计算特征距离
            for (const auto& stored_feature : stored_features) {
                if (features.size() != stored_feature.size()) {
                    return false;  // 特征维度不匹配
                }
                double distance = 0.0;

                try
                {
                    for (size_t i = 0; i < features.size(); ++i) {
                        distance += std::pow(features.at(i) - stored_feature.at(i), 2);
                    }
                    distance = std::sqrt(distance);
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                    return false;
                }
                
                if (distance <= 1.1) {
                    matched_user = user_id;
                    return true;
                }
            }
            return false;
    }
    */

    // 处理多张人脸
    std::string processFaces(const std::vector<cv::Mat>& result) {

        // std::vector<std::future<bool>> futures;

        std::string matched_user;

        // 存在多张人脸时
        for (int index = 1; index < result.size(); index++) {
            std::vector<float> features = facenet->outputs(result.at(index), {1, 3, 160, 160});
            cv::imwrite("result.jpg", result.at(index));  // 保存检测结果图片

            matched_user = hnsw->search(features, 3);// 特征搜索(返回最近3个匹配的用户名)
        }

        return matched_user;

        /*

            // 异步特征比对
            for (const auto& user_features : all_features) {
                const std::string& user_id = user_features.first;
                const auto& stored_features = user_features.second;

                futures.emplace_back(pool->enqueue([this, features, stored_features, user_id, &matched_user]() {
                        return compareFeatures(features, stored_features, user_id, matched_user);
                        }));  // 异步执行特征比对

            }

        // 等待剩余的异步任务完成
        for (auto& future : futures) {
            if (future.get() == true) {
                std::cout << "Matched user: " << matched_user << std::endl;
                return;
            }
        }*/
    }

    // 数据库处理,构建HNSW索引
    void buildHNSWIndex() {
        auto all_features = database->getAllFeatures();  // 获取所有人脸特征以及用户名
        for(const auto& item : all_features) {
            hnsw->addItem(item); // 向HNSW索引中添加人脸特征
        }
        std::cout << "HNSW index built." << std::endl;
    }

    void process() {
        std::unique_lock<std::mutex> lock(mutex);  // 锁住互斥量，确保线程安全
        std::string uname;
        QPixmap pixmap;

        // 运动检测
        cv::Mat pimg;  // 上一帧图像
        cv::Mat nimg;  // 当前帧图像
        cv::Mat grayPrev, grayNext;

        while (!stopFlag) {
            // 等待条件变量的通知，直到有新图像或停止标志被触发
            cv.wait(lock, [this] { return !image.empty() || stopFlag; });
            
            if (stopFlag) break;  // 如果停止标志被设置，则退出循环

            cv::resize(image, nimg, cv::Size(image.cols / 10, image.rows / 10));  // 将图像缩小为原来的 50%

            // 将图像转换为灰度图
            cv::cvtColor(nimg, grayNext, cv::COLOR_BGR2GRAY);
            if (grayPrev.empty()) {
                // 第一次循环，初始化上一个图像
                grayPrev = grayNext.clone();    
                continue;  // 不进行处理
            }

            // 计算光流
            cv::Mat flow;
            cv::calcOpticalFlowFarneback(grayPrev, grayNext, flow, 0.5, 3, 15, 3, 5, 1.2, 0);


            // 检查 flow 的维度
            if (flow.empty() || flow.channels() != 2) {
                std::cerr << "Flow calculation failed or has incorrect number of channels!" << std::endl;
                continue;  // 跳过当前循环
            }

            // 使用第二种重载分割 flow
            std::vector<cv::Mat> flowChannels;
            cv::split(flow, flowChannels);  // flowChannels 将会包含 flow 的各个通道

            // 检查分割结果
            if (flowChannels.size() != 2) {
                std::cerr << "Flow split did not produce two channels!" << std::endl;
                return;  // 跳出或返回
            }

            // 提取 x 和 y 分量
            cv::Mat flowX = flowChannels[0];
            cv::Mat flowY = flowChannels[1];

            // 计算光流的大小和方向
            cv::Mat magnitude, angle;
            cv::cartToPolar(flowX, flowY, magnitude, angle);

            // 计算平均运动量
            double avgMagnitude = cv::mean(magnitude)[0];
            // std::cout << "Average motion magnitude: " << avgMagnitude << std::endl;
            
            // 如果运动量大于阈值，则进行人脸检测
            if (!image.empty() && callback && avgMagnitude > 1.0) {
                std::cout << "Motion detected." << std::endl;
                std::vector<cv::Mat> result;

                // 使用 YOLO 模型进行图像处理
                yolo->runModel(image, "image", result);

                // 检查 YOLO 输出结果是否有效
                if (result.empty() || result[0].type() != CV_8UC3) {
                    qDebug() << "Received invalid cv::Mat.";
                    continue;
                }
                // 调用人脸处理函数处理 YOLO 检测到的人脸            
                // 如果检测到人脸，处理并创建 QPixmap
                if (result.size() > 1) std::string uname = processFaces(result);
                QImage img((uchar*)(result[0].data), static_cast<int>(result[0].cols), 
                                static_cast<int>(result[0].rows), static_cast<int>(result[0].step), QImage::Format_RGB888);
                pixmap = QPixmap::fromImage(img.rgbSwapped());
            }
            // 原图显示
            QImage img((uchar*)(image.data), static_cast<int>(image.cols), 
                                static_cast<int>(image.rows), static_cast<int>(image.step), QImage::Format_RGB888);
            pixmap = QPixmap::fromImage(img.rgbSwapped());

            // 使用回调函数返回处理后的图像
            callback(pixmap);
            // 更新上一帧图像
            grayPrev = grayNext.clone();
            // 释放处理后的图像，确保内存不会被反复占用
            image.release();
        }
    }


    std::thread processingThread;
    std::mutex mutex;
    std::condition_variable cv;
    std::function<void(const QPixmap&)> callback;
    cv::Mat image;
    bool stopFlag;

    std::string hnswconfg = "./hnsw.bin";

    // Pool* pool = nullptr;  // 根据系统线程数创建线程池
    CustomHNSW *hnsw = nullptr; // HNSW索引
    Yolo *yolo = nullptr;
    FaceNet *facenet = nullptr;
    FaceDatabase *database = nullptr;
};

#endif // IMAGEPROCESSOR_H
