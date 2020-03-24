//#include "radio.h"
#include "sx1278.h"
#include "sx1278-Hal.h"
#include "sx1278-LoRa.h"
#include "connect.h"
#include "string.h"
#include "board.h"
#include "uart.h"
#include "stdlib.h"
/*!
 * Connect s_board registers variable
 */
 
 #define SUB_STATION_NUM_MAX 64
 
 extern STRU_uart urt1;
 extern STRU_uart urt2;
 extern uint8_t test_flag; 
 uint32_t start_time;
 static uint32_t end_time;
 
///////////////////////////数据存储管理/////////////////////////////////////// 
/////////////////本节一个从下往上看的树状链表/////////////////////////////////

//接收单片机上报的数据到对应的结构体（按照协议书上的14个数据返回类型排序，根据struct_type来划分结构体）
brightness bp;//2（编号为协议书上对应条目编号，下同）
Single_LF145C1_pama lf145c1;//3
Max_min_value mmvalue_thisday,mmvalue_lastday;//4，5
History_someday data_history;//6
State_thisday state;//7
URT_auto_stratage stra;//8
llf145c1_addr light_addr_1,light_addr_2,light_addr_3;//10.1 10.2 10.3
sun_control sun_level_set,sun_level_get;//11 ， 12
client_infor client_r,client_w;//13 ， 14

send_infor send_info;//暂存发送信息，和response重传机制绑定使用
master_order order;//暂存服务器下发信息，方便下层处理时取数据方便

frequency_control frequency_val = {22 , 0};//频率控制,这个赋值是为了方便串口调试时预设
broad_set broad_val;//广播信息设置，如广播的发送信息、次数、间隔时长等

//对以上数据接收结构体，进行统一管理
data_manage struct_id_get[20] = {
  {0x01 , (uint8_t*)&bp , sizeof(bp)}, //2
  {0x03 , (uint8_t*)&lf145c1 , sizeof(lf145c1)}, //3
  {0x04 , (uint8_t*)&mmvalue_thisday , sizeof(mmvalue_thisday)}, //4
  {0x05 , (uint8_t*)&mmvalue_lastday , sizeof(mmvalue_lastday)}, //5
  {0x0D , (uint8_t*)&data_history, sizeof(data_history)}, //6
  {0x07 , (uint8_t*)&state , sizeof(state)}, //7
  {0x0C , (uint8_t*)&stra , sizeof(stra)}, //8
  {0x09 , (uint8_t*)&light_addr_1 , sizeof(light_addr_1)}, //10_1
  {0x02 , (uint8_t*)&light_addr_2 , sizeof(light_addr_2)}, //10_2
  {0x0E , (uint8_t*)&sun_level_set , sizeof(sun_level_set)}, //11
  {0x1E , (uint8_t*)&sun_level_get , sizeof(sun_level_get)}, //12
  {0x06 , (uint8_t*)&client_r , sizeof(client_r)}, //13
  {0x08 , (uint8_t*)&client_w , sizeof(client_w)}, //14
};

//准备向单片机下发的结构体数据存储
brightness_set bright_all = {0x003C , 0x0A},bright_single = {0x003C , 0x00};//1，2，为了串口测试而做的临时赋值
history_data history_days;//6
URT_auto_stratage_set stra_set;//9
sun_set sun_vol_set={0x11};//11（测试预设值）
get_client_infor client_set={0xff,0x00,0x00,0x00,0x00};//13，14（测试预设值）

//对上面下发结构体的统一管理，特别注意的问题：STM32结构体存在内部对齐的问题，也就是sizeof()的结果根据结构体内部数据排列的不同而不同，然而STM8没有
data_manage struct_id_set[20] = {
  {0x11 , (uint8_t*)&bright_all , sizeof(bright_all)}, //1
  {0x01 , (uint8_t*)&bright_single , sizeof(bright_single)}, //2
  {0x0D , (uint8_t*)&history_days , 1}, //6
  {0x0B , (uint8_t*)&stra_set , 15}, //9
  {0x0E , (uint8_t*)&sun_vol_set, 2}, //11
  {0x08 , (uint8_t*)&client_set , 5}, //14
};

//对子站属性的管理表，每一行是一个子站，现在已经支持服务器增减子站条目（写了没测试）
sub_nature sub_data[SUB_STATION_NUM_MAX] = {//目前设置的最大子站数为64，这一条最好写在铁电存储器中（待补）
//组网中子站编号 ， 子站地址 ， 子站连接状态 ， 接收数据结构体列表 ， 下发数据结构体列表
	{0x01 , 0x6745 , 0 , struct_id_get , struct_id_set},
	{0x02 , 0x0100 , 0 , struct_id_get , struct_id_set},
	{0x03 , 0x0200 , 0 , struct_id_get , struct_id_set},
	{0x04 , 0x0300 , 0 , struct_id_get , struct_id_set},
	{0x05 , 0x0400 , 0 , struct_id_get , struct_id_set},
	{0x06 , 0x0500 , 0 , struct_id_get , struct_id_set},
	{0x07 , 0x0600 , 0 , struct_id_get , struct_id_set},
};
//主站自身的对子站的记录，应该再加一个子站强制连接数（待补）
sub_list list = {7 , SUB_STATION_NUM_MAX , 0 , sub_data};
///////////////////////////////////////////////////////////////////////////////////////

//////////////////////////数据的发送接收相关结构体定义/////////////////////////////////////
package_data data_r,data_t;//lora的发送和接收，数据位的存储位置（atpr和atpt中指针指向的区域）
STRU_auto_pack atpr, atpt; //lora的发送接收打包格式
STRU_auto_buff abf;//lora的发送接收数据包，最靠近lora的缓存区
STRU_auto_ver atv;//lora的发送接收关键信息记录
STRU_eep_addr s_adr;//STM32保存的主地址（应该写进eeprom（待补））

STRU_lv_policy lvp;				//
STRU_lv_lit	   lvl[64];		//
STRU_lv_now		 lvn[64];		//
STRU_lv_max		 lvm[64];

mas_buff mas_buffer;//BC28下发的数据位缓存区，但目前是直接切割字符串直接给结构体赋值的，应该没使用这个
////////////////////////////////////////////////////////////////////////////////

/////////////////////////////BC28的上报/////////////////////////////////////////

