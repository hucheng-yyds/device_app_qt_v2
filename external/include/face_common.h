#pragma once

typedef void* FaceHandle;
typedef void* FaceGroupHandle;

typedef enum FaceRetCode {
    RET_OK = 0,
    RET_INTERNAL_ERROR,
    RET_NO_FACE,
    RET_NULL_POINTER,
    RET_UNEXPECTED_MODEL,
    RET_BROKEN_FILE,
    RET_OUT_OF_RANGE,
    RET_FILE_NOT_FOUND,
    RET_INVALID_ARGUMENT,
    RET_UNAUTHORIZED,
    RET_UNSUPPORTED,
    RET_UNINITIALIZED,
    RET_NOT_FOUND,
    RET_DUP_INIT,
} FaceRetCode;

typedef enum Log_Level {
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_INFO = 2,
} Log_Level ;

typedef enum FaceImageType {
//    NV21 = 0,
//    BGR = 1,
} FaceImageType;

typedef struct FaceModels {
    const char *detect_model;
    const char *postfilter_model;
    const char *refine_model;
    const char *pose_blur_model;
    const char *stn_model;
    const char *feature_model;
    const char *liveness_ir_model;
    const char *liveness_bgr_model;
    const char *liveness_bgrir_model;
    const char *anchor_path;
    const char *group_model_path;
    const char *age_gender_model;
    const char *rc_model;
    const char *occl_model;
} FaceModels;

typedef struct Point {
    float x;
    float y;
} Point;

typedef struct Landmark {
    float score;
    Point points[81];
} Landmark;

typedef struct FaceRect {
    int left;
    int top;
    int right;
    int bottom;
} FaceRect;

typedef struct FacePose {
    float roll;
    float pitch;
    float yaw;
    float blur;
} FacePoseBlur;

typedef struct FaceAttr {
    float age;              // age
    float gender[2];        // ["male", "female"]

    float hair[5];          // [bald, little_hair, short_hair, long_hair, unknown]
    float hair_confidence;  //invalid param
    float beard[4];         // [no_beard, moustache, whisker, unknown]
    float beard_confidence; //invalid param
    float hat[9];           // [no_hat, safety_helmet, chef_hat, student_hat, helmet, taoism_hat, kerchief, others, unknown]
    float hat_confidence;   //invalid param
    float respirator[6];    // [no_respirator, surgical, anti-pollution, common, kitchen_transparent, unknown]
    float respirator_confidence; //invalid param
    float glasses[5];       // [no_glasses, glasses_with_dark_frame, regular_glasses, sunglasses, unknown]
    float glass_confidence; //invalid param
    float skin_color[5];    // [yellow, white, black, brown, unknown]
    float skin_color_confidence; //invalid param
} FaceAttr;

typedef struct FaceOccl {
    float leftEye[4];          // [no_occlusion, occlusion, unknown, eye_closed]
    float leftEye_confidence;  // 0~1，confidence of the predict
    float rightEye[4];          // [no_occlusion, occlusion, unknown, eye_closed]
    float rightEye_confidence;  // 0~1，confidence of the predict
    float nose[3];          // [no_occlusion, occlusion, unknown]
    float nose_confidence;  // 0~1，confidence of the predict
    float head[3];          // [no_occlusion, occlusion, unknown]
    float head_confidence;  // 0~1，confidence of the predict
    float leftCheek[3];          // [no_occlusion, occlusion, unknown]
    float leftCheek_confidence;  // 0~1，confidence of the predict
    float rightCheek[3];          // [no_occlusion, occlusion, unknown]
    float rightCheek_confidence;  // 0~1，confidence of the predict
    float mouthAndChin[3];          // [no_occlusion, occlusion, unknown]
    float mouthAndChin_confidence;  // 0~1，confidence of the predict
} FaceOccl;
