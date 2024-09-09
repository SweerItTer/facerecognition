锘块敇锟�#include "widget.h"
#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>

int main()
{
	bool is_cuda = false;
	//閿熼ズ纭锋嫹閿熸枻鎷锋ā閿熸枻鎷�(鏉冮敓鏂ゆ嫹閿熶茎纭锋嫹)
	const wchar_t* model_path = L"D:/Program/project/gitee/Face/facenet/FaceNet/ONNX_model/full_model.onnx";
	Ort::Env env(ORT_LOGGING_LEVEL_ERROR, "ONNX_test");

	Ort::SessionOptions session_options;
//	if (is_cuda) {
//		//to do something
//	} else {
//	   std::cout << "Running on CPU/n";
//	}

	Widget w(env, model_path, session_options);

	const std::vector<int64_t> input_shape = {1,3,160,160};//閿熻姤瀹氶敓鏂ゆ嫹閿熸枻鎷峰浘鐗囬敓鏂ゆ嫹灏�
	while (true) {
		//閿熸枻鎷烽敓鏂ゆ嫹鍥剧墖璺敓鏂ゆ嫹
		std::string img1, img2;
//		std::cout << "Enter Image1 path: ";
//		std::cin >> img1;
//		std::cout << "Enter Image2 path: ";
//		std::cin >> img2;
		img1 = "D:/Program/project/C++/ONNX/img/1.jpg";
		img2 = "D:/Program/project/C++/ONNX/img/2.jpg";
		try {
			std::vector<float> output_data1 = w.outputs(img1, input_shape);
			std::vector<float> output_data2 = w.outputs(img2, input_shape);

			double distance = 0.0;
		//娆ч敓鏂ゆ嫹閿熸枻鎷锋閿熸枻鎷烽敓锟�
			for (size_t i = 0; i < output_data1.size(); ++i) {
				//閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鍏冮敓鎴拝鎷烽敓鏂ゆ嫹鍊奸敓鏂ゆ嫹骞抽敓鏂ゆ嫹閿熸枻鎷�
				distance += std::pow(output_data1[i] - output_data2[i], 2);
			}
			//閿熷姭鍖℃嫹閿熸枻鎷�
			distance = std::sqrt(distance);

			if (distance <= 1.1) {
				std::cout << "is the same person." << std::endl;
			} else {
				std::cout << "is not same one." << std::endl;
			}
			std::cout << "Euclidean distance between the two vectors: " << distance << std::endl;
			// 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟�
			output_data1.clear();
			output_data2.clear();
		} catch (const std::exception& e) {
			std::cerr << "Error: " << e.what() << std::endl;
		}
		// break;
	}

	return 0;
}
