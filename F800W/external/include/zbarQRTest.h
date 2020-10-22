#ifndef ZBAR_QR_TEST_H
#define ZBAR_QR_TEST_H

/*
function:zbarProcessQR
����:       ʶ��QR��ά�룬ͨ��ָ��Żض�ά������ݣ�
����˵��:   data:�����ά��Ҷ�ͼƬ��buffer��ַ; width,height ��Ӧ��ά��Ҷ�ͼƬ�Ŀ�͸ߣ�
            qrContent:��ö�ά�����ݵ�buffer��ַ���ռ����û������趨(���ݽ�Ҫʶ��Ķ�ά����������);
            qrSize:�û������趨��qrContent��С��

return : ����ֵ>=1 ˵��ʶ����qr��ά�룬����ûʶ��
*/
#ifdef __cplusplus
extern "C" {
#endif
int zbarProcessQR(unsigned char *data, int width, int height,unsigned char* qrContent,int qrSize);
#ifdef __cplusplus
}
#endif

#endif

