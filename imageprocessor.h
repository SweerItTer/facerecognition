#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <functional>

#include <QPixmap>
#include <QImage>
#include <QDebug>

#include "./Yolov8/YOLO/yolo.h"
#include "./Sql/facedatabase.h"
#include "./Facenet/facenet.h"

class Yolo;

// 处理图像的线程类
class ImageProcessor {
public:
    ImageProcessor(Yolo *y, FaceNet *f, FaceDatabase *db) 
        : stopFlag(false), yolo(y), facenet(f), database(db) {
        // 创建线程
        processingThread = std::thread(&ImageProcessor::process, this);
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
    bool compareFeatures(const std::vector<float>& features,
                        const std::vector<std::vector<float>>& stored_features, const std::string& user_id, std::string& matched_user) {
            // 计算特征距离
            for (const auto& stored_feature : stored_features) {
                double distance = 0.0;
                for (size_t i = 0; i < features.size(); ++i) {
                    distance += std::pow(features[i] - stored_feature[i], 2);
                }
                distance = std::sqrt(distance);

                if (distance <= 1.1) {
                    matched_user = user_id;
                    return true;
                }
            }
            return false;
    }

    // 处理多张人脸
    void processFaces(const std::vector<cv::Mat>& result,
                     std::vector<std::pair<std::string, std::vector<std::vector<float>>>>& all_features) {
                
        std::vector<std::future<bool>> futures;
        std::string matched_user;

        // 限制同时运行的异步任务数量
        const size_t max_concurrent_tasks = std::thread::hardware_concurrency();  // 根据系统线程数调整
        size_t task_counter = 0;

        // 存在多张人脸时
        for (int index = 1; index < result.size(); index++) {
            std::vector<float> features = facenet->outputs(result[index], {1, 3, 160, 160});

            // 异步特征比对
            for (const auto& user_features : all_features) {
                const std::string& user_id = user_features.first;
                const auto& stored_features = user_features.second;

                // 控制并发任务数
                if (task_counter >= max_concurrent_tasks) {
                    for (auto& future : futures) {
                        if (future.get()) {
                            std::cout << "Matched user: " << matched_user << std::endl;
                            return;
                        }
                    }
                    futures.clear();
                    task_counter = 0;
                }

                futures.push_back(std::async(std::launch::async, 
                                            &ImageProcessor::compareFeatures, 
                                            this, 
                                            std::cref(features), 
                                            std::cref(stored_features), 
                                            user_id, 
                                            std::ref(matched_user)));
                ++task_counter;
            }
        }

        // 等待剩余的异步任务完成
        for (auto& future : futures) {
            if (future.get()) {
                std::cout << "Matched user: " << matched_user << std::endl;
                return;
            }
        }
    }


void process() {
    std::unique_lock<std::mutex> lock(mutex);  // 锁住互斥量，确保线程安全
    std::string uname;
    auto all_features = database->getAllFeatures();  // 获取所有人脸特征以及用户名

    while (!stopFlag) {
        // 等待条件变量的通知，直到有新图像或停止标志被触发
        cv.wait(lock, [this] { return !image.empty() || stopFlag; });

        if (stopFlag) break;  // 如果停止标志被设置，则退出循环

        // 如果图像存在并且回调函数有效，则进行处理
        if (!image.empty() && callback) {
            std::vector<cv::Mat> result;

            // 使用 YOLO 模型进行图像处理
            yolo->runModel(image, "image", result);

            // 检查 YOLO 输出结果是否有效
            if (result.empty() || result[0].type() != CV_8UC3) {
                qDebug() << "Received invalid cv::Mat.";
                continue;
            }
            // 调用人脸处理函数处理 YOLO 检测到的人脸            
            if (result.size() > 1)  processFaces(result, all_features);
            // 将 cv::Mat 转换为 QImage 和 QPixmap，方便在 Qt 中显示
            QImage img((uchar*)(result[0].data), static_cast<int>(result[0].cols), 
                        static_cast<int>(result[0].rows), static_cast<int>(result[0].step), QImage::Format_RGB888);
            QPixmap pixmap = QPixmap::fromImage(img.rgbSwapped());

            // 使用回调函数返回处理后的图像
            callback(pixmap);

            // 释放处理后的图像，确保内存不会被反复占用
            image.release();
        }
    }
}


    std::thread processingThread;
    std::mutex mutex;
    std::condition_variable cv;
    cv::Mat image;
    bool stopFlag;
    std::function<void(const QPixmap&)> callback;

    Yolo *yolo = nullptr;
    FaceNet *facenet = nullptr;
    FaceDatabase *database = nullptr;
};

#endif // IMAGEPROCESSOR_H
