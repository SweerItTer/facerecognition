#define _use_dnn false //opencv dnn无法加载模型(结构不允许)

#include <QString>
#include "facenet.h"
#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>
#include <vector>

FaceNet::~FaceNet()
{
	if (session) {
		session->release();
		delete session;
	 }
}

bool FaceNet::loadModel(QString model_path) {
	if(model_path.isEmpty()){
		std::cerr << "model path is empty!" << std::endl;
		return isLoaded;
	}
	env = new Ort::Env(ORT_LOGGING_LEVEL_ERROR, "");
	Ort::SessionOptions session_options;

	session = new Ort::Session(*env, model_path.toStdWString().c_str(), session_options);
	isLoaded = true;
	return isLoaded;
}

//修改图片大小
cv::Mat FaceNet::resizeImage(const cv::Mat& image, const cv::Size& size, bool letterbox_image) {
	cv::Mat resized_image;

	if (letterbox_image) {
		int iw = image.cols;
		int ih = image.rows;
		int w = size.width;
		int h = size.height;
		double scale = std::min(static_cast<double>(w) / iw, static_cast<double>(h) / ih);
		int nw = static_cast<int>(iw * scale);
		int nh = static_cast<int>(ih * scale);

		// 使用 INTER_CUBIC 插值进行图像缩放
		cv::resize(image, resized_image, cv::Size(nw, nh), 0, 0, cv::INTER_CUBIC);

		// 创建一个新的大小为 size 的空白图像，并将调整大小后的图像粘贴到中心位置
		cv::Mat new_image = cv::Mat::zeros(size, resized_image.type()) + cv::Scalar(128, 128, 128);
		int x_offset = (w - nw) / 2;
		int y_offset = (h - nh) / 2;
		resized_image.copyTo(new_image(cv::Rect(x_offset, y_offset, resized_image.cols, resized_image.rows)));

		resized_image = new_image;
	} else {
		// 直接进行图像缩放到指定大小
		cv::resize(image, resized_image, size, 0, 0, cv::INTER_CUBIC);
	}

	return resized_image;
}

//归一化
cv::Mat FaceNet::preprocess_input(cv::Mat& image) {
	image.convertTo(image, CV_32F, 1.0 / 255);
	return image;
}


std::vector<float> FaceNet::matToVector(const cv::Mat& img) {
	std::vector<float> result;
    // Assume img is a 3-channel image
    int channels = img.channels();
    int height = img.rows;
    int width = img.cols;

    // Split the image into separate channels
    std::vector<cv::Mat> chw(channels);
    cv::split(img, chw);

    // Concatenate the channels in NCHW order
    for (int c = 0; c < channels; ++c) {
        result.insert(result.end(), (float*)chw[c].datastart, (float*)chw[c].dataend);
    }

    return result;
}


std::vector<float> FaceNet::outputs(const cv::Mat& image, const std::vector<int64_t>& inputTensorShape){
	std::vector<float> inputTensors = {/* 填充输入数据 */};

	cv::Mat img_input;
	//重置大小
		img_input = resizeImage(image, cv::Size(static_cast<int>(inputTensorShape[2]),
											static_cast<int>(inputTensorShape[3])), true);
	//归一化
	img_input = preprocess_input(img_input);
	// 将预处理后的图像转换为输入向量
	inputTensors = matToVector(img_input);

/*模型推理*/
	Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
	Ort::Allocator* allocator = reinterpret_cast<Ort::Allocator*>(new Ort::AllocatorWithDefaultOptions());
	// 创建实例并赋值给指针
	const char* inputName = session->GetInputName(0, *allocator);
	const char* outputName = session->GetOutputName(0, *allocator);

	std::vector<const char*> inputNames = {inputName};
	std::vector<const char*> outputNames = {outputName};

	std::vector<Ort::Value> inputTensorsOrt;
	inputTensorsOrt.push_back(
		Ort::Value::CreateTensor<float>(//返回的是Ort::Value类型数据
			memoryInfo,//信息储存的内存
			inputTensors.data(),//被复制的数据
			inputTensors.size(),//被复制的数据长度
			inputTensorShape.data(),//形状数据
			inputTensorShape.size()//形状长度
		)
	);

	std::cout << "----FaceNet start run-----\n";
	// 运行模型推理
	std::vector<Ort::Value> outputTensorsOrt = session->Run(
		Ort::RunOptions{nullptr},
		inputNames.data(), inputTensorsOrt.data(), inputNames.size(),
		outputNames.data(), outputNames.size()
	);
	allocator->release();
	delete allocator;
	// 提取输出张量数据
	float* floatArray = outputTensorsOrt[0].GetTensorMutableData<float>();
	std::vector<float> outputTensors(floatArray, floatArray + outputTensorsOrt[0].GetTensorTypeAndShapeInfo().GetElementCount());
	return outputTensors;
}
