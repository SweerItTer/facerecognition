#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <functional>
#include <QPixmap>
#include <QImage>
#include <QDebug>

#include "./Yolov8/YOLO/yolo.h"

class Yolo;

// 处理图像的线程类
class ImageProcessor {
public:
    ImageProcessor(Yolo *y) : stopFlag(false), yolo(y) {
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
    void process() {
        std::unique_lock<std::mutex> lock(mutex);
        while (!stopFlag) {
            cv.wait(lock, [this] { return !image.empty() || stopFlag; });

            if (stopFlag) break;

            // Process the image if it's valid
            if (!image.empty() && callback) {
                cv::Mat result;
                yolo->runModel(image, "image", result);
                if (result.empty() || result.type() != CV_8UC3) {
                    qDebug() << "Received invalid cv::Mat.";
                    continue;
                }

                QImage img(result.data, result.cols, result.rows, result.step, QImage::Format_RGB888);
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
};

#endif // IMAGEPROCESSOR_H