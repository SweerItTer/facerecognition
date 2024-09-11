//main函数在最外层目录

#define _use_dnn false
//opencv dnn 

#include "facenet.h"
#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>
#include <vector>

FaceNet::FaceNet(Ort::Env& env, const wchar_t* model_path, Ort::SessionOptions& session_options)
{
	session = new Ort::Session(env, model_path, session_options);
}

FaceNet::~FaceNet()
{
	if (session) {
		session->release();
		delete session;
	 }
}

// 澶勭悊鍥惧儚閫氶亾鍜屽ぇ灏�
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

		// 淇敼閫氶亾
		cv::resize(image, resized_image, cv::Size(nw, nh), 0, 0, cv::INTER_CUBIC);

		// 鐏拌壊濉厖
		cv::Mat new_image = cv::Mat::zeros(size, resized_image.type()) + cv::Scalar(128, 128, 128);
		int x_offset = (w - nw) / 2;
		int y_offset = (h - nh) / 2;
		resized_image.copyTo(new_image(cv::Rect(x_offset, y_offset, resized_image.cols, resized_image.rows)));

		resized_image = new_image;
	} else {
		// 鐩存帴淇敼
		cv::resize(image, resized_image, size, 0, 0, cv::INTER_CUBIC);
	}

	return resized_image;
}

// 褰掍竴鍖�
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


std::vector<float> FaceNet::outputs(const std::string& img_path, const std::vector<int64_t>& inputTensorShape){
	std::vector<float> inputTensors = {/* 濠电娀娼чˇ浠嬪磻閸曨垰鍌ㄩ柡宥冨妽缂嶅洭鏌熼幆褍鏆辩€殿喗濞婇弻鈩冩媴閸濆嫷鏆悗瑙勬惈閹凤拷 */};

	std::cout << "----process input image------\n";
/*闂備焦鎮堕崕鏌ュ磿閹惰棄纾挎慨姗嗗幖缁剁偤鏌涢弴銊ュ箺闁稿⿵鎷�*/
	cv::Mat image = cv::imread(img_path,cv::IMREAD_COLOR);//闂佽崵濮村ú鈺咁敋瑜戦妵鎰板炊椤掆偓閻愬﹪鏌涢幘妤€鍠氶弳顒勬⒑鏉炴媽鍏屽褎顨呴悾鐢告晸閿燂拷
	if (image.empty()) {
		throw std::runtime_error("Failed to read image: " + img_path);
	}

	cv::Mat img_input;
	//闂傚倷鐒﹁ぐ鍐矓閸洘鍋柛鈩兠欢鐐垫喐瀹ュ鍎嶉柨鐕傛嫹
	img_input = resizeImage(image, cv::Size(static_cast<int>(inputTensorShape[2]),
											static_cast<int>(inputTensorShape[3])), true);
	//闁荤喐绮庢晶妤冩暜婵犲嫮鍗氶柟缁㈠枛缁€宀勬煥閻曞倹瀚�
	img_input = preprocess_input(img_input);
	// 闂佽绻愮换鎰涘┑瀣ㄢ偓鍌炲煛閸屾氨绐為梺鍛婃处閸樹粙宕愰悙鐑樼厱婵﹩鍓氶幑锝嗙箾閸喎鐏寸€规洖鐖奸獮鍡氼槻闁告挸銈稿鍫曟倻閸℃顫悗瑙勬礀瀵墎绮欐径瀣ㄥ亰閻犳亽鍔庨悙鏇㈡⒑缁嬭法绠為柛搴ㄤ憾楠炲啫饪伴崼鐔哄帓闂佽法鍣﹂幏锟�
	
	inputTensors = matToVector(img_input);

/*婵犵妲呴崹顏堝焵椤掆偓绾绢參鍩€椤掑倸鍘存鐐村浮婵℃瓕顦抽柛濠忔嫹*/
	Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
	Ort::Allocator* allocator = reinterpret_cast<Ort::Allocator*>(new Ort::AllocatorWithDefaultOptions());
	// 闂備礁鎲＄敮妤冪矙閹寸姷纾介柟鎯х亪閸嬫捇宕烽鐐扮钵缂備浇缈伴崕鐢告偘椤曗偓楠炲棝骞嶉鍝勪沪闂備胶枪閿曘劑宕戦幇顔煎灊濞村吋娼欑粻浼存煕閵夘喖澧悗姘炬嫹
	const char* inputName = session->GetInputName(0, *allocator);
	const char* outputName = session->GetOutputName(0, *allocator);

	std::vector<const char*> inputNames = {inputName};
	std::vector<const char*> outputNames = {outputName};

	std::vector<Ort::Value> inputTensorsOrt;
	inputTensorsOrt.push_back(
		Ort::Value::CreateTensor<float>(//闂佸搫顦弲婊堝蓟閵娿儍娲冀椤撶喎鍓梺鍛婃处閸撴氨鐟rt::Value缂傚倷绶￠崑澶愵敋瑜旈幃妤呮倻閽樺鐎梺缁橆殔閻楀棛绮婇敓锟�
			memoryInfo,//濠电儑绲藉ú鐘诲礈濠靛洤顕遍柛娑欐綑绾惧ジ鏌曟繛鍨姢闁圭晫濞€閺岋綁濡搁妷銉患闂佸摜鍋涢顓㈠焵椤掍礁鍠曢柟鍑ゆ嫹
			inputTensors.data(),//闂佽崵鍋為崙褰掑磻閸曨垁鍥蓟閵夈儳顦梺鍛婃尫閻掞箓鎮鹃柆宥嗙厸濞达絽鎼。鑲┾偓瑙勬惈閹凤拷
			inputTensors.size(),//闂佽崵鍋為崙褰掑磻閸曨垁鍥蓟閵夈儳顦梺鍛婃尫閻掞箓鎮鹃柆宥嗙厸濞达絽鎼。鑲┾偓瑙勬尫缁舵艾顫忚ぐ鎺戝瀭妞ゆ梻鈷堥崬锟�
			inputTensorShape.data(),//闁荤喐绮忛崺鍥垂缂佹ɑ鍙忛柟闂寸閺嬩線鏌ｅΔ鈧悧鍡欑矈閿燂拷
			inputTensorShape.size()//闁荤喐绮忛崺鍥垂缂佹ɑ鍙忛柟闂寸劍閳锋劙鏌涢…鎴濇灈妞ゆ棑鎷�
		)
	);

	std::cout << "----start to run model-----\n";
	// 闂佸搫顦弲婊堝礉濮椻偓閵嗕線骞嬪顏嗗枛閸ㄦ儳鐣烽崶锝呬壕闁绘垼妫勭粻鎶芥煏婵炑冨閳ь剨鎷�
	std::vector<Ort::Value> outputTensorsOrt = session->Run(
		Ort::RunOptions{nullptr},
		inputNames.data(), inputTensorsOrt.data(), inputNames.size(),
		outputNames.data(), outputNames.size()
	);
	allocator->release();
	delete allocator;
	// 闂備礁婀辩划顖炲礉閺嚶颁汗闁搞儯鍔嶇紞鍥煙閹冩毐婵絽顦遍埀顒侇問閸犳碍绻涢埀顒勬煕閿濆棙銇濈€殿噮鍋婇幃褔宕煎┑鍫涘亰
	float* floatArray = outputTensorsOrt[0].GetTensorMutableData<float>();
	std::vector<float> outputTensors(floatArray, floatArray + outputTensorsOrt[0].GetTensorTypeAndShapeInfo().GetElementCount());
	return outputTensors;
}
