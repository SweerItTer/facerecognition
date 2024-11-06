# Face Recognition System

## 项目概述
这是一个基于深度学习的实时人脸识别系统，能够从网络视频流中检测和识别人脸，并将识别结果存储到数据库中。该系统采用多级处理流程，确保高效和准确的人脸识别。

## 主要功能
- 网络视频流获取与处理
- 实时人脸检测
- 高精度人脸识别
- 快速人脸特征检索
- 识别结果数据库存储

## 技术架构
### 核心组件
1. **视频处理模块** (Ffmpeg目录)
   - 使用FFmpeg进行网络视频流的拉取和解码

2. **人脸检测模块** (Yolov8目录)
   - 采用YOLOv8模型进行人脸检测
   - 作为预处理步骤，确保画面中存在人脸时才进行后续识别

3. **人脸识别模块** (Facenet目录)
   - 使用FaceNet模型提取人脸特征
   - 通过HNSW算法构建高效的特征索引，实现快速人脸匹配
   - 采用双重验证机制提高识别准确性：
     1. HNSW快速检索：从特征库中快速筛选出最可能的候选项
     2. 余弦相似度二次验证：对候选项进行精确度量，通过严格的相似度阈值确保匹配准确性
   ```cpp
   // 余弦相似度计算示例
   float cosine_similarity(const std::vector<float>& v1, const std::vector<float>& v2) {
       float dot_product = 0.0f;
       float norm1 = 0.0f;
       float norm2 = 0.0f;
       
       for (size_t i = 0; i < v1.size(); i++) {
           dot_product += v1[i] * v2[i];
           norm1 += v1[i] * v1[i];
           norm2 += v2[i] * v2[i];
       }
       
       return dot_product / (sqrt(norm1) * sqrt(norm2));
   }
   ```

4. **数据管理模块** (Sql目录)
   - 封装数据库操作
   - 存储识别结果和时间戳

5. **用户界面** (UI目录)
   - 基于Qt框架构建的图形界面

## 系统要求
### 依赖项
- OpenCV 4.5.1
- FFmpeg
- ONNX Runtime
- Qt 5.12
- MySQL
- Inno Setup 6（仅用于生成安装包）

### 平台支持
- 当前支持：Windows
- 计划支持：跨平台

### 硬件要求
- 本系统经过优化，可在配置适中的笔记本电脑上流畅运行
- 推荐配置：
  - CPU: 现代多核处理器
  - RAM: 8GB及以上
  - 显卡: 支持基本图形处理的集成显卡即可

## 数据库配置
### 数据库要求
1. 创建名为`FaceDB`的数据库
2. 在`FaceDB`中创建`FaceFeatures`表，包含以下字段：
   - `name` (VARCHAR): 用于存储人名
   - `feature_vector1` (VARBINARY(512)): 第一组人脸特征向量
   - `feature_vector2` (VARBINARY(512)): 第二组人脸特征向量
   - `feature_vector3` (VARBINARY(512)): 第三组人脸特征向量

### 特征向量生成
特征向量通过以下过程生成和处理：
1. 使用FaceNet模型的`output()`函数生成特征向量
   - 返回值为浮点数类型容器（std::vector<float>）
   - 每个向量代表一张人脸的独特特征
2. 系统为每个人存储3组特征向量，提高识别的准确性
3. 存储前，特征向量被序列化为二进制格式，以适应数据库的VARBINARY类型

### 示例代码
```cpp
// 生成特征向量的示例
std::vector<cv::Mat> face_images[3];

// 特征向量的典型用途
for (int i = 0; i < 3; ++i) {
    // 获取特征向量
    std::vector<float> vector = facenet.output(face_images[i]);
    
    // 将特征向量存储到数据库（伪代码）
    db.storeFeatureVector(person_name, vector, i + 1);
}
```

