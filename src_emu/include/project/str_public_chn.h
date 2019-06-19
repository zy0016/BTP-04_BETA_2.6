/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/

static const unsigned char *strChn[]={
	/*STR_PUBLIC_NULL */
	(unsigned char *) "",	/* 0 */
	/*STR_PUBLIC_EMPTY_NUM */
	(unsigned char *) "空号码!",
	/*STR_PUBLIC_OK */
	(unsigned char *) "确定",
	/*STR_PUBLIC_EXIT */
	(unsigned char *) "退出",
	/*STR_PUBLIC_OPERATION_FAILED*/
	(unsigned char *) "操作失败!",
	/*STR_PUBLIC_OPEN*/
	(unsigned char *) "打开",
	/*STR_PUBLIC_CLOSE */
	(unsigned char *) "关闭",
	/*STR_PUBLIC_PROMPT */
	(unsigned char *) "提示",
	/*STR_PUBLIC_WAITING*/
	(unsigned char *) "请稍等...",
	/*STR_PUBLIC_PHONE_BOOK*/
	(unsigned char *) "电话本",
	/*STR_PUBLIC_FILE_MANAGER*/
	(unsigned char *) "文件管理器",	/* 10 */
	/*STR_PUBLIC_RETURN*/
	(unsigned char *) "返回",
	/*STR_PUBLIC_FORMAT_FLASH*/
	(unsigned char *) "格式化flash2",//tbd
	/*STR_PUBLIC_ENTER*/
	(unsigned char *) "进入",
	/*STR_AS_RING_VOLUME_CONTROL*/
	(unsigned char *) "调整振铃音量",
	/*STR_AS_SPEAKER_VOLUME_CONTROL*/
	(unsigned char *) "调整通话音量",
	/*STR_AS_RING_STATUS*/
	(unsigned char *) "振铃模式",
	/*STR_AS_RS_RING*/
	(unsigned char *) "响铃",
	/*STR_AS_RS_VIBRATE*/
	(unsigned char *) "振动",
	/*STR_AS_RS_VIBRING*/
	(unsigned char *) "振动响铃",
	/*STR_AS_RS_SILENT*/
	(unsigned char *) "静音",
	/*STR_AS_SET_SUCCESS*/
	(unsigned char *) "设置成功",
	/*STR_AS_RING_LIST*/
	(unsigned char *) "铃声列表",
	/*STR_AS_GROUP_TYPE*/
	(unsigned char *) "电话本分组",
	/*STR_AS_KEY_TONE*/
	(unsigned char *) "按键音控制",
	/*STR_AS_OPEN*/
	(unsigned char *) "打开",
	/*STR_AS_CLOSE*/
	(unsigned char *) "关闭",
	/*STR_AS_RING_SELE*/
	(unsigned char *) "铃声选择",
	/*STR_AS_CALL*/
	(unsigned char *) "电话",
	/*STR_AS_SMS*/
	(unsigned char *) "短消息",
	/*STR_AS_MINU_NOTI*/
	(unsigned char *) "时间提示",
	/*STR_AS_ONE_MINU*/
	(unsigned char *) "一分钟",
	/*STR_AS_FIVE_MINU*/
	(unsigned char *) "五分钟",
	/*STR_AS_TEN_MINU*/
	(unsigned char *) "十分钟",
	/*STR_AS_FIFT_MINU*/
	(unsigned char *) "十五分钟",
	/*STR_AS_CONN_NOTI*/
	(unsigned char *) "接通提示",
	/*STR_AS_OPEN_RING*/
	(unsigned char *) "开机音乐",
	/*STR_AS_KEYTONE_SELE*/
	(unsigned char *) "按键音",
	/*STR_AS_AUDITION*/
	(unsigned char *) "试听",
	/*STR_AS_STOP*/
	(unsigned char *) "停止",
	/*STR_AS_SAVE*/
	(unsigned char *) "保存",
	/*STR_AS_KEYTONE_LIST*/
	(unsigned char *) "按键音列表",
	/*STR_PM_POWER_ON*/
	(unsigned char *) "开机",
	/*STR_PM_VOLTAGE_ALARM*/
	(unsigned char *) "电池电量不足",
	/*STR_PM_CHARGING*/
	(unsigned char *) "正在充电",
	/*STR_PM_CHARGED*/
	(unsigned char *) "充电完成",
	/*STR_CB_EMPTY */
	(unsigned char *) "限制码不能为空!",
	/*STR_CB_CONFIRMCANCEL_CONTENT_UP */
	(unsigned char *) "是否取消",
	/*STR_CB_CONFIRMCANCEL_CB_CONTENT_DOWN */
	(unsigned char *) "全部呼叫限制!",
	/*STR_CB_CANCELNO */
	(unsigned char *) "取消失败!",
	/*STR_CB_CANCELOK */
	(unsigned char *) "呼叫限制已被取消!",
	/*STR_CB_BARRING_SUCC */
	(unsigned char *) "呼叫限制成功!",
	/*STR_CB_BARRING_FAIL */
	(unsigned char *) "呼叫限制失败!",
	/*STR_CB_MBRSTRCT */
	(unsigned char *) "呼叫限制",
	/*STR_CB_BAOC */
	(unsigned char *) "呼出限制",
	/*STR_CB_BOIC */
	(unsigned char *) "限制国际漫游",
	/*STR_CB_BICROAM_ALL */
	(unsigned char *) "国际漫游时呼入限制",
	/*STR_CB_CANCEL */
	(unsigned char *) "取消所有呼...",
	/*STR_CB_CANCEL_ALL */
	(unsigned char *) "取消所有呼叫限制",
	/*STR_CB_BOICEXHC_1 */
	(unsigned char *) "限制国际漫游",
	/*STR_CB_BOICEXHC_2 */
	(unsigned char *) "制国际漫游除",
	/*STR_CB_BOICEXHC_3 */
	(unsigned char *) "国际漫游除本",
	/*STR_CB_BOICEXHC_4 */
	(unsigned char *) "际漫游除本国",
	/*STR_CB_BICROAM_1 */
	(unsigned char *) "国际漫游时呼",
	/*STR_CB_BICROAM_2 */
	(unsigned char *) "际漫游时呼入",
	/*STR_CB_BICROAM_3 */
	(unsigned char *) "漫游时呼入限",
	/*STR_CB_BICROAM_4 */
	(unsigned char *) "游时呼入限制",
	/*STR_CB_CANCEL_1 */
	(unsigned char *) "取消所有呼叫",
	/*STR_CB_CANCEL_2 */
	(unsigned char *) "消所有呼叫限",
	/*STR_CB_CANCEL_3 */
	(unsigned char *) "所有呼叫限制",
	/*STR_CF_MBTRANSF */
	(unsigned char *) "呼叫转移",
	/*STR_CF_TRANSALL */
	(unsigned char *) "无条件转移",
	/*STR_CF_SHUTOROUT */
	(unsigned char *) "关机/出服务区",
	/*STR_CF_TRANSBUSY */
	(unsigned char *) "遇忙转移",
	/*STR_CF_TRANSANSWER */
	(unsigned char *) "无应答转移",
	/*STR_CF_UNTRANSALL */
	(unsigned char *) "取消所有转移",
	/*STR_CF_TRANS_SUCC */
	(unsigned char *) "设置转移成功!",
	/*STR_CF_TRANS_FAIL */
	(unsigned char *) "设置转移失败!",
	/*STR_CF_EMPTY */
	(unsigned char *) "号码不能为空!",
	/*STR_CF_CONFIRMCANCEL_CONTENT_UP */
	(unsigned char *) "是否取消",
	/*STR_CF_CONFIRMCANCEL_CF_CONTENT_DOWN */
	(unsigned char *) "全部呼叫转移?",
	/*STR_CF_CANCEL    */
	(unsigned char *) "取消",
	/*STR_CF_CANCEL_ALL_DIVERT*/
	(unsigned char *) "已取消!",
	/*STR_CF_SELECT_STATUS*/
	(unsigned char *) "选择状态",
	/*STR_CF_DIVERT_NUMBER*/
	(unsigned char *) "请输入转移号码",
	/*STR_HIM_CONTR_CONTRAST*/
	(unsigned char *) "调整对比度",
	/*STR_HIM_CONTR_BRIGHT*/
	(unsigned char *) "调整亮度",
	/*STR_HIM_DISPLAY*/
	(unsigned char *) "显示设置",
	/*STR_HIM_LANGUAGE*/
	(unsigned char *) "语言设置",
	/*STR_HIM_FACTORY*/
	(unsigned char *) "出厂设置",
	/*STR_HIM_POWER_SAVING*/
	(unsigned char *) "节电模式",
	/*STR_HIM_PHONE_SET*/
	(unsigned char *) "手机设置",
	/*STR_HIM_CONTRAST*/
	(unsigned char *) "对比度",
	/*STR_HIM_BRIGHT*/
	(unsigned char *) "亮度",
	/*STR_HIM_LCD_BLIGHT*/
	(unsigned char *) "屏幕",
	/*STR_HIM_KEYPAD_BLIGHT*/
	(unsigned char *) "键盘",
	/*STR_HIM_20S*/
	(unsigned char *) "20秒",	/* 100 */
	/*STR_HIM_30S*/
	(unsigned char *) "30秒",
	/*STR_HIM_60S*/
	(unsigned char *) "60秒",
	/*STR_HIM_90S*/
	(unsigned char *) "90秒",
	/*STR_HIM_10S*/
	(unsigned char *) "10秒",
	/*STR_HIM_15S*/
	(unsigned char *) "15秒",
	/*STR_HIM_5S*/
	(unsigned char *) "5秒",
	/*STR_INIT_OK */
	(unsigned char *) "确定",
	/*STR_INIT_CANCEL */
	(unsigned char *) "取消",
	/*STR_INIT_YES */
	(unsigned char *) "是",
	/*STR_INIT_NO */
	(unsigned char *) "否",
	/*STR_INIT_NOW */
	(unsigned char *) "清华科睿-Unis920",
	/*STR_INIT_HELLO */
	(unsigned char *) "欢迎您!",
	/*STR_INIT_ENTER_PHONECODE */
	(unsigned char *) "请输入开机密码",
	/*STR_INIT_CODE_WRONG */
	(unsigned char *) "输入密码错误",
	/*STR_INIT_PHCD_RETRY */
	(unsigned char *) "请重试",
	/*STR_INIT_ENTER_PIN */
	(unsigned char *) "请输入PIN",
	/*STR_INIT_ENTER_PUK */
	(unsigned char *) "请输入PUK",
	/*STR_INIT_ENTER_NEWPIN */
	(unsigned char *) "及新的PIN",
	/*STR_INIT_WAIT_CLEAR */
	(unsigned char *) "请稍候",
	/*STR_INIT_MASTER_CLEAR */
	(unsigned char *) "正在进行主清除",
	/*STR_INIT_MASTER_RESET */
	(unsigned char *) "正在进行主复位",
	/*STR_INIT_IF_SHUTDOWN */
	(unsigned char *) "SIM卡未加入\n是否关机?",
	/*STR_INIT_NO_PH_FUN */
	(unsigned char *) "您将无法使用\n电话功能",
	/*STR_INIT_PIN_RTM */
	(unsigned char *) "PIN输入错误\n还有%d次机会\n请重试",
	/*STR_INIT_PUK_RTM */
	(unsigned char *) "PUK输入错误\n还有%d次机会\n请重试",
	/*STR_INIT_PIN_BLOCK */
	(unsigned char *) "PIN Blocked\n请输入PUK\n共有%d次机会\n",
	/*STR_PB_ALLCONTACT */
	(unsigned char *) "全部联系人",
	/*STR_PB_FIND */
	(unsigned char *) "查找",
	/*STR_PB_ADD */
	(unsigned char *) "新添",
	/*STR_PB_MULTIDELETE */
	(unsigned char *) "多项删除",
	/*STR_PB_MULTICOPY */
	(unsigned char *) "多项复制",
	/*STR_PB_SETTING */
	(unsigned char *) "设置",
	/*STR_PB_CONTACTINFO */
	(unsigned char *) "联系人信息",
	/*STR_PB_EDIT */
	(unsigned char *) "编辑",
	/*STR_PB_DELETE */
	(unsigned char *) "删除",
	/*STR_PB_COPY */
	(unsigned char *) "复制",
	/*STR_PB_CHANGEGROUP */
	(unsigned char *) "组更改",
	/*STR_PB_NAME */
	(unsigned char *) "姓名",
	/*STR_PB_MOBILE */
	(unsigned char *) "移动电话",
	/*STR_PB_HOME */
	(unsigned char *) "住宅电话",
	/*STR_PB_OFFICE */
	(unsigned char *) "办公电话",
	/*STR_PB_MEMO */
	(unsigned char *) "备注",
	/*STR_PB_NUMBER */
	(unsigned char *) "号码",
	/*STR_PB_ADDTOPHONE */
	(unsigned char *) "添加到话机",
	/*STR_PB_ADDTOSIM */
	(unsigned char *) "添加到SIM卡",
	/*STR_PB_MULTISELECT */
	(unsigned char *) "多选",
	/*STR_PB_SELECTCONTACTS */
	(unsigned char *) "选择联系人",
	/*STR_PB_SELECTEDCONTACTS */
	(unsigned char *) "选中的联系人",
	/*STR_PB_FINDBY */
	(unsigned char *) "查找条件",
	/*STR_PB_TEXT */
	(unsigned char *) "备注",
	/*STR_PB_FINDNAME */
	(unsigned char *) "按姓名查找",
	/*STR_PB_FINDNUMBER */
	(unsigned char *) "按号码查找",
	/*STR_PB_FINDTEXT */
	(unsigned char *) "按备注查找",
	/*STR_PB_SEARCHRESULT */
	(unsigned char *) "查询结果",
	/*STR_PB_SEARCHNONE */
	(unsigned char *) "没有找到",
	/*STR_PB_COPYTOPHONE */
	(unsigned char *) "复制到话机",
	/*STR_PB_COPYTOSIM */
	(unsigned char *) "复制到SIM卡",
	/*STR_PB_SELECTTYPE */
	(unsigned char *) "选择类型",
	/*STR_PB_SELECTCOPYFIELD */
	(unsigned char *) "选择复制号码",
	/*STR_PB_MULTICOPYTYPE */
	(unsigned char *) "多项复制类型",
	/*STR_PB_SIMTOPHONE */
	(unsigned char *) "从SIM卡到话机",
	/*STR_PB_PHONETOSIM */
	(unsigned char *) "从话机到SIM卡",
	/*STR_PB_REMAINSPACE */
	(unsigned char *) "剩余空间 %d",
	/*STR_PB_PERSONAL */
	(unsigned char *) "私人",
	/*STR_PB_BUSINESS */
	(unsigned char *) "商务",
	/*STR_PB_OTHER */
	(unsigned char *) "其它",
	/*STR_PB_DELETING */
	(unsigned char *) "正在删除...",
	/*STR_PB_COPYING */
	(unsigned char *) "正在复制...",
	/*STR_PB_LOADING */
	(unsigned char *) "正在装入...",
	/*STR_PB_LISTNAME */
	(unsigned char *) "列出姓名",
	/*STR_PB_PHONE_FULL */
	(unsigned char *) "话机空间已满",
	/*STR_PB_SIM_FULL */
	(unsigned char *) "SIM卡空间已满",
	/*STR_PB_SELECT_NONE */
	(unsigned char *) "没有选中任何记录",
	/*STR_PB_SELECT_LIMIT */
	(unsigned char *) "已达到最大可选值",
	/*STR_PB_NAME_EMPTY */
	(unsigned char *) "姓名不能为空",
	/*STR_PB_NUMBER_EMPTY */
	(unsigned char *) "电话号码不能为空",
	/*STR_PB_CONFIRM_DELETE */
	(unsigned char *) "真的要删除吗?",
	/*STR_PB_LISTNAMENUMBER */
	(unsigned char *) "列出姓名和号码",
	/*STR_PB_SETTING_MAIN_TITLE */
	(unsigned char *) "电话本设置",
	/*STR_PB_SETTING_BTN_OK */
	(unsigned char *) "确定",
	/*STR_PB_SETTING_BTN_EXIT */
	(unsigned char *) "退出",
	/*STR_PB_SETTING_SIM */
	(unsigned char *) "SIM",
	/*STR_PB_SETTING_PHONE */
	(unsigned char *) "电话",
	/*STR_PB_SETTING_GROUPSET */
	(unsigned char *) "设置组",
	/*STR_PB_SETTING_MEMORYSTATUS */
	(unsigned char *) "内存状态",
	/*STR_PB_SETTING_VIEWTYPE */
	(unsigned char *) "显示类型",
	/*STR_PB_SETTING_STORAGETYPE */
	(unsigned char *) "存储类型",
	/*STR_PB_SETTING_ERROR_READ */
	(unsigned char *) "读取设置失败",
	/*STR_PB_SETTING_ERROR_WRITE */
	(unsigned char *) "写入设置失败",
	/*STR_PB_SETTING_SUCCESS_READ */
	(unsigned char *) "读取设置完毕",
	/*STR_PB_SETTING_SUCCESS_WRITE */
	(unsigned char *) "写入设置完毕",
	/*STR_PB_SETTING_PROMPT */
	(unsigned char *) "提示",
	/*STR_PB_SETTING_MAIN_BTN_OK */
	(unsigned char *) "确定",
	/*STR_PB_SETTING_MAIN_BTN_EXIT */
	(unsigned char *) "退出",
	/*STR_PB_SETTING_MAIN_GROUPSET */
	(unsigned char *) "组设置",
	/*STR_PB_SETTING_MAIN_MEMORY */
	(unsigned char *) "内存状态",
	/*STR_PB_SETTING_MAIN_VIEWTYPE */
	(unsigned char *) "查看类型",
	/*STR_PB_SETTING_MAIN_STORAGE */
	(unsigned char *) "存储状态",
	/*STR_PB_SETTING_SG_TITLE */
	(unsigned char *) "选择组别",
	/*STR_PB_SETTING_SG_BTN_OK */
	(unsigned char *) "确定",	/* 200 */
	/*STR_PB_SETTING_SG_BTN_EXIT */
	(unsigned char *) "退出",
	/*STR_PB_SETTING_SG_SIM */
	(unsigned char *) "SIM",
	/*STR_PB_SETTING_SG_PERSON */
	(unsigned char *) "个人",
	/*STR_PB_SETTING_SG_BUSINESS */
	(unsigned char *) "商务",
	/*STR_PB_SETTING_SG_OTHER */
	(unsigned char *) "其他",
	/*STR_PB_SETTING_SG_MENU_ITEM_1 */
	(unsigned char *) "选择图象",
	/*STR_PB_SETTING_SG_MENU_ITEM_2 */
	(unsigned char *) "选择声音",
	/*STR_PB_SETTING_SP_TITLE */
	(unsigned char *) "选择图象",
	/*STR_PB_SETTING_SP_BTN_OK */
	(unsigned char *) "存储",
	/*STR_PB_SETTING_SP_BTN_EXIT */
	(unsigned char *) "退出",
	/*STR_PB_SETTING_MEMORY_TITLE */
	(unsigned char *) "内存状态",
	/*STR_PB_SETTING_MEMORY_BTN_EXIT */
	(unsigned char *) "退出",
	/*STR_PB_SETTING_MEMORY_SIM */
	(unsigned char *) "SIM",
	/*STR_PB_SETTING_MEMORY_PHONE */
	(unsigned char *) "电话",
	/*STR_PB_SETTING_MEMORY_SIMTITLE */
	(unsigned char *) "SIM",
	/*STR_PB_SETTING_MEMORY_PHNTITLE */
	(unsigned char *) "电话",
	/*STR_PB_SETTING_MEMORY_TOTAL */
	(unsigned char *) "总数:%d",
	/*STR_PB_SETTING_MEMORY_FREE */
	(unsigned char *) "剩余:%d",
	/*STR_PB_SETTING_VIEW_TITLE */
	(unsigned char *) "显示类型",
	/*STR_PB_SETTING_VIEW_BTN_OK */
	(unsigned char *) "确定",
	/*STR_PB_SETTING_VIEW_BTN_EXIT */
	(unsigned char *) "退出",
	/*STR_PB_SETTING_VIEW_NAME */
	(unsigned char *) "姓名",
	/*STR_PB_SETTING_VIEW_NAMNUM */
	(unsigned char *) "姓名和号码",
	/*STR_PB_SETTING_STORAGE_TITLE */
	(unsigned char *) "存储类型",
	/*STR_PB_SETTING_STORAGE_BTN_OK */
	(unsigned char *) "确定",
	/*STR_PB_SETTING_STORAGE_BTN_EXT */
	(unsigned char *) "退出",
	/*STR_PB_SETTING_STORAGE_SIM */
	(unsigned char *) "SIM",
	/*STR_PB_SETTING_STORAGE_PHONE */
	(unsigned char *) "电话",
	/*STR_PB_COPYNUMBERAS*/
	(unsigned char *) "复制号码为",
	/*STR_PB_DELETE_SUCCESSFUL*/
	(unsigned char *) "删除成功!",
	/*STR_PB_DELETE_FAILED*/
	(unsigned char *) "删除失败!",
	/*STR_PB_COPY_SUCCESSFUL*/
	(unsigned char *) "复制成功!",
	/*STR_PB_COPY_FAILED*/
	(unsigned char *) "复制失败!",
	/*STR_PB_ADD_SUCCESSFUL*/
	(unsigned char *) "添加成功!",
	/*STR_PB_ADD_FAILED*/
	(unsigned char *) "添加失败!",
	/*STR_PB_UPDATE_SUCCESSFUL*/
	(unsigned char *) "修改成功!",
	/*STR_PB_UPDATE_FAILED*/
	(unsigned char *) "修改失败!",
	/*STR_PB_GRPCHG_SUCCESSFUL*/
	(unsigned char *) "组更改成功!",
	/*STR_PB_GRPCHG_FAILED*/
	(unsigned char *) "组更改失败!",
	/*STR_PB_SAVE_FAILED*/
	(unsigned char *) "存储失败!",
	/*STR_PB_NO_CONTACTS*/
	(unsigned char *) "没有联系人!",
	/*STR_PB_LOADCONTACT_FAILED*/
	(unsigned char *) "装载失败!",
	/*STR_PB_MSGWIN_ALARM*/
	(unsigned char *) "警告",
	/*STR_PB_MSGWIN_PROMPT*/
	(unsigned char *) "提示",
	/*STR_PB_OK*/
	(unsigned char *) "确认",
	/*STR_PB_EXIT*/
	(unsigned char *) "退出",
	/*STR_PB_CANCEL*/
	(unsigned char *) "取消",
	/*STR_PB_COPY_REPORT_TOTAL*/
	(unsigned char *) "%d个联系人",
	/*STR_PB_COPY_REPORT_OK*/
	(unsigned char *) "%d个复制成功",
	/*STR_PB_DELETE_REPORT_TOTAL*/
	(unsigned char *) "%d个联系人",
	/*STR_PB_DELETE_REPORT_OK*/
	(unsigned char *) "%d个删除成功",
	/*STR_PB_SIM_LOCATION*/
	(unsigned char *) "SIM 位置:%d",
	/*STR_PB_NOSIM*/
	(unsigned char *) "没有SIM卡",
	/*STR_PB_CHINESE_NAME_LIMITED*/
	(unsigned char *) "姓名被截短",
	/*STR_PB_DB_SIM_INIT_ERR*/
	(unsigned char *) "读SIM电话本失败",
	/*STR_SWITCH_ON */
	(unsigned char *) "开",
	/*STR_SWITCH_OFF */
	(unsigned char *) "关",
	/*STR_NS_NETWORK_SELECT */
	(unsigned char *) "网络选择",
	/*STR_NS_CALL_WAITING */
	(unsigned char *) "呼叫等待",
	/*STR_NS_CLIR */
	(unsigned char *) "主叫号码保密",
	/*STR_NS_AUTO_SELELCT */
	(unsigned char *) "自动选择",
	/*STR_NS_MANUAL_SELELCT */
	(unsigned char *) "人工选择",
	/*STR_NS_PREFER_NETWORK */
	(unsigned char *) "优先网络",
	/*STR_NS_BUTTON_REGISTER */
	(unsigned char *) "注册",
	/*STR_NS_MENU_ADD */
	(unsigned char *) "增加",
	/*STR_NS_MENU_DELETE */
	(unsigned char *) "删除",
	/*STR_NS_INPUT_PRIORITY */
	(unsigned char *) "请输入优先级别(<=10)",
	/*STR_NS_CHOOSE */
	(unsigned char *) "请选择网络",
	/*STR_NS_PRIORITY_WRONG */
	(unsigned char *) "数字错误!",
	/*STR_NS_SEARCHING_NETWORK */
	(unsigned char *) "正在搜寻...",
	/*STR_NS_NO_AVAILABLE_NETWORK */
	(unsigned char *) "没有可用网络",
	/*STR_NS_CMCC */
	(unsigned char *) "中国移动",
	/*STR_NS_UNICOM */
	(unsigned char *) "中国联通",
	/*STR_NS_REGISERING_NETWORK */
	(unsigned char *) "正在注册...",
	/*STR_NS_PLEASE_WAIT */
	(unsigned char *) "请稍等...",
	/*STR_NS_CLIR_DEFAULT */
	(unsigned char *) "默认设置",
	/*STR_NS_CLIR_SHOW */
	(unsigned char *) "显示号码",
	/*STR_NS_CLIR_HIDE */
	(unsigned char *) "隐藏号码",
	/*STR_NS_NO_NETWORK */
	(unsigned char *) "没有网络",
	/*STR_NS_NETWORK_UNKNOWN */
	(unsigned char *) "未知网络",
	/*STR_NS_FAILURE */
	(unsigned char *) "失败",
	/*STR_NS_WARNING */
	(unsigned char *) "注意",
	/*STR_NS_GET_NO_NETWORKNAME*/
	(unsigned char *) "无法得到网络名称",
	/*STR_NS_SIM_FAILURE*/
	(unsigned char *) "请检查SIM卡!",
	/*STR_NS_OPERATION_FORBIDDEN*/
	(unsigned char *) "操作不允许",
	/*STR_NS_OPERATION_NOT_SUPPORT*/
	(unsigned char *) "不支持此项操作",
	/*STR_NS_NO_SIM*/
	(unsigned char *) "没有SIM卡",
	/*STR_NS_TRY*/
	(unsigned char *) "请稍后再做网络选择",
	/*STR_NS_UPDATE_FAILURE*/
	(unsigned char *) "位置更新失败",
	/*STR_NS_SELECT_FAILURE*/
	(unsigned char *) "选网失败",
	/*STR_CS_CALL_SETTING */
	(unsigned char *) "应答类型",
	/*STR_CS_AUTO_ANSWER */
	(unsigned char *) "自动:",
	/*STR_CS_FLIP_ANSWER */
	(unsigned char *) "翻盖:",
	/*STR_CS_ANY_KEY_ANSWER */
	(unsigned char *) "任意键:",
	/*STR_CS_AUTOANSWER_FAILUER*/
	(unsigned char *) "自动应答设置失败",
	/*STR_CS_NO_FIND_EARPHONE*/
	(unsigned char *) "没有发现耳机",
	/*STR_CS_NO_FUNCTION*/
	(unsigned char *) "没有此项功能",
	/*STR_CH_CALL_HISTORY */
	(unsigned char *) "通话记录",
	/*STR_CH_MISSED_RECORD */
	(unsigned char *) "未接听",
	/*STR_CH_ANSWERED_RECORD */
	(unsigned char *) "已接听",	/* 300 */
	/*STR_CH_DIALED_RECORD */
	(unsigned char *) "呼出号码",
	/*STR_CH_CALL_TIME */
	(unsigned char *) "通话时长",
	/*STR_CH_MENU_DETAIL */
	(unsigned char *) "查看详细信息",
	/*STR_CH_MENU_DELETE */
	(unsigned char *) "删除本条记录",
	/*STR_CH_MENU_DELETE_ALL */
	(unsigned char *) "删除所有记录",
	/*STR_CH_MENU_SAVE */
	(unsigned char *) "存储至电话本",
	/*STR_CH_20_MISSED_RECORD */
	(unsigned char *) "最近20个未接来电",
	/*STR_CH_20_ANSWERED_RECORD */
	(unsigned char *) "最近20个已接来电",
	/*STR_CH_20_DIALED_RECORD */
	(unsigned char *) "最近20个呼出号码",
	/*STR_CH_LAST_CALL_TIME */
	(unsigned char *) "上次通话:",
	/*STR_CH_TOTAL_INCOMING_TIME */
	(unsigned char *) "累计接听:",
	/*STR_CH_TOTAL_OUTGOING_TIME */
	(unsigned char *) "累计呼出:",
	/*STR_CH_RESET_TIMER */
	(unsigned char *) "计时清零:",
	/*STR_CH_RESET_TIMER_CONFIRM */
	(unsigned char *) "确实要清零吗?",
	/*STR_CH_DETAIL_MENU_CALL */
	(unsigned char *) "呼叫本号码",
	/*STR_CH_DETAIL_MENU_SMS */
	(unsigned char *) "发送短信息",
	/*STR_CH_NAME */
	(unsigned char *) "姓名",
	/*STR_CH_NUMBER */
	(unsigned char *) "号码",
	/*STR_CH_DATE */
	(unsigned char *) "日期",
	/*STR_CH_TIME */
	(unsigned char *) "时间",
	/*STR_CH_DURATION */
	(unsigned char *) "时长",
	/*STR_CH_NO_RECORD*/
	(unsigned char *) "没有通话记录",
	/*STR_CH_NO_NAME*/
	(unsigned char *) "<未知姓名>",
	/*STR_CH_CLEAR_ZERO*/
	(unsigned char *) "真的要清零吗?",
	/*STR_CH_MISSED_NUMBER*/
	(unsigned char *) "个未接电话",
	/*STR_LAN_SWITCH_LANGUAGE */
	(unsigned char *) "语言设置",
	/*STR_LAN_LANGUAGE */
	(unsigned char *) "简体中文",
	/*STR_TON_PHONECALL */
	(unsigned char *) "呼出号码",
	/*STR_TON_SIMCALL */
	(unsigned char *) "SIM卡拨号",
	/*STR_TON_MENU_STORE */
	(unsigned char *) "保存",
	/*STR_TON_MENU_SHOWID */
	(unsigned char *) "显示ID",
	/*STR_TON_MENU_HIDEID */
	(unsigned char *) "隐藏ID",
	/*STR_TON_CLEAR */
	(unsigned char *) "清除",
	/*STR_TON_CHECKNET */
	(unsigned char *) "正在检测网络...",
	/*STR_TON_NETNOTSUPPORT */
	(unsigned char *) "网络不支持",
	/*STR_TON_SIMNOFOUND*/
	(unsigned char *) "位置%d空",
	/*STR_TON_SIMNUM*/
	(unsigned char *) "存储范围为1-%d",
	/*STR_TON_NOSIM*/
	(unsigned char *) "没有SIM卡!",
	/*STR_CC_ANSWER */
	(unsigned char *) "应答",
	/*STR_CC_END_HOLD_AND_ACCEPT */
	(unsigned char *) "挂断保持并接听",
	/*STR_CC_HOLD_ACTIVE_AND_ACCEPT */
	(unsigned char *) "保持通话并接听",
	/*STR_CC_END_ACTIVE_AND_ACCEPT */
	(unsigned char *) "挂断通话并接听",
	/*STR_CC_MUTE */
	(unsigned char *) "静音",
	/*STR_CC_HOLDMUTE */
	(unsigned char *) "保持和静音",
	/*STR_CC_UNMUTE */
	(unsigned char *) "发音",
	/*STR_CC_HOLD */
	(unsigned char *) "保持",
	/*STR_CC_NEW_CALL */
	(unsigned char *) "新建连接",
	/*STR_CC_END */
	(unsigned char *) "挂断",
	/*STR_CC_REDIAL */
	(unsigned char *) "重拨",
	/*STR_CC_RESUME */
	(unsigned char *) "恢复",
	/*STR_CC_SWITCH */
	(unsigned char *) "切换",
	/*STR_CC_END_CALL_ON_HOLD */
	(unsigned char *) "挂断保持",
	/*STR_CC_END_CALL_ON_ACTIVE */
	(unsigned char *) "挂断通话",
	/*STR_CC_CONNECTING_NOW */
	(unsigned char *) "通话中",
	/*STR_CC_HOLD_NOW */
	(unsigned char *) "呼叫保持",
	/*STR_CC_INCOMMING */
	(unsigned char *) "电话呼入...",
	/*STR_CC_CONNECTING_END */
	(unsigned char *) "呼叫终止",
	/*STR_CC_CONNECTING_FAILED */
	(unsigned char *) "通话失败",
	/*STR_CC_END_UNKNOW */
	(unsigned char *) "未知状态",
	/*STR_CC_DAILING */
	(unsigned char *) "正在呼出...",
	/*STR_CC_DAILING_END */
	(unsigned char *) "结束",
	/*STR_CC_EMERGENCY */
	(unsigned char *) "紧急呼叫",
	/*STR_CC_DROP */
	(unsigned char *) "拒绝",
	/*STR_CC_UNKNOW_NUM */
	(unsigned char *) "未知号码",
	/*STR_CC_NUM_TOO_LONG */
	(unsigned char *) "号码太长!",
	/*STR_SMS_CONFIRM */
	(unsigned char *) "确定",
	/*STR_SMS_EXIT */
	(unsigned char *) "退出",
	/*STR_SMS_CANCEL */
	(unsigned char *) "取消",
	/*STR_SMS_ERROR */
	(unsigned char *) "错误",
	/*STR_SMS */
	(unsigned char *) "短信息",
	/*STR_SMS_INITIALIZING */
	(unsigned char *) "正在初始化...",
	/*STR_SMS_FLASH_IS_FULL */
	(unsigned char *) "手机存储空间已满!",
	/*STR_SMS_SIM_IS_FULL */
	(unsigned char *) "SIM卡空间已满!",
	/*STR_SMS_NEED_DELETE_SOME_MSG */
	(unsigned char *) "删除一些短信!",
	/*STR_SMS_SAVE */
	(unsigned char *) "储存",
	/*STR_SMS_WRITE */
	(unsigned char *) "写信息",
	/*STR_SMS_READ_MSG */
	(unsigned char *) "读信息",
	/*STR_SMS_CONFIG */
	(unsigned char *) "设置",
	/*STR_SMS_VOICEMAIL */
	(unsigned char *) "语音信箱",
	/*STR_SMS_QUICKTEXT */
	(unsigned char *) "常用短语",
	/*STR_SMS_SAVED */
	(unsigned char *) "已存信息",
	/*STR_SMS_VOICEMAIL_LISTEN */
	(unsigned char *) "接听留言",
	/*STR_SMS_VOICEMAIL_SETUP */
	(unsigned char *) "更改号码",
	/*STR_SMS_VOICEMAIL_NUMBER */
	(unsigned char *) "语音信箱号码:",
	/*STR_SMS_VOICEMAIL_NONUMBER */
	(unsigned char *) "未设置",
	/*STR_SMS_GET_VOICEMAIL_ERROR */
	(unsigned char *) "查询号码失败!",
	/*STR_SMS_SET_VOICEMAIL_ERROR */
	(unsigned char *) "设置号码失败!",
	/*STR_SMS_SET_VOICEMAIL_OK */
	(unsigned char *) "语音信箱号码已存!",
	/*STR_SMS_RECEIVER */
	(unsigned char *) "收件人",
	/*STR_SMS_SEND */
	(unsigned char *) "发送",
	/*STR_SMS_IS_SENDING_NOW */
	(unsigned char *) "短消息正在发送中...",
	/*STR_SMS_MUST_INPUT_DA_NUMBER */
	(unsigned char *) "请输入收件人号码!",
	/*STR_SMS_MUST_INPUT_CONTENT */
	(unsigned char *) "短消息内容不能为空!",
	/*STR_SMS_ONE_ITEM_TOO_LONG */
	(unsigned char *) "号码太长!",
	/*STR_SMS_RECEIVER_IS_TOO_MORE */
	(unsigned char *) "指定的收件人太多!",
	/*STR_SMS_MSG_SEND_SUCCESS */
	(unsigned char *) "短消息发送成功!",
	/*STR_SMS_MSG_SEND_FAIL */
	(unsigned char *) "短消息发送失败!",
	/*STR_SMS_MSG_SENDING_TO */
	(unsigned char *) "正在发送短消息至:",
	/*STR_SMS_TOTAL_SEND_COUNT */
	(unsigned char *) "收件人总数:",
	/*STR_SMS_SEND_OK_COUNT */
	(unsigned char *) "已发送成功数:",	/* 400 */
	/*STR_SMS_SEND_FAIL_LIST */
	(unsigned char *) "未发送成功收件人:",
	/*STR_SMS_RESEND */
	(unsigned char *) "重试",
	/*STR_SMS_SAVING_MSG */
	(unsigned char *) "正在储存短消息...",
	/*STR_SMS_MSG_SAVE_FAIL */
	(unsigned char *) "短消息储存失败!",
	/*STR_SMS_MSG_SAVE_SUCCESS */
	(unsigned char *) "短消息已存!",
	/*STR_SMS_BOX_EMPTY */
	(unsigned char *) "此信箱中无信息!",
	/*STR_SMS_READ */
	(unsigned char *) "阅读",
	/*STR_SMS_SENDER_NAME */
	(unsigned char *) "发信人:\r\n",
	/*STR_SMS_SENDER_TIME */
	(unsigned char *) "发送于:\r\n",
	/*STR_SMS_CONTENT */
	(unsigned char *) "内容:\r\n",
	/*STR_SMS_DELETE */
	(unsigned char *) "删除",
	/*STR_SMS_REPLY */
	(unsigned char *) "回复",
	/*STR_SMS_EDIT */
	(unsigned char *) "编辑",
	/*STR_SMS_USERNUMBER */
	(unsigned char *) "提取号码",
	/*STR_SMS_FORWARD */
	(unsigned char *) "转发",
	/*STR_SMS_COPY */
	(unsigned char *) "复制短信",
	/*STR_SMS_COPY_WND_TITLE */
	(unsigned char *) "复制至另一存储器",
	/*STR_SMS_MOVE */
	(unsigned char *) "移动短信",//tbd
	/*STR_SMS_DELETE_CONFIRM*/
	(unsigned char *) "确定删除?",
	/*STR_SMS_DELETING */
	(unsigned char *) "正在删除...",
	/*STR_SMS_DELETE_FAIL */
	(unsigned char *) "短消息删除失败!",
	/*STR_SMS_DELETE_SUCCESS */
	(unsigned char *) "短消息已删除!",
	/*STR_SMS_KEEP_ORIGINAL */
	(unsigned char *) "保存原记录",
	/*STR_SMS_DELETE_ORIGINAL */
	(unsigned char *) "删除原记录",
	/*STR_SMS_COPYING */
	(unsigned char *) "正在复制...",
	/*STR_SMS_COPY_MSG_OK */
	(unsigned char *) "短信息已复制!",
	/*STR_SMS_COPY_MSG_FAIL */
	(unsigned char *) "复制短信息失败!",
	/*STR_SMS_MOVING */
	(unsigned char *) "正在移动短信息",
	/*STR_SMS_MOVE_MSG_OK */
	(unsigned char *) "短信息移动成功!",
	/*STR_SMS_MOVE_MSG_FAIL */
	(unsigned char *) "移动短信息失败!",
	/*STR_SMS_NOT_SUPPORT */
	(unsigned char *) "暂不支持此操作!",
	/*STR_SMS_NEW_MESSAGE_INCOMING */
	(unsigned char *) "%d 条新短信息",
	/*STR_SMS_SETTING */
	(unsigned char *) "设置",
	/*STR_SMS_STORAGE_INFO */
	(unsigned char *) "内存状态",
	/*STR_SMS_SET_SCA */
	(unsigned char *) "信息中心地址",
	/*STR_SMS_SET_TYPE */
	(unsigned char *) "存储类型",
	/*STR_SMS_SET_PERIOD */
	(unsigned char *) "有效期",
	/*STR_SMS_SET_PERIOD_1_HOUR */
	(unsigned char *) "1 小时",
	/*STR_SMS_SET_PERIOD_6_HOURS */
	(unsigned char *) "6 小时",
	/*STR_SMS_SET_PERIOD_24_HOURS */
	(unsigned char *) "24 小时",
	/*STR_SMS_SET_PERIOD_72_HOURS */
	(unsigned char *) "72 小时",
	/*STR_SMS_SET_PERIO_1_WEEK */
	(unsigned char *) "1 星期",
	/*STR_SMS_SET_PERIOD_MAX */
	(unsigned char *) "最大值",
	/*STR_SMS_SET_STORAGE_INFO */
	(unsigned char *) "存储信息",
	/*STR_SMS_SET_SUCC */
	(unsigned char *) "设置成功!",
	/*STR_SMS_SET_FAILED */
	(unsigned char *) "设置失败!",
	/*STR_SMS_INFO_FLASH */
	(unsigned char *) "手机",
	/*STR_SMS_INFO_SIM */
	(unsigned char *) "SIM卡",
	/*STR_SMS_INFO_USED */
	(unsigned char *) "已用:",
	/*STR_SMS_INFO_TOTAL */
	(unsigned char *) "总共:",
	/*STR_SMS_INFO_OK */
	(unsigned char *) "确定",
	/*STR_SMS_INFO_EXIT */
	(unsigned char *) "退出",
	/*STR_SMS_INFO_CLEAR */
	(unsigned char *) "清除",
	/*STR_SMS_INFO_NOTICE_INVALID */
	(unsigned char *) "输入错误!",
	/*STR_SMS_OUTBOX */
	(unsigned char *) "已存信息",
	/*STR_SMS_OUTBOX_OK */
	(unsigned char *) "阅读",
	/*STR_SMS_OUTBOX_EXIT */
	(unsigned char *) "退出",
	/*STR_SMS_OUTBOX_DETAIL */
	(unsigned char *) "详细信息",
	/*STR_SMS_OUTBOX_DETAIL_DELETE */
	(unsigned char *) "删除",
	/*STR_SMS_OUTBOX_DETAIL_EDIT */
	(unsigned char *) "编辑",
	/*STR_SMS_OUTBOX_DETAIL_FORWARD */
	(unsigned char *) "转发",
	/*STR_SMS_OUTBOX_NO_REC */
	(unsigned char *) "没有已存信息!",
	/*STR_SMS_OUTBOX_DEL_SUCC */
	(unsigned char *) "删除成功!",
	/*STR_SMS_OUTBOX_DEL_FAILED */
	(unsigned char *) "删除失败!",
	/*STR_SMS_IME_NAME_FULL_SCREEN_HAND_WRITE*/
	(unsigned char *) "全屏手写",
	/*STR_QT_SEND */
	(unsigned char *) "发送",
	/*STR_QT_NEW */
	(unsigned char *) "增加",
	/*STR_QT_MODIFY */
	(unsigned char *) "修改",
	/*STR_QT_DELETE */
	(unsigned char *) "删除",
	/*STR_QT_DETAIL */
	(unsigned char *) "查看",
	/*STR_QT_EXIT */
	(unsigned char *) "退出",
	/*STR_QT_CANCEL */
	(unsigned char *) "取消",
	/*STR_QT_MENU */
	(unsigned char *) "菜单",
	/*STR_QUICK_TEXT */
	(unsigned char *) "常用短语",
	/*STR_QT_DETAIL_TIT */
	(unsigned char *) "详细信息",
	/*STR_QT_NEW_TITLE */
	(unsigned char *) "增加",
	/*STR_QT_MODIFY_TITLE */
	(unsigned char *) "修改",
	/*STR_QT_SAVE */
	(unsigned char *) "确定",
	/*STR_QT_STATIC_0 */
	(unsigned char *) "下班有事吗?一起吃饭?",
	/*STR_QT_STATIC_1 */
	(unsigned char *) "我已下飞机,大约 分钟后来接我",
	/*STR_QT_STATIC_2 */
	(unsigned char *) "飞机点起飞,大约 点到达",
	/*STR_QT_STATIC_3 */
	(unsigned char *) "今晚不回家吃饭,加班",
	/*STR_QT_STATIC_4 */
	(unsigned char *) "我已到",
	/*STR_QT_MODIFY_SUCC */
	(unsigned char *) "修改成功!",
	/*STR_QT_MODIFY_FAILED */
	(unsigned char *) "修改失败!",
	/*STR_QT_ADD_SUCC */
	(unsigned char *) "增加成功!",
	/*STR_QT_DELETE_SUCC */
	(unsigned char *) "删除成功!",
	/*STR_QT_DELETE_FAILED */
	(unsigned char *) "删除失败!",
	/*STR_QT_ADD_NO_SPACE */
	(unsigned char *) "电话已满!",
	/*STR_QT_EMPTY */
	(unsigned char *) "<空>",
	/*STR_SS_CLEAR */
	(unsigned char *) "清除",
	/*STR_SS_CANCEL */
	(unsigned char *) "取消",
	/*STR_SS_SECURITY */
	(unsigned char *) "安全设置",
	/*STR_SS_CHGPASS */
	(unsigned char *) "修改密码",
	/*STR_SS_OLDPASS */
	(unsigned char *) "旧密码",
	/*STR_SS_NEWPASS */
	(unsigned char *) "新密码",
	/*STR_SS_REPASS */
	(unsigned char *) "确认新密码",
	/*STR_SS_STATUS */
	(unsigned char *) "设置状态",
	/*STR_SS_LockNow */
	(unsigned char *) "现在上锁",
	/*STR_SS_AutoLock */
	(unsigned char *) "自动上锁",	/* 500 */
	/*STR_SS_PINSetting */
	(unsigned char *) "PIN1设置",
	/*STR_SS_CallBarring */
	(unsigned char *) "呼叫限制",
	/*STR_SS_SecurityCode */
	(unsigned char *) "修改安全码",
	/*STR_SS_PIN1Code */
	(unsigned char *) "修改PIN1码",
	/*STR_SS_PIN2Code */
	(unsigned char *) "修改PIN2码",
	/*STR_SS_BarringCode */
	(unsigned char *) "修改限制码",
	/*STR_SS_InputPUK */
	(unsigned char *) "输入PUK1码",
	/*STR_SS_InputNewPIN */
	(unsigned char *) "新的PIN1码",
	/*STR_SS_InputRepPIN */
	(unsigned char *) "确认PIN1码",
	/*STR_SS_InputPUK2 */
	(unsigned char *) "输入PUK2码",
	/*STR_SS_InputNewPIN2 */
	(unsigned char *) "新的PIN2码",
	/*STR_SS_InputRepPIN2 */
	(unsigned char *) "确认PIN2码",
	/*STR_SS_INPUT_BARRINGCODE */
	(unsigned char *) "请输入限制码",
	/*STR_SS_INPUT_PHONECODE */
	(unsigned char *) "输入安全码",
	/*STR_SS_LockNowSetting */
	(unsigned char *) "现在上锁已设置!",
	/*STR_SS_AutoLock_Open */
	(unsigned char *) "自动上锁已打开!",
	/*STR_SS_CH_SECURITY_YES */
	(unsigned char *) "安全码已被修改!",
	/*STR_SS_CH_PIN_YES */
	(unsigned char *) "PIN码已被修改!",
	/*STR_SS_CH_PIN_NO */
	(unsigned char *) "PIN码修改失败!",
	/*STR_SS_CH_PIN2_YES */
	(unsigned char *) "PIN2码已被修改!",
	/*STR_SS_CH_PIN2_NO */
	(unsigned char *) "PIN2码修改失败!",
	/*STR_SS_CH_BAR_YES */
	(unsigned char *) "限制码已被修改!",
	/*STR_SS_CH_BAR_NO */
	(unsigned char *) "限制码修改失败!",
	/*STR_SS_NoLockNowSetting */
	(unsigned char *) "设置失败!",
	/*STR_SS_NoAutoLockSetting */
	(unsigned char *) "设置失败!",
	/*STR_SS_InputPIN */
	(unsigned char *) "请输入PIN码",
	/*STR_SS_InputBARRINGCODE */
	(unsigned char *) "请输入限制码",
	/*STR_SS_TWOPWDERROR */
	(unsigned char *) "密码确认错误!",
	/*STR_SS_PWDERROR */
	(unsigned char *) "密码错误!",
	/*STR_SS_ISEMPTY */
	(unsigned char *) "密码不能为空!",
	/*STR_SS_UNLOCKFAILED */
	(unsigned char *) "解锁失败!",
	/*STR_SS_UNBLOCKPIN_SUCC */
	(unsigned char *) "PIN码解锁成功!",
	/*STR_SS_UNBLOCKPIN_FAIL */
	(unsigned char *) "设置失败!",
	/*STR_SS_PINSTATUS_CHANGED*/
	(unsigned char *) "该状态已被设置!",
	/*STR_SS_TIMEOUT*/
	(unsigned char *) "设置失败,请重试!",
	/*STR_SS_BARRING_OPERATE*/
	(unsigned char *) "操作不允许!",
	/*STR_SS_SELECT_STATUS*/
	(unsigned char *) "选择状态",
	/*STR_SS_NO_NETWORK*/
	(unsigned char *) "当前无网络!",
	/*STR_SS_PIN_LOCKED*/
	(unsigned char *) "PIN码已被锁!",
	/*STR_SS_PIN2_LOCKED*/
	(unsigned char *) "PIN2码已被锁!",
	/*STR_SS_TIMES_PROMPT*/
	(unsigned char *) "还有%d次机会!",
	/*STR_SEQL_INVALIDATE*/
	(unsigned char *) "命令无效",
	/*STR_SEQL_SUC*/
	(unsigned char *) "操作成功",
	/*STR_SEQL_FAIL*/
	(unsigned char *) "操作失败",
	/*STR_SEQL_TIME_OUT*/
	(unsigned char *) "操作超时",
	/*STR_SEQL_ERROR*/
	(unsigned char *) "操作出错",
	/*STR_SEQL_CAPTION*/
	(unsigned char *) "字符串命令",
	/*STR_SEQL_WAITING*/
	(unsigned char *) "正在呼出\n\n请等待...",
	/*STR_SEQL_CF*/
	(unsigned char *) "呼叫转移\n\n",
	/*STR_SEQL_CF_UNCODITIONAL*/
	(unsigned char *) "无条件转移\n\n",
	/*STR_SEQL_CF_BUSY*/
	(unsigned char *) "遇忙转移\n\n",
	/*STR_SEQL_CF_NOREPLY*/
	(unsigned char *) "无应答转移\n\n",
	/*STR_SEQL_CF_NOTREACHABLE*/
	(unsigned char *) "不可达转移\n\n",
	/*STR_SEQL_CF_ALL*/
	(unsigned char *) "全部转移\n\n",
	/*STR_SEQL_CF_ALLCONDITIONAL*/
	(unsigned char *) "全部条件转移\n\n",
	/*STR_SEQL_CLIR*/
	(unsigned char *) "主叫号码隐藏\n",
	/*STR_SEQL_CLIP*/
	(unsigned char *) "拨入显示号码\n\n",
	/*STR_SEQL_COLP*/
	(unsigned char *) "连接号码显示\n",
	/*STR_SEQL_CLIR_INDICATOR*/
	(unsigned char *) "默认\n",
	/*STR_SEQL_CLIR_INVOCATION*/
	(unsigned char *) "开\n",
	/*STR_SEQL_CLIR_SUPPRESSION*/
	(unsigned char *) "关\n",
	/*STR_SEQL_CLIR_NOTPROVISION*/
	(unsigned char *) "未提供此服务\n",
	/*STR_SEQL_CLIR_PERMATNENT*/
	(unsigned char *) "永久提供\n",
	/*STR_SEQL_CLIR_UNKNOWN*/
	(unsigned char *) "未知\n",
	/*STR_SEQL_CLIR_TEMP_RESTRICT*/
	(unsigned char *) "临时限制性提供\n",
	/*STR_SEQL_CLIR_TEMP_ALLOW*/
	(unsigned char *) "临时提供\n",
	/*STR_SEQL_CLIP_ENABLE*/
	(unsigned char *) "开",
	/*STR_SEQL_CLIP_DISABLE*/
	(unsigned char *) "关",
	/*STR_SEQL_CLIP_NOTPROVISION*/
	(unsigned char *) "无此服务\n",
	/*STR_SEQL_CLIP_PROVISION*/
	(unsigned char *) "提供\n",
	/*STR_SEQL_CLIP_UNKNOWN*/
	(unsigned char *) "网络未知\n",
	/*STR_SEQL_CB*/
	(unsigned char *) "呼叫限制\n\n",
	/*STR_SEQL_CW*/
	(unsigned char *) "呼叫等待\n\n",
	/*STR_SEQL_CLASS_VOICE*/
	(unsigned char *) "语音",
	/*STR_SEQL_CLASS_DATA*/
	(unsigned char *) "数据",
	/*STR_SEQL_CLASS_FAX*/
	(unsigned char *) "传真",
	/*STR_SEQL_CLASS_SHORTMESSAGE*/
	(unsigned char *) "短消息",
	/*STR_SEQL_CLASS_ALL*/
	(unsigned char *) "所有",
	/*STR_SEQL_STATUS_ENABLE*/
	(unsigned char *) "开\n",
	/*STR_SEQL_STATUS_DISABLE*/
	(unsigned char *) "关\n",
	/*STR_STK_TITLE*/
	(unsigned char *) "STK",
	/*STR_STK_LEFT_CAP_BTN*/
	(unsigned char *) "确定",
	/*STR_STK_RIGHT_CAP_BTN*/
	(unsigned char *) "取消",
	/*STR_STK_DEDITBOX_LENGTH_TOO_SHORT*/
	(unsigned char *) "输入长度不够!",
	/*STR_STK_FMT_NOT_SUPPORTED*/
	(unsigned char *) "格式不支持!",
	/*STR_STK_CALL_DEFAULT_NAME*/
	(unsigned char *) "STK应用",
	/*STR_STK_CALL_NOW*/
	(unsigned char *) "现在通话?",
	/*STR_STK_CALL_CONFIRM*/
	(unsigned char *) "请确认",
	/*STR_CEER_1 */
	(unsigned char *) "无效号码",
	/*STR_CEER_3 */
	(unsigned char *) "用户不可及",
	/*STR_CEER_6 */
	(unsigned char *) "无可用信道",
	/*STR_CEER_8 */
	(unsigned char *) "呼叫限制",
	/*STR_CEER_16 */
	(unsigned char *) "通话结束",
	/*STR_CEER_17 */
	(unsigned char *) "对方忙",
	/*STR_CEER_18 */
	(unsigned char *) "无响应",
	/*STR_CEER_19 */
	(unsigned char *) "无应答",
	/*STR_CEER_21 */
	(unsigned char *) "呼叫被拒绝",
	/*STR_CEER_22 */
	(unsigned char *) "号码已更改",
	/*STR_CEER_26 */
	(unsigned char *) "通话终止",
	/*STR_CEER_27 */
	(unsigned char *) "呼叫失败",	/* 600 */
	/*STR_CEER_28 */
	(unsigned char *) "号码不完整",
	/*STR_CEER_29 */
	(unsigned char *) "设备拒绝",
	/*STR_CEER_30 */
	(unsigned char *) "通话结束",
	/*STR_CEER_31 */
	(unsigned char *) "通话结束",
	/*STR_CEER_34 */
	(unsigned char *) "网络忙",
	/*STR_CEER_38 */
	(unsigned char *) "呼叫失败",
	/*STR_CEER_41 */
	(unsigned char *) "呼叫失败",
	/*STR_CEER_42 */
	(unsigned char *) "网络忙",
	/*STR_CEER_43 */
	(unsigned char *) "呼叫失败",
	/*STR_CEER_44 */
	(unsigned char *) "呼叫失败",
	/*STR_CEER_47 */
	(unsigned char *) "无此服务",
	/*STR_CEER_49 */
	(unsigned char *) "无此服务",
	/*STR_CEER_50 */
	(unsigned char *) "无此服务",
	/*STR_CEER_55 */
	(unsigned char *) "用户组禁止",
	/*STR_CEER_57 */
	(unsigned char *) "呼叫失败",
	/*STR_CEER_58 */
	(unsigned char *) "呼叫失败",
	/*STR_CEER_63 */
	(unsigned char *) "无此服务",
	/*STR_CEER_65 */
	(unsigned char *) "呼叫失败",
	/*STR_CEER_68 */
	(unsigned char *) "余额不足",
	/*STR_CEER_69 */
	(unsigned char *) "呼叫失败",
	/*STR_CEER_70 */
	(unsigned char *) "呼叫失败",
	/*STR_CEER_79 */
	(unsigned char *) "呼叫失败",
	/*STR_CEER_81 */
	(unsigned char *) "呼叫失败",
	/*STR_CEER_87 */
	(unsigned char *) "用户组禁止",
	/*STR_CEER_88 */
	(unsigned char *) "呼叫失败",
	/*STR_CEER_91 */
	(unsigned char *) "无网络",
	/*STR_CEER_95 */
	(unsigned char *) "呼叫失败",
	/*STR_CEER_96 */
	(unsigned char *) "呼叫失败",
	/*STR_CEER_97 */
	(unsigned char *) "呼叫失败",
	/*STR_CEER_98 */
	(unsigned char *) "呼叫失败",
	/*STR_CEER_99 */
	(unsigned char *) "呼叫失败",
	/*STR_CEER_100 */
	(unsigned char *) "呼叫失败",
	/*STR_CEER_101 */
	(unsigned char *) "呼叫失败",
	/*STR_CEER_102 */
	(unsigned char *) "呼叫失败",
	/*STR_CEER_111 */
	(unsigned char *) "呼叫失败",
	/*STR_CEER_127 */
	(unsigned char *) "非固定号码",
	/*STR_CEER_240 */
	(unsigned char *) "操作失败",
	/*STR_CEER_241 */
	(unsigned char *) "呼叫禁止",
	/*STR_CEER_252 */
	(unsigned char *) "呼入禁止",
	/*STR_CEER_253 */
	(unsigned char *) "呼叫失败",	/* 640 */
	/*STR_CEER_254 */
	(unsigned char *) "呼叫失败",
	/*STR_CEER_255 */
	(unsigned char *) "状态未知",
	/*STR_CC_NOTSUPPORT*/
	(unsigned char *) "无此功能!",		
	/*STR_CF_CANCEL_SUCC*/
	(unsigned char *) "取消呼叫转移成功!",
	/*STR_CF_CANCEL_FAILED*/
	(unsigned char *) "取消呼叫转移失败!",
	/*STR_NS_TIME_OUT*/
	(unsigned char *) "超时",
	/*STR_CH_ERROR*/
	(unsigned char *) "严重错误!",	
	/*STR_NS_NO_NETWORK_SERVICE*/
	(unsigned char *) "没有网络服务",
	/*STR_NS_NO_PREFERRED_NETWORK*/
	(unsigned char *) "没有优先网络",
	/*STR_NS_NO_AVAILALBLE_NETWORK*/
	(unsigned char *) "没有发现可及网络",	/* 650 */
	/*STR_SS_UNBLOCKPIN2_SUCC*/
	(unsigned char *) "PIN2码解锁成功!",
	/*STR_SS_PWD_LENGTH_4*/
	(unsigned char *) "密码都应是4位!",
	/*STR_SS_PWD_LENGTH_6*/
	(unsigned char *) "密码都应是6位!",		
	/*STR_SS_PWD_LENGTH_8*/
	(unsigned char *) "PUK码应是8位!",
	/*STR_CB_BOICEXHC_5 */		/* 0904 add */
	(unsigned char *) "漫游除本国 ",
	/*STR_CB_BICROAM_5 */
	(unsigned char *) "时呼入限制 ",
	/*STR_CB_CANCEL_4 */
	(unsigned char *) "有呼叫限制 ",
	/*STR_SEQL_NOSERVICE*/	/*0906add*/
	(unsigned char *) "无此服务!",
	/*STR_SEQL_INVALID_PASSWORD*/ 
	(unsigned char *) "密码错误!",
	/*STR_SMS_CANCEL_ALL_SENDING */
	(unsigned char *) "取消全部",
	/*STR_SMS_SET_IN_SIM*/
	(unsigned char *) "SIM",
	/*STR_SMS_SET_IN_FLASH*/
	(unsigned char *) "手机",
	/*STR_CC_BEHELD */
	(unsigned char *) "通话被保持",		
	/*STR_CC_BERECOVERED*/
	(unsigned char *) "通话被恢复",
	/*STR_HIM_KEYPAD_BLIGHT_SET*/
	(unsigned char *) "键盘背景灯设置",
	/*STR_HIM_LCD_BLIGHT_SET*/
	(unsigned char *) "屏幕背景灯设置",	
	/*STR_PUBLIC_SYSTEM_BUSY */
	(unsigned char *) "系统忙,请稍候...",
	/*STR_SMS_SIM_UNKNOWN*/
	(unsigned char *) "未知",
	/*STR_NS_REGISTER_OK*/
	(unsigned char *) "注册成功",
	/*STR_PM_VOLTAGE_POWER_OFF*/
	(unsigned char *) "电池已到关机电压",/* 670 */
	/*STR_HIM_CLOCK_TYPE*/
	(unsigned char *) "时钟类型",
	/*STR_HIM_NUM_CLOCK*/
	(unsigned char *) "数 字",
	/*STR_HIM_SIMU_CLOCK*/
	(unsigned char *) "模 拟",
	/*STR_CEER_USER_1*/
	(unsigned char *) "电话未接",	
	/*STR_SS_LENGTH_LESS_4*/
	(unsigned char *) "长度不应小于4!",
	/*IDS_SELECT*/
	(unsigned char *) "选词",
	/*IDS_CAPTION*/
	(unsigned char *) "金山词霸",
	/*STR_PB_SIM_SETTING_MEMORY_TOTAL */
	(unsigned char *) "总数:%s",
	/*STR_PB_SIM_SETTING_MEMORY_FREE */
	(unsigned char *) "剩余:%s",
	/*STR_VOICE_PROMPT*/
	(unsigned char *) "声音提示",/* 680 */
	/*STR_TIME_PROMPT*/
	(unsigned char *) "时间提示",
	/*STR_CONNECT_PROMPT*/
	(unsigned char *) "接通提示",
	/*STR_SMS_WRITER_TYPE_TITLE*/
	(unsigned char *)	"短消息类型选项",/*liuyingxin add*//*683*/
	/*STR_EMS_WRITER_TITLE*/
	(unsigned char *)	"写动画短信",
	/*STR_EMS_PURE_TEXT*/
	(unsigned char *)	"文本短信",
	/*STR_EMS_ANIMATION*/
	(unsigned char *)	"动画短信",
	/*STR_EMS_IN_COMMON_USE*/
	(unsigned char *)	"常用动画",
	/*STR_EMS_SEND_ITEM_1*/
	(unsigned char *)	"爱你",
	/*STR_EMS_SEND_ITEM_2*/
	(unsigned char *)	"泡菜",
	/*STR_EMS_SEND_ITEM_3*/
	(unsigned char *)	"万事如意",
	/*STR_EMS_SEND_ITEM_4*/
	(unsigned char *)	"睡觉",
	/*STR_EMS_SEND_ITEM_5*/
	(unsigned char *)	"生日快乐",
	/*STR_EMS_SEND_ITEM_6*/
	(unsigned char *)	"昔日重现",
	/*STR_EMS_SEND_ITEM_7*/
	(unsigned char *)	"新年快乐",
	/*STR_EMS_SEND_ITEM_8*/
	(unsigned char *)	"思念",
	/*STR_EMS_SEND_ITEM_9*/
	(unsigned char *)	"圣诞快乐",
	/*STR_EMS_SEND_ITEM_10*/
	(unsigned char *)	"亲密接触",
	/*STR_EMS_SEND_ITEM_11*/
	(unsigned char *)	"中秋快乐",
	/*STR_EMS_SEND_ITEM_12*/
	(unsigned char *)	"多谢",
	/*STR_EMS_SEND_ITEM_13*/
	(unsigned char *)	"对不起",/*700*/
	/*STR_EMS_SEND_ITEM_14*/
	(unsigned char *)	"月光",
	/*STR_EMS_SEND_ITEM_15*/
	(unsigned char *)	"国旗",
	/*STR_EMS_SEND_ITEM_16*/
	(unsigned char *)	"笑一笑",
	/*STR_EMS_SEND_ITEM_17*/
	(unsigned char *)	"拜年",
	/*STR_EMS_SEND_ITEM_18*/
	(unsigned char *)	"一路顺风",
	/*STR_EMS_SEND_ITEM_19*/
	(unsigned char *)	"我饿了",
	/*STR_EMS_SEND_ITEM_20*/
	(unsigned char *)	"我错了",
	/*STR_EMS_MENU_INSERT*/
	(unsigned char *)	"插入",
	/*STR_EMS_MENU_PREVIEW*/
	(unsigned char *)	"预览",
	/*STR_EMS_MENU_EXIT*/
	(unsigned char *)	"退出",
	/*STR_EMS_MENU_OK*/
	(unsigned char *)	"确定",
	/*STR_EMS_SENDER_TITLE*/
	(unsigned char *)	"预览/选取动画",
	/*STR_EMS_PLAYER_TITLE*/
	(unsigned char *)	"动画播放",
	/*STR_EMS_SEND*/
	(unsigned char *)	"发送",
	/*STR_EMS_DETAIL*/
	(unsigned char *)	"动画短信详细",
	/*STR_EMS_SHORT_MESSAGE*/
	(unsigned char *)	"动画短信",
	/*STR_EMS_SMS_SHORT_MESSAGE*/
	(unsigned char *)	"文本短信",
	/*STR_EMS_STATUS*/
	(unsigned char *)	"状态: ",
	/*STR_EMS_STATUS_UNREAD*/
	(unsigned char *)	"未读 ",
	/*STR_EMS_STATUS_READED*/
	(unsigned char *)	"已读 ",
	/*STR_EMS_STATUS_UNSEND*/
	(unsigned char *)	"未发 ",
	/*STR_EMS_STATUS_UNALLREADY*/
	(unsigned char *)	"未收齐 ",
	/*STR_EMS_STATUS_NUMBER*/
	(unsigned char *)	"号码:",
	/*STR_EMS_STATUS_TOTALNUMBER*/
	(unsigned char *)	"共分%d条",
	/*STR_EMS_STATUS_ALREADYRECEIVEDNUMBER*/
	(unsigned char *)	"已收到%d条",
	/*STR_EMS_DETAIL_INFORMATION*/
	(unsigned char *)	"详细",
	/*STR_EMS_LOOKOVER_UNRECEIVEDRALLEADY*/
	(unsigned char *)	"查看未收齐动画短信",
	/*STR_EMS_SUCCESS_DUMP*/
	(unsigned char *)	"转储成功!",
	/*STR_EMS_FAILED_DUMP*/
	(unsigned char *)	"转储失败!",
	/*STR_EMS_TOO_BIG*/
	(unsigned char *)	"动画太大!",
	/*STR_EMS_DATA_FORMAT_ERROR*/
	(unsigned char *)	"数据格式错误!",
	/*STR_EMS_TOO_LONG*/
	(unsigned char *)	"动画太长!",
	/*STR_EMS_STORE_SPACE_OVERLOAD*/
	(unsigned char *)	"EMS存储空间已满!",
	/*STR_EMS_SAVE_FAILED*/
	(unsigned char *)	"存储失败!",
	/*STR_EMS_ANIMATION_TITLE*/
	(unsigned char *)	"< 动画短信 >",
	/*STR_EMS_PLAY_SELECTED*/
	(unsigned char *)	"播放已选动画",
	/*STR_EMS_DELETE_SELECTED*/
	(unsigned char *)	"删除已选动画",
	/*STR_EMS_FAILED_READ_FILE*/
	(unsigned char *) 	"打开文件失败",
	/*STR_EMS_NO_MEMORY*/
	(unsigned char *) 	"内存不足",
	/*STR_EMS_NO_SYSTEM_RESOURCE*/
	(unsigned char *) 	"系统资源不足",
	/*STR_EMS_UNSUPPORTED_FILE_FORMAT*/
	(unsigned char *) 	"不支持的短信格式",
	/*STR_EMS_STORAGE_INFO*/
	(unsigned char *) 	"动画短信存储信息",
	/*STR_EMS_VIEW_NONALLREADY_INFO*/
	(unsigned char *) 	"查看未收齐动画",
	/*STR_EMS_DUMP_WVG*/
	(unsigned char *) 	"转存动画",
	/*STR_EMS_MENU_SEE*/
	(unsigned char *) 	"查看",
	/*STR_EMS_TEXT*/
	(unsigned char *) 	"动画短信文本",
	/*STR_EMS_ANIMATOR*/
	(unsigned char *)	"动画",
	/*STR_EMS_LOADING*/
    (unsigned char *)	"装载中...",
	/*STR_EMS_SPLIT*/
    (unsigned char *)	"短信将分%d条发出",	
	/*STR_EMS_FILE_NUMBER_TOO_MANY*/
    (unsigned char *)	"动画文件数已达最大",
	/*STR_EMS_CANNOT_SAVE*/
    (unsigned char *)	"无法存储",/*liuyingxin end*/
	/*STR_SS_AutoLock_Close */
    (unsigned char *) 	"自动上锁已关闭!",
    /*STR_NS_RESET */
    (unsigned char *) 	"清零",
    /*STR_MENU_PENADJUST */
    (unsigned char *) 	"笔校准",
    /*STR_MENU_PERSONALSET*/
    (unsigned char *) 	"个性化设置",
    /*STR_MENU_PERSONALDESK*/
    (unsigned char *) 	"个性化桌面",
    /*STR_MENU_CUSTOMSTR*/
    (unsigned char *) 	"开机欢迎词", 
    /*STR_CC_NOTALLOWED*/
    (unsigned char *) 	"操作不允许",  
    /*STR_VOICE_HINT_SETTING*/
    (unsigned char *) 	"声音提示", 
    /*STR_PB_HANDLEING_DATA	*/
    (unsigned char *) 	"正处理数据..", 
    /*STR_PS_SETTING_INIT*/
    (unsigned char *) 	"开机欢迎词", 
    /*STR_PS_INFO_CLEAR*/
    (unsigned char *) 	"清除", 
    /*STR_PS_INFO_EXIT*/
    (unsigned char *) 	"退出",	
    /*STR_PS_INFO_OK*/
    (unsigned char *) 	"确定",
    /*STR_SMS_NEW_MSG_INCOMING*/
    (unsigned char *) 	"收到新短信。\r\n现在阅读?",     	
    /*STR_INIT_NO_SIMCARD*/
    (unsigned char *) 	"没有SIM卡!",	
    /*STR_INIT_CAN_NOT_INITION*/
    (unsigned char *) 	"将导致无法初始化",
    /*STR_INIT_CHECK_SIMCARD*/
    (unsigned char *) 	"请检查SIM卡",
    /*STR_INIT_SIMCARD_DESTORYED*/
    (unsigned char *) 	"SIM卡已经毁坏",
    /*STR_INIT_SIMCARD_ERROR*/
    (unsigned char *) 	"SIM错误!",
    /*STR_INIT_ERROR*/
    (unsigned char *) 	"错误",	
    /*STR_HIM_BLIGHT*/
    (unsigned char *) 	"背景灯",
    /*STR_PUBLIC_TIMESET*/
    (unsigned char *) 	"时间设置",     	
    /*STR_SMS_DELETE_ALL*/
    (unsigned char *) 	"删除所有",
    /*STR_SMS_DELETE_ALL_FLASH*/
    (unsigned char *) 	"手机",
    /*STR_SMS_DELETE_ALL_SIM*/
    (unsigned char *) 	"SIM卡",     	
    /*STR_SMS_NO_FLASH_MSG*/
    (unsigned char *) 	"手机中没有信息!",
    /*STR_SMS_NO_SIM_MSG*/
    (unsigned char *) 	"SIM卡中没有信息!",
    /*STR_NS_CALL_COME*/
    (unsigned char *) 	"有电话呼入",  
    /*STR_INIT_DEL*/
    (unsigned char *) 	"删除",   
    /*STR_PUBLIC_TIMESET_OK*/
  	(unsigned char *)	"时间设置成功",
  	/*STR_PUBLIC_TIMESET_FAIL*/
  	(unsigned char *)  	"时间设置失败",	
  	/*STR_TIME_IS_SETTING*/
 	(unsigned char *)  	"时间设置，请稍侯",
 	/*STR_QT_INSERT*/
 	(unsigned char *)  	"插入",
 	/*STR_PUBLIC_MON*/
 	(unsigned char *)  	"星期一",
 	/*STR_PUBLIC_TUE*/
 	(unsigned char *)  	"星期二",
 	/*STR_PUBLIC_WED*/
 	(unsigned char *)  	"星期三",
 	/*STR_PUBLIC_THU*/
 	(unsigned char *)  	"星期四",
 	/*STR_PUBLIC_FRI*/
 	(unsigned char *)  	"星期五",
 	/*STR_PUBLIC_SAT*/
 	(unsigned char *)  	"星期六",
 	/*STR_PUBLIC_SUN*/
 	(unsigned char *)  	"星期日", 
 	/*STR_PUBLIC_PCMANAGER*/
 	(unsigned char *)  	"PC管理器",	
 	/*STR_INIT_RECOVERY_OK*/
 	(unsigned char *)  	"恢复设置成功!",
 	/*STR_INIT_RECOVERY_FAIL*/
 	(unsigned char *)  	"恢复设置失败!",
 	/*STR_INIT_RECOVERY_NOTICE*/
 	(unsigned char *)  	"注意:所有设置都\n\n将恢复为出厂设置\n\n(将会自动关机)",
 	/*STR_INIT_RECOVERY*/
 	(unsigned char *)  	"恢复出厂设置",
 	/*STR_INIT_RECOVERY_CONFIRM*/
 	(unsigned char *)  	"您确定吗?",
 	/*STR_AS_ALARM_CLOCK*/
 	(unsigned char *)  	"闹铃",
 	/*STR_AS_ALARM_SET*/
 	(unsigned char *)  	"闹铃设置",
 	/*STR_AS_ALARM_ONE*/
 	(unsigned char *)  	"闹铃1",/*800*/
 	/*STR_AS_ALARM_TWO*/
 	(unsigned char *)  	"闹铃2",
 	/*STR_AS_ALARM_THREE*/
 	(unsigned char *)  	"闹铃3",
 	/*STR_AS_SET_TIME*/
 	(unsigned char *)  	"时间:",
 	/*STR_AS_ALARM_STATUS*/
 	(unsigned char *)  	"状态:", 
 	/*STR_CH_DELETE_ALL_CONFIRM*/
 	(unsigned char *)  	"确实要删除吗?",
	/*STR_PS_SETTING_INIT_OLD*/
    (unsigned char *)  "原欢迎词",	
	/*STR_PS_SETTING_INIT_NEW*/
    (unsigned char *)  "新欢迎词",      	
    /*STR_AS_ALARM_TYPE*/
    (unsigned char *)  "类型:",
    /*STR_AS_RING_SELE2*/
    (unsigned char *)  "铃声:",
    /*STR_AS_ALARM_ONCE*/
    (unsigned char *)  "仅闹一次",
    /*STR_AS_ALARM_DAYS*/
    (unsigned char *)  "每日提醒",
    /*STR_AS_ALARM_ERROR*/
    (unsigned char *)  "闹铃设置重复",
    /*STR_ENGINE_MODE*/
    (unsigned char *)   "工程模式",
 	/*STR_CIT_MODE*/
    (unsigned char *)   "CIT 模式",
 	/*STR_ERR_LOG_MODE*/
	(unsigned char *)   "错误代码日志",
 	/*STR_ATCMD_MODE*/
 	(unsigned char *)   "AT命令模式",
 	/*STR_DISPLAY_TEST*/
 	(unsigned char *)   "屏幕测试",
 	/*STR_BACKLIGHT_TEST*/
 	(unsigned char *)   "背景灯测试",
 	/*STR_DISPLAY_BACKLIGHT*/
    (unsigned char *)   "屏幕背景灯测试",
 	/*STR_KEYPAD_BACKLIGHT*/
 	(unsigned char *)   "键盘背景灯测试",
 	/*STR_ALERT_TEST*/
 	(unsigned char *)   "警告音测试",
 	/*STR_VIBRATOR_TEST*/
 	(unsigned char *)   "震动测试",
 	/*STR_LED_TEST*/
 	(unsigned char *)   "LED 灯测试",
 	/*STR_VERSION_TEST*/
 	(unsigned char *)   "版本信息",
 	/*STR_SW_VESION*/
    (unsigned char *)   "软件版本信息",
 	/*STR_HW_VESION*/
 	(unsigned char *)   "硬件版本信息",
 	/*STR_PDA_SIDE*/
    (unsigned char *)   "PDA 版本:",
 	/*STR_WAVECOME_SIDE*/
 	(unsigned char *)   "WAVECOME 版本:",
 	/*STR_KEYPAD_TEST*/
 	(unsigned char *)   "键盘测试",
 	/*STR_OFF_KEY_EXIT*/
 	(unsigned char *)   "请按键（挂断键退出）",
 	/*STR_TEST_ON*/
 	(unsigned char *)   "测试开始",
	/*STR_TEST_OFF*/
	(unsigned char *)   "测试停止",
	/*STR_EM_INPUT_ATCMD*/
	(unsigned char *)   "请输入 AT 命令:",
	/*STR_EM_SEND_AT*/
	(unsigned char *)   "发送",
	/*STR_EM_VIEW_AT_RESULT*/
	(unsigned char *)   "回显结果",
	/*STR_EM_ERR_INFO*/
	(unsigned char *)   "错误信息",
	/*STR_BACKLIGHT_ON*/
    (unsigned char *)   "打开背景灯",
 	/*STR_BACKLIGHT_OFF*/
 	(unsigned char *)   "关闭背景灯",
 	/*STR_AS_SCHEDULE_ALARM*/
 	(unsigned char *)   "日程表闹铃",  	
 	/*STR_QT_STATIC_5*/
 	(unsigned char *)   "现在正忙,稍后给你回电",
 	/*STR_QT_STATIC_6*/
 	(unsigned char *)   "堵车,我几分钟后到",
 	/*STR_QT_STATIC_7*/
 	(unsigned char *)   "我已在路上了,请稍候",
 	/*STR_QT_STATIC_8*/
 	(unsigned char *)   "多谢",
 	/*STR_QT_STATIC_9*/
 	(unsigned char *)   "祝你身体康健",
 	/*STR_QT_STATIC_10*/
 	(unsigned char *)   "恭喜发财",
 	/*STR_QT_STATIC_11*/
 	(unsigned char *)   "春节愉快,合家欢乐",
 	/*STR_QT_STATIC_12*/
 	(unsigned char *)   "祝你一路平安",
 	/*STR_QT_STATIC_13*/
 	(unsigned char *)   "节日快乐",
 	/*STR_QT_STATIC_14*/
 	(unsigned char *)   "生日快乐",
 	/*STR_NS_EMERGENCY_ONLY*/
 	(unsigned char *)   "只能紧急呼叫", /*850*/
	/*STR_INIT_RECOVERY_WAIT*/
	(unsigned char *)   "请稍等...",
	/*STR_PUBLIC_ISWAITING*/ 
	(unsigned char *)   "系统忙，请重试!",
	/*STR_EMS_KILL*/ 
	(unsigned char *)   "删除",
	/*STR_POKER_EASY*/
	(unsigned char *)   "初级",
	/*STR_POKER_NORMAL*/ 
	(unsigned char *)   "中级",
	/*STR_POKER_DIFFICULT*/ 
	(unsigned char *)   "高级",
};
