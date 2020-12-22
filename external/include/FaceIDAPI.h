#ifndef FACEIDAPI_H
#define FACEIDAPI_H
#include <vector>

enum FUNC_CTRL_STATUS
{
	CTRL_REGISTER = 0x00000001,             //视频注册信号
	CTRL_REGISTER_CANCEL = 0x00000002,      //注册取消信号
	CTRL_RECOGNITION = 0x00000004,          //识别信号
	CTRL_SAVE_REGINFO = 0x00000008,         //保存注册集信号
	CTRL_DELETE_ONE = 0x00000010,           //删除单人信号
	CTRL_EXIT_MODULE = 0x00000020,          //退出当前模块
	CTRL_CLEAR_RECOGNITION = 0x00000040,    //清零识别模块
	CTRL_UPDATE_REGFEATURE=0x00000080,      //更新当前模型的人脸特征数据
	CTRL_RECOGNITION_ALL= 0x00000100,       //暂时无效
	CTRL_REGISTER_BY_PIC = 0x00000200,      //单张彩色图像注册信号
	CTRL_IMPORT_FEATURE_DATA = 0x00000400   //导入注册特征信号
};

enum FUNC_RETURN_STATUS
{
	RETURN_NO_VALID = 0x00000000,           //无效状态
	
	RETURN_RECOGING = 0x00000001,           //正在识别状态
    RETURN_REC_SUCCESS = 0x00000003,        //识别成功状态

	RETURN_REGISTER_SUCCESS = 0x00000002,   //注册成功状态
	RETURN_REGISTERING = 0x00000004,        //正在注册状态
	RETURN_REGISTER_FAIL = 0x00000005,      //注册失败

	RETURN_CANCELING = 0x00000008,          //正在取消注册状态
	RETURN_CANCEL_SUCCESS = 0x00000010,     //取消注册成功状态

	RETURN_SAVE_REGINFO_SUCCESS = 0x00000020,       //保存单人注册成功状态
	RETURN_SAVE_REGINFOING = 0x00000040,            //正在保存单人注册集状态
	RETURN_SAVE_REGINFO_FAIL = 0x00000080,          //保存单人注册集失败状态
	RETURN_SAVE_REGINFO_FULL = 0x00000100,          //注册集已满状态
	RETURN_SAVE_HAVE_REGISTERED = 0x00000200,       //当前人已经注册

	RETURN_DELETEING = 0x00000400,                  //正在删除单人状态
	RETURN_DELETE_SUCCESS = 0x00000800,             //删除单人成功状态

	RETURN_CLEAR_RECOGNITION_SUCCESS = 0x00001000,  //清零识别模块成功

	RETURN_UPDATE_REGFEATUREING = 0x00002000,       //正在更新特征集
	RETURN_UPDATE_REGFEATURE_SUCCESS = 0x00004000,  //更新特征集成功
	RETURN_UPDATE_REGFEATURE_FAIL = 0x00008000,     //更新特征集失败

	RETURN_RECOALLING = 0x00010000,                 //暂时无效
	RETURN_RECOALL_SUCCESS = 0x00020000,            //暂时无效
	RETURN_RECOALL_FAIL = 0x00040000,               //暂时无效
    
    RETURN_IMPORT_FEATUREING = 0x00080000,          //正在导入特征
    RETURN_IMPORT_FEATURE_SUCCESS = 0x00100000,     //导入特征成功
    RETURN_IMPORT_FEATURE_FAIL = 0x00200000,        //导入特征识别
};

enum ERROR_STATUS
{
	ERROR_CREATE_CONTEXT=0, //初始化模块失败状态(暂时无效)
	ERROR_RELEASE_CONTEXT=1 //释放模块失败状态(暂时无效)
};

