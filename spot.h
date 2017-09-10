#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <wincon.h>
#include <conio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <ctype.h>
#include <time.h>

#ifndef TYPE_H_INCLUDED
#define TYPE_H_INCLUDED

#define SCR_ROW 40             /*��Ļ����*/
#define SCR_COL 100             /*��Ļ����*/
typedef struct city_node {
    char city_id[48];              /**< ���б��*/
    char name[48];                /**< ��������*/
    char jiandu_num[48];          /**< �ල�绰*/
    char zixun_num[48];           /**< ��ѯ�绰*/
    struct city_node *next;       /**< ָ����һ���н���ָ��*/
    struct region_node *rnext;    /**< ָ�����ڵ��ָ��*/
} CITY_NODE;
typedef struct region_node {
    char city_id[48];              /**< �������б��*/
    char region_id[48];            /**< �������*/
    char name[48];                /**< ��������*/
    char level[48];               /**< ��������*/
    char address[48];             /**< ������ַ*/
    char price[48];                /**< ��Ʊ�۸�*/
    char opentime[48];            /**< ����ʱ��*/
    struct region_node *next;     /**< ָ����һ��������ָ��*/
    struct spot_node *snext;      /**< ָ�򾰵����ָ��*/
} REGION_NODE;

typedef struct spot_node {
    char region_id[48];            /**< �����������*/
    char spot_id[48];              /**< ������*/
    char name[48];                /**< ��������*/
    char zixun_num[48];             /**< ����λ��*/
    char opentime[48];            /**< ���ʱ��*/
    char feature[48];            /**< ������ɫ*/
    struct spot_node *next;       /**< ָ����һ��������ָ��*/
} SPOT_NODE;
/**
 *��Ļ������Ϣ�������ṹ
 */
typedef struct layer_node {
    char LayerNo;            /**< �������ڲ���*/
    SMALL_RECT rcArea;       /**< ����������������*/
    CHAR_INFO *pContent;     /**< �������������ַ���Ԫԭ��Ϣ�洢������*/
    char *pScrAtt;           /**< �������������ַ���Ԫԭ����ֵ�洢������*/
    struct layer_node *next; /**< ָ����һ����ָ��*/
} LAYER_NODE;

/**
 *��ǩ���ṹ
 */
typedef struct labe1_bundle {
    char **ppLabel;        /**< ��ǩ�ַ��������׵�ַ*/
    COORD *pLoc;           /**< ��ǩ��λ�����׵�ַ*/
    int num;               /**< ��ǩ����*/
} LABEL_BUNDLE;

/**
 *�����ṹ
 */
typedef struct hot_area {
    SMALL_RECT *pArea;     /**< ������λ�����׵�ַ*/
    char *pSort;           /**< �������(�������ı���ѡ���)�����׵�ַ*/
    char *pTag;            /**< ������������׵�ַ*/
    int num;               /**< ��������*/
} HOT_AREA;

LAYER_NODE *gp_top_layer = NULL;               /*����������Ϣ����ͷ*/

CITY_NODE *gp_head2 = NULL;                     /*����ͷָ��*/
char *gp_city_info_filename = "city.dat";        /*���л�����Ϣ�����ļ�*/
char *gp_region_info_filename = "region.dat";  /*������Ϣ�����ļ�*/
char *gp_spot_info_filename = "spot.dat";      /*������Ϣ�����ļ�*/
char *gp_sys_name = "������Ϣ����ϵͳ";    /*ϵͳ����*/

char *ga_main_menu[] = {"�ļ�(F)",             /*ϵͳ���˵���*/
                        "����ά��(M)",
                        "���ݲ�ѯ(Q)",
                        "����ͳ��(S)",
                        "��㿴��(R)",
                        "����(H)"
};

char *ga_sub_menu[] = {"[S] ���ݱ���",          /*ϵͳ�Ӳ˵���*/
                       "[B] ���ݱ���",
                       "[R] ���ݻָ�",
                       "[X] �˳�    Alt+X",
        //
                       "[S] ������Ϣ",
                       "[T] ������Ϣ",
                       "[D] ������Ϣ",
        //
                       "[S] ������Ϣ",
                       "[T] ������Ϣ",
                       "[D] ������Ϣ",
        //
                       "[T] ���о���ͳ��",

                       "[S] ��㿴������",
                       "[T] ��㿴������",
                       "[D] ��㿴������",
        //
                       "[T] ��������",
                       "",
                       "[A] ����..."
};

int ga_sub_menu_count[] = {4, 3, 3, 1, 3, 3};  /*�����˵������Ӳ˵��ĸ���*/
int gi_sel_menu = 1;                        /*��ѡ�е����˵����,��ʼΪ1*/
int gi_sel_sub_menu = 0;                    /*��ѡ�е��Ӳ˵����,��ʼΪ0,��ʾδѡ��*/

CHAR_INFO *gp_buff_menubar_info = NULL;     /*��Ų˵�����Ļ���ַ���Ϣ�Ļ�����*/
CHAR_INFO *gp_buff_stateBar_info = NULL;    /*���״̬����Ļ���ַ���Ϣ�Ļ�����*/

char *gp_scr_att = NULL;    /*�����Ļ���ַ���Ԫ����ֵ�Ļ�����*/
char gc_sys_state = '\0';   /*��������ϵͳ״̬���ַ�*/

HANDLE gh_std_out;          /*��׼����豸���*/
HANDLE gh_std_in;           /*��׼�����豸���*/

