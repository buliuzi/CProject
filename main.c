#include "dorm.h"

unsigned long ul;

int main()
{
    COORD size = {SCR_COL, SCR_ROW};              /*���ڻ�������С*/

    gh_std_out = GetStdHandle(STD_OUTPUT_HANDLE); /* ��ȡ��׼����豸���*/
    gh_std_in = GetStdHandle(STD_INPUT_HANDLE);   /* ��ȡ��׼�����豸���*/

    SetConsoleTitle(gp_sys_name);                 /*���ô��ڱ���*/
    SetConsoleScreenBufferSize(gh_std_out, size); /*���ô��ڻ�������С80*25*/

    LoadData2();                   /*���ݼ���*/
    InitInterface();          /*�����ʼ��*/
    RunSys2(&gp_head2);             /*ϵͳ����ģ���ѡ������*/
    CloseSys(gp_head2);            /*�˳�ϵͳ*/

    return 0;
}

BOOL LoadData2()
{
    int Re = 0;
    Re = CreatList2(&gp_head2);
    gc_sys_state |= Re;
    gc_sys_state &= ~(4 + 8 + 16 - Re);
    printf("\n�����������......");
    getch();
    return TRUE;
}

int CreatList2(CITY_NODE **phead)
{
    // ���������������
    CITY_NODE *pCityNode = NULL, cityTmp, *hd = NULL;
    REGION_NODE *pRegionNode = NULL, regionTmp;
    SPOT_NODE *pSpotNode = NULL, spotTmp;

    int re = 0;

    FILE *pFile = NULL;

    if ((pFile = fopen(gp_city_info_filename, "rb")) == NULL)
    {
        printf("��װ������Ϣ�����ļ���ʧ��!\n");
        return re;
    }
    printf("��װ������Ϣ�����ļ��򿪳ɹ�!\n");

    printf("������Ϣ�����ļ����سɹ���\n");
    while(fread(&cityTmp, sizeof(CITY_NODE), 1, pFile) == 1) {
        pCityNode = (CITY_NODE *)malloc(sizeof(CITY_NODE));
        *pCityNode = cityTmp;
        pCityNode->rnext = NULL;
        pCityNode->next = hd;
        hd = pCityNode;
    }
    fclose(pFile);

    if (hd == NULL)
    {
        printf("������Ϣ������ʧ�ܣ�\n");
        return re;
    }
    printf("������Ϣ�������ɹ���\n");
    *phead = hd;
    re += 4;

    ;
    if((pFile = fopen(gp_region_info_filename, "rb")) == NULL) {
        printf("���������ļ��򿪴���\n");
        return re;
    }
    printf("������Ϣ�����ļ����سɹ���\n");

    while(fread(&regionTmp, sizeof(REGION_NODE),1,pFile)==1) {
        pRegionNode = (REGION_NODE*)malloc(sizeof(REGION_NODE));
        *pRegionNode = regionTmp;

        for(pCityNode=hd;pCityNode!=NULL;pCityNode=pCityNode->next) {
            if(strcmp(pCityNode->city_id,pRegionNode->city_id)==0) {
                break;
            }
        }

        if(pCityNode == NULL) {
            free(pRegionNode);
        }
        else {
            pRegionNode->next = pCityNode->rnext;
            pRegionNode->snext = NULL;
            pCityNode->rnext = pRegionNode;
        }
    }
    fclose(pFile);

    if((pFile = fopen(gp_spot_info_filename, "rb")) == NULL) {
        printf("���������ļ��򿪴���\n");
        return re;
    }
    printf("������Ϣ�����ļ����سɹ���\n");
    re += 16;

    while(fread(&spotTmp, sizeof(SPOT_NODE),1,pFile)) {
        pSpotNode = (SPOT_NODE*)malloc(sizeof(SPOT_NODE));
        *pSpotNode = spotTmp;
        int find = 0;

        for(pCityNode=hd;pCityNode!=NULL;pCityNode=pCityNode->next) {
            for(pRegionNode=pCityNode->rnext;pRegionNode!=NULL;pRegionNode=pRegionNode->next) {
                if((strcmp(pCityNode->city_id, pRegionNode->city_id)==0)&&(strcmp(pRegionNode->region_id,pSpotNode->region_id)==0)) {
                    find = 1;
                    break;
                }
            }
            if(find == 1){
                break;
            }
        }
        if(find) {
            pSpotNode->next = pRegionNode->snext;
            pRegionNode->snext = pSpotNode;
        }
        else {
            free(pSpotNode);
        }
    }
    fclose(pFile);
    return re;
}

/**
 * ��������: InitInterface
 * ��������: ��ʼ������.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void InitInterface()
{
    WORD att = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
               | BACKGROUND_BLUE;  /*��ɫǰ������ɫ����*/

    SetConsoleTextAttribute(gh_std_out, att);  /*���ÿ���̨��Ļ�������ַ�����*/

    ClearScreen();  /* ����*/

    /*��������������Ϣ��ջ������ʼ�������Ļ���ڵ�����һ�㵯������*/
    gp_scr_att = (char *)calloc(SCR_COL * SCR_ROW, sizeof(char));/*��Ļ�ַ�����*/
    gp_top_layer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    gp_top_layer->LayerNo = 0;      /*�������ڵĲ��Ϊ0*/
    gp_top_layer->rcArea.Left = 0;  /*�������ڵ�����Ϊ������Ļ����*/
    gp_top_layer->rcArea.Top = 0;
    gp_top_layer->rcArea.Right = SCR_COL - 1;
    gp_top_layer->rcArea.Bottom = SCR_ROW - 1;
    gp_top_layer->pContent = NULL;
    gp_top_layer->pScrAtt = gp_scr_att;
    gp_top_layer->next = NULL;

    ShowMenu();     /*��ʾ�˵���*/
    ShowState();    /*��ʾ״̬��*/

    return;
}

/**
 * ��������: ClearScreen
 * ��������: �����Ļ��Ϣ.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void ClearScreen(void)
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD home = {0, 0};
    unsigned long size;

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );/*ȡ��Ļ��������Ϣ*/
    size =  bInfo.dwSize.X * bInfo.dwSize.Y; /*������Ļ�������ַ���Ԫ��*/

    /*����Ļ���������е�Ԫ���ַ���������Ϊ��ǰ��Ļ�������ַ�����*/
    FillConsoleOutputAttribute(gh_std_out, bInfo.wAttributes, size, home, &ul);

    /*����Ļ���������е�Ԫ���Ϊ�ո��ַ�*/
    FillConsoleOutputCharacter(gh_std_out, ' ', size, home, &ul);

    return;
}

/**
 * ��������: ShowMenu
 * ��������: ����Ļ����ʾ���˵�, ����������, �����˵���һ������ѡ�б��.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void ShowMenu()
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    COORD size;
    COORD pos = {0, 0};
    int i, j;
    int PosA = 2, PosB;
    char ch;

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );
    size.X = bInfo.dwSize.X;
    size.Y = 1;
    SetConsoleCursorPosition(gh_std_out, pos);
    for (i=0; i < 5; i++) /*�ڴ��ڵ�һ�е�һ�д�������˵���*/
    {
        printf("  %s  ", ga_main_menu[i]);
    }

    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;
    SetConsoleCursorInfo(gh_std_out, &lpCur);  /*���ع��*/

    /*���붯̬�洢����Ϊ��Ų˵�����Ļ���ַ���Ϣ�Ļ�����*/
    gp_buff_menubar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
    SMALL_RECT rcMenu ={0, 0, size.X-1, 0} ;

    /*�����ڵ�һ�е����ݶ��뵽��Ų˵�����Ļ���ַ���Ϣ�Ļ�������*/
    ReadConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    /*����һ����Ӣ����ĸ��Ϊ��ɫ�������ַ���Ԫ��Ϊ�׵׺���*/
    for (i=0; i<size.X; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (char)((gp_buff_menubar_info+i)->Char.AsciiChar);
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            (gp_buff_menubar_info+i)->Attributes |= FOREGROUND_RED;
        }
    }

    /*�޸ĺ�Ĳ˵����ַ���Ϣ��д�����ڵĵ�һ��*/
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);
    COORD endPos = {0, 1};
    SetConsoleCursorPosition(gh_std_out, endPos);  /*�����λ�������ڵ�2�е�1��*/

    /*���˵�����Ϊ�������������Ϊ�˵���ţ���������Ϊ0(��ť��)*/
    i = 0;
    do
    {
        PosB = PosA + strlen(ga_main_menu[i]);  /*��λ��i+1�Ų˵������ֹλ��*/
        for (j=PosA; j<PosB; j++)
        {
            gp_scr_att[j] |= (i+1) << 2; /*���ò˵��������ַ���Ԫ������ֵ*/
        }
        PosA = PosB + 4;
        i++;
    } while (i<5);

    TagMainMenu(gi_sel_menu);  /*��ѡ�����˵���������ǣ�gi_sel_menu��ֵΪ1*/

    return;
}

/**
 * ��������: ShowState
 * ��������: ��ʾ״̬��.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��: ״̬���ַ�����Ϊ�׵׺���, ��ʼ״̬��״̬��Ϣ.
 */
void ShowState()
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD size;
    COORD pos = {0, 0};
    int i;

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );
    size.X = bInfo.dwSize.X;
    size.Y = 1;
    SMALL_RECT rcMenu ={0, bInfo.dwSize.Y-1, size.X-1, bInfo.dwSize.Y-1};

    if (gp_buff_stateBar_info == NULL)
    {
        gp_buff_stateBar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
        ReadConsoleOutput(gh_std_out, gp_buff_stateBar_info, size, pos, &rcMenu);
    }

    for (i=0; i<size.X; i++)
    {
        (gp_buff_stateBar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                                | BACKGROUND_RED;
/*
        ch = (char)((gp_buff_stateBar_info+i)->Char.AsciiChar);
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            (gp_buff_stateBar_info+i)->Attributes |= FOREGROUND_RED;
        }
*/
    }

    WriteConsoleOutput(gh_std_out, gp_buff_stateBar_info, size, pos, &rcMenu);

    return;
}

/**
 * ��������: TagMainMenu
 * ��������: ��ָ�����˵�������ѡ�б�־.
 * �������: num ѡ�е����˵����
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void TagMainMenu(int num)
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD size;
    COORD pos = {0, 0};
    int PosA = 2, PosB;
    char ch;
    int i;

    if (num == 0) /*numΪ0ʱ������ȥ�����˵���ѡ�б��*/
    {
        PosA = 0;
        PosB = 0;
    }
    else  /*���򣬶�λѡ�����˵������ֹλ��: PosAΪ��ʼλ��, PosBΪ��ֹλ��*/
    {
        for (i=1; i<num; i++)
        {
            PosA += strlen(ga_main_menu[i-1]) + 4;
        }
        PosB = PosA + strlen(ga_main_menu[num-1]);
    }

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );
    size.X = bInfo.dwSize.X;
    size.Y = 1;

    /*ȥ��ѡ�в˵���ǰ��Ĳ˵���ѡ�б��*/
    for (i=0; i<PosA; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (gp_buff_menubar_info+i)->Char.AsciiChar;
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            (gp_buff_menubar_info+i)->Attributes |= FOREGROUND_RED;
        }
    }

    /*��ѡ�в˵���������ǣ��ڵװ���*/
    for (i=PosA; i<PosB; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN
                                               | FOREGROUND_RED;
    }

    /*ȥ��ѡ�в˵������Ĳ˵���ѡ�б��*/
    for (i=PosB; i<bInfo.dwSize.X; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (char)((gp_buff_menubar_info+i)->Char.AsciiChar);
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            (gp_buff_menubar_info+i)->Attributes |= FOREGROUND_RED;
        }
    }

    /*�����ñ�ǵĲ˵�����Ϣд�����ڵ�һ��*/
    SMALL_RECT rcMenu ={0, 0, size.X-1, 0};
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    return;
}

/**
 * ��������: CloseSys
 * ��������: �ر�ϵͳ.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void CloseSys(CITY_NODE *hd)
{
    CITY_NODE *pCityNode1 = hd, *pCityNode2;
    REGION_NODE *pRegionNode1, *pRegionNode2;
    SPOT_NODE *pSpotNode1, *pSpotNode2;

    while (pCityNode1 != NULL) /*�ͷ�ʮ�ֽ�������Ķ�̬�洢��*/
    {
        pCityNode2 = pCityNode1->next;
        pRegionNode1 = pCityNode1->rnext;
        while (pRegionNode1 != NULL) /*�ͷ�ѧ��������Ϣ֧���Ķ�̬�洢��*/
        {
            pRegionNode2 = pRegionNode1->next;
            pSpotNode1 = pRegionNode1->snext;
            while (pSpotNode1 != NULL) /*�ͷŽɷ���Ϣ֧���Ķ�̬�洢��*/
            {
                pSpotNode2 = pSpotNode1->next;
                free(pSpotNode1);
                pSpotNode1 = pSpotNode2;
            }
            free(pRegionNode1);
            pRegionNode1 = pRegionNode2;
        }
        free(pCityNode1);  /*�ͷ��������Ķ�̬�洢��*/
        pCityNode1 = pCityNode2;
    }

    ClearScreen();        /*����*/

    /*�ͷŴ�Ų˵�����״̬�����Ա�����ѧ�����������Ϣ��̬�洢��*/
    free(gp_buff_menubar_info);
    free(gp_buff_stateBar_info);
    free(gp_sex_code);
    free(gp_type_code);

    /*�رձ�׼���������豸���*/
    CloseHandle(gh_std_out);
    CloseHandle(gh_std_in);

    /*�����ڱ�������Ϊ���н���*/
    SetConsoleTitle("���н���");

    return;
}

