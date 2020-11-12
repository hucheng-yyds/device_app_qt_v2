#ifndef INCLUDE_H
#define INCLUDE_H
#ifdef __cplusplus
extern "C" {
#endif
extern void CVR_LogSwitch(bool flag);
extern int CVR_GetSAMID(char *SAMID, int *length);
extern int CVR_GetStatus();
extern int CVR_InitComm(const char *path, int protocolType);
extern int CVR_CloseComm();
extern int CVR_Authenticate();
extern int CVR_AuthenticateForNoJudge();
extern int CVR_Read_Content(int active);
extern int CVR_Read_FPContent(int active);
extern int CVR_FindCard ();
extern int CVR_SelectCard();
extern int CVR_AnalysisCardInfo(unsigned char *infoBuff, int infoLen);
extern int GetPeopleName(char *strTmp, int *strLen);
extern int GetPeopleSex(char *strTmp, int *strLen);
extern int GetPeopleNation(char *strTmp, int *strLen);
extern int GetPeopleBirthday(char *strTmp, int *strLen);
extern int GetPeopleIDCode(char *strTmp, int *strLen);
extern int GetDepartment(char *strTmp, int *strLen);
extern int GetStartDate(char *strTmp, int *strLen);
extern int GetEndDate(char *strTmp, int *strLen);
extern int GetCertType (unsigned char * strTmp, int *strLen);
extern int GetFPDate (unsigned char *pData, int * pLen);
extern int GetPeopleAddress (char *strTmp, int *strLen);
extern int GetPassCheckID(char *strTmp, int *strLen);
extern int GetIssuesNum(char *strTmp, int *strLen);
extern int GetBMPData (unsigned char *pData, int * pLen);
extern int GetPeopleChineseName(char *strTmp, int *strLen);
extern int GetPeopleCertVersion(char *strTmp, int *strLen);
extern int ID_GetInfo(int nPortNo, char* extendPort, int nBaudRate, int nTimeout, char* szPhotoHead,
                char* szPhotoFace, char* szPhotoBack, char* szPhotoCard,char* szIdentityInfo);
extern int CVR_MF_HL_Request(unsigned char nMode, unsigned int  *pSNR, unsigned short *pTagType);
extern int CVR_MF_HL_Write(unsigned char  nMode, unsigned int nSNR, unsigned char nBlock,
                        unsigned char nKey[6], unsigned char *pWriteBuff, unsigned int nBuffLen);
extern int CVR_MF_HL_Read(unsigned char  nMode, unsigned int nSNR, unsigned char nBlock,
                        unsigned char nKey[6], unsigned char *pReadBuff, unsigned int *pBuffLen);
#ifdef __cplusplus
}
#endif
#endif // INCLUDE_H
