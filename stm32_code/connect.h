
/*!
 * \file       connect.h
 * \brief      SX1278 RF chip driver
 *
 * \version    2.0.B2
 * \date       May 6 2013
 * \author     Gregory Cristian
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#ifndef __CONNECT_H__
#define __CONNECT_H__

#include "board.h"
//ÉÏ´«·şÎñÆ÷¸ñÊ½
typedef struct{
	uint16_t at_size;
	char at_buff[50];
}mas_buff;
//BC28ÏÂ·¢°ü
typedef struct{
	uint8_t bc_size;
	uint8_t bc_type;
	uint8_t bc_data[20];
}bc_pack;

//·şÎñÆ÷ÏÂ·¢Ö¸Áî
typedef struct{
	uint8_t subnum; //Ä¿±ê×Ó»úºÅ
	uint8_t type;
	uint8_t struct_type;
	uint8_t* master_buff; //Êı¾İ½ÓÊÕ»º´æÇø
	uint8_t state;
	uint16_t s_addr;
}master_order;

////////½á¹¹Ìå¹ÜÀí±í
typedef struct{
  uint8_t struct_type;//½á¹¹Ìå
  uint8_t* struct_p; //½á¹¹ÌåÊ×Î»Ö¸Õë
  uint8_t struct_size; //½á¹¹Ìå
}data_manage;

//×ÓÕ¾ÊôĞÔ
typedef struct{
	uint8_t subnum;
  uint16_t s_addr;
	uint8_t state;
	data_manage* data_manage_p_get;
	data_manage* data_manage_p_set;
}sub_nature;

//×ÓÕ¾¹ÜÀíÁĞ±í
typedef struct{
	uint8_t num;//µ±Ç°×ÓÕ¾Êı
	uint8_t num_max;//×î´ó×ÓÕ¾Êı
	uint8_t num_tmp;//Ôİ´æ×ÓÕ¾Êı£¨±ÈÈçÏÂ·¢Ôö¼õ×ÓÕ¾ÊıÄ¿£¬È´¿ÉÄÜ×ÓÕ¾³¤¶ÈÏŞÖÆ»òÕÒ²»µ½×ÓÕ¾£©
	sub_nature* sub_nature_p;//×ÓÕ¾ÊôĞÔ¹ÜÀí±íÖ¸Õë
}sub_list;

	//Êı¾İ°üµÄ½á¹¹Îª
	typedef struct{
		uint16_t m_addr; //Ö÷Õ¾µÄÎïÀíµØÖ·
		uint16_t s_addr; //×ÓÕ¾µÄÎïÀíµØÖ·
		uint8_t  type;   //Çø·ÖÉÏÏÂĞĞĞÅºÅ£ºÏÂĞĞ£º0XC0£ºÇ¿ÖÆÁ¬½Ó£¬0XC1£ºÇëÇóÊı¾İ¡£   ÉÏĞĞ£º0X40:Ç¿ÖÆÁ¬½Ó»ØÓ¦£¬0X41£ºÃû»¨ÓĞÖ÷»ØÓ¦£¬0X42£ºÊı¾İÍ¨ĞÅ»ØÓ¦¡£
		uint8_t  struct_type;//Çø·ÖÊı¾İÀàĞÍÎ»£ºÓëSTM8ÓëÂ·µÆÊ¹ÓÃµÄÖ¸ÁîÀàĞÍÒ»ÖÂ¡£ÈôÊÇÁ¬½ÓÏà¹Ø£¬Í³Ò»Îª0xff.
		uint8_t *data_p;//³õÊ¼»¯Ê±ºòÈÃÆäÖ¸ÏòSTRU_auto_packÀàĞÍ
		uint16_t crc;    //Ç°ÃæËùÓĞÊı¾İÇóºÏ£¬È¡ºó2×Ö½Ú
	}STRU_auto_pack;

		typedef struct{
		uint8_t data_size; //Êı¾İ»º´æÇøµÄ´óĞ¡
		uint8_t data[24]; //Êı¾İ»º´æÇø
	}package_data;

//	typedef struct{
//        uint8_t  size;
//        uint8_t  state;  //×´Ì¬			  
//		
//				uint8_t  num;    //
//			  uint8_t  step;   //²½
//				uint8_t  nc;
//		    uint32_t time;
//		
//				
//			  uint32_t s_time;
//        uint8_t  type;   //×ÓÕ¾µÄÎïÀíµØÖ·
//        uint8_t  pack_new;   //0xC0 Î»Îª 1
//        uint16_t crc;    //Ç°ÃæËùÓĞÊı¾İÇóºÏ£¬È¡ºó2×Ö½Ú
//	}STRU_auto_ver;

		typedef struct{
        uint8_t  pack_size;//Êı¾İ°ü´óĞ¡£¨´Óabs.size»ñÈ¡£©
				uint8_t  type_r;//½ÓÊÜ°üÀàĞÍ£¨ÉÏĞĞÏÂĞĞ£©
				uint8_t  type_t;//·¢ËÍ°üµÄÊı¾İÀàĞÍ£¨ÉÏĞĞÏÂĞĞ£©
				uint8_t  struct_type;//Êı¾İµÄÀàĞ
				uint8_t	 struct_type_r;//½ÓÊÕµ½µÄÊı¾İÀàĞÍº
	}STRU_auto_ver;//ÎŞÏß°üµÄĞÅÏ¢£¨Ö¸ÁîÀàĞÍ£¬Êı¾İÀàĞÍ£©
		

	
	typedef struct{
				uint8_t size;//×Ó»úÊı
				uint16_t m_addr;     //ÕâÊÇ±¾Ö÷Õ¾¹ÌÓĞµÄÎïÀíµØÖ·
        uint16_t s_addr[64]; //×ÓÕ¾Õ¾ÎïÀíµØÖ·,
	}STRU_eep_addr;


	//·¢ËÍĞÅÏ¢»º´æ,ÓÃÓÚ»Ø´«È·ÈÏ
	typedef struct{
		uint16_t send_s_addr;
		uint8_t send_type;
		uint8_t send_struct_type;
		uint8_t send_flag;
		uint8_t send_chance;
		uint8_t send_time_s;
	}send_infor;
	extern send_infor send_info;
	extern uint32_t start_time;
	
	//·¢ËÍ½ÓÊÕ»º´æÇø
	typedef struct{
		uint16_t size;
		uint8_t buff[32];
	}STRU_auto_buff;

	
	
	
	typedef struct{ 
		  uint8_t  brightness_percent;//ÁÁ¶È°Ù·Ö±È
		}brightness;

		/////////Ê±¼ä´ÁĞÅÏ¢,(2)µÄÁÁµÆĞÅÏ¢ --------ÄÚ²¿¼ÇÂ¼Ê¹ÓÃ
	typedef struct{  
		  uint32_t start_time;	//ÁÁµÆÆğÊ¼Ê±¼ä´Á
		  uint32_t end_time;	//ÁÁµÆ½áÊøÊ±¼ä´Á
		}Brightness_level;

		/////////µ¥¸öLF145C1Ö¸Áî(3)---------Õâ¸öĞèÒª
	typedef struct{//ÎŞÏß´«ËÍ°ü¡¾10B¡¿
		  uint16_t light_vol;		//¹â·üµçÑ¹
		  uint16_t battery_vol;		//µç³ØµçÑ¹
		  uint8_t current_front;	//³ä·ÅµçÁ÷·½Ïò
		  uint8_t current_value;	//³ä·ÅµçÁ÷Öµ
		  uint16_t led_vol;			//LED¹âÔ´°åµçÑ¹
		  uint16_t temple;			//°å¼¶ÎÂ¶È
		}Single_LF145C1_pama;

		/////////×îÖµ(4,5)----------°üÀ¨Á½ÖÖ,Ò»ÖÖÊÇµ±ÌìµÄ,Ò»ÖÖÊÇ×òÌìµÄ
	typedef struct{//ÎŞÏß´«ËÍ°ü¡¾16B¡¿
		  uint16_t charge_current_max;		//³äµçÁ÷µÄ×î´óÖµ
		  uint16_t discharge_current_max;	//·ÅµçÁ÷µÄ×î´óÖµ
		  uint16_t battery_value_max;		//µç³Ø×éµçÑ¹µÄ×î´óÖµ
		  uint16_t battery_value_min;		//µç³Ø°üµçÑ¹µÄ×îĞ¡Öµ
		  uint8_t temple_max;				//°å¼¶ÎÂ¶È×î´óÖµ
		  uint8_t temple_flag_max;			//ÎÂ¶È·ûºÅÎ»×î´óÖµ
		  uint8_t temple_min;				//°å¼¶ÎÂ¶È×îĞ¡Öµ
		  uint8_t temple_flag_min;			//ÎÂ¶È·ûºÅÎ»×îĞ¡Öµ
		  uint16_t charge_AH;				//³äµçAHÊı
		  uint16_t discharge_AH;			//·ÅµçAHÊı
		}Max_min_value;

		/////////ÀúÊ·Êı¾İ(6)----------Õâ¸öÔİ²»Ê¹ÓÃ,ÊÇ´æÔÚĞ¾Æ¬ÖĞµÄÀúÊ·Êı¾İ
	typedef struct{
		  uint8_t related_history;				//Ïà¶ÔÌìÊı
		  uint16_t battery_value_max_history;	//µç³Ø×éµçÑ¹µÄ×î´óÖµ
		  uint16_t charge_AH_history;			//³äµçAHÊı
		  uint16_t discharge_AH_history;		//·ÅµçAHÊı
		}History_someday;

		/////////µ±ÈÕ×´Ì¬(7)---------µ±ÌìµÄÊµÊ±Öµ
	typedef struct{//ÎŞÏß´«ËÍ°ü
		  uint8_t  step;						//³ÌĞòÖĞµÄstep±äÁ¿
		  uint8_t  dayornight;					//ºÚÒ¹°×ÌìµÄ±êÖ¾£º1ÊÇ°×Ìì£¬2ÊÇºÚÒ¹
		  uint8_t  count;						//¾ÍÊÇ³ÌĞòÖĞµÄnig_ct±äÁ¿°×Ìì×ªµ½ºÚÒ¹£¬ºÚÒ¹×ªµ½°×ÌìµÄ¼ÆÊıÎ»£¨ÊıÖµÊÇ100ÒÔÄÚ£©
		  uint32_t  time_day;					//°×ÌìÊ±¼ä£¨µ¥Î»£ºÃë£©
		  uint32_t  time_night;					//Ò¹ÍíÊ±¼ä£¨µ¥Î»£ºÃë£©
		  uint32_t  charge_AS_day;				//°×Ìì³äÈëµÄAhÊı(³ıÒÔ36000ºóµ¥Î»¾ÍÊÇAH)
		  uint32_t  discharge_AS_night;			//Ò¹Íí·Å³öµÄAhÊı(³ıÒÔ36000ºóµ¥Î»¾ÍÊÇAH)  
		}State_thisday;

		////////ÁÁµÆ²ßÂÔ(8)--------´æÔÚĞ´²ßÂÔÓë²ßÂÔ¶ÁÈ¡,¾ßÌåÄÚÈİ²»Ğè¿¼ÂÇ
	typedef struct{//ÎŞÏß´«ËÍ°ü¡¾15B¡¿
				uint8_t  time1;		//µÚÒ»½×¶ÎÊ±¼ä
				uint8_t  level1;	//ÆğÊ¼ÁÁ¶ÈÕ¼×Ü×î¸ßÁÁ¶ÈµÄ°Ù·Ö±È
				uint8_t  speed1;	//Ë¥¼õµÄËÙÂÊ
				uint8_t  time2;		//µÚ¶ş½×¶ÎÊ±¼ä
				uint8_t  level2;	//ÆğÊ¼Ê±¼äÕ¼×î¸ßÁÁ¶ÈµÄ°Ù·Ö±È
				uint8_t  speed2;	//Ë¥¼õµÄËÙÂÊ
				uint8_t  time3;		//µÚÈı½×¶Î
				uint8_t  level3;	//ÆäÊµÁÁ¶ÈÕ¼×î¸ßÁÁ¶ÈµÄ°Ù·Ö±È
				uint8_t  speed3;	//Ë¥¼õµÄËÙÂÊ
				uint8_t  time4;		//µÚËÄ½×¶ÎÊ±¼ä
				uint8_t  level4;	//ÆğÊ¼ÁÁ¶ÈÕ¼×Ü×î¸ßÁÁ¶ÈµÄ°Ù·Ö±È
				uint8_t  speed4;	//Ë¥¼õµÄËÙÂÊ
				uint8_t  time5;		//µÚÎå½×¶ÎÊ±¼ä
				uint8_t  level5;	//ÆğÊ¼ÁÁ¶ÈÕ¼×Ü×î¸ßÁÁ¶ÈµÄ°Ù·Ö±È
				uint8_t  speed5;	//Ë¥¼õËÙÂÊ
		}URT_auto_stratage;

		/////////ÁÁµÆ²ßÂÔÉèÖÃ(9)
		//uart1Manage case9:

		/////////Â·µÆµØÖ·(10)
	typedef struct{
		  uint8_t addr_h;	//·µ»ØµØÖ·¸ßÎ»
		  uint8_t addr_l;	//·µ»ØµØÖ·µÍÎ»
		}llf145c1_addr;

		/////////¹â¿ØµçÑ¹(11,12)
	typedef struct{
		  uint8_t sunctrl_level; //¹â¿ØµçÑ¹µÈ¼¶
		}sun_control;

		////////¶ÁÈ¡¿Í»§ĞÅÏ¢(13 ,14)
	typedef struct{
			uint8_t save_byte;	//±£Áô×Ö½Ú
			uint8_t byte1;		//µÚÒ»×Ö½Ú
			uint8_t byte2;		//µÚ¶ş×Ö½Ú
			uint8_t byte3;		//µÚÈı×Ö½Ú
			uint8_t byte4;		//µÚËÄ×Ö½Ú
			uint8_t byte5;		//µÚÎå×Ö½Ú
		}client_infor;
	
	


//Á´±í½á¹¹Ìå
	typedef struct{
			uint16_t Address;
			uint32_t update_time;
			brightness bp;
		  Single_LF145C1_pama lf145c1;
			Max_min_value mmvalue_thisday;
			Max_min_value mmvalue_lastday;
			History_someday data_history;
			Brightness_level Brightness_level_Slave;
			State_thisday state;
			URT_auto_stratage stra;
			llf145c1_addr light_addr_1;
			llf145c1_addr light_addr_2;
			sun_control sun_level_set;
			sun_control sun_level_get;
			client_infor client_r;
			client_infor client_w;		
		}Node_List;
	
typedef struct{
			uint8_t* p_point[14];
			uint8_t  data_size[14];
}point_point; 
		
//ÏÂ·¢µÄÊı¾İĞÅÏ¢½á¹¹Ìå	
	typedef struct{ 
		  uint16_t brightness_time;//
		  uint8_t  brightness_percent;//
		}brightness_set;
		
	typedef struct{ 
		  uint8_t  related_days;//
		}history_data;	
	
	typedef struct{//
				uint8_t  time1;		//µÚÒ»½×¶ÎÊ±¼ä
				uint8_t  level1;	//ÆğÊ¼ÁÁ¶ÈÕ¼×Ü×î¸ßÁÁ¶ÈµÄ°Ù·Ö±È
				uint8_t  speed1;	//Ë¥¼õµÄËÙÂÊ
				uint8_t  time2;		//µÚ¶ş½×¶ÎÊ±¼ä
				uint8_t  level2;	//ÆğÊ¼Ê±¼äÕ¼×î¸ßÁÁ¶ÈµÄ°Ù·Ö±È
				uint8_t  speed2;	//Ë¥¼õµÄËÙÂÊ
				uint8_t  time3;		//µÚÈı½×¶Î
				uint8_t  level3;	//ÆäÊµÁÁ¶ÈÕ¼×î¸ßÁÁ¶ÈµÄ°Ù·Ö±È
				uint8_t  speed3;	//Ë¥¼õµÄËÙÂÊ
				uint8_t  time4;		//µÚËÄ½×¶ÎÊ±¼ä
				uint8_t  level4;	//ÆğÊ¼ÁÁ¶ÈÕ¼×Ü×î¸ßÁÁ¶ÈµÄ°Ù·Ö±È
				uint8_t  speed4;	//Ë¥¼õµÄËÙÂÊ
				uint8_t  time5;		//µÚÎå½×¶ÎÊ±¼ä
				uint8_t  level5;	//ÆğÊ¼ÁÁ¶ÈÕ¼×Ü×î¸ßÁÁ¶ÈµÄ°Ù·Ö±È
				uint8_t  speed5;	//Ë¥¼õËÙÂÊ
		}URT_auto_stratage_set;
	
	typedef struct{ 
		  uint8_t  sunctrl_level;//
		}sun_set;		

	typedef struct{ 
		  uint8_t  byte1;//µÚÒ»×Ö½Ú
		  uint8_t  byte2;//µÚ¶ş×Ö½Ú
		  uint8_t  byte3;//µÚÈı×Ö½Ú
		  uint8_t  byte4;//µÚËÄ×Ö½Ú
		  uint8_t  byte5;//µÚÎå×Ö½Ú
		}get_client_infor;			

//ÉèÖÃºó£¬»¹ÓĞ¿ÉÄÜÒª¶ÁÈ¡Ô­À´µÄ²ßÂÔ	
typedef struct{ //¹²15¸ö×Ö½Ú
	uint8_t seg1;
	uint8_t lit1;
	uint8_t dec1;
	uint8_t seg2;
	uint8_t lit2;
	uint8_t dec2;
	uint8_t seg3;
	uint8_t lit3;
	uint8_t dec3;
	uint8_t seg4;
	uint8_t lit4;
	uint8_t dec4;
	uint8_t seg5;
	uint8_t lit5;
	uint8_t dec5;
}STRU_lv_policy;
	
	

	//ÁÁ¶ÈÊı¾İ£¬¹²¼ÇÂ¼64¸ö
	typedef struct{		
		uint8_t  lit;		//µÆµÄÁÁ¶È
		uint16_t time;  //ÁÁµÆÊ±¼ä
		uint32_t btime; //ÉèÖÃÂ·µÆµÄÁÁ¶ÈµÄÆğÊ¼Ê±¼ä£¬¿ÉÒÔ¸ù¾İÕâ¸öÖµ¼ÆËã³öÂ·µÆ»¹ÄÜÁÁ¶à¾Ã
	}STRU_lv_lit;
	
	
	//µ±ÌìµÄÂ·µÆÊı¾İ£¬Ã¿¸ô5·ÖÖÓ¸üĞÂÒ»´Î
	//µ±ÌìÊı¾İÇé¿ö£¬¹²¼ÇÂ¼64¸öÕâÑùµÄÊı¾İ
	typedef struct{
		int16_t volt;			//µç³ØµçÑ¹
		int16_t ch_a;  		//³äµçµçÁ÷
		int16_t dch_a;  	//·ÅµçµçÁ÷		
		int16_t ch_ah;	  //³äµçahÊı
		int16_t dch_ah;   //·ÅµçahÊı
		int16_t temp;			//ÎÂ¶È		
		int32_t time;     //
	}STRU_lv_now;
	
	//Ã¿¸ô15·ÖÖÓ¸üĞÂÒ»´Î
	//µ±ÌìµÄ×î´óÖµÇé¿ö,Ã¿¸öµÆµÄĞÅÏ¢¶¼ÓĞ£¬¹²ÉèÖÃ64¸ö£»
	typedef struct{
		int16_t volt_max;	//µç³Ø×éµçÑ¹×î´óÖµ
		int16_t volt_min; //µç³Ø×éµçÑ¹×îĞ¡Öµ
		int16_t ch_a_max; //³äµçµçÁ÷×î´óÖµ
		int16_t dch_a_max;//·ÅµçµçÁ÷×îĞ¡Öµ
		int16_t temp_max;	//°åÉÏÎÂ¶È×î´óÖµ
		int16_t temp_min; //°åÉÏÎÂ¶È×îĞ¡Öµ
		int16_t ch_ah;    //³äµçahÊı
		int16_t dch_ah;   //·ÅµçahÊı
	}STRU_lv_max;
	
	//×òÌì×î´óÖµÇé¿ö
		
//¿ìËÙÂÖÑ¯µÄÊı¾İÑ¡Ôñ£¨´ó¸Å1Ğ¡Ê±ÉÏ±¨Ò»´Î£©
typedef struct{
	uint8_t stuct_num;
	uint8_t data_type[14];
}struct_choose_fast;

//ÂıËÙÂÖÑ¯µÄÊı¾İÑ¡Ôñ£¨´ó¸Å12Ğ¡Ê±ÉÏ±¨Ò»´Î£
typedef struct{
	uint8_t stuct_num;
	uint8_t data_type[14];
}struct_choose_slow;
//ÆµÂÊ¿ØÖ

typedef struct{
	uint16_t broad_s_addr;
	uint8_t broad_type;
	uint8_t broad_struct_type;
	uint8_t broad_chance;
	uint8_t broad_flag;
}broad_set;

typedef struct{
	//×Ó»úµÄ
	uint8_t frequency_setnum;
	uint8_t frequency_getnum;
	uint8_t frequency_self;
	//×Ô¼ºµÄ
	uint8_t high;
	uint8_t med;
	uint8_t low;
	uint8_t num;
}frequency_control;
void BC28_send(uint16_t s_addr , uint8_t type , uint8_t struct_type);
void BC28_recv(uint16_t s_addr , uint8_t type , uint8_t struct_type);
void BC28_data_pack(void);
void at_send_str(char* s1);
void at_send_data(uint8_t* val_p , uint8_t val_size);
void send_data_deliver(uint8_t* buffer , uint8_t* data_size_p_t , uint8_t struct_type);
void recv_data_deliver(uint8_t* buffer , uint8_t* data_size_p_r , uint8_t struct_type);
void connect_10m_call(void);
void fast_fresh(void);
void slow_fresh(void);
void fresh_struct_data(void);
void connect_process(void);
void connect_init(void);
void unresponse_resend(void);
void data_judge(void);
void send_reserve(uint8_t i);
void send_auto_pack(uint16_t addr, uint8_t type,uint8_t struct_type);
void SX1278LoRaSetRFFrequency_reset(uint8_t num);
void broad_send_model(void);
#endif //__SX1278_H__