void RunSys2(CITY_NODE **phead)
{
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    BOOL bRet = TRUE;
    int i, loc, num;
    int cNo, cAtt;      /*cNo:�ַ���Ԫ���, cAtt:�ַ���Ԫ����*/
    char vkc, asc;      /*vkc:���������, asc:�ַ���ASCII��ֵ*/

    while (bRet)
    {
        /*�ӿ���̨���뻺�����ж�һ����¼*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);

        if (inRec.EventType == MOUSE_EVENT) /*�����¼������¼�����*/
        {
            pos = inRec.Event.MouseEvent.dwMousePosition;  /*��ȡ�������λ��*/
            cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3; /*ȡ��λ�õĲ��*/
            cAtt = gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2;/*ȡ���ַ���Ԫ����*/
            if (cNo == 0) /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
            {
                /* cAtt > 0 ������λ�ô�������(���˵����ַ���Ԫ)
                 * cAtt != gi_sel_menu ������λ�õ����˵���δ��ѡ��
                 * gp_top_layer->LayerNo > 0 ������ǰ���Ӳ˵�����
                 */
                if (cAtt > 0 && cAtt != gi_sel_menu && gp_top_layer->LayerNo > 0)
                {
                    PopOff();            /*�رյ������Ӳ˵�*/
                    gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                    PopMenu(cAtt);       /*��������������˵����Ӧ���Ӳ˵�*/
                }
            }
            else if (cAtt > 0) /*�������λ��Ϊ�����Ӳ˵��Ĳ˵����ַ���Ԫ*/
            {
                TagSubMenu(cAtt); /*�ڸ��Ӳ˵�������ѡ�б��*/
            }

            if (inRec.Event.MouseEvent.dwButtonState
                == FROM_LEFT_1ST_BUTTON_PRESSED) /*������������ߵ�һ��*/
            {
                if (cNo == 0) /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
                {
                    if (cAtt > 0) /*�����λ�ô�������(���˵����ַ���Ԫ)*/
                    {
                        PopMenu(cAtt);   /*��������������˵����Ӧ���Ӳ˵�*/
                    }
                        /*�����λ�ò��������˵����ַ���Ԫ�������Ӳ˵�����*/
                    else if (gp_top_layer->LayerNo > 0)
                    {
                        PopOff();            /*�رյ������Ӳ˵�*/
                        gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                    }
                }
                else /*��Ų�Ϊ0��������λ�ñ������Ӳ˵�����*/
                {
                    if (cAtt > 0) /*�����λ�ô�������(�Ӳ˵����ַ���Ԫ)*/
                    {
                        PopOff(); /*�رյ������Ӳ˵�*/
                        gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/

                        /*ִ�ж�Ӧ���ܺ���:gi_sel_menu���˵����,cAtt�Ӳ˵����*/
                        bRet = ExeFunction(gi_sel_menu, cAtt);
                    }
                }
            }
            else if (inRec.Event.MouseEvent.dwButtonState
                     == RIGHTMOST_BUTTON_PRESSED) /*�����������Ҽ�*/
            {
                if (cNo == 0) /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
                {
                    PopOff();            /*�رյ������Ӳ˵�*/
                    gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                }
            }
        }
        else if (inRec.EventType == KEY_EVENT  /*�����¼�ɰ�������*/
                 && inRec.Event.KeyEvent.bKeyDown) /*�Ҽ�������*/
        {
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode; /*��ȡ�������������*/
            asc = inRec.Event.KeyEvent.uChar.AsciiChar; /*��ȡ������ASC��*/

            /*ϵͳ��ݼ��Ĵ���*/
            if (vkc == 112) /*�������F1��*/
            {
                if (gp_top_layer->LayerNo != 0) /*�����ǰ���Ӳ˵�����*/
                {
                    PopOff();            /*�رյ������Ӳ˵�*/
                    gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                }
                bRet = ExeFunction(5, 1);  /*���а������⹦�ܺ���*/
            }
            else if (inRec.Event.KeyEvent.dwControlKeyState
                     & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))
            { /*������������Alt��*/
                switch (vkc)  /*�ж���ϼ�Alt+��ĸ*/
                {
                    case 88:  /*Alt+X �˳�*/
                        if (gp_top_layer->LayerNo != 0)
                        {
                            PopOff();
                            gi_sel_sub_menu = 0;
                        }
                        bRet = ExeFunction(1,4);
                        break;
                    case 70:  /*Alt+F*/
                        PopMenu(1);
                        break;
                    case 77: /*Alt+M*/
                        PopMenu(2);
                        break;
                    case 81: /*Alt+Q*/
                        PopMenu(3);
                        break;
                    case 83: /*Alt+S*/
                        PopMenu(4);
                        break;
                    case 72: /*Alt+H*/
                        PopMenu(5);
                        break;
                }
            }
            else if (asc == 0) /*�������Ƽ��Ĵ���*/
            {
                if (gp_top_layer->LayerNo == 0) /*���δ�����Ӳ˵�*/
                {
                    switch (vkc) /*�������(���ҡ���)������Ӧ�������Ƽ�*/
                    {
                        case 37:
                            gi_sel_menu--;
                            if (gi_sel_menu == 0)
                            {
                                gi_sel_menu = 5;
                            }
                            TagMainMenu(gi_sel_menu);
                            break;
                        case 39:
                            gi_sel_menu++;
                            if (gi_sel_menu == 6)
                            {
                                gi_sel_menu = 1;
                            }
                            TagMainMenu(gi_sel_menu);
                            break;
                        case 40:
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                    }
                }
                else  /*�ѵ����Ӳ˵�ʱ*/
                {
                    for (loc=0,i=1; i<gi_sel_menu; i++)
                    {
                        loc += ga_sub_menu_count[i-1];
                    }  /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
                    switch (vkc) /*�����(���ҡ��ϡ���)�Ĵ���*/
                    {
                        case 37:
                            gi_sel_menu--;
                            if (gi_sel_menu < 1)
                            {
                                gi_sel_menu = 5;
                            }
                            TagMainMenu(gi_sel_menu);
                            PopOff();
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                        case 38:
                            num = gi_sel_sub_menu - 1;
                            if (num < 1)
                            {
                                num = ga_sub_menu_count[gi_sel_menu-1];
                            }
                            if (strlen(ga_sub_menu[loc+num-1]) == 0)
                            {
                                num--;
                            }
                            TagSubMenu(num);
                            break;
                        case 39:
                            gi_sel_menu++;
                            if (gi_sel_menu > 5)
                            {
                                gi_sel_menu = 1;
                            }
                            TagMainMenu(gi_sel_menu);
                            PopOff();
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                        case 40:
                            num = gi_sel_sub_menu + 1;
                            if (num > ga_sub_menu_count[gi_sel_menu-1])
                            {
                                num = 1;
                            }
                            if (strlen(ga_sub_menu[loc+num-1]) == 0)
                            {
                                num++;
                            }
                            TagSubMenu(num);
                            break;
                    }
                }
            }
            else if ((asc-vkc == 0) || (asc-vkc == 32)){  /*������ͨ��*/
                if (gp_top_layer->LayerNo == 0)  /*���δ�����Ӳ˵�*/
                {
                    switch (vkc)
                    {
                        case 70: /*f��F*/
                            PopMenu(1);
                            break;
                        case 77: /*m��M*/
                            PopMenu(2);
                            break;
                        case 81: /*q��Q*/
                            PopMenu(3);
                            break;
                        case 83: /*s��S*/
                            PopMenu(4);
                            break;
                        case 72: /*h��H*/
                            PopMenu(5);
                            break;
                        case 13: /*�س�*/
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                    }
                }
                else /*�ѵ����Ӳ˵�ʱ�ļ������봦��*/
                {
                    if (vkc == 27) /*�������ESC��*/
                    {
                        PopOff();
                        gi_sel_sub_menu = 0;
                    }
                    else if(vkc == 13) /*������»س���*/
                    {
                        num = gi_sel_sub_menu;
                        PopOff();
                        gi_sel_sub_menu = 0;
                        bRet = ExeFunction(gi_sel_menu, num);
                    }
                    else /*������ͨ���Ĵ���*/
                    {
                        /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
                        for (loc=0,i=1; i<gi_sel_menu; i++)
                        {
                            loc += ga_sub_menu_count[i-1];
                        }

                        /*�����뵱ǰ�Ӳ˵���ÿһ��Ĵ����ַ����бȽ�*/
                        for (i=loc; i<loc+ga_sub_menu_count[gi_sel_menu-1]; i++)
                        {
                            if (strlen(ga_sub_menu[i])>0 && vkc==ga_sub_menu[i][1])
                            { /*���ƥ��ɹ�*/
                                PopOff();
                                gi_sel_sub_menu = 0;
                                bRet = ExeFunction(gi_sel_menu, i-loc+1);
                            }
                        }
                    }
                }
            }
        }
    }
}

void PopPrompt(int num)
{

}

int PopInputMenu(char *plabel_name[], int label_num, char *ppcondition[], int hot_area_num) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot = 1, n = label_num;
    int i, maxlen, str_len;
    for (i=0,maxlen=0; i<n; i++)
    {
        str_len = strlen(plabel_name[i]);
        if (maxlen < str_len)
        {
            maxlen = str_len;
        }
    }

    /**************���������ھ���*********************/
    pos.X = maxlen + 6;
    pos.Y = n;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + 27+6;
    rcPop.Top = (SCR_ROW - pos.Y) / 2-10;
    rcPop.Bottom = rcPop.Top + pos.Y + n;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN ;  /*��������������׺���*/
    labels.num = n;
    labels.ppLabel = plabel_name;
    COORD aLoc[n];


    /******���ñ�ǩ�������λ��*****/
    for(i = 0; i < n-1; i++) {
        if (i == 0) {
            aLoc[i].X = rcPop.Left + 2;
            aLoc[i].Y = rcPop.Top + 1;
        }
        else {
            aLoc[i].X = rcPop.Left + 2;
            aLoc[i].Y = aLoc[i-1].Y + 2;
        }
    }
    aLoc[n-1].X = rcPop.Left + (pos.X - strlen(plabel_name[n-1]))/2;
    aLoc[n-1].Y = aLoc[n-2].Y + 2;
    labels.pLoc = aLoc;

    /****����������Ϣ****/
    areas.num = hot_area_num;
    SMALL_RECT aArea[hot_area_num];
    char aSort[hot_area_num];
    for (i = 0; i < hot_area_num-2; i++) {
        aSort[i] = 1;
    }
    aSort[hot_area_num-2] = 0;
    aSort[hot_area_num-1] = 0;
    char aTag[hot_area_num];
    for (i = 0; i < hot_area_num; i++) {
        aTag[i] = i+1;
    }
    for (i=0; i<hot_area_num-2; i++)
    {
        aArea[i].Left = aLoc[i+1].X + strlen(plabel_name[i+1]);
        aArea[i].Top = aLoc[i+1].Y ;
        aArea[i].Right = aLoc[i+1].X + 27;
        aArea[i].Bottom = aLoc[i+1].Y;
    }
    aArea[hot_area_num-2].Left = aLoc[hot_area_num-1].X ;
    aArea[hot_area_num-2].Top = aLoc[hot_area_num-1].Y ;
    aArea[hot_area_num-2].Right = aLoc[hot_area_num-1].X + 3;
    aArea[hot_area_num-2].Bottom = aLoc[hot_area_num-1].Y;
    aArea[hot_area_num-1].Left = aLoc[hot_area_num-1].X + 8;
    aArea[hot_area_num-1].Top = aLoc[hot_area_num-1].Y ;
    aArea[hot_area_num-1].Right = aLoc[hot_area_num-1].X + 11;
    aArea[hot_area_num-1].Bottom = aLoc[hot_area_num-1].Y;

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);
    pos.X = rcPop.Left + 2;
    pos.Y = aLoc[hot_area_num-1].Y+1;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);

    if (DealInput2(&areas, &iHot, ppcondition) != 13)
    {
        return -1;
    }
    else {
        return iHot;
    }
}

int PopChoiceMenu(char *plabel_name[], int choice_num) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot = 1, n = choice_num;
    int i, maxlen, str_len;

    for (i=0,maxlen=0; i<n; i++)
    {
        str_len = strlen(plabel_name[i]);
        if (maxlen < str_len)
        {
            maxlen = str_len;
        }
    }

    /**************���������ھ���*********************/
    pos.X = maxlen + 6;
    pos.Y = n + 5;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2-8;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN ;  /*��������������׺���*/
    labels.num = n;
    labels.ppLabel = plabel_name;
    COORD aLoc[n];


    /******���ñ�ǩ�������λ��*****/
    for (i=0; i<n; i++)
    {
        aLoc[i].X = rcPop.Left + 3;
        aLoc[i].Y = rcPop.Top + 2 + i;

    }
    str_len = strlen(plabel_name[n-1]);
    aLoc[n-1].X = rcPop.Left + 3 + (maxlen-str_len)/2;
    aLoc[n-1].Y = aLoc[n-1].Y + 2;

    labels.pLoc = aLoc;

    areas.num = n;
    SMALL_RECT aArea[n];
    char aSort[n];
    char aTag[n];
    for (i=0; i<n; i++)
    {
        aArea[i].Left = aLoc[i].X;
        aArea[i].Top = aLoc[i].Y ;
        aArea[i].Right = aLoc[i].X + strlen(plabel_name[i]) - 1;
        aArea[i].Bottom = aLoc[i].Y;
        aSort[i] = 0;
        aTag[i] = i + 1;
    }

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 + n;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    if (DealInput2(&areas, &iHot, NULL) != 13)
    {
        return -1;
    }
    return iHot;
}
/**
 * ��������: PopMenu
 * ��������: ����ָ�����˵����Ӧ���Ӳ˵�.
 * �������: num ָ�������˵����
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void PopMenu(int num)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh;
    int i, j, loc = 0;

    if (num != gi_sel_menu)       /*���ָ�����˵�������ѡ�в˵�*/
    {
        if (gp_top_layer->LayerNo != 0) /*�����ǰ�����Ӳ˵�����*/
        {
            PopOff();
            gi_sel_sub_menu = 0;
        }
    }
    else if (gp_top_layer->LayerNo != 0) /*���ѵ������Ӳ˵����򷵻�*/
    {
        return;
    }

    gi_sel_menu = num;    /*��ѡ�����˵�����Ϊָ�������˵���*/
    TagMainMenu(gi_sel_menu); /*��ѡ�е����˵����������*/
    LocSubMenu(gi_sel_menu, &rcPop); /*���㵯���Ӳ˵�������λ��, �����rcPop��*/

    /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
    for (i=1; i<gi_sel_menu; i++)
    {
        loc += ga_sub_menu_count[i-1];
    }
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel = ga_sub_menu + loc;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = ga_sub_menu_count[gi_sel_menu-1]; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[labels.num];/*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    for (i=0; i<labels.num; i++) /*ȷ����ǩ�ַ��������λ�ã����������������*/
    {
        aLoc[i].X = rcPop.Left + 2;
        aLoc[i].Y = rcPop.Top + i + 1;
    }
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = labels.num;       /*�����ĸ��������ڱ�ǩ�ĸ��������Ӳ˵�������*/
    SMALL_RECT aArea[areas.num];                    /*������������������λ��*/
    char aSort[areas.num];                      /*��������������������Ӧ���*/
    char aTag[areas.num];                         /*����������ÿ�������ı��*/
    for (i=0; i<areas.num; i++)
    {
        aArea[i].Left = rcPop.Left + 2;  /*������λ*/
        aArea[i].Top = rcPop.Top + i + 1;
        aArea[i].Right = rcPop.Right - 2;
        aArea[i].Bottom = aArea[i].Top;
        aSort[i] = 0;       /*�������Ϊ0(��ť��)*/
        aTag[i] = i + 1;           /*������˳����*/
    }
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/
    pos.X = rcPop.Left + 2;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++)
    { /*��ѭ�������ڿմ��Ӳ���λ�û����γɷָ�����ȡ���˲˵������������*/
        pCh = ga_sub_menu[loc+pos.Y-rcPop.Top-1];
        if (strlen(pCh)==0) /*����Ϊ0������Ϊ�մ�*/
        {   /*���Ȼ�����*/
            FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-3, pos, &ul);
            for (j=rcPop.Left+2; j<rcPop.Right-1; j++)
            {   /*ȡ���������ַ���Ԫ����������*/
                gp_scr_att[pos.Y*SCR_COL+j] &= 3; /*��λ��Ľ�������˵���λ*/
            }
        }

    }
    /*���Ӳ˵���Ĺ��ܼ���Ϊ�׵׺���*/
    pos.X = rcPop.Left + 3;
    att =  FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++)
    {
        if (strlen(ga_sub_menu[loc+pos.Y-rcPop.Top-1])==0)
        {
            continue;  /*�����մ�*/
        }
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
    }
    return;
}

