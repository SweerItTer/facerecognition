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
    bool compareFeatures(const std::vector<float>& features,
     const std::vector<std::vector<float>>& stored_features,
      const std::string& user_id, std::string& matched_user) {
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


    void process() {
        std::unique_lock<std::mutex> lock(mutex);
        std::string uname;
        while (!stopFlag) {
            cv.wait(lock, [this] { return !image.empty() || stopFlag; });

            if (stopFlag) break;

            // Process the image if it's valid
            if (!image.empty() && callback) {
                std::vector<cv::Mat> result;
                yolo->runModel(image, "image", result);
                if (result.empty() || result[0].type() != CV_8UC3) {
                    qDebug() << "Received invalid cv::Mat.";
                    continue;
                }

                // if (result.size() > 1) {
                //     auto all_features = database->getAllFeatures(); // 获取所有人脸特征以及用户名
                //     // 存在多张人脸时
                //     for (int index = 1; index < result.size(); index++) {
                //         // 获取人脸特征数据
                //         std::vector<float> features = facenet->outputs(result[index], {1, 3, 160, 160});
                //         for (const auto& user_features : all_features) {
                //             const std::string& user_id = user_features.first;
                //             const auto& stored_features = user_features.second;
                //             // 比较features和stored_features
                //             // 如果匹配，输出user_id
                //             try {
                //                 double distance = 0.0; // 距离
                //                 for(int faceindex = 0; faceindex < stored_features.size(); faceindex++){
                //                     // 计算距离
                //                     for (size_t i = 0; i < features.size(); ++i) {
                //                         distance += std::pow(features[i] - stored_features[faceindex][i], 2);
                //                     }
                //                     distance = std::sqrt(distance);
                //                     std::cout << "Euclidean distance between the two vectors: " << distance << std::endl;
                //                     if (distance <= 1.1) {
                //                         std::cout << user_id << std::endl;
                //                         uname = user_id;
                //                         break;
                //                     } else {
                //                         std::cout << "next" << std::endl;
                //                         continue;
                //                     }
                //                 }  
                //             } catch (const std::exception& e) {
                //                 std::cerr << "Error: " << e.what() << std::endl;
                //                 break;
                //             }
                //             if (uname != "") {
                //                 break;
                //             }
                //         }
                //     }
                //     if (uname == "") {
                //         std::cout << "No match found." << std::endl; // 姑且认为人脸没有录入
                //     }
                // }

                if (result.size() > 1) {
                    auto all_features = database->getAllFeatures(); // 获取所有人脸特征以及用户名
                    std::vector<std::future<bool>> futures;
                    std::string matched_user;

                    // 存在多张人脸时
                    for (int index = 1; index < result.size(); index++) {
                        // 获取人脸特征数据
                        std::vector<float> features = facenet->outputs(result[index], {1, 3, 160, 160});
                        for (const auto& user_features : all_features) {
                            const std::string& user_id = user_features.first;
                            const auto& stored_features = user_features.second;

                            // 使用异步任务进行特征比对
							futures.push_back(std::async(std::launch::async,
												std::bind(&ImageProcessor::compareFeatures,
													   this,            // 传递当前对象的实例
													   std::cref(features),
													   std::cref(stored_features),
													   user_id,
													   std::ref(matched_user))));
						}
                    }

                    // 等待所有异步任务完成
                    for (auto& future : futures) {
                        if (future.get()) {
                            std::cout << "Identified as: " << matched_user << std::endl;
                            uname = matched_user;
                            break;
                        }
                    }

                    if (uname.empty()) {
                        std::cout << "No match found." << std::endl; // 姑且认为人脸没有录入
                    }
                }
                QImage img(result[0].data, result[0].cols, result[0].rows, result[0].step, QImage::Format_RGB888);
                QPixmap pixmap = QPixmap::fromImage(img.rgbSwapped());
                callback(pixmap);

                image.release(); // Clear image after processing
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