//注册姿态引导(暂时无效)
enum POSE_GUIDE_STATUS
{
	KEEP=0,
	FRONT,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

//视频类型
enum VIDEO_TYPE
{
	NV21=0,
	NV12,
	RGB,
	BGR,
	GRAY
};

//ISP参数类型(暂时无效)
enum ISP_PARAM_TYPE
{
	SENSOR_EXPOSURE_TIME = 0,
	SENSOR_ANALOGY_GAIN,
	ISP_ANALOGY_GAIN,
	ISP_DIGITAL_GAIN,
};

enum PICTURESRESULT
{
    RETURN_SUCESS = 0,                     //图片注册成功
    RETURN_MALLOC_ERR,                     //内存分配错误
    RETURN_QUALITY_BAD,                    //图片质量问题
    RETURN_NO_FACE,                        //未发现人脸
    RETURN_SIZE_SMALL,                     //人脸框太小
};

enum REC_MODE
{
    REC_MAX_FACE = 0,                     //识别最大人脸(default)
    REC_ALL_FACE,                         //识别所有人脸
};

enum IR_MODE
{
    IR_ENABLE = 0,                     //使能红外模块(default)
    IR_DISABLE,                        //禁止红外模块
};

enum RETURNREGSUC
{
    RETURN_DISABLE = 0,                //禁止返回识别成功的数据
    RETURN_ENABLE,                    //使能返回识别成功的数据
};

enum PRINT_LEVEL
{
    SET_LOG_OFF    = 0,                 //default
    SET_LOG_DEBUG,
    SET_LOG_INFO,
};

typedef struct DS_FaceInfo
{
	unsigned int XMin;                      //人脸框左上角横坐标
	unsigned int YMin;                      //人脸框左上角纵坐标
	unsigned int XMax;                      //人脸框右下角横坐标
	unsigned int YMax;                      //人脸框右下角纵坐标
#ifdef USE_GUID
	unsigned char ID[16];                   //识别出来的人脸ID
#else
    unsigned long ID;                       //识别出来的人脸ID
#endif
	//unsigned char debugInfo[255];           //调试信息(暂时无效)
	//unsigned char imgData[112 * 112 * 3];   //人脸图像(暂时无效)

	int rgbLiveOrNot;                       //可见光活体判断，1是活体，0是非活体
	int irLiveOrNot;                        //红外活体判断，1是活体，0是非活体
	int faceMaskOrNot;                      //口罩判断(-1无效，0是非口罩， 1是口罩)
    int recStatus;                          //识别状态：RETURN_NO_VALID, RETURN_RECOGING or RETURN_REC_SUCCESS
    float recScoreVal;                      //识别score
    unsigned int trackID;                   //人脸跟踪ID

    int pose;                               //姿态(-1->9: 0,1,2,5,6有效姿态)
    int helmetOrNot;                        //安全帽(not used)
    
	/*
	unsigned int age;
	unsigned int gender;
	*/
}DS_FaceInfo;

typedef struct DS_FaceIDInData
{
	unsigned int FuncFlag;          //模块输入控制信号

	unsigned char *ptrYUVSubStream[2]; //视频人脸注册或识别传给算法的彩色BGR格式图像

	unsigned char *ptrIRSubStream;  //视人脸识别传给算法的红外图像

	unsigned char *ptrBGRImage;     //单张图像注册的彩色BGR格式图像
	int bgrImageWidth;              //彩色BGR格式图像的宽度
	int bgrImageHeight;             //彩色BGR格式图像的高度

	int colorVideoType;             //彩色视频数据类型
    int irVideoType;                //灰度数据类型

#ifdef USE_GUID
    unsigned char importFaceID[16]; //需要添加/删除单人的GUID
#else
    unsigned long importFaceID;     //需要添加/删除单人的GUID
#endif

	float faceThresh[6];            //人脸识别阈值

	unsigned char *ptrImportFeatureData;    //导入特征buf地址
	unsigned int importFeatureLen;          //导入特征长度
    
    unsigned int recFaceMode;               //REC_MODE
    
    int minRecFaceW;                //识别最小人脸框size
    int maxRecFaceW;                //识别最大人脸框size
}DS_FaceIDInData;

typedef struct DS_FaceIDOutData
{
	unsigned int curStatus;             //模块当前状态
	float regProgress;                  //注册进度百分比
	unsigned int regGuidePose;          //注册姿态引导(暂时无效)
#ifdef USE_GUID
	unsigned char faceID[16];           //注册成功后返回的当前注册人ID
#else
    unsigned long faceID;               //注册成功后返回的当前注册人ID
#endif
	unsigned int totalRegPersonsNum;    //注册集人数

    std::vector<DS_FaceInfo> faceInfo;  //人脸检测和识别信息集合
    
	unsigned int curFaceNum;            //当前帧图像检测到的人脸数目
    
    unsigned int pictureStatus;         //图片注册结果以及失败原因
    
    unsigned char *returnRegsucImg;     //返回识别成功的一帧数据
    unsigned int returnImgW;            //返回识别成功的数据的宽
    unsigned int returnImgH;            //返回识别成功的数据的高
}DS_FaceIDOutData;

//typedef void(*SetCameraPara)(const float sensorISPParam, const int paramType);
//typedef float(*GetCameraPara)(const int paramType);
//输入数据回调函数 应用层定义
//* 函数说明：应用层将图像数据传输到算法层
/** 输入参数：
    ptrFaceIDInData:输入数据指针
    ptrAppHandle：应用层数据句柄指针
* 输出参数：无
* 返回值：无
*/
typedef void(*FaceIDInDataCallback)(DS_FaceIDInData *ptrFaceIDInData, void *ptrAppHandle);
//输出数据回调函数，应用层定义
//* 函数说明：算法层将运行结果传输到应用层
/** 输入参数：
    ptrFaceIDOutData:输出数据指针
    ptrAppHandle：应用层数据句柄指针
* 输出参数：无
* 返回值：无
*/
typedef void(*FaceIDOutDataCallback)(DS_FaceIDOutData *ptrFaceIDOutData, void *ptrAppHandle);
//显示结果回调函数，应用层定义
//* 函数说明：算法调试用，在界面上显示人脸坐标框等信息，应用层置空则算法层不调用
/** 输入参数：
    ptrFaceIDOutData:输出数据指针
    ptrAppHandle：应用层数据句柄指针
* 输出参数：无
* 返回值：无
*/
//typedef void(*CameraDrawRect)(DS_FaceIDOutData *ptrFaceIDOutData,void *ptrAppHandle);
//isp曝光设置回调函数，暂时无效
//typedef int (*AlgAeInitial)(int sensorId, unsigned char u8Compensation, unsigned char bManualExpValue, int u32ExpValue);


typedef struct DS_FaceIDParas
{
	//SetCameraPara setCameraPara;

	FaceIDInDataCallback xFaceIDInDataCallback;  //输入数据回调函数指针(回调方式才使用)
	FaceIDOutDataCallback xFaceIDOutDataCallback;//输出数据回调函数指针(回调方式才使用)

	//CameraDrawRect xCameraDrawRect;
	//AlgAeInitial algAeInitial;

	unsigned int imgWidth[2];               //视频检测或识别所用图图宽
	unsigned int imgHeight[2];              //视频检测或识别所用图图高
	unsigned int irMode;                    //设置是否启用IR_MODE

	unsigned char ptrRegFilePath[512];      //注册集保存全局路径
	unsigned char ptrModelFileAbsDir[512];  //模型文件保存全局路径
    unsigned char ptrFaceImgFilePath[512];  ////注册人脸图像全局路径

	unsigned char recognizeLevel;           //识别等级(暂时无效)

	void *ptrAppHandle;                     //应用层与算法层数据交互钩子，应用层定义(回调方式才使用)

	int reserve[16];                        //keep for use

	unsigned int reRegsucessMode;           //是否开启返回识别数据线程

}DS_FaceIDParas;

//* 函数说明：获取版本号（用法见app.cpp）
/** 输入参数：
    ptrFaceIDHandle:人脸FaceID句柄指针(或者置NULL)
* 输出参数：无
* 返回值：
*/
const char *DS_GetFaceIDVersion(void *ptrFaceIDHandle);

//* 函数说明：算法模块初始化函数（用法见app.cpp）
/** 输入参数：
    ptrFaceIDParas:人脸静态参数指针
* 输出参数：无
* 返回值：人脸FaceID句柄指针
*/
void *DS_CreateFaceIDContext(DS_FaceIDParas *ptrFaceIDParas);

//* 函数说明：//算法模块资源释放函数（用法见app.cpp）
/** 输入参数：
    ptrFaceIDHandle:人脸FaceID句柄指针
* 输出参数：无
* 返回值：1为释放成功，否则为失败
*/
int DS_ReleaseFaceIDContext(void *ptrFaceIDHandle);

//* 函数说明：算法模块设置输入和获取输出函数（用法见appCall.cpp）
/** 输入参数：
    ptrFaceIDHandle:人脸FaceID句柄指针
    ptrFaceIDInData:输入参数指针
    ptrFaceIDOutData:输出结果指针
* 输出参数：无
* 返回值：0为函数异常
*/
int DS_FaceIDProcess(void *ptrAppHandle, DS_FaceIDInData* ptrFaceIDInData, DS_FaceIDOutData *ptrFaceIDOutData);

//* 函数说明：设置打印等级
/** 输入参数：
    ptrFaceIDHandle: 人脸FaceID句柄指针
    printLevel: 等级设置，see: PRINT_LEVEL
* 输出参数：无
* 返回值：0
*/
int DS_SetPrintLevel(void *ptrAppHandle, int printLevel);

//* 函数说明：检测线程主函数,便于应用层创建和控制优先级
/** 输入参数：
    ptrFaceIDHandle: 人脸FaceID句柄指针
* 输出参数：无
* 返回值：如果为NULL,异常
*/
void *DS_FaceDetition(void *ptrAppHandle);

//* 函数说明：识别线程主函数,便于应用层创建和控制优先级
/** 输入参数：
    ptrFaceIDHandle: 人脸FaceID句柄指针
* 输出参数：无
* 返回值：如果为NULL,异常
*/
void *DS_FaceRecognition(void *ptrAppHandle);

#endif