/**
 * ��������: PopUp
 * ��������: ��ָ�������������������Ϣ, ͬʱ��������, ����������λ����Ϣ��ջ.
 * �������: pRc ��������λ�����ݴ�ŵĵ�ַ
 *           att �������������ַ�����
 *           pLabel ���������б�ǩ����Ϣ��ŵĵ�ַ
             pHotArea ����������������Ϣ��ŵĵ�ַ
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void PopUp(SMALL_RECT *pRc, WORD att, LABEL_BUNDLE *pLabel, HOT_AREA *pHotArea)
{
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j, row;

    /*������������λ���ַ���Ԫ��Ϣ��ջ*/
    size.X = pRc->Right - pRc->Left + 1;    /*�������ڵĿ��*/
    size.Y = pRc->Bottom - pRc->Top + 1;    /*�������ڵĸ߶�*/
    /*�����ŵ������������Ϣ�Ķ�̬�洢��*/
    nextLayer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    nextLayer->next = gp_top_layer;
    nextLayer->LayerNo = gp_top_layer->LayerNo + 1;
    nextLayer->rcArea = *pRc;
    nextLayer->pContent = (CHAR_INFO *)malloc(size.X*size.Y*sizeof(CHAR_INFO));
    nextLayer->pScrAtt = (char *)malloc(size.X*size.Y*sizeof(char));
    pCh = nextLayer->pScrAtt;
    /*���������ڸ���������ַ���Ϣ���棬�����ڹرյ�������ʱ�ָ�ԭ��*/
    ReadConsoleOutput(gh_std_out, nextLayer->pContent, size, pos, pRc);
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {
        /*�˶���ѭ�����������ַ���Ԫ��ԭ������ֵ���붯̬�洢���������Ժ�ָ�*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            *pCh = gp_scr_att[i*SCR_COL+j];
            pCh++;
        }
    }
    gp_top_layer = nextLayer;  /*��ɵ������������Ϣ��ջ����*/
    /*���õ������������ַ���������*/
    pos.X = pRc->Left;
    pos.Y = pRc->Top;
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {
        FillConsoleOutputAttribute(gh_std_out, att, size.X, pos, &ul);
        pos.Y++;
    }
    /*����ǩ���еı�ǩ�ַ������趨��λ�����*/
    for (i=0; i<pLabel->num; i++)
    {
        pCh = pLabel->ppLabel[i];
        if (strlen(pCh) != 0)
        {
            WriteConsoleOutputCharacter(gh_std_out, pCh, strlen(pCh),
                                        pLabel->pLoc[i], &ul);
        }
    }
    /*���õ������������ַ���Ԫ��������*/
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {
        /*�˶���ѭ�������ַ���Ԫ�Ĳ��*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = gp_top_layer->LayerNo;
        }
    }

    for (i=0; i<pHotArea->num; i++)
    {
        /*�˶���ѭ�����������������ַ���Ԫ���������ͺ��������*/
        row = pHotArea->pArea[i].Top;
        for (j=pHotArea->pArea[i].Left; j<=pHotArea->pArea[i].Right; j++)
        {
            gp_scr_att[row*SCR_COL+j] |= (pHotArea->pSort[i] << 6)
                                         | (pHotArea->pTag[i] << 2);
        }
    }
    return;
}

/**
 * ��������: PopOff
 * ��������: �رն��㵯������, �ָ���������ԭ��ۺ��ַ���Ԫԭ����.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void PopOff(void)
{
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j;

    if ((gp_top_layer->next==NULL) || (gp_top_layer->pContent==NULL))
    {   /*ջ�״�ŵ���������Ļ��Ϣ�����ùر�*/
        return;
    }
    nextLayer = gp_top_layer->next;
    /*�ָ�������������ԭ���*/
    size.X = gp_top_layer->rcArea.Right - gp_top_layer->rcArea.Left + 1;
    size.Y = gp_top_layer->rcArea.Bottom - gp_top_layer->rcArea.Top + 1;
    WriteConsoleOutput(gh_std_out, gp_top_layer->pContent, size, pos, &(gp_top_layer->rcArea));
    /*�ָ��ַ���Ԫԭ����*/
    pCh = gp_top_layer->pScrAtt;
    for (i=gp_top_layer->rcArea.Top; i<=gp_top_layer->rcArea.Bottom; i++)
    {
        for (j=gp_top_layer->rcArea.Left; j<=gp_top_layer->rcArea.Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = *pCh;
            pCh++;
        }
    }
    free(gp_top_layer->pContent);    /*�ͷŶ�̬�洢��*/
    free(gp_top_layer->pScrAtt);
    free(gp_top_layer);
    gp_top_layer = nextLayer;
    gi_sel_sub_menu = 0;
    return;
}

/**
 * ��������: DrawBox
 * ��������: ��ָ�����򻭱߿�.
 * �������: pRc �������λ����Ϣ�ĵ�ַ
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void DrawBox(SMALL_RECT *pRc)
{
    char chBox[] = {'+','-','|'};  /*�����õ��ַ�*/
    COORD pos = {pRc->Left, pRc->Top};  /*��λ����������Ͻ�*/

    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����Ͻ�*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
    {   /*��ѭ�����ϱ߿����*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }
    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����Ͻ�*/
    for (pos.Y = pRc->Top+1; pos.Y < pRc->Bottom; pos.Y++)
    {   /*��ѭ�����߿�����ߺ��ұ���*/
        pos.X = pRc->Left;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
        pos.X = pRc->Right;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
    }
    pos.X = pRc->Left;
    pos.Y = pRc->Bottom;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����½�*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
    {   /*���±߿����*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }
    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����½�*/
    return;
}

/**
 * ��������: TagSubMenu
 * ��������: ��ָ���Ӳ˵�������ѡ�б��.
 * �������: num ѡ�е��Ӳ˵����
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void TagSubMenu(int num)
{
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int width;

    LocSubMenu(gi_sel_menu, &rcPop);  /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    if ((num<1) || (num == gi_sel_sub_menu) || (num>rcPop.Bottom-rcPop.Top-1))
    {   /*����Ӳ˵����Խ�磬������Ӳ˵��ѱ�ѡ�У��򷵻�*/
        return;
    }

    pos.X = rcPop.Left + 2;
    width = rcPop.Right - rcPop.Left - 3;
    if (gi_sel_sub_menu != 0) /*����ȡ��ԭѡ���Ӳ˵����ϵı��*/
    {
        pos.Y = rcPop.Top + gi_sel_sub_menu;
        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
        FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
        pos.X += 1;
        att |=  FOREGROUND_RED;/*�׵׺���*/
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
    }
    /*���ƶ��Ӳ˵�������ѡ�б��*/
    pos.X = rcPop.Left + 2;
    pos.Y = rcPop.Top + num;
    att = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*�ڵװ���*/
    FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
    gi_sel_sub_menu = num;  /*�޸�ѡ���Ӳ˵����*/
    return;
}

/**
 * ��������: LocSubMenu
 * ��������: ���㵯���Ӳ˵��������ϽǺ����½ǵ�λ��.
 * �������: num ѡ�е����˵����
 * �������: rc �������λ����Ϣ�ĵ�ַ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void LocSubMenu(int num, SMALL_RECT *rc)
{
    int i, len, loc = 0;

    rc->Top = 1; /*������ϱ߶��ڵ�2�У��к�Ϊ1*/
    rc->Left = 1;
    for (i=1; i<num; i++)
    {   /*����������߽�λ��, ͬʱ�����һ���Ӳ˵������Ӳ˵��ַ��������е�λ��*/
        rc->Left += strlen(ga_main_menu[i-1]) + 4;
        loc += ga_sub_menu_count[i-1];
    }
    rc->Right = strlen(ga_sub_menu[loc]);/*��ʱ��ŵ�һ���Ӳ˵����ַ�������*/
    for (i=1; i<ga_sub_menu_count[num-1]; i++)
    {   /*������Ӳ˵��ַ��������䳤�ȴ����rc->Right*/
        len = strlen(ga_sub_menu[loc+i]);
        if (rc->Right < len)
        {
            rc->Right = len;
        }
    }
    rc->Right += rc->Left + 3;  /*����������ұ߽�*/
    rc->Bottom = rc->Top + ga_sub_menu_count[num-1] + 1;/*���������±ߵ��к�*/
    if (rc->Right >= SCR_COL)  /*�ұ߽�Խ��Ĵ���*/
    {
        len = rc->Right - SCR_COL + 1;
        rc->Left -= len;
        rc->Right = SCR_COL - 1;
    }
    return;
}
/**
 * ��������: DealInput
 * ��������: �ڵ�������������������, �ȴ�����Ӧ�û�����.
 * �������: pHotArea
 *           piHot ����������ŵĴ�ŵ�ַ, ��ָ�򽹵�������ŵ�ָ��
 * �������: piHot ����굥�������س���ո�ʱ���ص�ǰ�������
 * �� �� ֵ:
 *
 * ����˵��:
 */