### 创建表的SQL语句
```sql
CREATE DATABASE IF NOT EXISTS FaceDB;
USE FaceDB;
CREATE TABLE IF NOT EXISTS FaceFeatures (
    name VARCHAR(255) NOT NULL,
    feature_vector1 VARBINARY(512) NOT NULL,
    feature_vector2 VARBINARY(512) NOT NULL,
    feature_vector3 VARBINARY(512) NOT NULL
);
```
### 快速预览
- 数据库名：`FaceDB`
- 主要表：`FaceFeatures`和`AttendanceRecords`
- 配置文件位置：[./Sql/aboutSql.txt](./Sql/aboutSql.txt)
> 注意：完整的数据库设置步骤、表结构说明和示例代码请查看配置文档
## 构建说明
项目使用CMake构建系统。在Windows环境下构建步骤如下：

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### 生成安装包
项目提供了自动化的安装包生成工具，位于`installer`目录下：

```
installer/
├── config.ini          # 配置文件（版本号、路径等）
├── deploy.bat          # 部署脚本
└── setup.iss.template  # Inno Setup模板文件
```

#### 使用方法
1. 进入installer目录：
```bash
cd installer
```

2. 运行部署脚本：
```bash
# 清理并重新构建Debug版本
deploy.bat clean debug

# 或构建Release版本
deploy.bat clean release

# 不清理直接构建
deploy.bat
```

3. 生成的安装包位于项目根目录的`installer_output`文件夹中：
```
installer_output/FaceRecognition_Setup_X.X.X.X.exe
```

#### 配置说明
在`installer/config.ini`中可以配置：
- 版本号
- 公司信息
- 依赖库路径
- Inno Setup路径

## 项目结构
```
.
├── CMakeLists.txt          # 主CMake配置文件
├── Facenet/                # FaceNet模型及相关代码
├── Ffmpeg/                 # FFmpeg视频处理相关代码
├── Sql/                    # 数据库操作相关代码
├── UI/                     # Qt用户界面代码
├── Yolov8/                # YOLOv8模型及相关代码
├── installer/             # 安装包生成工具
├── imageprocessor.h        # 图像处理器头文件
├── main.cpp               # 主程序入口
├── script.cpp             # 核心处理脚本
└── script.h               # 脚本头文件
```

## 工作流程
1. 从指定服务器拉取视频流
2. YOLOv8模型进行人脸检测
3. 当检测到一个或多个人脸时，激活FaceNet进行特征提取
4. 使用HNSW算法在现有数据库中快速检索最相近的人脸特征
5. 确定身份并将识别结果连同时间戳存入数据库

## 性能说明
- 系统经过优化，能在普通笔记本电脑上实现流畅运行
- 采用HNSW算法进行特征检索，大幅提升识别速度
- 使用多级处理策略，仅在检测到人脸时才进行特征提取，节省计算资源

## 注意事项
- 确保所有依赖正确安装
- 数据库配置需要根据实际环境调整
- 模型文件在对应文件夹下
```
├── Yolov8/sourse/base.onnx
├── Facenet/ONNX_model/full_model.onnx
```

## 待办事项
- [ ] 实现跨平台支持
- [ ] 优化检测和识别速度
- [ ] 增加更多配置选项

## 使用指南
### 初始配置
1. **数据库设置**
   - 确保MySQL服务已启动
   - 执行数据库初始化脚本（见数据库配置章节）
   - 检查数据库连接配置

2. **摄像头/视频流配置**
   - 在界面中输入RTSP流地址

### 人脸注册
1. 点击"人脸注册"按钮
2. 输入人员相关信息
3. 按照提示采集3张不同的人脸照片
4. 系统会自动提取特征并存入数据库

### 实时识别
1. 启动视频流
2. 系统会自动进行：
   - 人脸检测
   - 特征提取
   - 身份识别
3. 识别结果会实时显示在界面上
4. 同时自动记录到数据库中

### 历史记录查询
- [ ] 实现
1. 点击"历史记录"按钮
2. 可按以下条件筛选：
   - 时间范围
   - 人员姓名
   - 识别位置

### 常见问题处理
1. **视频流无法连接**
   - 检查网络连接
   - 验证RTSP地址是否正确
   - 确认视频服务器是否在线

2. **识别准确率问题**
   - 确保注册照片质量良好
   - 调整摄像头角度和光线
   - 可以重新采集人脸特征

3. **数据库连接失败**
   - 检查MySQL服务状态
   - 验证数据库用户名和密码
   - 确认数据库端口是否正确