int CreatList2(CITY_NODE **pphead);              /*���������ʼ��*/
void InitInterface(void);                 /*ϵͳ�����ʼ��*/
void ClearScreen(void);                         /*����*/
void ShowMenu(void);                            /*��ʾ�˵���*/
void PopMenu(int num);                          /*��ʾ�����˵�*/
void PopPrompt(int num);                        /*��ʾ��������*/
void PopUp(SMALL_RECT *, WORD, LABEL_BUNDLE *, HOT_AREA *);  /*����������Ļ��Ϣά��*/
void PopOff(void);                              /*�رն��㵯������*/
void DrawBox(SMALL_RECT *parea);                /*���Ʊ߿�*/
void LocSubMenu(int num, SMALL_RECT *parea);    /*���˵������˵���λ*/
void ShowState(void);                           /*��ʾ״̬��*/
void TagMainMenu(int num);                      /*��Ǳ�ѡ�е����˵���*/
void TagSubMenu(int num);                       /*��Ǳ�ѡ�е��Ӳ˵���*/
int DealConInput(HOT_AREA *phot_area, int *pihot_num);  /*����̨���봦��*/
int DealInput2(HOT_AREA *pHotArea, int *piHot, char **ppcondition);
BOOL ShowResult(char **pString, int n,int col );
BOOL ShowResult2(char **pString, int n,int col );
BOOL ShowSearchResult(char **pString, int n,int col );
int ShowSearchResult2(char **pString, int n, int col);
void SetHotPoint(HOT_AREA *phot_area, int hot_num);     /*��������*/
void RunSys2(CITY_NODE **pphd);                 /*ϵͳ����ģ���ѡ�������*/
BOOL ExeFunction(int main_menu_num, int sub_menu_num);  /*����ģ��ĵ���*/
void CloseSys(CITY_NODE *phd);                  /*�˳�ϵͳ*/
BOOL ShowModule(char **pString, int n);

BOOL LoadData2(void);           /*���ݼ���*/
BOOL SaveData(void);           /*��������*/
BOOL BackupData(void);         /*��������*/
BOOL RestoreData(void);        /*�ָ�����*/
BOOL ExitSys(void);            /*�˳�ϵͳ*/
BOOL HelpTopic(void);          /*��������*/
BOOL AboutThis(void);          /*����ϵͳ*/

BOOL MaintainCityInfo(void);           /*ά��������Ϣ*/
BOOL MaintainScenicAreaInfo(void);    /*ά������������Ϣ*/
BOOL MaintainAttractionInfo(void);   /*ά�����������Ϣ*/

BOOL InsertCityNodeSubMenu(void);
BOOL ModifyCityNodeSubMenu(void);
BOOL ModifyCityNodeSubMenu2(void);
BOOL DeleteCityNodeSubMenu(void);
BOOL InsertScenicAreaNodeSubMenu(void);
BOOL ModifyScenicAreaNodeSubMenu(void);
BOOL DeleteScenicAreaNodeSubMenu(void);
BOOL InsertAttractionNodeSubMenu(void);
BOOL ModifyAttractionNodeSubMenu(void);
BOOL DeleteAttractionNodeSubMenu(void);

BOOL QueryCityInfo(void);           /*��ѯ������Ϣ*/
BOOL QueryScenicAreaInfo(void);    /*��ѯ����������Ϣ*/
BOOL QueryAttractionInfo(void);   /*��ѯ���������Ϣ*/

BOOL RandomView(void);    /*��ѯ����������Ϣ*/
BOOL StatusCityInfo();   /*��ѯ���������Ϣ*/

BOOL SaveSysData2(CITY_NODE *phd);                       /*����ϵͳ����*/
BOOL BackupSysData(CITY_NODE *hd, char *filename);     /*����ϵͳ����*/
BOOL RestoreSysData(CITY_NODE **pphd, char *filename);  /*�ָ�ϵͳ����*/


// buliuzi

BOOL add_city(CITY_NODE **head, CITY_NODE *pcity_node);
BOOL ConfirmCityInsertion(CITY_NODE **head, CITY_NODE *pcity_node);
CITY_NODE *SeekCityNodeByID(CITY_NODE *hd, char *id);
BOOL add_region(CITY_NODE *head, REGION_NODE *pregion_node);
REGION_NODE *SeekRegionNodeByID(CITY_NODE *hd, char *id);
BOOL ConfirmRegionInsertion(CITY_NODE *pcity_node, REGION_NODE *pregion_node);
BOOL add_spot(REGION_NODE *pregion_node, SPOT_NODE *pspot_node);
SPOT_NODE *SeekSpotNodeByID(CITY_NODE *hd, char *id);
BOOL ConfirmSpotInsertion(CITY_NODE *pcity_node, SPOT_NODE *pspot_node);

BOOL delete_spot(CITY_NODE **head, char *id);
BOOL delete_region(CITY_NODE *head, char *id);
BOOL delete_city(CITY_NODE **head, char *id);

CITY_NODE *SeekCityNodeByName(CITY_NODE *hd, char *name);
REGION_NODE *SeekRegionNodeByName(CITY_NODE *hd, char *name);
SPOT_NODE * SeekSpotNodeByName(CITY_NODE *hd, char *name);
CITY_NODE *RandomCity(CITY_NODE *hd);
REGION_NODE *RandomRegion(CITY_NODE *hd);
SPOT_NODE *RandomSpot(CITY_NODE *hd);


BOOL RandomCityView(void);
BOOL RandomRegionView(void);
BOOL RandomSpotView(void);
#endif /**< TYPE_H_INCLUDED*/