int DealInput2(HOT_AREA *pHotArea, int *piHot, char **ppcondition)
{
    INPUT_RECORD inRec[2];
    DWORD res;
    COORD pos = {0, 0};
    int num = *piHot, arrow, iRet = 0;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/
    int i, j , TxtHotAreaNum = 0;

    for (i=0; i<pHotArea->num; i++) /*�����ı�����������*/
    {
        if(pHotArea->pSort[i] == 1)
        {
            TxtHotAreaNum++;
        }
    }
    for (i=0; i<TxtHotAreaNum; i++)
    {
        ppcondition[i] = (char*)malloc(30*sizeof(char));
        ppcondition[i][0] = '\0';
    }

    SetHotPoint(pHotArea, *piHot);

    while (TRUE)
    {
        /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 2, &res);
        if ((inRec[0].EventType == MOUSE_EVENT) &&
            (inRec[0].Event.MouseEvent.dwButtonState
             == FROM_LEFT_1ST_BUTTON_PRESSED))
        {
            pos = inRec[0].Event.MouseEvent.dwMousePosition;
            cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3;   /*ȡ���������ڵĲ���*/
            cTag = (gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2) & 15;  /*ȡ���ַ���Ԫ���������*/
            cSort = (gp_scr_att[pos.Y * SCR_COL + pos.X] >> 6) & 3;  /*ȡ����������*/

            if ((cNo == gp_top_layer->LayerNo) && cTag > 0)
            {
                *piHot = cTag;
                num = *piHot;
                SetHotPoint(pHotArea, *piHot);
                if (cSort == 0)  /*ѡ�е���ȡ����ť*/
                {
                    iRet = 13;
                    break;
                }
                else
                {
                    j = 0;
                    for (i=0; i<num; i++)
                    {
                        if(pHotArea->pSort[i] == 1)
                        {
                            j++;
                        }
                    }
                    /**���ù��λ�ã�ʹ��ʼ�ճ������������ַ����ĺ���**/
                    COORD CursorPosition = {pHotArea->pArea[num-1].Left+strlen(ppcondition[j-1]),
                                            pHotArea->pArea[num-1].Top
                    };
                    SetConsoleCursorPosition(gh_std_out, CursorPosition);
                }
            }
        }
        else if (inRec[0].EventType == KEY_EVENT && inRec[0].Event.KeyEvent.bKeyDown)
        {
            vkc = inRec[0].Event.KeyEvent.wVirtualKeyCode;
            asc = inRec[0].Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0)
            {
                arrow = 0;
                switch (vkc)
                {
                    /*�����(���ϡ��ҡ���)�Ĵ���*/
                    case 37:
                        arrow = 1;
                        break;
                    case 38:
                        arrow = 2;
                        break;
                    case 39:
                        arrow = 3;
                        break;
                    case 40:
                        arrow = 4;
                        break;
                }
                if (arrow > 0)
                {
                    num = *piHot;
                    while (TRUE)
                    {
                        if (arrow < 3)
                        {
                            num--;
                        }
                        else
                        {
                            num++;
                        }
                        if (num == 0)
                        {
                            num = pHotArea->num;
                        }
                        if (num == pHotArea->num + 1)
                        {
                            num = 1;
                        }
                        if (((arrow % 2) && (pHotArea->pArea[num-1].Top
                                             == pHotArea->pArea[*piHot-1].Top)) || ((!(arrow % 2))
                                                                                    && (pHotArea->pArea[num-1].Top
                                                                                        != pHotArea->pArea[*piHot-1].Top)) || num == *piHot)
                        {
                            /*ͬ�е��������Ҽ��л�����ͬ�е����¼��л���������ͬһ������Ҳִ��break*/
                            break;
                        }
                    }
                    if (num > 0 && num <= pHotArea->num)
                    {
                        *piHot = num;
                        SetHotPoint(pHotArea, *piHot);
                        if (pHotArea->pSort[num-1] == 1)
                        {
                            j = 0;
                            for (i=0; i<num; i++)
                            {
                                if(pHotArea->pSort[i] == 1)
                                {
                                    j++;
                                }
                            }
                            /**���ù��λ�ã�ʹ��ʼ�ճ������������ַ����ĺ���**/
                            COORD CursorPosition = {pHotArea->pArea[num-1].Left+strlen(ppcondition[j-1]),
                                                    pHotArea->pArea[num-1].Top
                            };
                            SetConsoleCursorPosition(gh_std_out, CursorPosition);
                        }
                    }
                }
            }
            else if (vkc == 8)
            {
                /*�˸��*/
                if (pHotArea->pSort[num-1] == 1)
                {
                    COORD CursorPosition;
                    j=0;
                    for (i=0; i<num; i++)
                    {
                        if(pHotArea->pSort[i] == 1)
                        {
                            j++;
                        }
                    }
                    if (*(ppcondition[j-1]+strlen(ppcondition[j-1])-1) > 31
                        && *(ppcondition[j-1]+strlen(ppcondition[j-1])-1) < 127)/*����Ƿ������ַ�*/
                    {
                        *(ppcondition[j-1]+strlen(ppcondition[j-1])-1) = '\0';
                        CursorPosition.X = pHotArea->pArea[num-1].Left+strlen(ppcondition[j-1]);
                        CursorPosition.Y = pHotArea->pArea[num-1].Top;
                        SetConsoleCursorPosition(gh_std_out, CursorPosition);
                        FillConsoleOutputCharacter(gh_std_out, '\0', 1, CursorPosition, &ul);   /*ɾ����λ���ɿ��ַ����*/
                    }
                    else/*�������ַ������ĺ��֣�ɾ���������ַ�����һ������*/
                    {
                        *(ppcondition[j-1]+strlen(ppcondition[j-1])-2) = '\0';
                        CursorPosition.X = pHotArea->pArea[num-1].Left+strlen(ppcondition[j-1]);
                        CursorPosition.Y = pHotArea->pArea[num-1].Top;
                        SetConsoleCursorPosition(gh_std_out, CursorPosition);
                        FillConsoleOutputCharacter(gh_std_out, '\0', 2, CursorPosition, &ul);/*ɾ����λ���ɿ��ַ����*/
                    }
                }
            }
            else if (vkc == 9)
            {
                /*tab����ʾ�л�����*/

                num++;
                if (num == pHotArea->num + 1)
                {
                    num = 1;
                }
                *piHot = num;
                SetHotPoint(pHotArea, *piHot);
            }
            else if (vkc == 27)
            {
                /*ESC��*/

                iRet = 27;
                /*���ع��*/
                CONSOLE_CURSOR_INFO console_cursor_info;
                console_cursor_info.dwSize = 20;
                console_cursor_info.bVisible = FALSE;
                SetConsoleCursorInfo(gh_std_out, &console_cursor_info);
                break;
            }

            else if (vkc == 13)
            {
                /*�س�����ʾ���µ�ǰ��ť*/
                if (pHotArea->pSort[num-1] == 0) /*�����ǰ��������ǰ���������*/
                {
                    iRet = 13;
                    break;
                }
                else
                {
                    num++;    /*������ı������������л�����һ������*/
                    *piHot = num;
                    SetHotPoint(pHotArea, *piHot);
                    if (pHotArea->pSort[num-1] == 1) /*�����ʱ���ı�������*/
                    {
                        j = 0;
                        for (i=0; i<num; i++)
                        {
                            if(pHotArea->pSort[i] == 1)
                            {
                                j++;
                            }
                        }
                        /**���ù��λ�ã�ʹ��ʼ�ճ������������ַ����ĺ���**/
                        COORD CursorPosition = {pHotArea->pArea[num-1].Left+strlen(ppcondition[j-1]),
                                                pHotArea->pArea[num-1].Top
                        };
                        SetConsoleCursorPosition(gh_std_out, CursorPosition);
                    }
                }
            }
            else  /*������ͨ��*/
            {
                if (pHotArea->pSort[num-1] == 1)
                {
                    char string[10];
                    j=0;
                    for (i=0; i<num; i++)
                    {
                        if(pHotArea->pSort[i] == 1)
                        {
                            j++;
                        }
                    }

                    /*������ַ������ô��ڸ������ĳ���*/
                    if (strlen(ppcondition[j-1])<pHotArea->pArea[num-1].Right - pHotArea->pArea[num-1].Left)
                    {
                        for (i=0; i<res; i++)
                        {
                            string[i] = inRec[i].Event.KeyEvent.uChar.AsciiChar;
                        }
                        string[i] = '\0';
                        strcat(ppcondition[j-1],string);
                        CONSOLE_SCREEN_BUFFER_INFO console_screen_buffer_info;
                        DWORD NumOfAttsWritten;
                        WORD att_new = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
                        GetConsoleScreenBufferInfo(gh_std_out, &console_screen_buffer_info);
                        printf("%s", string);
                        FillConsoleOutputAttribute(gh_std_out, att_new, strlen(string), console_screen_buffer_info.dwCursorPosition, &NumOfAttsWritten);
                    }
                }
            }
        }
    }
    return iRet;
}
/**
 * ��������: DealInput
 * ��������: �ڵ�������������������, �ȴ�����Ӧ�û�����.
 * �������: pHotArea
 *           piHot ����������ŵĴ�ŵ�ַ, ��ָ�򽹵�������ŵ�ָ��
 * �������: piHot ����굥�������س���ո�ʱ���ص�ǰ�������
 * �� �� ֵ:
 *
 * ����˵��:
 */
int DealInput(HOT_AREA *pHotArea, int *piHot)
{
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    int num, arrow, iRet = 0;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/

    SetHotPoint(pHotArea, *piHot);
    while (TRUE)
    {    /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if ((inRec.EventType == MOUSE_EVENT) &&
            (inRec.Event.MouseEvent.dwButtonState
             == FROM_LEFT_1ST_BUTTON_PRESSED))
        {
            pos = inRec.Event.MouseEvent.dwMousePosition;
            cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3;
            cTag = (gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2) & 15;
            cSort = (gp_scr_att[pos.Y * SCR_COL + pos.X] >> 6) & 3;

            if ((cNo == gp_top_layer->LayerNo) && cTag > 0)
            {
                *piHot = cTag;
                SetHotPoint(pHotArea, *piHot);
                if (cSort == 0)
                {
                    iRet = 13;
                    break;
                }
            }
        }
        else if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown)
        {
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;;
            if (asc == 0)
            {
                arrow = 0;
                switch (vkc)
                {  /*�����(���ϡ��ҡ���)�Ĵ���*/
                    case 37: arrow = 1; break;
                    case 38: arrow = 2; break;
                    case 39: arrow = 3; break;
                    case 40: arrow = 4; break;
                }
                if (arrow > 0)
                {
                    num = *piHot;
                    while (TRUE)
                    {
                        if (arrow < 3)
                        {
                            num--;
                        }
                        else
                        {
                            num++;
                        }
                        if ((num < 1) || (num > pHotArea->num) ||
                            ((arrow % 2) && (pHotArea->pArea[num-1].Top
                                             == pHotArea->pArea[*piHot-1].Top)) || ((!(arrow % 2))
                                                                                    && (pHotArea->pArea[num-1].Top
                                                                                        != pHotArea->pArea[*piHot-1].Top)))
                        {
                            break;
                        }
                    }
                    if (num > 0 && num <= pHotArea->num)
                    {
                        *piHot = num;
                        SetHotPoint(pHotArea, *piHot);
                    }
                }
            }
            else if (vkc == 27)
            {  /*ESC��*/
                iRet = 27;
                break;
            }
            else if (vkc == 13 || vkc == 32)
            {  /*�س�����ո��ʾ���µ�ǰ��ť*/
                iRet = 13;
                break;
            }
        }
    }
    return iRet;
}

BOOL ShowResult(char **pString, int n,int col )
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot = 1;
    int i, j, maxlen, str_len, row;

    if ((n - 2) % col == 0)
    {
        row = (n - 2) / col;
    }
    else
    {
        row = (n - 2) / col + 1;
    }
    /**�ҳ��ַ������ܳ����һ��**/
    for (j=0,maxlen=0; j<row; j++)
    {
        for (i=0, str_len=0; i<col; i++)
        {
            if (j*col+i+1==n-1)
            {
                break;
            }
            str_len += strlen(pString[j*col+i+1]);
        }
        if (maxlen < str_len)
        {
            maxlen = str_len;
        }
    }
    /**���ô��ڴ�С**/
    pos.X = maxlen + col * 6 -1;
    pos.Y = row + 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    /**��������ַ�����Ϣ**/
    att = BACKGROUND_BLUE | BACKGROUND_GREEN ;  /*��������������׺���*/
    labels.num = n;
    labels.ppLabel = pString;
    COORD aLoc[n];
    /**��������ַ�����λ��**/
    aLoc[0].X = (pos.X - strlen(pString[0])) / 2 + rcPop.Left;
    aLoc[0].Y = rcPop.Top + 1;
    aLoc[1].X = rcPop.Left + 2;
    aLoc[1].Y = rcPop.Top + 3;
    for (i=1; i<col; i++)
    {
        aLoc[i+1].X = aLoc[i].X + strlen(pString[i]) + 6;/*ʹ�����ַ�֮������̶��ַ���*/
        aLoc[i+1].Y = rcPop.Top + 3;
    }
    for (j=1; j<row; j++)
    {
        for (i=0; i<col; i++)
        {
            if (j*col+i+1==n-1)
            {
                break;
            }
            aLoc[j*col+i+1].X = aLoc[i+1].X;
            aLoc[j*col+i+1].Y = rcPop.Top + 4 + j;
        }
    }
    aLoc[n-1].X = (pos.X - strlen(pString[n-1])) / 2 + rcPop.Left;
    aLoc[n-1].Y = rcPop.Bottom - 1;

    labels.pLoc = aLoc;

    /*����������Ϣ*/
    areas.num = 1;
    SMALL_RECT aArea[] = {{
                                  aLoc[n-1].X, aLoc[n-1].Y,
                                  aLoc[n-1].X + 3, aLoc[n-1].Y
                          }
    };
    char aSort[] = {0};
    char aTag[] = {1};

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Bottom - 2;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);

    DealInput2(&areas, &iHot, NULL);
    PopOff();

    return bRet;
}

int ShowSearchResult(char **pString, int n,int col )
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot = 1;
    int i, j, maxlen, str_len, row;

    if ((n - 2) % col == 0)
    {
        row = (n - 2) / col;
    }
    else
    {
        row = (n - 2) / col + 1;
    }
    /**�ҳ��ַ������ܳ����һ��**/
    for (j=0,maxlen=0; j<row; j++)
    {
        for (i=0, str_len=0; i<col; i++)
        {
            if (j*col+i+1==n-1)
            {
                break;
            }
            str_len += strlen(pString[j*col+i+1]);
        }
        if (maxlen < str_len)
        {
            maxlen = str_len;
        }
    }
    /**���ô��ڴ�С**/
    pos.X = maxlen + col * 6 -1;
    pos.Y = row + 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    /**��������ַ�����Ϣ**/
    att = BACKGROUND_BLUE | BACKGROUND_GREEN ;  /*��������������׺���*/
    labels.num = n;
    labels.ppLabel = pString;
    COORD aLoc[n];
    /**��������ַ�����λ��**/
    aLoc[0].X = (pos.X - strlen(pString[0])) / 2 + rcPop.Left;
    aLoc[0].Y = rcPop.Top + 1;
    aLoc[1].X = rcPop.Left + 2;
    aLoc[1].Y = rcPop.Top + 3;
    for (i=1; i<col; i++)
    {
        aLoc[i+1].X = aLoc[i].X + strlen(pString[i]) + 6;/*ʹ�����ַ�֮������̶��ַ���*/
        aLoc[i+1].Y = rcPop.Top + 3;
    }
    for (j=1; j<row; j++)
    {
        for (i=0; i<col; i++)
        {
            if (j*col+i+1==n-1)
            {
                break;
            }
            aLoc[j*col+i+1].X = aLoc[i+1].X;
            aLoc[j*col+i+1].Y = rcPop.Top + 4 + j;
        }
    }
    aLoc[n-1].X = (pos.X - strlen(pString[n-1])) / 2 + rcPop.Left;
    aLoc[n-1].Y = rcPop.Bottom - 1;

    labels.pLoc = aLoc;

    /*����������Ϣ*/
    areas.num = row;
    SMALL_RECT aArea[row];
    char aSort[row];
    char aTag[row];
    i = 3;
    for (j=0; j<row-1; j++)
    {
        aArea[j].Left = aLoc[i].X;
        aArea[j].Top = aLoc[i].Y ;
        aArea[j].Right = aLoc[i+1].X + strlen(pString[i+1]);
        aArea[j].Bottom = aLoc[i].Y;
        aSort[j] = 0;
        aTag[j] = j + 1;
        i = i+col;
    }
    aArea[j].Left = aLoc[n-1].X;
    aArea[j].Top = aLoc[n-1].Y ;
    aArea[j].Right = aLoc[n-1].X + 3;
    aArea[j].Bottom = aLoc[n-1].Y;
    aSort[j] = 0;
    aTag[j] = j + 1;

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Bottom - 2;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);

    DealInput2(&areas, &iHot, NULL);
    PopOff();

    return iHot;
}