//给一个字符串，然后直接送到BC28上报缓存区的尾部
void at_send_str(char* s1){
	memcpy(mas_buffer.at_buff+mas_buffer.at_size , s1 , strlen(s1));
	mas_buffer.at_size = mas_buffer.at_size+ strlen(s1);
	//mas_buffer.at_buff[mas_buffer.at_size] = '\0';
}

//给一个整型值和它的大小，然后转化为字符串后，直接送到BC28上报缓存区的尾部
void at_send_data(uint8_t* val_p , uint8_t val_size){
	//uint8_t i;
	uint16_t val_16;
	uint16_t* val_int_p;
	uint8_t* val_char_p;
	char val_s[val_size+1];
	if(val_size == 1){
		val_char_p = (uint8_t*)val_p;
		sprintf(val_s , "%.2X" , *val_char_p);
	}
	if(val_size == 2){
		val_int_p = (uint16_t*)val_p;
		val_16 = ((*val_int_p)<<8)+((*val_int_p)>>8);
		sprintf(val_s , "%.4X" , val_16);
	}
	at_send_str(val_s);
}

//BC28的上报函数，调用这条可以向云平台发送AT指令
void BC28_send(uint16_t s_addr , uint8_t type , uint8_t struct_type){
	uint8_t i;
	uint8_t ii;
	uint8_t* data_p;
	order.s_addr = s_addr;
	order.type = type;
	order.struct_type = struct_type;
	urt1SendChar(0x99);
	urt1SendChar(0x99);
	urt1SendChar(type);
	//order.master_buff = mas_buffer.at_buff;
	//urt2SendStr((uint8_t*)(nb_initial_seq[0]) , (uint8_t)strlen((char*)nb_initial_seq[0]));//
	for(i=0 ; i<list.num ; i++){
		urt1SendStr((uint8_t*)&s_addr , 2);
		if((list.sub_nature_p+i)->s_addr == s_addr){ //遍历查找子地址
			switch(type){
				case 0x30: //地址添加确认（AT+NMGS=4，20010001）
					at_send_str("AT+NMGS=");
					at_send_str("4,");
					at_send_str("20");
					if(list.num != list.num_max){	
						at_send_str("01");//地址添加成功
						at_send_data((uint8_t*)&order.s_addr , 2);
					}
					else{
						at_send_str("00");//因为会超过地址数最大值，所以地址添加失败
						at_send_data((uint8_t*)&order.s_addr , 2);
					}
					mas_buffer.at_buff[mas_buffer.at_size] = 0x0D;
					mas_buffer.at_buff[mas_buffer.at_size+1] = 0x0A;
					urt1SendStr((uint8_t*)mas_buffer.at_buff , mas_buffer.at_size+2);
					mas_buffer.at_size = 0;
					strcpy(mas_buffer.at_buff , "");//必须要将缓存区清零，否则下次会继续添加在上一次发送字符串的尾部
					break;

				case 0x31: //地址删除确认（AT+NMGS=4，21010001）
					at_send_str("AT+NMGS=");
					at_send_str("4,");
					at_send_str("21");
					if(list.num != list.num_tmp){	
						at_send_str("01");//成功
						at_send_data((uint8_t*)&order.s_addr , 2);
					}
					else{
						at_send_str("00");//因为找不到地址而失败（临时地址数目与操作后的地址数目仍然一样）
						at_send_data((uint8_t*)&order.s_addr , 2);
					}
					mas_buffer.at_buff[mas_buffer.at_size] = 0x0D;
					mas_buffer.at_buff[mas_buffer.at_size+1] = 0x0A;
					urt1SendStr((uint8_t*)mas_buffer.at_buff , mas_buffer.at_size+2);
					mas_buffer.at_size = 0;
					strcpy(mas_buffer.at_buff , "");
					break;

				case 0x40: //强制连接确认（AT+NMGS=4，18010001）
					at_send_str("AT+NMGS=");
					at_send_str("4,");
					at_send_str("18");
					order.state = (list.sub_nature_p+i)->state;
					if(order.state){	
						at_send_str("01");//成功
						at_send_data((uint8_t*)&order.s_addr , 2);
					}
					else{
						at_send_str("00");//只要返回这条，就说明某个子站掉连接了（可能名花有主，可能强制连接不回应或错误回应）
						at_send_data((uint8_t*)&order.s_addr , 2);
					}
					mas_buffer.at_buff[mas_buffer.at_size] = 0x0D;
					mas_buffer.at_buff[mas_buffer.at_size+1] = 0x0A;
					urt1SendStr((uint8_t*)mas_buffer.at_buff , mas_buffer.at_size+2);
					urt2SendStr((uint8_t*)mas_buffer.at_buff , mas_buffer.at_size+2);
					mas_buffer.at_size = 0;
					strcpy(mas_buffer.at_buff , "");
					break;

				case 0x43: //强制断开确认（AT+NMGS=4，19010001）
					at_send_str("AT+NMGS=");
					at_send_str("4,");
					at_send_str("19");
					order.state = (list.sub_nature_p+i)->state;
					if(order.state){	
						at_send_str("00");//失败
						at_send_data((uint8_t*)&order.s_addr , 2);
					}
					else{
						at_send_str("01");//成功
						at_send_data((uint8_t*)&order.s_addr , 2);
					}
					mas_buffer.at_buff[mas_buffer.at_size] = 0x0D;
					mas_buffer.at_buff[mas_buffer.at_size+1] = 0x0A;
					urt1SendStr((uint8_t*)mas_buffer.at_buff , mas_buffer.at_size+2);
					urt2SendStr((uint8_t*)mas_buffer.at_buff , mas_buffer.at_size+2);
					mas_buffer.at_size = 0;
					strcpy(mas_buffer.at_buff , "");
					break;
				
				case 0x44: //频率设置上报(4,23010001) //这条只适用于单个子站频率修改，因为改为了广播，所以校验甚至上报都没什么意义
					at_send_str("AT+NMGS=");
					at_send_str("4,");
					at_send_str("23");
					//order.state = (list.sub_nature_p+i)->state;
					if(frequency_val.frequency_setnum != frequency_val.frequency_getnum){
						at_send_str("00");
						at_send_data((uint8_t*)&order.s_addr , 2);
					}
					else{
						at_send_str("01");
						at_send_data((uint8_t*)&order.s_addr , 2);
					}
					mas_buffer.at_buff[mas_buffer.at_size] = 0x0D;
					mas_buffer.at_buff[mas_buffer.at_size+1] = 0x0A;
					urt1SendStr((uint8_t*)mas_buffer.at_buff , mas_buffer.at_size+2);
					urt2SendStr((uint8_t*)mas_buffer.at_buff , mas_buffer.at_size+2);
					mas_buffer.at_size = 0;
					strcpy(mas_buffer.at_buff , "");
					break;
					
				case 0x42://轮询数据上报或下发指令的数据上报，目前只写了单灯数据上报（03）,后面可根据需求再补
					if((list.sub_nature_p+i)->state == 1){
						urt1SendChar(0x88);
						urt1SendChar(0x88);
						for(ii = 0 ; ii<20 ; ii++){
							if(((((list.sub_nature_p+i)->data_manage_p_get)+ii)->struct_type) == atpr.struct_type){		
								data_p = (uint8_t*)(((list.sub_nature_p+i)->data_manage_p_get)[ii]).struct_p; 
								switch(struct_type){
									case 0x03:
										at_send_str("AT+NMGS=");
										at_send_str("17,");
										at_send_str("00");
										at_send_str("01");
										at_send_str("0001");
										at_send_str("1A");
									//逐个添加结构体的值到发送字符串上，其实最好有一个统一的操作函数，但结构体内容的大小不一致，暂时没想出来优秀的写法
										at_send_data((uint8_t*)&s_addr , (uint8_t)sizeof(s_addr));
										at_send_data((uint8_t*)&(((Single_LF145C1_pama*)data_p)->current_front) , (uint8_t)sizeof(((Single_LF145C1_pama*)data_p)->current_front));
										at_send_data((uint8_t*)&(((Single_LF145C1_pama*)data_p)->current_value) , (uint8_t)sizeof(((Single_LF145C1_pama*)data_p)->current_value));
									  at_send_data((uint8_t*)&(((Single_LF145C1_pama*)data_p)->battery_vol) , (uint8_t)sizeof(((Single_LF145C1_pama*)data_p)->battery_vol));
										at_send_data((uint8_t*)&(((Single_LF145C1_pama*)data_p)->light_vol) , (uint8_t)sizeof(((Single_LF145C1_pama*)data_p)->light_vol));
										at_send_data((uint8_t*)&(((Single_LF145C1_pama*)data_p)->led_vol) , (uint8_t)sizeof(((Single_LF145C1_pama*)data_p)->led_vol));
										at_send_data((uint8_t*)&(((Single_LF145C1_pama*)data_p)->temple) , (uint8_t)sizeof(((Single_LF145C1_pama*)data_p)->temple));
										mas_buffer.at_buff[mas_buffer.at_size] = 0x0D;
										mas_buffer.at_buff[mas_buffer.at_size+1] = 0x0A;
										urt1SendStr((uint8_t*)mas_buffer.at_buff , mas_buffer.at_size+2);
										//urt2SendStr((uint8_t*)mas_buffer.at_buff , mas_buffer.at_size+2);
										mas_buffer.at_size = 0;
										strcpy(mas_buffer.at_buff , "");
										break;
								}
								break;
							}
						}
					}
					else{
						//这些else都空着的，也许以后会加入上报错误的机制
					}
					break;
				}
				break;
			}
		else{
			//这些else都空着的，也许以后会加入上报错误的机制
		}
	}
}
///////////////////////////////////////////////////////////////////////////////

