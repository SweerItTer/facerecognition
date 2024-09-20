#ifndef FACENET_H
#define FACENET_H

#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>

class FaceNet
{

public:
	FaceNet(const wchar_t* model_path);

	~FaceNet();

	//模型输出
	std::vector<float> outputs(const std::string& img_path,//图片路径
						 const std::vector<int64_t>& inputTensorShape//“NCHW”格式 N代表批处理大小，C代表通道，H代表高度，W代表宽度）
						 );

private:
	//调整图片大小为输入标准
	static cv::Mat resizeImage(const cv::Mat& image, const cv::Size& size, bool letterbox_image);
	//图片矢量数据归一化
	static cv::Mat preprocess_input(cv::Mat& image);
	//mat转vector
	static std::vector<float> matToVector(const cv::Mat& mat);

	Ort::Env* env;
	Ort::Session* session = nullptr;
};
#endif // FACENET_H