void SetHotPoint(HOT_AREA *pHotArea, int iHot)
{
    CONSOLE_CURSOR_INFO lpCur;
    COORD pos = {0, 0};
    WORD att1, att2, att3;
    int i, width;

    att1 = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*��ѡ���������ı����Ժڵװ���*/
    att2 = BACKGROUND_BLUE | BACKGROUND_GREEN ;  /*��׺���*/
    att3 = BACKGROUND_BLUE | BACKGROUND_GREEN |BACKGROUND_RED ;  /*�׵׺���*/
    for (i=0; i<pHotArea->num; i++)
    {
        /*����ť��������Ϊ�׵׺���*/
        pos.X = pHotArea->pArea[i].Left;
        pos.Y = pHotArea->pArea[i].Top;
        width = pHotArea->pArea[i].Right - pHotArea->pArea[i].Left + 1;
        if (pHotArea->pSort[i] == 0)
        {
            /*�����ǰ�ť��*/
            FillConsoleOutputAttribute(gh_std_out, att2, width, pos, &ul);
        }
        else
        {
            /*�������ı���*/
            FillConsoleOutputAttribute(gh_std_out, att3, width, pos, &ul);
        }
    }

    pos.X = pHotArea->pArea[iHot-1].Left;
    pos.Y = pHotArea->pArea[iHot-1].Top;
    width = pHotArea->pArea[iHot-1].Right - pHotArea->pArea[iHot-1].Left + 1;
    if (pHotArea->pSort[iHot-1] == 0)
    {
        /*�����������ǰ�ť��*/
        FillConsoleOutputAttribute(gh_std_out, att1, width, pos, &ul);
        GetConsoleCursorInfo(gh_std_out, &lpCur);
        lpCur.bVisible = FALSE;
        SetConsoleCursorInfo(gh_std_out, &lpCur);
    }
    else if (pHotArea->pSort[iHot-1] == 1)
    {
        /*�������������ı�����*/
        SetConsoleCursorPosition(gh_std_out, pos);
        GetConsoleCursorInfo(gh_std_out, &lpCur);
        lpCur.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out, &lpCur);
    }
}

/**
 * ��������: ExeFunction
 * ��������: ִ�������˵��ź��Ӳ˵���ȷ���Ĺ��ܺ���.
 * �������: m ���˵����
 *           s �Ӳ˵����
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE �� FALSE
 *
 * ����˵��: ����ִ�к���ExitSysʱ, �ſ��ܷ���FALSE, ������������Ƿ���TRUE
 */
BOOL ExeFunction(int m, int s)
{
    BOOL bRet = TRUE;
    /*����ָ�����飬����������й��ܺ�������ڵ�ַ*/
    BOOL (*pFunction[ga_sub_menu_count[0]+ga_sub_menu_count[1]+ga_sub_menu_count[2]+ga_sub_menu_count[3]+ga_sub_menu_count[4]])(void);
    int i, loc;

    /*�����ܺ�����ڵ�ַ�����빦�ܺ����������˵��ź��Ӳ˵��Ŷ�Ӧ�±������Ԫ��*/
    pFunction[0] = SaveData;
    pFunction[1] = BackupData;
    pFunction[2] = RestoreData;
    pFunction[3] = ExitSys;

    pFunction[4] = MaintainCityInfo;
    pFunction[5] = MaintainScenicAreaInfo;
    pFunction[6] = MaintainAttractionInfo;
    pFunction[7] = QueryCityInfo;
    pFunction[8] = QueryScenicAreaInfo;
    pFunction[9] = QueryAttractionInfo;
    pFunction[10] = RandomView;
    pFunction[11] = StatusCityInfo;
    pFunction[12] = HelpTopic;
    pFunction[13] = NULL;
    pFunction[14] = AboutDorm;

    for (i=1,loc=0; i<m; i++)  /*�������˵��ź��Ӳ˵��ż����Ӧ�±�*/
    {
        loc += ga_sub_menu_count[i-1];
    }
    loc += s - 1;

    if (pFunction[loc] != NULL)
    {
        bRet = (*pFunction[loc])();  /*�ú���ָ�������ָ��Ĺ��ܺ���*/
    }

    return bRet;
}

BOOL SaveData(void)
{
    BOOL bRet;

    bRet = SaveSysData2(gp_head2);
    if(bRet)
    {
        char *plabel_name[] = {"���ݱ���ɹ�",
                               "ȷ��"
        };
        ShowModule(plabel_name, 2);
    }
    return bRet;
}

BOOL BackupData(void)
{
    BOOL bRet;
    time_t current_time;
    struct tm *current_tm;
    char backupfile[30] = "Backup ";
    time(&current_time);                         /*ȡ��ϵͳ��ǰʱ��*/
    current_tm = localtime(&current_time);       /*ȡ��������*/
    strncat(backupfile,asctime(current_tm), 10);
    bRet = BackupSysData(gp_head2, backupfile);
    char *plabel_name[] =
            {
                    "���ݱ��ݳɹ�",
                    "ȷ��"
            };

    ShowModule(plabel_name, 2);

    return bRet;
}

BOOL RestoreData(void)
{
    BOOL bRet = TRUE;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot = 1;
    char *plabel_name[] = {"�������������ļ���",
                           "ȷ��    ȡ��"
    };
    /**************���������ھ���*********************/
    pos.X = strlen(plabel_name[0]) + 6;
    pos.Y = 8;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN ;  /*��������������׺���*/
    labels.num = 2;
    labels.ppLabel = plabel_name;
    COORD aLoc[2];

    /******���ñ�ǩ�������λ��*****/

    aLoc[0].X = rcPop.Left + (pos.X-strlen(plabel_name[0]))/2;
    aLoc[0].Y = rcPop.Top + 2;
    aLoc[1].X = rcPop.Left + (pos.X-strlen(plabel_name[1]))/2;
    aLoc[1].Y = rcPop.Top + 6;

    labels.pLoc = aLoc;

    /******��������******/
    areas.num = 3;
    SMALL_RECT aArea[3];
    char aSort[3] = {1, 0, 0};
    char aTag[3] = {1, 2, 3};
    aArea[0].Left = aLoc[0].X;
    aArea[0].Top = aLoc[0].Y + 2 ;
    aArea[0].Right = aLoc[0].X + strlen(plabel_name[0]) - 1;
    aArea[0].Bottom = aLoc[0].Y + 2;
    aArea[1].Left = aLoc[1].X;
    aArea[1].Top = aLoc[1].Y ;
    aArea[1].Right = aLoc[1].X + 3;
    aArea[1].Bottom = aLoc[1].Y;
    aArea[2].Left = aLoc[1].X + 8;
    aArea[2].Top = aLoc[1].Y ;
    aArea[2].Right = aLoc[1].X + 11;
    aArea[2].Bottom = aLoc[1].Y;

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);

    char *ppcondition[1];
    if (DealInput2(&areas, &iHot, ppcondition) == 13)
    {
        PopOff();
        if (iHot == 2) /*���ñ��ݺ���*/
        {
            if (*ppcondition[0] == '\0')
            {
                char *plabel_name[] = {"���������ϢΪ��",
                                       "ȷ��"
                };
                ShowModule(plabel_name, 2);
            }
            else if (RestoreSysData(&gp_head2, ppcondition[0]))
            {
                char *plabel_name[] = {"��Ϣ����ɹ���",
                                       "ȷ��"
                };
                ShowModule(plabel_name, 2);
            }
        }
    }
    else
    {
        PopOff();
    }
    return bRet;
}

BOOL ExitSys(void)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"ȷ���˳�ϵͳ��", "ȷ��    ȡ��"};
    int iHot = 1;

    pos.X = strlen(pCh[0]) + 6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
    labels.num = 2;
    labels.ppLabel = pCh;
    COORD aLoc[] = {{rcPop.Left+3, rcPop.Top+2},
                    {rcPop.Left+5, rcPop.Top+5}};
    labels.pLoc = aLoc;

    areas.num = 2;
    SMALL_RECT aArea[] = {{rcPop.Left + 5, rcPop.Top + 5,
                                  rcPop.Left + 8, rcPop.Top + 5},
                          {rcPop.Left + 13, rcPop.Top + 5,
                                  rcPop.Left + 16, rcPop.Top + 5}};
    char aSort[] = {0, 0};
    char aTag[] = {1, 2};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);

    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);

    if (DealInput(&areas, &iHot) == 13 && iHot == 1)
    {
        bRet = FALSE;
    }
    else
    {
        bRet = TRUE;
    }
    PopOff();

    return bRet;
}
BOOL MaintainCityInfo(void) {
    BOOL bRet = TRUE;
    char *plabel_name[] = {"¼�������Ϣ",
                           "�޸ĳ�����Ϣ",
                           "ɾ��������Ϣ",
                           "ȡ��"
    };

    int iHot = PopChoiceMenu(plabel_name, 4);

    if (iHot == -1) {
        PopOff();
    }
    else {
        if (iHot == 1){
            PopOff();
            InsertCityNodeSubMenu();
        }
        else if (iHot == 2) {
            PopOff();
            ModifyCityNodeSubMenu();
        }
        else if (iHot == 3) {
            PopOff();
            DeleteCityNodeSubMenu();
        }
        else {
            PopOff();
        }
    }
    return bRet;
}

BOOL InsertCityNodeSubMenu(void) {
    BOOL bRet = TRUE;
    char *plabel_name[] = {"�������¼��ĳ�����Ϣ",
                           "��  ��ID",
                           "��������",
                           "�ල�绰",
                           "��ѯ�绰",
                           "ȷ��    ȡ��"
    };
    int n = 6;
    int inputNum = 4;
    char *ppcondition[inputNum];
    int iHot = PopInputMenu(plabel_name, n, ppcondition, inputNum+2);
    if (iHot == -1) {
        PopOff();
    }
    else {
        if (iHot == 5) {
            PopOff();
            if((strlen(ppcondition[0])==0) || (strlen(ppcondition[1])==0)) {
                char *error_plabel_name[] = {"������д�Ϸ��ĳ���ID�ͳ�������",
                                             "ȷ��"
                };
                ShowModule(error_plabel_name, 2);
                return FALSE;
            }
            else {
                CITY_NODE *cityFound = SeekCityNodeByID(gp_head2, ppcondition[0]);
                if(cityFound!=NULL) {
                    char *error_plabel_name[] = {"����Ҫ����ĳ�����Ϣ�Ѵ��ڣ�����ɾ�������޸ģ�",
                                                 "ȷ��"
                    };
                    ShowModule(error_plabel_name, 2);
                    return FALSE;
                }
                else{
                    CITY_NODE *pCityNode = (CITY_NODE *)malloc(sizeof(CITY_NODE));
                    pCityNode->rnext = NULL;
                    pCityNode->next = NULL;
                    strcpy(pCityNode->city_id, ppcondition[0]);
                    strcpy(pCityNode->name, ppcondition[1]);
                    strcpy(pCityNode->jiandu_num, ppcondition[2]);
                    strcpy(pCityNode->zixun_num, ppcondition[3]);
                    ConfirmCityInsertion(&gp_head2, pCityNode);
                    char *error_plabel_name[] = {"������ӳɹ���",
                                                 "ȷ��"
                    };
                    ShowModule(error_plabel_name, 2);
                    return bRet;
                }

            }
        }
        else if (iHot == 6){
            PopOff();
        }
    }
    return bRet;
}

BOOL ModifyCityNodeSubMenu(void) {
    BOOL bRet = TRUE;
    char *plabel_name[] = {"���ݳ��������޸�",
                           "���ݳ�  ��ID�޸�",
                           "ȡ��"
    };
    int n = 3;
    int iHot = PopChoiceMenu(plabel_name, n);
    if (iHot == -1) {
        PopOff();
    }
    else {
        if (iHot == 1) {
            PopOff();
        }
        else if (iHot == 2){
            PopOff();
        }
        else if (iHot == 3) {
            PopOff();
        }
        else {
            PopOff();
        }
    }
    return bRet;
}

BOOL DeleteCityNodeSubMenu(void) {
    BOOL bRet = TRUE;
    char *plabel_name[] = {"���ݳ�������ɾ��",
                           "���ݳ�  ��IDɾ��",
                           "ȡ��"
    };
    int n = 3;
    int iHot = PopChoiceMenu(plabel_name, n);
    if (iHot == -1) {
        PopOff();
    }
    else {
        if (iHot == 1) {
            PopOff();
        }
        else if (iHot == 2){
            PopOff();
            char *plabel[] = {"�������ɾ���ĳ���ID��Ϣ",
                              "��  ��ID",
                              "ȷ��    ȡ��"
            };
            int n = 3;
            int inputNum = 3;
            char *ppcondition[inputNum];
            int aHot = PopInputMenu(plabel, n, ppcondition, inputNum);
            if(aHot==2) {
                PopOff();
                CITY_NODE *pCityNode = SeekCityNodeByID(gp_head2, ppcondition[0]);
                if(pCityNode==NULL) {
                    char *plabel_name[] = {"��ɾ���ĳ��в����ڣ�",
                                           "ȷ��"
                    };
                    ShowModule(plabel_name, 2);
                }
                else {
                    BOOL del = delete_city(&gp_head2,ppcondition[0]);
                    if(del){
                        char *error_plabel_name[] = {"ɾ���ɹ���",
                                                     "ȷ��"
                        };
                        ShowModule(error_plabel_name, 2);
                        return bRet;
                    }
                    else {
                        char *error_plabel_name[] = {"ɾ��ʧ�ܣ�",
                                                     "ȷ��"
                        };
                        ShowModule(error_plabel_name, 2);
                        return bRet;
                    }
                }
            }
            else{
                PopOff();
            }
        }
        else if (iHot == 3) {
            PopOff();
        }
        else {
            PopOff();
        }
    }
    return bRet;
}

BOOL MaintainScenicAreaInfo(void) {
    BOOL bRet = TRUE;
    char *plabel_name[] = {"¼�뾰����Ϣ",
                           "�޸ľ�����Ϣ",
                           "ɾ��������Ϣ",
                           "ȡ��"
    };

    int iHot = PopChoiceMenu(plabel_name, 4);

    if (iHot == -1) {
        PopOff();
    }
    else {
        if (iHot == 1){
            PopOff();
            InsertScenicAreaNodeSubMenu();

        }
        else if (iHot == 2) {
            PopOff();
            ModifyScenicAreaNodeSubMenu();
        }
        else if (iHot == 3) {
            PopOff();
            DeleteScenicAreaNodeSubMenu();
        }
        else {
            PopOff();
        }
    }
    return bRet;

}