/////////////////////////BC28下发处理机制//////////////////////////////////////

char str_2[3];
char str_4[5];
bc_pack bc_recv_info;//

//切指定位置开始的2个字符，转化为16进制的数字
void byte2_cut(char* string , uint8_t* val , uint8_t byte_pos){
	str_2[0] = string[byte_pos];
	str_2[1] = string[byte_pos+1];
	//str_2[2] = '\0';
	//*val = atoi(str_2);//这是之前用的一个字符串转数字的函数，但猛然发现不支持16进制
	//*val = StrHexToInt((unsigned char*)&str_2[0]);
	*val = StringHexToInt((unsigned char*)&str_2[0]);
}

//切指定位置开始的2个字符，转化为16进制的数字
void byte4_cut(char* string , uint16_t* val , uint8_t byte_pos){
	str_4[0] = string[byte_pos];
	str_4[1] = string[byte_pos+1];
	str_4[2] = string[byte_pos+2];
	str_4[3] = string[byte_pos+3];
	//str_4[4] = '\0';
	//*val = atoi(str_4);
//	*val = StrHexToInt((unsigned char*)&str_4[2]);
//	*val = (*val<<8) + StrHexToInt((unsigned char*)&str_4[0]);
	*val = StringHexToInt((unsigned char*)&str_4[2]);
	*val = (*val<<8) + StringHexToInt((unsigned char*)&str_4[0]);
}

