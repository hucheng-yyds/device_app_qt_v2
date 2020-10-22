#ifndef ZBAR_QR_TEST_H
#define ZBAR_QR_TEST_H

/*
function:zbarProcessQR
功能:       识别QR二维码，通过指针放回二维码的内容；
参数说明:   data:输入二维码灰度图片的buffer地址; width,height 对应二维码灰度图片的宽和高；
            qrContent:获得二维码内容的buffer地址，空间由用户自行设定(根据将要识别的二维码内容上限);
            qrSize:用户自行设定的qrContent大小，

return : 返回值>=1 说明识别到了qr二维码，否则没识别到
*/
#ifdef __cplusplus
extern "C" {
#endif
int zbarProcessQR(unsigned char *data, int width, int height,unsigned char* qrContent,int qrSize);
#ifdef __cplusplus
}
#endif

#endif

