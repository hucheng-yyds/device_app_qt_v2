#pragma once

#include "face_common.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define FACE_API __attribute__((visibility("default")))

#ifdef __cplusplus
extern "C" {
#endif
		/**
		 * get sdk version and time
		 * @return the string of (version_time)
		 */

		FACE_API  const char* getVersion();

		/**
		* initialize sdk with models
		* @param[in]  FaceModels
		* @return     error code
		*/

		FACE_API FaceRetCode init(FaceModels models);

		/**
		* release sdk
		* @return     error code
		*/

		FACE_API FaceRetCode release();

		/**
		* release all FaceHandle
		* @param[in]  handle			FaceHandle array
		* @param[in]  size				number of FaceHandle
		* @return     error code
		*/

		FACE_API FaceRetCode releaseAllFace(FaceHandle *handle, int size);

		/**
		 * set log level to  print necessary information
		 * @param logLevel LOG_LEVEL_ERROR and LOG_LEVEL_INFO
		 * @return
		 */
		FACE_API FaceRetCode setLogLevel(Log_Level logLevel);

		/**
		* set detect config
		* @param[in]  detect_threshold         detect threshold
		* @param[in]  postfilter_threshold     postfilter threshold
		* @return     error code
		*/
		FACE_API FaceRetCode set_detect_config(float detect_threshold, float postfilter_threshold);

		/**
		* detect faces from image
		* @param[in]  image
		* @param[in]  width      image width
		* @param[in]  height     image height
		* @param[in]  type       image type,  NV21 and BGR both supported!!!
		* @param[in]  face_within_thr  only reserve faces mostly inside image 
		* @param[out] handle     pointers of detected faces
		* @param[out] count      number of detected faces
		* @return     error code
		*/

		FACE_API FaceRetCode detect(const char *image, int width, int height, FaceImageType type, float face_within_thr, FaceHandle **handle, int *length);

		/**
		* get track  face
		* @param[in]  handle         Face handles
		* @param[in]  length    	number of faces
		* @param[out] track_id      track_id list
		* @return     error code
		*/

		FACE_API FaceRetCode track(FaceHandle *handle, int length, int *track_id);


		/**
		* ir filter
		* @param[in]  handle        	Normal Face handles
		* @param[in]  length    		number of Normal faces
		* @param[in] ir_handle     		ir Face handles
		* @param[in] ir_length     		number of ir faces
		* @param[out] filter_result     pointer to the result , 1 pass if_filter, 0 failed if_filter
		* @return     error code
		*/

		FACE_API FaceRetCode ir_filter(FaceHandle *handle, int length, FaceHandle *ir_handle, int ir_length, char *filter_result);

		/**
		* set if_filter parameter
		* @param[in]  x_k
		* @param[in]  x_b
		* @param[in]  y_k
		* @param[in]  y_b
		* @param[in]  overlap_rate  range[0.1--0.9] the overlap rate threshold for ir detect rect and bgr detect rect, 
					  default 0.4, if two rects overlap_rate is larger than threshold, they are in pairs.
		* @return     error code
		*/

		FACE_API FaceRetCode set_match_config(float x_k, float x_b, float y_k, float y_b, float overlap_rate);

		/**
		* get pose of detected face
		* @param[in]  handle         Face handle
		* @param[out] pose_result    pose of face
		* @return     error code
		*/

		FACE_API FaceRetCode getPoseBlurAttribute(FaceHandle handle, FacePoseBlur *poseblur_result);

		/**
		* extract feature from detected face
		* @param[in]  handle                 Face handle
		* @param[out] feature_result         feature of detected face
		* @param[out] feature_length         the length of feature
		* @return     error code
		*/

		FACE_API FaceRetCode extract(FaceHandle handle, char** feature_result, int *size);


		/**
		* release feature
		* @param[in]  feature                feature
		* @return     error code
		*/

		FACE_API FaceRetCode releaseFeature(char* feature);

		/**
		* get rgb image score of detected face
		* @param[in]  handle			 Face handles
		* @param[out] result             liveness of face
		* @return     error code
		*/

		FACE_API FaceRetCode getLiveness_bgr(FaceHandle handle, float *liveness_result);

                /**
                * get ir image livness score of detected face
                * @param[in]  handle			 Face handles
                * @param[out] result             liveness of face
                * @return     error code
                */

                FACE_API FaceRetCode getLiveness_ir(FaceHandle handle, float *liveness_result);



		/**
		* for multiple targets: bgr targets match ir targets;
		* @param[in]  handle			 bgr handles
		* @param[in]  handle			 ir handles
		* @param[out] pair             pair init with pair[length]=ir_length, match:i->pair[i],if pair[i] == ir_length is attack;
		* @return     error code
		*/

		FACE_API FaceRetCode BGR_IR_match(FaceHandle *handle, int length, FaceHandle *ir_handle, int ir_length, int *pair);

		/**
		* get livness_ir of detected face
		* @param[in]  handle			 bgr handles
		* @param[in]  handle			 ir handles
		* @param[out] result             liveness of face
		* @return     error code
		*/

		FACE_API FaceRetCode getLiveness_bgrir(FaceHandle bgr_handle, FaceHandle ir_handle, float *liveness_result);

		/**
		* get similarity of two features
		* @param[in]  first_feature
		* @param[in]  second_feature
		* @param[in]  feature_length     the length of feature
		* @param[out] result             the similarity of two features [0-100]
		* @return     error code
		*/

		FACE_API FaceRetCode compare(const char *first_feature, const char *second_feature, int feature_length, float *result);


		/**
		* get FaceRect with Facehandle
		* @param[in]  handle
		* @param[out] result      face attribute results
		* @return     error code
		*/

		FACE_API FaceRetCode getFaceAttrResult(FaceHandle handle, FaceAttr *result);

		/**
		* get FaceRect with Facehandle
		* @param[in]  handle
		* @param[out] result      face occlusion results
		* @return     error code
		*/

		FACE_API FaceRetCode getFaceOcclResult(FaceHandle handle, FaceOccl *result);


		/**
		* get most similar face id and similarity from a face group
		* @param[in]  handle			face group handle
		* @param[in]  target			target feature
		* @param[in]  feature_length    target feature length
		* @param[out] result			similarity
		* @param[out] idx				id of the most similar face
		* @return     error code
		*/


		FACE_API FaceRetCode identifyFromFaceGroup(FaceGroupHandle handle, const char *target, int feature_length, float *result, uint64_t *idx);

		/**
		* get top k similar face id and similarity from a face group, maximum is 10
		* @param[in]  handle			face group handle
		* @param[in]  target			target feature
		* @param[in]  feature_length    target feature length
		* @param[in]  k                 how many results to return
		* @param[out] result			similarity
		* @param[out] idx				id of the most similar face
		* @return     error code
		*/


		FACE_API FaceRetCode top_k_FromFaceGroup(FaceGroupHandle handle, const char *target, int feature_length, int k, float *result, uint64_t *idx);


		/**
		* create a face group
		* @param[in]  handle			face group handle
		* @return     error code
		*/

		FACE_API FaceRetCode createFaceGroup(FaceGroupHandle *handle);

		/**
		* insert a face into face group
		* @param[in]  handle				face group handle
		* @param[in]  feature				feature
		* @param[in]  feature_length		feature length
		* @param[in]  face_id				face id
		* @return     error code
		*/

		FACE_API FaceRetCode insertFaceGroup(FaceGroupHandle handle, const char *feature, int feature_length, uint64_t face_id);

		/**
		* remove a face from face group
		* @param[in]  handle			Face Group Handle
		* @param[in]  face_id			face id of the face to be remove
		* @return     error code
		*/

		FACE_API FaceRetCode removeFaceGroup(FaceGroupHandle handle, uint64_t face_id);

		/**
		* get the number of faces in group
		* @param[in]	handle				face group handle
		* @param[out]	total_sum			number of faces
		* @return		error code
		*/

		FACE_API FaceRetCode getFaceGroupCount(FaceGroupHandle handle, size_t *total_sum);

		/**
		* clear all faces in the group
		* @param[in]	handle				face group handle
		* @return		error code
		*/

		FACE_API FaceRetCode resetFaceGroup(FaceGroupHandle handle);

		/**
		* release a group
		* @param[in]	handle				face group handle
		* @return		error code
		*/

		FACE_API FaceRetCode releaseFaceGroup(FaceGroupHandle handle);


                /**
                * get FaceRect with Facehandle
                * @param[in]  handle
                * @param[out] result      pointer of result
                * @return     error code
                */

		FACE_API FaceRetCode getFaceRect(FaceHandle handle, FaceRect *result);

		/**
		* get FaceLandmark with Facehandle
		* @param[in]  handle
		* @param[out] result      pointer of result
		* @return     error code
		*/

		FACE_API FaceRetCode getFaceLandmark(FaceHandle handle, Landmark *result);

#ifdef __cplusplus
}
#endif