//////////////////////////
void BC28_data_pack(){//放在quick_run中轮询等待云平台发来消息并处理字符串信息
	uint16_t cloud_s_addr;
	uint8_t sub_index;
	uint8_t j;
	if(nb.new_event!=1) return; nb.new_event = 0;
	//if(urt2.RxNum<=2) return;	urt2.RxNew = 0;
	//char nb_package[30];
	//char recv[64] = "NNMI:6,0545670A001C";
	//nb.recv_valid_len = 20;
	for(int i=0;i<nb.recv_valid_len;i++){
			//nb_package[i]=urt2.Rxb[i];
			//urt1SendChar(nb.recv[i]);//
			urt1SendChar(nb.recv[i]);
	}
	urt1SendChar('C');
	urt1SendChar('O');
	urt1SendChar('P');
	urt1SendChar('Y');	
	test_flag = 0;
	uint8_t flag_len = 0;
	char NNMI_flag[5] = "NNMI:";
	uint16_t find_result=strnstrnum((const char*)nb.recv,NNMI_flag,6);//
	//uint16_t find_result=strnstrnum((const char*)recv,NNMI_flag,6);
	if(find_result!=255){//接收成功
			//char str5[25];
		char *str5;
		str5=substring((char*)nb.recv,find_result+5,nb.recv_valid_len);//
		//str5=substring((char*)recv,find_result+5,nb.recv_valid_len);
		urt1SendStr((unsigned char*)str5 , nb.recv_valid_len);//
		byte2_cut(str5 , &bc_recv_info.bc_type , 2+flag_len);//str5是我们最后切出来的完整数据，类似于3，100001
		//urt1SendStr((uint8_t*)str5 , 20);
		//urt1SendChar(bc_recv_info.bc_type);
		//云平台下发的指令类型处理
		switch(bc_recv_info.bc_type){//解析命令类型
			case 0x10: //增加一个子站在列表最后（3，100001）（还没测试过！）
				if(list.num_max >= (list.num+1)){
					sub_data[list.num].subnum = list.num + 1;
					byte4_cut(str5 , &sub_data[list.num].s_addr , 4+flag_len);//直接添加新的子站地址
					sub_data[list.num].data_manage_p_get = struct_id_get;
					sub_data[list.num].data_manage_p_set = struct_id_set;
					list.num = list.num + 1;
					BC28_send(sub_data[list.num].s_addr , 0x30 , 0xff);
				}
				break;

			case 0x11: //减少列表中的某个子站 (3，110001)（还没测试过！）
				//sub_data[list.num].subnum = list.num - 1;
				byte4_cut(str5 , &cloud_s_addr , 4+flag_len);
				list.num = list.num_tmp;
				for(sub_index=0 ; sub_index<list.num;sub_index++){
					if(cloud_s_addr == sub_data[sub_index].s_addr){
						list.num = list.num - 1;
						for(j=sub_index ; j<list.num ; j++){
							sub_data[j] = sub_data[j+1];//从目标地址往后，逐个向前移位
						}
						//把最后一个元素置为0（有可能最后一个元素已经达到了线性表尾部）
						sub_data[list.num].s_addr = 0;
						sub_data[list.num].subnum = 0;
						sub_data[list.num].state = 0;
						sub_data[list.num].data_manage_p_get = 0;
						sub_data[list.num].data_manage_p_set = 0;
						BC28_send(sub_data[list.num].s_addr , 0x31 , 0xff);
						break;
					}
				}
				break;

			case 0x08: //强制连接（3，080001）（测试通过）
				urt1SendChar(0x99);
				urt1SendChar(0x99);
				urt1SendChar(0x99);
				order.struct_type = 0xff;
				order.type = 0xC0;
				byte4_cut(str5 , &order.s_addr , 4+flag_len);//
				urt1SendStr((uint8_t*)&order.s_addr , 2);
				BC28_recv(order.s_addr , 0xC0 , order.struct_type);
				break;

			case 0x09://强制断开（3，090001）（测试通过）
				order.struct_type = 0xff;
				order.type = 0xC2;
				byte4_cut(str5 , &order.s_addr , 4+flag_len);
				urt1SendStr((uint8_t*)&order.s_addr , 2);
				BC28_recv(order.s_addr , 0xC2 , order.struct_type);
				break;
			
			case 0x12://频段广播(4, 12000122)（还没测试！）
				order.struct_type = 0xff;
				order.type = 0xC4;
				byte4_cut(str5 , &order.s_addr , 4+flag_len);
				urt1SendStr((uint8_t*)&order.s_addr , 2);
				byte2_cut(str5 , &frequency_val.frequency_setnum , 8+flag_len);//这一句将频率值收下
				BC28_recv(0xffff , 0xC4 , order.struct_type);
				break;
			
			case 0x05://下发数据指令（6，0500010A003C）（测试通过）
				order.struct_type = 0x01;
				order.type = 0xC1;
				byte4_cut(str5 , &order.s_addr , 4+flag_len);
				urt1SendStr((uint8_t*)&order.s_addr , 2);
				//urt1SendStr((uint8_t*)&order.s_addr , 2);
				order.master_buff = bc_recv_info.bc_data;
				byte2_cut(str5 , &bc_recv_info.bc_data[0] , 8+flag_len);//亮度值
				byte4_cut(str5 , (uint16_t*)&bc_recv_info.bc_data[2] , 10+flag_len);//时间
				urt1SendStr((uint8_t*)&bc_recv_info.bc_data[2] , 2);
				urt1SendChar(0xFF);
				urt1SendStr(bc_recv_info.bc_data , 8);//验证数据接收的是否正确
				urt1SendChar(0xFF);
				BC28_recv(order.s_addr , 0xC1 , order.struct_type);//送到下发函数去发送
				break;
		
			default:
				break;
		}		
		nb.new_event=0;//标志位一定要清零
		//urt2.RxNew=0;
	}
}

//BC28处理完字符串后，紧接着的向STM8操作的下发函数
void BC28_recv(uint16_t s_addr , uint8_t type , uint8_t struct_type){
	uint8_t i;
	uint8_t ii;
	uint8_t* data_p;
	for(i=0 ; i<list.num ; i++){
		if((list.sub_nature_p+i)->s_addr == s_addr){ //找列表中的子站
			//urt1SendStr((uint8_t*)&(list.sub_nature_p+i)->s_addr , 2);
			switch(type){
				case 0xC0://强制连接
					order.type = 0xC0;
					send_reserve(i);//待信息暂存的发送（配合回传函数response使用）
					break;
				case 0xC2://强制断开
					order.type = 0xC2;
					send_reserve(i);
					break;
				
				case 0xC4://频率广播
					broad_val.broad_s_addr = 0xffff;
					broad_val.broad_type = 0xC4;
					broad_val.broad_struct_type = 0xff;
					broad_val.broad_chance = 3;
					broad_val.broad_flag = 1;//广播开始
					break;
				
				case 0xC1://
					//urt1SendChar(0x09);
					if((list.sub_nature_p+i)->state == 1){//ÒÑ¾­Á¬½Ó
						//»ñÈ¡¶ÔÓ¦×Ó»ú¶ÔÓ¦½á¹¹ÌåÖ¸Õë
						for(ii = 0 ; ii<20 ; ii++){
							if(((((list.sub_nature_p+i)->data_manage_p_set)+ii)->struct_type) == order.struct_type){
								data_p = (uint8_t*)((((list.sub_nature_p+i)->data_manage_p_set)+ii)->struct_p);
								//size = (((list.sub_nature_p+i)->data_manage_p_set)+ii)->struct_size;
								//这个写法本来是用于把缓存区数据整个搬下来到结构体，但是因为结构体对齐的问题，怕出现数据错误，就暂时没加
								/*
								for(j=0 ; j<size ; j++){ 
									data_p[j] = order.master_buff[j];
								}
								*/
								//urt1SendChar(sizeof(bright_single));
								switch(struct_type){
									case 0x01://单灯开关
										((brightness_set*)data_p)->brightness_percent = (uint8_t)bc_recv_info.bc_data[0];
										((brightness_set*)data_p)->brightness_time = *((uint16_t*)&(bc_recv_info.bc_data[2]));
										urt1SendStr((uint8_t*)&(((brightness_set*)data_p)->brightness_time) , 2);
										//urt1SendChar(sizeof(bright_single));
										order.type = 0xC1;
										urt1SendChar(0x11);
										urt1SendChar(0x11);
										urt1SendChar(0x11);
										urt1SendChar(0x11);
										send_reserve(i);//发送带重传机制的数据包给STM8
										break;
									
									case 0x11://广播开关
										((brightness_set*)data_p)->brightness_percent = (uint8_t)bc_recv_info.bc_data[0];
										((brightness_set*)data_p)->brightness_time = *((uint16_t*)&(bc_recv_info.bc_data[2]));
										order.type = 0xC1;
										broad_val.broad_s_addr = 0xffff;
										broad_val.broad_type = 0xC1;
										broad_val.broad_struct_type = 0x11;
										broad_val.broad_chance = 3;
										broad_val.broad_flag = 1;
										break;
								}								
								break;
							}
							else{
								//错误上报机制暂时保留
							}
						}
					}
					else{
						//
					}
					break;
				}
				break;
			}
		else{
			//
		}
	}
}
/////////////////////////////////////////////////////////////////////////////