BOOL InsertScenicAreaNodeSubMenu(void) {
    BOOL bRet = TRUE;
    char *plabel_name[] = {"�������¼��ľ�����Ϣ",
                           "��������",
                           "��  ��ID",
                           "��  ��ID",
                           "��������",
                           "������ַ",
                           "��Ʊ�۸�",
                           "����ʱ��",
                           "ȷ��    ȡ��"
    };
    int n = 9;
    int inputNum = 9;
    char *ppcondition[inputNum];
    int iHot = PopInputMenu(plabel_name, n, ppcondition, inputNum );
    if (iHot == -1) {
        PopOff();
    }
    else {
        if (iHot == 8) {
            PopOff();
            if((strlen(ppcondition[0])==0) || (strlen(ppcondition[1])==0)) {
                char *error_plabel_name[] = {"������д�Ϸ��ľ���ID�;�������",
                                             "ȷ��"
                };
                ShowModule(error_plabel_name, 2);
                return FALSE;
            }
            else {
                REGION_NODE *cityFound = SeekRegionNodeByID(gp_head2, ppcondition[1]);

                if(cityFound!=NULL) {
                    char *error_plabel_name[] = {"����Ҫ����ľ�����Ϣ�Ѵ��ڣ�����ɾ�������޸ģ�",
                                                 "ȷ��"
                    };
                    ShowModule(error_plabel_name, 2);
                    return FALSE;
                }
                else{
                    REGION_NODE *pRegionNode = (REGION_NODE *)malloc(sizeof(REGION_NODE));
                    pRegionNode->snext = NULL;
                    pRegionNode->next = NULL;
                    strcpy(pRegionNode->name, ppcondition[0]);
                    strcpy(pRegionNode->region_id, ppcondition[1]);
                    strcpy(pRegionNode->city_id, ppcondition[2]);
                    strcpy(pRegionNode->level, ppcondition[3]);
                    strcpy(pRegionNode->address, ppcondition[4]);
                    strcpy(pRegionNode->price, ppcondition[5]);
                    strcpy(pRegionNode->opentime, ppcondition[6]);
                    ConfirmRegionInsertion(gp_head2, pRegionNode);
                    char *error_plabel_name[] = {"������ӳɹ���",
                                                 "ȷ��"
                    };
                    ShowModule(error_plabel_name, 2);
                    return bRet;
                }

            }
        }
        else if (iHot == 9){
            PopOff();
        }
    }
    return bRet;
}

BOOL ModifyScenicAreaNodeSubMenu(void) {
    BOOL bRet = TRUE;
    char *plabel_name[] = {"���ݾ��������޸�",
                           "���ݾ�  ��ID�޸�",
                           "ȡ��"
    };
    int n = 3;
    int iHot = PopChoiceMenu(plabel_name, n);
    if (iHot == -1) {
        PopOff();
    }
    else {
        if (iHot == 1) {
            PopOff();
        }
        else if (iHot == 2){
            PopOff();
        }
        else if (iHot == 3) {
            PopOff();
        }
        else {
            PopOff();
        }
    }
    return bRet;
}

BOOL DeleteScenicAreaNodeSubMenu(void) {
    BOOL bRet = TRUE;
    char *plabel_name[] = {"���ݾ�������ɾ��",
                           "���ݾ�  ��IDɾ��",
                           "ȡ��"
    };
    int n = 3;
    int iHot = PopChoiceMenu(plabel_name, n);
    if (iHot == -1) {
        PopOff();
    }
    else {
        if (iHot == 1) {
            PopOff();
        }
        else if (iHot == 2){
            PopOff();
            char *plabel[] = {"�������ɾ���ľ���ID��Ϣ",
                              "��  ��ID",
                              "ȷ��    ȡ��"
            };
            int n = 3;
            int inputNum = 3;
            char *ppcondition[inputNum];
            int aHot = PopInputMenu(plabel, n, ppcondition, inputNum);
            if(aHot==2) {
                PopOff();
                REGION_NODE *pRegionNode = SeekRegionNodeByID(gp_head2, ppcondition[0]);
                if(pRegionNode==NULL) {
                    char *plabel_name[] = {"��ɾ���ľ��������ڣ�",
                                           "ȷ��"
                    };
                    ShowModule(plabel_name, 2);
                }
                else {
                    BOOL del = delete_region(gp_head2,ppcondition[0]);
                    if(del){
                        char *error_plabel_name[] = {"ɾ���ɹ���",
                                                     "ȷ��"
                        };
                        ShowModule(error_plabel_name, 2);
                        return bRet;
                    }
                    else {
                        char *error_plabel_name[] = {"ɾ��ʧ�ܣ�",
                                                     "ȷ��"
                        };
                        ShowModule(error_plabel_name, 2);
                        return bRet;
                    }
                }
            }
            else{
                PopOff();
            }
        }
        else if (iHot == 3) {
            PopOff();
        }
        else {
            PopOff();
        }
    }
    return bRet;
}

BOOL MaintainAttractionInfo(void) {
    BOOL bRet = TRUE;
    char *plabel_name[] = {"¼�뾰����Ϣ",
                           "�޸ľ�����Ϣ",
                           "ɾ��������Ϣ",
                           "ȡ��"
    };

    int iHot = PopChoiceMenu(plabel_name, 4);

    if (iHot == -1) {
        PopOff();
    }
    else {
        if (iHot == 1){
            PopOff();
            InsertAttractionNodeSubMenu();

        }
        else if (iHot == 2) {
            PopOff();
            ModifyAttractionNodeSubMenu();
        }
        else if (iHot == 3) {
            PopOff();
            DeleteAttractionNodeSubMenu();
        }
        else {
            PopOff();
        }
    }
    return bRet;
}

BOOL InsertAttractionNodeSubMenu(void) {
    BOOL bRet = TRUE;
    char *plabel_name[] = {"�������¼��ľ�����Ϣ",
                           "��������",
                           "��  ��ID",
                           "��  ��ID",
                           "����λ��",
                           "���ʱ��",
                           "�����ص�",
                           "ȷ��    ȡ��"
    };
    int n = 8;
    int inputNum = 8;
    char *ppcondition[inputNum];
    int iHot = PopInputMenu(plabel_name, n, ppcondition, inputNum );
    if (iHot == -1) {
        PopOff();
    }
    else {
        if (iHot == 7) {
            PopOff();
            if((strlen(ppcondition[0])==0) || (strlen(ppcondition[1])==0)) {
                char *error_plabel_name[] = {"������д�Ϸ��ľ���ID�;�������",
                                             "ȷ��"
                };
                ShowModule(error_plabel_name, 2);
                return FALSE;
            }
            else {
                REGION_NODE *cityFound = SeekRegionNodeByID(gp_head2, ppcondition[2]);
                if(cityFound==NULL) {
                    char *error_plabel_name[] = {"����Ҫ����ľ���Ĺ������������ڣ�",
                                                 "ȷ��"
                    };
                    ShowModule(error_plabel_name, 2);
                    return FALSE;
                }
                SPOT_NODE *spotFound = SeekSpotNodeByID(gp_head2, ppcondition[1]);

                if(spotFound!=NULL) {
                    char *error_plabel_name[] = {"����Ҫ����ľ�����Ϣ�Ѵ��ڣ�����ɾ�������޸ģ�",
                                                 "ȷ��"
                    };
                    ShowModule(error_plabel_name, 2);
                    return FALSE;
                }
                else{
                    SPOT_NODE *pSpotNode = (SPOT_NODE *)malloc(sizeof(SPOT_NODE));
                    pSpotNode->next = NULL;
                    strcpy(pSpotNode->name, ppcondition[0]);
                    strcpy(pSpotNode->spot_id, ppcondition[1]);
                    strcpy(pSpotNode->region_id, ppcondition[2]);
                    strcpy(pSpotNode->address, ppcondition[3]);
                    strcpy(pSpotNode->opentime, ppcondition[4]);
                    strcpy(pSpotNode->feature, ppcondition[5]);
                    ConfirmSpotInsertion(gp_head2, pSpotNode);
                    char *error_plabel_name[] = {"������ӳɹ���",
                                                 "ȷ��"
                    };
                    ShowModule(error_plabel_name, 2);
                    return bRet;
                }

            }
        }
        else if (iHot == 8){
            PopOff();
        }
    }
    return bRet;
}

BOOL ModifyAttractionNodeSubMenu(void) {
    BOOL bRet = TRUE;
    char *plabel_name[] = {"���ݾ��������޸�",
                           "���ݾ�  ��ID�޸�",
                           "ȡ��"
    };
    int n = 3;
    int iHot = PopChoiceMenu(plabel_name, n);
    if (iHot == -1) {
        PopOff();
    }
    else {
        if (iHot == 1) {
            PopOff();
        }
        else if (iHot == 2){
            PopOff();
        }
        else if (iHot == 3) {
            PopOff();
        }
        else {
            PopOff();
        }
    }
    return bRet;
}

BOOL DeleteAttractionNodeSubMenu(void) {
    BOOL bRet = TRUE;
    char *plabel_name[] = {"���ݾ�������ɾ��",
                           "���ݾ�  ��IDɾ��",
                           "ȡ��"
    };
    int n = 3;
    int iHot = PopChoiceMenu(plabel_name, n);
    if (iHot == -1) {
        PopOff();
    }
    else {
        if (iHot == 1) {
            PopOff();
        }
        else if (iHot == 2){
            PopOff();
            char *plabel[] = {"�������ɾ���ľ���ID��Ϣ",
                              "��  ��ID",
                              "ȷ��    ȡ��"
            };
            int n = 3;
            int inputNum = 3;
            char *ppcondition[inputNum];
            int aHot = PopInputMenu(plabel, n, ppcondition, inputNum);
            if(aHot==2) {
                PopOff();
                SPOT_NODE *pSpotNode = SeekSpotNodeByID(gp_head2, ppcondition[0]);
                if(pSpotNode==NULL) {
                    char *plabel_name[] = {"��ɾ���ľ��㲻���ڣ�",
                                           "ȷ��"
                    };
                    ShowModule(plabel_name, 2);
                }
                else {
                    BOOL del = delete_spot(&gp_head2,ppcondition[0]);
                    if(del){
                        char *error_plabel_name[] = {"ɾ���ɹ���",
                                                     "ȷ��"
                        };
                        ShowModule(error_plabel_name, 2);
                        return bRet;
                    }
                    else {
                        char *error_plabel_name[] = {"ɾ��ʧ�ܣ�",
                                                     "ȷ��"
                        };
                        ShowModule(error_plabel_name, 2);
                        return bRet;
                    }
                }
            }
            else{
                PopOff();
            }
        }
        else if (iHot == 3) {
            PopOff();
        }
        else {
            PopOff();
        }
    }
    return bRet;
}

BOOL QueryCityInfo(void)
{
    BOOL bRet = TRUE;
    char *plabel_name[] = {"���ݳ������Ʋ�ѯ",
                           "���ݳ�  ��ID��ѯ",
                           "ȡ��"
    };
    int n = 3;
    int iHot = PopChoiceMenu(plabel_name, n);
    if (iHot == -1) {
        PopOff();
    }
    else {
        if (iHot == 1) {
            PopOff();
            char *plabel[] = {"���������ѯ�ĳ���������Ϣ",
                              "��������",
                              "ȷ��    ȡ��"
            };
            int n = 3;
            int inputNum = 3;
            char *ppcondition[inputNum];
            int aHot = PopInputMenu(plabel, n, ppcondition, inputNum);
            if(aHot==2) {
                PopOff();
                CITY_NODE *pCityNode = SeekCityNodeByName(gp_head2, ppcondition[0]);
                if(pCityNode==NULL) {
                    char *plabel_name[] = {"���ѯ�ĳ��в����ڣ�",
                                           "ȷ��"
                    };
                    ShowModule(plabel_name, 2);
                }
                else {
                    //�����ѯ�Ľ������Ŀ
                    CITY_NODE *tmp = pCityNode;
                    int num = 0;
                    while(tmp!=NULL){
                        num++;
                        tmp = tmp->next;
                    }

                    char *plabel_ret[2*num+4];
                    tmp = pCityNode;
                    int i = 0;
                    plabel_ret[i++] = "���ư������³��У�";
                    plabel_ret[i++] = "��  ��ID";
                    plabel_ret[i++] = "��������";
                    while(tmp!=NULL){
                        plabel_ret[i++] = tmp->city_id;
                        plabel_ret[i++] = tmp->name;
                        tmp = tmp->next;
                    }
                    plabel_ret[i] = "ȷ��";
                    int bHot = ShowSearchResult(plabel_ret, 2*num+4, 2);
                    int k =0;
                    for(k=0;k<bHot-1;k++){
                        pCityNode = pCityNode->next;
                    }
                    char *plabel_ret2[2*4+2];
                    plabel_ret2[0] = "���ѯ�ĳ�����Ϣ";
                    plabel_ret2[1] = "��������:";
                    plabel_ret2[2] = pCityNode->name;
                    plabel_ret2[3] = "��  ��ID";
                    plabel_ret2[4] = pCityNode->city_id;
                    plabel_ret2[5] = "�ල�绰";
                    plabel_ret2[6] = pCityNode->jiandu_num;
                    plabel_ret2[7] = "��ѯ�绰";
                    plabel_ret2[8] = pCityNode->zixun_num;
                    plabel_ret2[9] = "ȷ��";
                    ShowResult(plabel_ret2, 10, 2);
                }

            }
            else{
                PopOff();
            }
        }
        else if (iHot == 2){
            PopOff();
            char *plabel[] = {"���������ѯ�ĳ���ID��Ϣ",
                              "��  ��ID",
                              "ȷ��    ȡ��"
            };
            int n = 3;
            int inputNum = 3;
            char *ppcondition[inputNum];
            int aHot = PopInputMenu(plabel, n, ppcondition, inputNum);
            if(aHot==2) {
                PopOff();
                CITY_NODE *pCityNode = SeekCityNodeByID(gp_head2, ppcondition[0]);
                if(pCityNode==NULL) {
                    char *plabel_name[] = {"���ѯ�ĳ��в����ڣ�",
                                           "ȷ��"
                    };
                    ShowModule(plabel_name, 2);
                }
                else {
                    char *plabel_ret[2*4+2];
                    plabel_ret[0] = "���ѯ�ĳ�����Ϣ";
                    plabel_ret[1] = "��������:";
                    plabel_ret[2] = pCityNode->name;
                    plabel_ret[3] = "��  ��ID";
                    plabel_ret[4] = pCityNode->city_id;
                    plabel_ret[5] = "�ල�绰";
                    plabel_ret[6] = pCityNode->jiandu_num;
                    plabel_ret[7] = "��ѯ�绰";
                    plabel_ret[8] = pCityNode->zixun_num;
                    plabel_ret[9] = "ȷ��";
                    ShowResult(plabel_ret, 10, 2);
                }

            }
            else{
                PopOff();
            }

        }
        else if (iHot == 3) {
            PopOff();

        }
        else {
            PopOff();
        }
    }
    return bRet;
}