/////////////////很早以前老师写的链表操作，但建了一套新的管理模式就没使用，暂留//////////
void s_addr_clear(void){
	uint8_t ii;
	for(ii=0;ii<64;ii++){
		s_adr.s_addr[ii]=0xffff;
	}
	s_adr.size=0;
}

void s_addr_insert(uint16_t newaddr){
	uint8_t ii;
	for(ii=0;ii<64;ii++){
		if(s_adr.s_addr[ii]==0xffff){
			s_adr.s_addr[ii]=newaddr;
			s_adr.size=ii+1;
			return;
		}
	}
}
void s_addr_delete(uint16_t addr, uint8_t *state){
	uint8_t ii;	
	*state=0;
	for(ii=0;ii<64;ii++){
		if(s_adr.s_addr[ii]==addr){ 
			*state=1;
			for(;ii<63;ii++){				
				s_adr.s_addr[ii]=s_adr.s_addr[ii+1];
			}
			s_adr.s_addr[63]=0xffff;
			ii=64;
		}
	}
	
	for(ii=0;ii<64;ii++){
		if(s_adr.s_addr[ii]==0xffff){
			s_adr.size=ii;
			return; 
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////lora发送部分//////////////////////////////////////

//带重传机制的发送函数，记录发送的信息，配合unresponse函数一起是使用
void send_reserve(uint8_t i){
	urt1SendChar(0x88);
	urt1SendChar(0x88);
	urt1SendChar(0x88);
	send_auto_pack((list.sub_nature_p+i)->s_addr , order.type , order.struct_type);
	//send_info.send_s_addr = (list.sub_nature_p+i)->s_addr;
	send_info.send_s_addr = atpt.s_addr;
	send_info.send_type = atpt.type;
	send_info.send_struct_type = atpt.struct_type;
	send_info.send_flag = 1;
	send_info.send_chance = 3;
  send_info.send_time_s = 10;
  start_time = HAL_GetTick();
}

//lora的打包发送函数，这也是最好的测试函数，可以看uart.c里的void urt1RM06(void)
void send_auto_pack(uint16_t addr, uint8_t type,uint8_t struct_type){
	uint8_t ii;
	uint8_t *p;
	uint8_t j;
	p=(uint8_t *)&atpt;
  atpt.m_addr=s_adr.m_addr;
  atpt.s_addr=addr;
  atpt.type=type;
	atpt.struct_type=struct_type;
	atpt.data_p=(uint8_t*)&data_t;//应该特别注意的是*data_p是data_t中的data_p
	//atpt.crc=0x0000;
	for(ii=0;ii<6;ii++) 	abf.buff[ii]=p[ii];
	if((struct_type==0xff) && (type != 0xC4)){
			abf.buff[6]=0;
			abf.buff[7]=0;
			abf.size = 8;
		//我写的冗余位校验存在问题，需要改一下，暂定的是ffff（待处理）
		/*
			for(ii=0;ii<abf.size-2;ii++){
				atpt.crc += abf.buff[ii];
			}
		*/
			//abf.buff[abf.size-2] = (atpt.crc>>8);
			//abf.buff[abf.size-1] = (atpt.crc);
			abf.buff[abf.size-2] = 0xff;
			abf.buff[abf.size-1] = 0xff;
			SX1278LoRaSetTxPacket(abf.buff,abf.size);//这是协议层和物理层的发送接口
			urt1SendStr(abf.buff , abf.size);
			abf.size=0;
		}
	else if(type==0xC4){ //频率设置
			abf.buff[6]= frequency_val.frequency_setnum;
			abf.buff[7]=0xff;
			abf.buff[8]=0xff;
			abf.size = 9;
		/*
			for(ii=0;ii<abf.size-2;ii++){
				atpt.crc += abf.buff[ii];
			}
		*/
			//abf.buff[abf.size-2] = (atpt.crc>>8);
			//abf.buff[abf.size-1] = (atpt.crc);
			SX1278LoRaSetTxPacket(abf.buff,abf.size);
			urt1SendStr(abf.buff , abf.size);
			abf.size=0;
	}
	else{
			send_data_deliver(data_t.data,&data_t.data_size,struct_type);//把对应的结构体数据传到发送数据缓存区data_t
			abf.size=data_t.data_size+8;
			for(j=0;j<data_t.data_size;j++) abf.buff[j+6]=atpt.data_p[j+1];
			data_t.data_size=0;
			abf.buff[abf.size-2]=0;
			abf.buff[abf.size-1]=0;
		/*
			for(ii=0;ii<abf.size-2;ii++){
				atpt.crc += abf.buff[ii];
			}
		*/
			//abf.buff[abf.size-2] = (atpt.crc>>8);
			//abf.buff[abf.size-1] = (atpt.crc);
			SX1278LoRaSetTxPacket(abf.buff,abf.size);
			urt1SendStr(abf.buff , abf.size);
			abf.size=0;
		}	
}

//之前老师留的，没使用
void s_addr_manage(void){
		//
}

/////////////////////////////////回传机制///////////////////////////////////
//因为没有回复，而进行的重传
uint32_t time_step;
void unresponse_resend(){
	if(send_info.send_flag == 1){//除非应答，否则会一直重发send_chance次
		//time_watch();
		end_time = HAL_GetTick();
		if(end_time - start_time >= 1000){//1000应该对应的是1S
			time_step ++;
			start_time = HAL_GetTick();
			end_time = HAL_GetTick();
			if(time_step == (send_info.send_time_s)){ //senf_time_s是设置的间隔秒数
				send_auto_pack(send_info.send_s_addr , send_info.send_type , send_info.send_struct_type);
				urt1SendChar(0x23);//看到串口打23，说明是一条回传的消息
				send_info.send_chance = send_info.send_chance - 1;
				time_step = 0;
				if(send_info.send_chance == 0){
					send_info.send_flag = 0;
					switch(send_info.send_type){
						case 0x40:
							BC28_send(atpr.s_addr , 0x40 , 0xff);
							break;
						case 0x43:
							BC28_send(atpr.s_addr , 0x43 , 0xff);
							break;
					}
				}
			}
		}
		else{
			return;
		}
	}
	else{
		return;
	}
}
//因为回复的数据存在明显问题，而进行的重传，可根据需要继续添加case
uint8_t data_chance;
void data_judge(void){
	switch(atv.struct_type_r){
		case 0x08://亮灯策略读取
			if(data_r.data[0] != 0x8A){ //测试时，发现有时候保留字节有问题，但重传会立即恢复正常
				if(data_chance == 3){ 
					data_chance = 0;
					return;
				}
				send_auto_pack(send_info.send_s_addr,0xC1,0x08);
				data_chance ++;
			}
			else{
				return;
			}
			break;
			
		case 0x01://单灯亮度和时间控制 
			//urt1SendChar(data_r.data[0]);
			if(data_r.data[0] != data_t.data[2]){//设置的亮度需要与返回的亮度一致，否则重传
				if(data_chance == 3){
					data_chance = 0;
					return;
				}
				send_auto_pack(send_info.send_s_addr,0xC1,0x01);
				data_chance ++;
			}
			break;
		}
}
////////////////////////////////////////////////////////////////////////////////////

////////////////////////////lora接收数据包处理////////////////////////////////////

//初始化的主站地址，当它发上数据包时是3412，然而单片机收下来是1234，同理单片机发出来的数据包也是3412，stm32收下了的还是1234
//目前我只保证了结果正确，这是因为前期收发测试是两个人写造成的，其中指针搬移也会导致数据倒位，如果觉得过程别扭，可以在某个位置倒一下
void connect_init(void){
	s_addr_clear();
	s_adr.m_addr=0x1234;
	//atpr.s_addr = (atpr.s_addr<<8) + (atpr.s_addr>>8);//这里提供一个倒位的方法
}

//lora的接收处理函数
void connect_process(void){
	uint8_t *p;
	uint8_t ii;
 	SX1278LoRaGetRxPacket((void *)abf.buff, &abf.size);
	unresponse_resend();//接收不到回信的回传机制
	if(abf.size<=0) return;
	 p=(uint8_t *)&atpr;
	 atpr.data_p=(uint8_t*)&data_r;
	 data_r.data_size=abf.size-8;
	 urt1SendChar(abf.size);
	 for(ii=0;ii<6;ii++) p[ii]=abf.buff[ii];
	 for(ii=0;ii<data_r.data_size;ii++) data_r.data[ii]=abf.buff[ii+6];
	 atv.pack_size=abf.size;
	 atv.type_r=atpr.type;
	 atv.struct_type_r=atpr.struct_type;
	 urt1SendStr(abf.buff,abf.size);
	 abf.size=0;
	 //urt1SendStr((uint8_t*)&atpr.s_addr , 2);
	switch(atv.type_r){
		case 0x40://强制连接回复
			if(atpr.m_addr==s_adr.m_addr)
				{
					urt1SendChar(0x40);
					send_info.send_flag = 0;//取消重传
					uint8_t i;
					for(i=0 ; i<list.num ; i++){
						urt1SendStr((uint8_t*)&((list.sub_nature_p+i)->s_addr) , 2);
						if((list.sub_nature_p+i)->s_addr == atpr.s_addr){ //去列表中找到子站
							(list.sub_nature_p+i)->state = 1;//将状态为置1，表示已经连接
							BC28_send(atpr.s_addr , 0x40 , 0xff);//上报给平台这一激动人心的消息
							break;
						}
					}
				}
			atv.type_r=0;
			break;
				
		case 0x41://名花有主回复
			if(atpr.m_addr==s_adr.m_addr){
					urt1SendChar(0x41);
					send_info.send_flag = 0;
					uint8_t i;
					for(i=0 ; i<list.num ; i++){
						if((list.sub_nature_p+i)->s_addr == atpr.s_addr){
							(list.sub_nature_p+i)->state = 0; //
							BC28_send(atpr.s_addr , 0x40 , 0xff);//上报给平台这一令人失望的消息
							break;
						}
					}
				}
			atv.type_r=0;
			break;
		
		case 0x43://强制断开
			if(atpr.m_addr==s_adr.m_addr){
					urt1SendChar(0x43);
					send_info.send_flag = 0;
					uint8_t i;
					for(i=0 ; i<list.num ; i++){
						if((list.sub_nature_p+i)->s_addr == atpr.s_addr){ 
							(list.sub_nature_p+i)->state = 0; 
							BC28_send(atpr.s_addr , 0x43 , 0xff);
							break;
						}
					}
				}
			atv.type_r = 0;
			break;
				
		case 0x44://单个子机频率设置的回复，不过现在已经改为广播，是不是需要不知道，就先暂时留着
			if(atpr.m_addr==s_adr.m_addr){
					urt1SendChar(0x44);
					send_info.send_flag = 0;
					//frequency_val.frequency_getnum = data_r.data[0];
					//broad_val.broad_flag = 1;
					//BC28_send(atpr.s_addr , 0x44 , 0xff);
					break;	
			}
			atv.type_r = 0;
			break;
				
		case 0x42://指令数据回复，对应0xC1
			if(atpr.m_addr==s_adr.m_addr){
				send_info.send_flag = 0;
				data_judge();//数据校验不对导致的重传
				recv_data_deliver((uint8_t*)data_r.data , &data_r.data_size , atv.struct_type_r);//将数据上存至对应的结构体
				BC28_send(atpr.s_addr , atv.type_r , atpr.struct_type);
			}
			atv.type_r=0;
			break;			
	}
}

//将结构体中的数据下发到发送数据缓存区中
void send_data_deliver(uint8_t* buffer , uint8_t* data_size_p_t , uint8_t struct_type){
  uint8_t ii;
  uint8_t jj;
  uint8_t* data_p;
  for(ii=0 ; ii<20 ; ii++){
    if(struct_id_set[ii].struct_type == struct_type){
      data_p = struct_id_set[ii].struct_p;
      *data_size_p_t = struct_id_set[ii].struct_size;
      for(jj=0 ; jj<*data_size_p_t ; jj++){
        buffer[jj] = data_p[jj];
      }
      break;
    }
  }      
}

//将接收数据缓存区中的数据收回到结构体中
void recv_data_deliver(uint8_t* buffer , uint8_t* data_size_p_r , uint8_t struct_type){
  uint8_t i;
	uint8_t ii;
  uint8_t j;
  uint8_t* data_p;
	if(*data_size_p_r > 0){
		for(i=0 ; i<list.num ; i++){
			urt1SendStr((uint8_t*)&((list.sub_nature_p+i)->s_addr) , 2);
			if((list.sub_nature_p+i)->s_addr == atpr.s_addr){ //
					for(ii = 0 ; ii<20 ; ii++){
						if(((((list.sub_nature_p+i)->data_manage_p_get)+ii)->struct_type) == atpr.struct_type){
							data_p = (uint8_t*)(((list.sub_nature_p+i)->data_manage_p_get)[ii]).struct_p;
							*data_size_p_r = (((list.sub_nature_p+i)->data_manage_p_get)+ii)->struct_size;
							for(j=0 ; j<*data_size_p_r ; j++){ 
								data_p[j] = buffer[j];
							}
							*data_size_p_r = 0;
							break;
						}
					}
					break;
			}
			else{
			//return;
			}
		} 
	}
}
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////广播模式//////////////////////////////////////////
uint32_t broad_fast_time;
uint32_t slow_time;
uint32_t broad_read_period = 5000;
uint8_t broad_i;
uint8_t broad_ii;

//广播发送函数，放在quick_run中等待广播要求
void broad_send_model(void){
	if(broad_val.broad_flag){
		if(sys_hal_tick_time - broad_fast_time > broad_read_period){
			if(broad_val.broad_chance!=0){
				broad_val.broad_chance--;
				broad_fast_time = sys_hal_tick_time;
				send_auto_pack(broad_val.broad_s_addr , broad_val.broad_type , broad_val.broad_struct_type);
			}
			else{
				broad_fast_time = sys_hal_tick_time;
				broad_val.broad_flag = 0;
				if(atpt.type == 0xC4){//在频率设置广播结束之后，将自身的频率调至和子机们一样
					urt1SendChar(frequency_val.frequency_setnum);
					SX1278LoRaSetRFFrequency_reset(frequency_val.frequency_setnum);
				}
				return;
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////轮询更新模式///////////////////////////////////////////////////

uint32_t s_time;
struct_choose_fast choose_fast = {1 , 0x03};//快速访问结构体类型条目，目前仅一个0x03
struct_choose_slow choose_slow = {1 , 0x04};//慢速.....
uint32_t fast_time;
uint32_t slow_time;
uint8_t fast_i;
uint8_t fast_ii;
uint8_t slow_i;
uint8_t slow_ii;
//用标志位来控制遍历顺序
uint8_t fast_addr_out_flag;//子站选择溢出标志（暂时没用，如果写成有限次轮询，可能会用到）
uint8_t fast_struct_out_flag;//数据类型选择溢出标志
uint8_t slow_addr_out_flag;//
uint8_t slow_struct_out_flag;

//目前轮询模式是在quick_run中一直跑的，据老师说：可以改成受平台控制的有限次轮询，这就需要放在一个特定的for循环里了
//快速轮询模式，访问间隔根据（子站建立连接数目*访问数据条目）决定，现在为了节约调试时间设置的比较快，到时候按需设置
void fast_fresh(void){
	uint32_t read_period = 450000 / (choose_fast.stuct_num * list.num);//对于7个子站一条数据的网络是1min一次，这里的list.num应该修改为子站连接数，而非子站个数
	if(sys_hal_tick_time - fast_time > read_period){
		//urt1SendChar(0x77);
		//if((list.sub_nature_p + fast_i)->s_addr){
		if((list.sub_nature_p+fast_i)->state == 1){//判断子站状态
				send_auto_pack((list.sub_nature_p+fast_i)->s_addr , 0xC1 , *(choose_fast.data_type+fast_ii));
				//send_auto_pack(0x0100 ,  0xC1 , *(choose_fast.data_type+fast_ii));//单一子机上报的测试函数
				fast_time = sys_hal_tick_time;
				send_info.send_s_addr = atpt.s_addr;
				send_info.send_type = atpt.type;
				send_info.send_struct_type = atpt.struct_type;
				send_info.send_flag = 1;
				send_info.send_chance = 3;
				send_info.send_time_s = 5;
				fast_time = sys_hal_tick_time;
				fast_ii ++;
				if(*(choose_fast.data_type+fast_ii)==0){//如果所选数据类型为0，说明应该访问下一个子站了
					fast_ii = 0;
					fast_struct_out_flag = 1;
				}
		}
		else{
			fast_i++;
		}
		if(fast_struct_out_flag){//如果数据类型位溢出了，则选择下一个子站
			fast_i ++;
			fast_struct_out_flag = 0;
		}
		if((list.sub_nature_p + fast_i)->s_addr == 0){
			fast_addr_out_flag = 1;
			fast_i = 0;
		}
	}
	//fast_time = sys_hal_tick_time;
}

//慢速上报，时间待调整
void slow_fresh(void){
	uint32_t read_period = 43200000 / (choose_slow.stuct_num * list.num);
	if(sys_hal_tick_time - slow_time > read_period){
		//if((list.sub_nature_p + fast_i)->s_addr){
		if((list.sub_nature_p+slow_i)->state == 1){//找子站
				send_auto_pack((list.sub_nature_p+slow_i)->s_addr , 0xC1 , *(choose_slow.data_type+slow_ii));
				//send_auto_pack(0x0100 ,  0xC1 , *(choose_fast.data_type+fast_ii));//单一子机上报的测试函数
				slow_time = sys_hal_tick_time;
				send_info.send_s_addr = atpt.s_addr;
				send_info.send_type = atpt.type;
				send_info.send_struct_type = atpt.struct_type;
				send_info.send_flag = 1;
				send_info.send_chance = 3;
				send_info.send_time_s = 5;
				slow_time = sys_hal_tick_time;
				slow_ii ++;
				if(*(choose_slow.data_type+slow_ii)==0){//如果所选数据类型为0，说明应该访问下一个子站了
					slow_ii = 0;
					slow_struct_out_flag = 1;
				}
		}
		else{//如果子站没有被连接，直接跳过去而不用等待一个周期
			slow_i++;
		}
		if(slow_struct_out_flag){//如果数据类型位溢出了，则选择下一个子站
			slow_i ++;
			slow_struct_out_flag = 0;
		}
		if((list.sub_nature_p + slow_i)->s_addr == 0){
			slow_addr_out_flag = 1;
			slow_i = 0;
		}
	}
}
//一个遍历所有子站的所有结构体数据的操作，但其实并不需要上报这么多，所以没用到，个人觉得也许会用在某一次维修，以对所有数据的全面检查
uint32_t fresh_time;
uint8_t s_addr_i;
uint8_t struct_type_ii;
void fresh_struct_data(void){ //Ñ­»·¸üÐÂ½á¹¹ÌåÊý¾Ý
	if((sys_hal_tick_time-fresh_time)>10000){
		if((list.sub_nature_p+s_addr_i)->s_addr){ //ÕÒ×ÓÕ¾
				if((list.sub_nature_p+s_addr_i)->state == 1){//ÒÑ¾­Á¬½Ó
					//»ñÈ¡¶ÔÓ¦×Ó»ú¶ÔÓ¦½á¹¹ÌåÖ¸Õë
						if(((((list.sub_nature_p+s_addr_i)->data_manage_p_get)+struct_type_ii)->struct_type)){
								send_auto_pack((list.sub_nature_p+s_addr_i)->s_addr , 0xC1 , ((((list.sub_nature_p+s_addr_i)->data_manage_p_get)+struct_type_ii)->struct_type));
								fresh_time = sys_hal_tick_time;
								send_info.send_s_addr = atpt.struct_type;
								send_info.send_type = atpt.type;
								send_info.send_struct_type = atpt.struct_type;
								send_info.send_flag = 1;
								send_info.send_chance = 3;
								send_info.send_time_s = 5;	
								struct_type_ii++;
							}
							else{
								struct_type_ii = 0;
							}
						
					}
					else{
						
					}
					s_addr_i++;
				}
		//}
		else{
			//×ÓÕ¾²»´æÔÚ±¨´
			s_addr_i = 0;
		}
	}
}

const uint32_t frequency_list[116] = {
0x67C000  , 
0x680000  , 
0x684000  , 
0x688000  , 
0x68C000  , 
0x690000  , 
0x694000  , 
0x698000  ,
0x69C000  ,
0x6A0000  ,
0x6A4000  ,
0x6A8000  ,
0x6AC000  ,
0x6B0000  ,
0x6B4000  ,
0x6B8000  ,
0x6BC000  ,
0x6C0000  ,
0x6C4000  ,
0x6C8000  ,
0x6CC000  ,
0x6D0000  ,
0x6D4000  ,
0x6D8000  ,
0x6DC000  ,
0x6E0000  ,
0x6E4000  ,
0x6E8000  ,
0x6EC000  ,
0x6F0000  ,
0x6F4000  ,
0x6F8000  ,
0x6FC000  ,
0x700000  ,
0x704000  ,
0x708000  ,
0x70C000  ,
0x710000  ,
0x714000  ,
0x718000  ,
0x71C000  ,
0x720000  ,
0x724000  ,
0x728000  ,
0x72C000  ,
0x730000  ,
0x734000  ,
0x738000  ,
0x73C000  ,
0x740000  ,
0x744000  ,
0x748000  ,
0x74C000  ,
0x750000  ,
0x754000  ,
0x758000  ,
0x75C000  ,
0x760000  ,
0x764000  ,
0x768000  ,
0x76C000  ,
0x770000  ,
0x774000  ,
0x778000  ,
0x77C000  ,
0x780000  ,
0x784000  ,
0x788000  ,
0x78C000  ,
0x790000  ,
0x794000  ,
0x798000  ,
0x79C000  ,
0x7A0000  ,
0x7A4000  ,
0x7A8000  ,
0x7AC000  ,
0x7B0000  ,
0x7B4000  ,
0x7B8000  ,
0x7BC000  ,
0x7C0000  ,
0x7C4000  ,
0x7C8000  ,
0x7CC000  ,
0x7D0000  ,
0x7D4000  ,
0x7D8000  ,
0x7DC000  ,
0x7E0000  ,
0x7E4000  ,
0x7E8000  ,
0x7EC000  ,
0x7F0000  ,
0x7F4000  ,
0x7F8000  ,
0x7FC000  ,
0x800000  ,
0x804000  ,
0x808000  ,
0x80C000  ,
0x810000  ,
0x814000  ,
0x818000  ,
0x81C000  ,
0x820000  ,
0x824000  ,
0x828000  ,
0x82C000  ,
0x830000  };

void SX1278LoRaSetRFFrequency_reset(uint8_t num)
{ //索引对应频段数值
    uint32_t frequency_target =  frequency_list[num-1];
  //用在任意位置进行热修改
    SX1278LR.RegFrfLsb = (uint8_t)(frequency_target & 0x000000ff);
    SX1278LR.RegFrfMid= (uint8_t)((frequency_target & 0x0000ff00)>>8);
    SX1278LR.RegFrfMsb = (uint8_t)((frequency_target & 0x00ff0000)>>16);
    SX1278WriteBuffer( REG_LR_FRFMSB, &SX1278LR.RegFrfMsb, 3 );
		SX1278LR.RegPaConfig = ( SX1278LR.RegPaConfig & RFLR_PACONFIG_PASELECT_MASK ) | RFLR_PACONFIG_PASELECT_PABOOST;
		SX1278Write( REG_LR_PACONFIG, SX1278LR.RegPaConfig );
		//SX1278Write(REG_LR_FRFMSB,frequency_val.high);
    //SX1278Write(REG_LR_FRFMID,frequency_val.med);
    //SX1278Write(REG_LR_FRFLSB,frequency_val.low);            
}