BOOL QueryScenicAreaInfo(void)
{
    BOOL bRet = TRUE;
    char *plabel_name[] = {"���ݾ������Ʋ�ѯ",
                           "���ݾ�  ��ID��ѯ",
                           "ȡ��"
    };
    int n = 3;
    int iHot = PopChoiceMenu(plabel_name, n);
    if (iHot == -1) {
        PopOff();
    }
    else {
        if (iHot == 1) {
            PopOff();
        }
        else if (iHot == 2){
            PopOff();
            char *plabel[] = {"���������ѯ�ľ���ID��Ϣ",
                              "��  ��ID",
                              "ȷ��    ȡ��"
            };
            int n = 3;
            int inputNum = 3;
            char *ppcondition[inputNum];
            int aHot = PopInputMenu(plabel, n, ppcondition, inputNum);
            if(aHot==2) {
                PopOff();
                REGION_NODE *pRegionNode = SeekRegionNodeByID(gp_head2, ppcondition[0]);
                if(pRegionNode==NULL) {
                    char *plabel_name[] = {"���ѯ�ľ��������ڣ�",
                                           "ȷ��"
                    };
                    ShowModule(plabel_name, 2);
                }
                else {
                    char *plabel_ret[7*2+2];
                    plabel_ret[0] = "���ѯ�ľ�����Ϣ";
                    plabel_ret[1] = "��������";
                    plabel_ret[2] = pRegionNode->name;
                    plabel_ret[3] = "��  ��ID";
                    plabel_ret[4] = pRegionNode->region_id;
                    plabel_ret[5] = "��  ��ID";
                    plabel_ret[6] = pRegionNode->city_id;
                    plabel_ret[7] = "��������";
                    plabel_ret[8] = pRegionNode->level;
                    plabel_ret[9] = "������ַ";
                    plabel_ret[10] = pRegionNode->address;
                    plabel_ret[11] = "��Ʊ�۸�";
                    plabel_ret[12] = pRegionNode->price;
                    plabel_ret[13] = "����ʱ��";
                    plabel_ret[14] = pRegionNode->opentime;
                    plabel_ret[15] = "ȷ��";
                    ShowResult(plabel_ret, 7*2+2, 2);
                }

            }
            else{
                PopOff();
            }
        }
        else if (iHot == 3) {
            PopOff();
        }
        else {
            PopOff();
        }
    }
    return bRet;
}

BOOL RandomView(void)
{
    BOOL bRet = TRUE;
    char *plabel_name[] = {"��㿴������",
                           "��㿴������",
                           "��㿴������",
                           "ȡ��"
    };
    int n = 4;
    int iHot = PopChoiceMenu(plabel_name, n);
    if (iHot == -1) {
        PopOff();
    }
    else {
        if (iHot == 1) {
            PopOff();
        }
        else if (iHot == 2){
            PopOff();
        }
        else if (iHot == 3) {
            PopOff();
        }
        else {
            PopOff();
        }
    }
    return bRet;
}

BOOL StatusCityInfo(void)
{
    BOOL bRet = TRUE;
    char *plabel_name[] = {"��㿴������",
                           "��㿴������",
                           "��㿴������",
                           "ȡ��"
    };
    int n = 4;
    int iHot = PopChoiceMenu(plabel_name, n);
    if (iHot == -1) {
        PopOff();
    }
    else {
        if (iHot == 1) {
            PopOff();
        }
        else if (iHot == 2){
            PopOff();
        }
        else if (iHot == 3) {
            PopOff();
        }
        else {
            PopOff();
        }
    }
    return bRet;
}

BOOL QueryAttractionInfo(void)
{
    BOOL bRet = TRUE;
    char *plabel_name[] = {"���ݾ������Ʋ���",
                           "���ݾ�  ��ID����",
                           "ȡ��"
    };
    int n = 3;
    int iHot = PopChoiceMenu(plabel_name, n);
    if (iHot == -1) {
        PopOff();
    }
    else {
        if (iHot == 1) {
            PopOff();
        }
        else if (iHot == 2){
            PopOff();
            char *plabel[] = {"���������ѯ�ľ���ID��Ϣ",
                              "��  ��ID",
                              "ȷ��    ȡ��"
            };
            int n = 3;
            int inputNum = 3;
            char *ppcondition[inputNum];
            int aHot = PopInputMenu(plabel, n, ppcondition, inputNum);
            if(aHot==2) {
                PopOff();
                SPOT_NODE *pSpotNode = SeekSpotNodeByID(gp_head2, ppcondition[0]);
                if(pSpotNode==NULL) {
                    char *plabel_name[] = {"���ѯ�ľ��㲻���ڣ�",
                                           "ȷ��"
                    };
                    ShowModule(plabel_name, 2);
                }
                else {
                    char *plabel_ret[6*2+2];
                    plabel_ret[0] = "���ѯ�ľ�����Ϣ";
                    plabel_ret[1] = "��������";
                    plabel_ret[2] = pSpotNode->name;
                    plabel_ret[3] = "��  ��ID";
                    plabel_ret[4] = pSpotNode->spot_id;
                    plabel_ret[5] = "��  ��ID";
                    plabel_ret[6] = pSpotNode->region_id;
                    plabel_ret[7] = "����λ��";
                    plabel_ret[8] = pSpotNode->address;
                    plabel_ret[9] = "���ʱ��";
                    plabel_ret[10] = pSpotNode->opentime;
                    plabel_ret[11] = "�����ص�";
                    plabel_ret[12] = pSpotNode->feature;
                    plabel_ret[13] = "ȷ��";
                    ShowResult(plabel_ret, 6*2+2, 2);
                }

            }
            else{
                PopOff();
            }
        }
        else if (iHot == 3) {
            PopOff();
        }
        else {
            PopOff();
        }
    }
    return bRet;
}

BOOL HelpTopic(void)
{
    BOOL bRet = TRUE;
    char *plabel_name[] = {"���˵������",
                           "�Ӳ˵����������",
                           "ȷ��"
    };

    ShowModule(plabel_name, 3);

    return bRet;
}

BOOL AboutDorm(void)
{
    BOOL bRet = TRUE;
    char *plabel_name[] = {"���˵������",
                           "�Ӳ˵������...",
                           "ȷ��"
    };

    ShowModule(plabel_name, 3);

    return bRet;
}


BOOL SaveSysData2(CITY_NODE *hd)
{
    CITY_NODE *pCityNode = NULL;
    REGION_NODE *pRegionNode = NULL;
    SPOT_NODE *pSpotNode = NULL;
    FILE *pFile;

    pFile = fopen(gp_city_info_filename, "wb");
    for(pCityNode=hd;pCityNode!=NULL;pCityNode=pCityNode->next) {
        fwrite(pCityNode, sizeof(CITY_NODE),1, pFile);
    }
    fclose(pFile);

    pFile = fopen(gp_region_info_filename, "wb");
    for(pCityNode=hd;pCityNode!=NULL;pCityNode=pCityNode->next) {
        pRegionNode = pCityNode->rnext;
        while(pRegionNode!=NULL) {
            fwrite(pRegionNode, sizeof(REGION_NODE), 1, pFile);
            pRegionNode = pRegionNode->next;
        }
    }
    fclose(pFile);

    pFile = fopen(gp_spot_info_filename, "wb");
    for(pCityNode=hd;pCityNode!=NULL;pCityNode=pCityNode->next) {
        for(pRegionNode = pCityNode->rnext;pRegionNode!=NULL;pRegionNode=pRegionNode->next) {
            pSpotNode = pRegionNode->snext;
            while(pSpotNode!=NULL){
                fwrite(pSpotNode, sizeof(SPOT_NODE), 1, pFile);
                pSpotNode = pSpotNode->next;
            }
        }
    }
    fclose(pFile);

    return TRUE;
}

/**
 * ��������: BackupSysData
 * ��������: ��ϵͳ����������������ݱ��ݵ�һ�������ļ�.
 * �������: hd ����ͷ���ָ��
 *           filename �����ļ���
 * �������:
 * �� �� ֵ: BOOL����, ����ΪTRUE
 *
 * ����˵��:
 */
BOOL BackupSysData(CITY_NODE *hd, char *filename)
{

    CITY_NODE *pcity_node;
    REGION_NODE *pregion_node;
    SPOT_NODE *pspot_node;
    unsigned long type_city_num = 0;
    unsigned long region_node_num = 0;
    unsigned long pspot_node_num = 0;
    int handle;
    /*����ʮ�������ֱ�ͳ�����ֻ���������Ϣ�ļ�¼����*/
    for (pcity_node=hd; pcity_node!=NULL; pcity_node=pcity_node->next)
    {
        type_city_num++;
        for (pregion_node = pcity_node->rnext; pregion_node!=NULL; pregion_node=pregion_node->next)
        {
            region_node_num++;
            for (pspot_node = pregion_node->snext; pspot_node!=NULL; pspot_node=pspot_node->next)
            {
                pspot_node_num++;
            }
        }
    }

    if((handle=open(filename, O_WRONLY|O_BINARY))==-1)
    {
        handle=open(filename, O_CREAT|O_BINARY|O_WRONLY, S_IWRITE);
    }

    /*��������������ݵļ�¼����*/
    write(handle, (char*)&type_city_num, sizeof(type_city_num));
    write(handle, (char*)&region_node_num, sizeof(region_node_num));
    write(handle, (char*)&pspot_node_num, sizeof(pspot_node_num));

    /*�����װ������Ϣ*/
    for (pcity_node=hd; pcity_node!=NULL; pcity_node=pcity_node->next)
    {
        write(handle, (char*)pcity_node, sizeof(CITY_NODE));
    }

    for (pcity_node=hd; pcity_node!=NULL; pcity_node=pcity_node->next)
    {
        /*�����װ������Ϣ*/
        pregion_node = pcity_node->rnext;
        while (pregion_node != NULL)
        {
            write(handle, (char*)pregion_node, sizeof(REGION_NODE));
            pregion_node = pregion_node->next;
        }
    }

    /*�����װ������Ϣ*/
    for (pcity_node=hd; pcity_node!=NULL; pcity_node=pcity_node->next)
    {
        for(pregion_node=pcity_node->rnext; pregion_node!=NULL; pregion_node=pregion_node->next)
        {
            pspot_node = pregion_node->snext;
            while (pspot_node != NULL)
            {
                write(handle, (char*)pspot_node, sizeof(SPOT_NODE));
                pspot_node = pspot_node->next;
            }
        }
    }
    close(handle);
    return TRUE;
}

/**
 * ��������: RestoreSysData
 * ��������: ��ָ�������ļ��лָ�ϵͳ�����������������.
 * �������: phead ����ͷ���ָ��ĵ�ַ
 *           filename ��ű������ݵ������ļ���
 * �������:
 * �� �� ֵ: BOOL����, ����ΪTRUE
 *
 * ����˵��:
 */
BOOL RestoreSysData(CITY_NODE **phead, char *filename)
{
    CITY_NODE *hd = NULL;
    CITY_NODE *pcity_node;
    REGION_NODE *pregion_node;
    SPOT_NODE *pspot_node;
    unsigned long city_node_num = 0;
    unsigned long region_node_num = 0;
    unsigned long spot_node_num = 0;
    unsigned long ulloop;
    int handle;
    int find;

    if ((handle=open(filename, O_RDONLY|O_BINARY))==-1)
    {
        /*������ļ������ڣ��򵯳���ʾ���ڲ�����FALSE*/
        char *plabel_name[] = {"��������ļ������ڣ�","ȷ��"};
        ShowModule(plabel_name, 2);
        return FALSE;
    }
    else
    {
        /*��ȡ���ֻ���������Ϣ�ļ�¼��*/
        read(handle, (char*)&city_node_num, sizeof(city_node_num));
        read(handle, (char*)&region_node_num, sizeof(region_node_num));
        read(handle, (char*)&spot_node_num, sizeof(spot_node_num));

        /*��ȡ������Ϣ������ʮ��������*/
        for (ulloop=1; ulloop<=city_node_num; ulloop++)
        {
            pcity_node = (CITY_NODE*)malloc(sizeof(CITY_NODE));
            read(handle, (char*)pcity_node, sizeof(CITY_NODE));
            pcity_node->rnext = NULL;
            pcity_node->next = hd;
            hd = pcity_node;
        }
        *phead = hd;

        for (ulloop=1; ulloop<=region_node_num; ulloop++)
        {
            pregion_node = (REGION_NODE*)malloc(sizeof(REGION_NODE));
            read(handle, (char*)pregion_node, sizeof(REGION_NODE));
            pregion_node->snext = NULL;
            pcity_node = hd;
            while (pcity_node != NULL
                   && pcity_node->city_id != pregion_node->city_id)
            {
                pcity_node = pcity_node->next;
            }

            if (pcity_node != NULL)
            {
                pregion_node->next = pcity_node->rnext;
                pcity_node->rnext = pregion_node;
            }
            else
            {
                free(pregion_node);
            }
        }

        /*��ȡ������Ϣ������������Ϣ֧��*/
        for (ulloop=1; ulloop<=spot_node_num; ulloop++)
        {
            pspot_node = (SPOT_NODE*)malloc(sizeof(SPOT_NODE));
            read(handle, (char*)pspot_node, sizeof(SPOT_NODE));
            pcity_node = hd;
            find = 0;

            while (pcity_node!=NULL && find==0)
            {
                pregion_node = pcity_node->rnext;
                while(pregion_node!=NULL && find==0)
                {
                    if(strcmp(pregion_node->region_id, pspot_node->region_id) == 0)
                    {
                        find = 1;
                        break;
                    }
                    pregion_node = pregion_node->next;
                }
                pcity_node = pcity_node->next;
            }

            if (find)
            {
                pspot_node->next = pregion_node->snext;
                pregion_node->snext = pspot_node;
            }
            else
            {
                free(pspot_node);
            }
        }
        close(handle);
        SaveSysData2(hd);  /*���ڴ������ݱ��浽�����ļ�*/
        return TRUE;
    }
}

BOOL ShowModule(char **pString, int n)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot = 1;
    int i, maxlen, str_len;

    for (i=0,maxlen=0; i<n; i++)
    {
        str_len = strlen(pString[i]);
        if (maxlen < str_len)
        {
            maxlen = str_len;
        }
    }

    pos.X = maxlen + 6;
    pos.Y = n + 5;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN ;  /*��������������׺���*/
    labels.num = n;
    labels.ppLabel = pString;
    COORD aLoc[n];

    for (i=0; i<n; i++)
    {
        aLoc[i].X = rcPop.Left + 3;
        aLoc[i].Y = rcPop.Top + 2 + i;

    }
    str_len = strlen(pString[n-1]);
    aLoc[n-1].X = rcPop.Left + 3 + (maxlen-str_len)/2;
    aLoc[n-1].Y = aLoc[n-1].Y + 2;

    labels.pLoc = aLoc;

    areas.num = 1;
    SMALL_RECT aArea[] = {{
                                  aLoc[n-1].X, aLoc[n-1].Y,
                                  aLoc[n-1].X + 3, aLoc[n-1].Y
                          }
    };

    char aSort[] = {0};
    char aTag[] = {1};

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 + n;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);

    DealInput2(&areas, &iHot, NULL);

    PopOff();

    return bRet;

}


// buliuzi

BOOL add_city(CITY_NODE **head, CITY_NODE *pcity_node) {
    CITY_NODE *city = *head;
    if(city == NULL) {
        (*head) = pcity_node;
        return TRUE;;
    }
    CITY_NODE *rCity = SeekCityNodeByID(city, pcity_node->city_id);
    if (rCity != NULL) {
        return FALSE;
    }
    else {
        while (city->next != NULL){
            city = city->next;
        }
        if(city->next == NULL) {
            city->next = pcity_node;
            return TRUE;
        }
    }

}

CITY_NODE *SeekCityNodeByID(CITY_NODE *hd, char *id)
{
    CITY_NODE *pcity_node;
    int find = 0;

    pcity_node = hd;
    while(pcity_node != NULL)
    {
        if (strcmp(pcity_node->city_id, id) == 0)
        {
            find = 1;
            break;
        }
        pcity_node = pcity_node->next;
    }
    if (find)
        return pcity_node;
    else
        return NULL;
}

// ���ز�ѯ���������
CITY_NODE *SeekCityNodeByName(CITY_NODE *hd, char *name)
{
    CITY_NODE *pcity_node;
    CITY_NODE *city_find=NULL;

    pcity_node = hd;
    while(pcity_node != NULL)
    {
        if (strcmp(pcity_node->name, name) == 0)
        {
            CITY_NODE *tmp = (CITY_NODE *)malloc(sizeof(CITY_NODE));
            strcpy(tmp->city_id, pcity_node->city_id);
            strcpy(tmp->name, pcity_node->name);
            strcpy(tmp->jiandu_num, pcity_node->jiandu_num);
            strcpy(tmp->zixun_num, pcity_node->zixun_num);
            tmp->next = city_find;
            tmp->rnext = NULL;
            city_find = tmp;
        }
        pcity_node = pcity_node->next;
    }
    return city_find;
}

BOOL ConfirmCityInsertion(CITY_NODE **head, CITY_NODE *pcity_node)
{
    CITY_NODE *city = *head;
    if(city == NULL) {
        (*head) = pcity_node;
        return TRUE;;
    }
    CITY_NODE *rCity = SeekCityNodeByID(city, pcity_node->city_id);
    if (rCity != NULL) {
        return FALSE;
    }
    else {
        while (city->next != NULL){
            city = city->next;
        }
        if(city->next == NULL) {
            city->next = pcity_node;
            return TRUE;
        }
    }
}

BOOL add_region(CITY_NODE *head, REGION_NODE *pregion_node)
{
    CITY_NODE *pcity_node = head;
    REGION_NODE *region_node = NULL;

    if (pcity_node -> rnext == NULL)
    {
        pcity_node -> rnext = pregion_node;
        return TRUE;
    }
    else
    {
        region_node = pcity_node -> rnext;
        while (region_node -> next != NULL)
        {
            region_node = region_node -> next;
        }
        if (region_node -> next == NULL)
        {
            region_node -> next = pregion_node;
            pregion_node -> snext = NULL;
            return TRUE;
        }
    }

    return FALSE;
}

REGION_NODE *SeekRegionNodeByName(CITY_NODE *hd, char *name)
{
    CITY_NODE *pCityNode = NULL;
    REGION_NODE *pRegionNode = NULL;
    REGION_NODE *regionFind = NULL;
    for(pCityNode=hd;pCityNode!=NULL;pCityNode=pCityNode->next) {
        for(pRegionNode = pCityNode->rnext;pRegionNode!=NULL;pRegionNode=pRegionNode->next) {
            if(strcmp(pRegionNode->name, name)==0){
                REGION_NODE *tmp = (REGION_NODE*)malloc(sizeof(REGION_NODE));
                strcpy(tmp->city_id, pRegionNode->city_id);
                strcpy(tmp->name, pRegionNode->name);
                strcpy(tmp->region_id, pRegionNode->region_id);
                strcpy(tmp->level, pRegionNode->level);
                strcpy(tmp->address, pRegionNode->address);
                strcpy(tmp->price, pRegionNode->price);
                strcpy(tmp->opentime, pRegionNode->opentime);
                tmp->snext = NULL;
                tmp->next = regionFind;
                regionFind = tmp;
            }
        }
    }
    return regionFind;
}

REGION_NODE *SeekRegionNodeByID(CITY_NODE *hd, char *id)
{
    CITY_NODE *pCityNode = NULL;
    REGION_NODE *pRegionNode = NULL, *tmp = NULL;
    SPOT_NODE *pSpotNode = NULL;
    int find = 0;
    for(pCityNode=hd;pCityNode!=NULL;pCityNode=pCityNode->next) {
        for(pRegionNode = pCityNode->rnext;pRegionNode!=NULL;pRegionNode=pRegionNode->next) {
            if(strcmp(pRegionNode->region_id, id)==0){
                tmp = pRegionNode;
                find = 1;
                break;
            }
        }
        if(find==1){
            break;
        }
    }
    return tmp;

}

BOOL ConfirmRegionInsertion(CITY_NODE *pcity_node, REGION_NODE *pregion_node)
{
    CITY_NODE *city = SeekCityNodeByID(pcity_node, pregion_node->city_id);
    BOOL flag = add_region(city, pregion_node);
    return flag;
}

BOOL add_spot(REGION_NODE *pregion_node, SPOT_NODE *pspot_node)
{
    REGION_NODE *region_node = pregion_node;
    SPOT_NODE *spot_node = NULL;
    if (region_node->snext == NULL)
    {
        region_node->snext = pspot_node;
        return TRUE;
    }
    else
    {
        spot_node = region_node->snext;
        while (spot_node->next != NULL)
        {
            spot_node = spot_node->next;
        }
        if (spot_node->next == NULL)
        {
            spot_node->next = pspot_node;
            return TRUE;
        }
    }
    return FALSE;
}

SPOT_NODE * SeekSpotNodeByID(CITY_NODE *hd, char *id){
    CITY_NODE *pCityNode = NULL;
    REGION_NODE *pRegionNode = NULL;
    SPOT_NODE *pSpotNode = NULL, *tmp=NULL;
    int find = 0;
    for(pCityNode=hd;pCityNode!=NULL;pCityNode=pCityNode->next) {
        for(pRegionNode = pCityNode->rnext;pRegionNode!=NULL;pRegionNode=pRegionNode->next) {
            pSpotNode = pRegionNode->snext;
            while(pSpotNode!=NULL){
                if(strcmp(pSpotNode->spot_id, id)==0) {
                    tmp = pSpotNode;
                    find = 1;
                    break;
                }
                pSpotNode = pSpotNode->next;
            }
            if(find==1){
                break;
            }
        }
        if(find==1){
            break;
        }
    }
    return tmp;
}

SPOT_NODE * SeekSpotNodeByName(CITY_NODE *hd, char *name){
    CITY_NODE *pCityNode = NULL;
    REGION_NODE *pRegionNode = NULL;
    SPOT_NODE *pSpotNode = NULL;
    SPOT_NODE *pSpotFound = NULL;
    for(pCityNode=hd;pCityNode!=NULL;pCityNode=pCityNode->next) {
        for(pRegionNode = pCityNode->rnext;pRegionNode!=NULL;pRegionNode=pRegionNode->next) {
            pSpotNode = pRegionNode->snext;
            while(pSpotNode!=NULL){
                if(strcmp(pSpotNode->name, name)==0) {
                    SPOT_NODE *tmp = (SPOT_NODE*)malloc(sizeof(SPOT_NODE));
                    strcpy(tmp->region_id, pSpotNode->region_id);
                    strcpy(tmp->name, pSpotNode->name);
                    strcpy(tmp->spot_id, pSpotNode->spot_id);
                    strcpy(tmp->address, pSpotNode->address);
                    strcpy(tmp->opentime, pSpotNode->opentime);
                    strcpy(tmp->feature, pSpotNode->feature);
                    tmp->next = pSpotFound;
                    pSpotFound = tmp;
                }
                pSpotNode = pSpotNode->next;
            }

        }
    }
    return pSpotFound;
}

BOOL ConfirmSpotInsertion(CITY_NODE *pcity_node, SPOT_NODE *pspot_node)
{
    REGION_NODE *pregion_node = SeekRegionNodeByID(pcity_node, pspot_node->region_id);
    BOOL flag = add_spot(pregion_node, pspot_node);
    return flag;
}

/*ɾ��������Ϣ*/
BOOL delete_city(CITY_NODE **head, char *id)
{
    CITY_NODE *pCityNodeprior=NULL,*pCityNodeCur=NULL;
    pCityNodeprior = NULL;
    pCityNodeCur = *head;
    while(pCityNodeCur!=NULL){
        if(strcmp(pCityNodeCur->city_id, id)!=0){
            pCityNodeprior = pCityNodeCur;
            pCityNodeCur = pCityNodeCur->next;
        }
        else {
            break;
        }
    }

    if(pCityNodeCur==NULL){
        return FALSE;
    }
    else {
        if(pCityNodeprior==NULL){
            *head = pCityNodeCur->next;
        }
        else {
            pCityNodeprior->next = pCityNodeCur->next;
        }

        // �ͷź��������ڵ�
        REGION_NODE *pRegionNode = pCityNodeCur->rnext;
        REGION_NODE *pRegionNodeCur = NULL;
        while(pRegionNode!=NULL){
            pRegionNodeCur = pRegionNode;
            pRegionNode = pRegionNode->next;

            SPOT_NODE *pSpotNode = pRegionNodeCur->snext;
            SPOT_NODE *pSpotNodeCur = NULL;
            while(pSpotNode!=NULL){
                pSpotNodeCur = pSpotNode;
                pSpotNode = pSpotNode->next;
                free(pSpotNodeCur);
            }
            free(pRegionNodeCur);
        }
        free(pCityNodeCur);
        return TRUE;
    }
    return  FALSE;
}

BOOL delete_region(CITY_NODE *head, char *id)
{
    CITY_NODE *pCityNode = NULL;
    REGION_NODE *pRegionNodePrior = NULL;
    REGION_NODE *pRegionNodeCur = NULL;
    int find = 0;
    for(pCityNode=head;pCityNode!=NULL;pCityNode=pCityNode->next){
        pRegionNodeCur = pCityNode->rnext;
        pRegionNodePrior = NULL;
        while(pRegionNodeCur!=NULL){
            if(strcmp(pRegionNodeCur->region_id, id)!=0){
                pRegionNodePrior = pRegionNodeCur;
                pRegionNodeCur = pRegionNodeCur->next;
            }
            else {
                find = 1;
                break;
            }
        }
        if(find==1){
            break;
        }
    }
    if(find==1){
        if(pRegionNodePrior==NULL){
            pCityNode->rnext = pRegionNodeCur->next;
        }
        else{
            pRegionNodePrior->next = pRegionNodeCur->next;
            SPOT_NODE *pSpotNode = pRegionNodeCur->snext;
            SPOT_NODE *pSpotNodeCur = NULL;
            while(pSpotNode!=NULL){
                pSpotNodeCur = pSpotNode;
                pSpotNode = pSpotNode->next;
                free(pSpotNodeCur);
            }
        }
        free(pRegionNodeCur);
        return TRUE;
    }
    return FALSE;
}

BOOL delete_spot(CITY_NODE **head, char *id)
{
    CITY_NODE *cp = *head;
    REGION_NODE *qp;
    SPOT_NODE *dp, *prior;
    while (cp != NULL)                       /*����Ҫɾ���ľ���ڵ�*/
    {
        qp = cp->rnext;
        while (qp != NULL)
        {
            dp = qp->snext;
            while (dp != NULL)
            {
                if (!strcmp(dp->spot_id, id))
                {
                    if (dp == qp->snext)
                        qp->snext = dp->next;
                    else
                        prior->next = dp->next;
                    free(dp);                      /*ɾ���ҵ��Ļ��ܽڵ�*/
                    return TRUE;
                }
                prior = dp;
                dp = dp->next;
            }
            qp = qp->next;
        }
        cp = cp->next;
    }
    return FALSE;
}




