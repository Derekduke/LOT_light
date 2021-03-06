#include "main.h"
#include "board.h"
#include "uart.h"

Node_List kangjian[2];
extern UART_HandleTypeDef huart1;  //PC通信
extern UART_HandleTypeDef huart2;  //for nbiot bc28

//发送测试
extern send_infor send_info;
extern uint32_t start_time;
extern broad_set broad_val;
//
extern __IO ITStatus Uart1Ready_R;
extern __IO ITStatus Uart2Ready_R;
extern STRU_uart urt1;
extern STRU_uart urt2;
extern STRU_ur_data rd1;
extern STRU_ur_data rd2;
uint8_t Rx1Buffer[1];
uint8_t Rx2Buffer[1];

uint8_t arry[64];

uint8_t test_flag; 

void urt1RM06(void);
void urt1RM07(void);
void urt1RM08(void);

//数据接收处理线程，A5 5A NN DD DD DD EE //04 表示3个数据 单片机接收指令, 其中CC是校验码
//
//=================================================================
//                         for PC
//=================================================================
void HAL_UART_TxCpltCallback( UART_HandleTypeDef * UartHandle){
	if(UartHandle->Instance==USART1){
		//注意这种通信模式的稳健性，它的方法
	}
}
//接收中断函数，接收过后，启动串口线程
void HAL_UART_RxCpltCallback(UART_HandleTypeDef*UartHandle)
{ 
	if(UartHandle->Instance==USART1)
	{ //只要见到AA55AA55 就认为头开始
			uint32_t *p1,*p2;
			urt1.timenow = HAL_GetTick();
			if(urt1.timelast<urt1.timenow){
				urt1.step=0;
			}
			urt1.timelast = urt1.timenow+500;
			switch(urt1.step){
				case 0:
					if(Rx1Buffer[0]==0xA5){
							urt1.RxCount=0;
							urt1.Rxb[urt1.RxCount]=Rx1Buffer[0];					
							urt1.step=1;
					}else{urt1.step=0;}
					break;
				case 1:				
					if(Rx1Buffer[0]==0x5A){
								urt1.RxCount++;
								urt1.Rxb[urt1.RxCount]=Rx1Buffer[0];
								urt1.step=2;							
					}else{ urt1.step=0;}
					break;
				case 2:
							urt1.RxCount++;
							urt1.Rxb[urt1.RxCount]=Rx1Buffer[0];
							urt1.RxNum=Rx1Buffer[0]+3; //应该接收的数据
							urt1.step=3; 
					
					break;
				case 3:				
					urt1.RxCount++;	
					urt1.Rxb[urt1.RxCount]=Rx1Buffer[0];
					if(Rx1Buffer[0]==0xEE && urt1.RxCount==urt1.RxNum){
						//给定的接收数据刚好够了, 使用4字节指针，传输速度更快，刚好完成256字节传输
						p1=(uint32_t *)urt1.cmd; p2=(uint32_t *)urt1.Rxb;						
						for(urt1.RxNum=0;urt1.RxNum<64;urt1.RxNum++) p1[urt1.RxNum]=p2[urt1.RxNum];
						urt1.RxNew=1;
						urt1.step=0;
					}else{
						urt1.step=3;
					}
				break;
				default:  urt1.step=0; break;				
		}
		HAL_UART_Receive_IT(&huart1,Rx1Buffer,1);//开启下一次接收中断		
	}

		if(UartHandle->Instance==USART2)//
	{ 
		urt1SendChar(Rx2Buffer[0]);//收到什么立刻给串口1返回去
		
		urt2.timenow = HAL_GetTick();
		if(urt2.timelast<urt2.timenow){//超时500ms会清零
			urt2.step=0;
				urt2.RxCount=0;
			}
			urt2.timelast = urt2.timenow+500;
		
			urt2.Rxb[urt2.RxCount]=Rx2Buffer[0];
			urt2.RxCount++;
			
			
			switch(urt2.step){
				case 0:
					    //urt2.Rxb[urt2.RxCount]=Rx2Buffer[0]; //串口接收位置
							//urt2.RxCount++;
					if(Rx2Buffer[0]==0x0D){
							urt2.step=1;
					}else{urt2.step=0;}
					break;
					case 1:
					if(Rx2Buffer[0]==0x0A){
						
						urt2.step=0;
						urt2.RxNum=urt2.RxCount-2;
						urt2.RxCount=0;
						
if(urt2.RxNum>2&&urt2.RxNum<64) {
	
	nb.recv_valid_len=urt2.RxNum;
	for(int i=0;i<urt2.RxNum;i++){
		nb.recv[i]=urt2.Rxb[i];
	}
	//urt2.RxNew=1;
	nb.new_event=1;
}else{urt2.RxNew=0;}
						
						
						
						//urt1SendStr(urt2.Rxb,urt2.RxNum);
					}else{urt2.step=0;}
					break;
					default:
						break;
					
				}

	HAL_UART_Receive_IT(&huart2,Rx2Buffer,1);
	}
	
}


void th_uart1Manage(void)
{//板载232通信可以考虑以后扩展打印机
	if(urt1.RxNew<1) return;	urt1.RxNew=0;	
	switch(urt1.cmd[3]){
		case 0x00:
			break;
		case 0x01:
			break;
		case 0x02:
			break;
		case 0x03:
			break;
		case 0x04:
			break;
		case 0x05:
			break;
		case 0x06:
			urt1RM06();
			break;
		case 0x07:
			urt1RM07();
			break;
		case 0x08:
			urt1RM08();
			break;
				default:
			break;
	}
	//urt1SendChar(0xBE);
	//urt1_HeaderTailSend(arry,3);
}


void th_uart2Manage(void)
{//板载232通信可以考虑以后扩展打印机
	if(urt1.RxNew<1) return;	urt1.RxNew=0;	
	switch(urt1.cmd[3]){
		case 0x00:
			break;
		case 0x01:
			break;
		case 0x02:
			break;
		case 0x03:
			break;
		case 0x04:
			break;
		case 0x05:
			break;
		case 0x06:
		//	urt1RM06();
			break;
		case 0x07:
		//	urt1RM07();
			break;
	}
	urt1SendChar(0xBE);
	//urt1_HeaderTailSend(arry,3);
}



//将命令加上首与尾，并发送出去------------此函数非常有用啊
void urt1_HeaderTailSend(uint8_t *arr, uint8_t num){
	uint8_t ii;
	//加头，前3个数固定， 第的个是指令
	urt1.aTxb[0]=0xB5; urt1.aTxb[1]=0xB5; urt1.aTxb[2]=num;
	for(ii=0;ii<num;ii++){urt1.aTxb[3+ii]=arr[ii];}
	
	urt1.aTxb[3+num] = 0xEE;	
	urt1SendStr(urt1.aTxb, 4+num);
}

//接收过来的命令在这里进行处理



void urt1SendStr(uint8_t *a, uint8_t num){
	///HAL_UART_Transmit_IT(&huart1,(uint8_t*)a, num);
	HAL_UART_Transmit(&huart1,(uint8_t*)a, num , 0xffff);
}	
void urt1SendChar(uint8_t a){
	urt1.aTxb[0]=a;
	//HAL_UART_Transmit_IT(&huart1,(uint8_t*)urt1.aTxb,1);
	HAL_UART_Transmit(&huart1,(uint8_t*)urt1.aTxb,1 , 0xffff);
}	

void urt2SendStr(uint8_t *a, uint8_t num){
	//HAL_UART_Transmit_IT(&huart2,(uint8_t*)a, num);
	HAL_UART_Transmit(&huart2,(uint8_t*)a, num , 0xffff);
}	
void urt2SendChar(uint8_t a){
	urt2.aTxb[0]=a;
	HAL_UART_Transmit(&huart2,(uint8_t*)urt2.aTxb,1,0xffff);
}	


void uart_run_start(void){
	HAL_UART_Receive_IT(&huart1,Rx1Buffer,1);
	HAL_Delay(1);
	HAL_UART_Receive_IT(&huart2,Rx2Buffer,1);
}


//-----------------------------------------------------------------------------------------
//   MANAGE FUNCTIONS 
//-----------------------------------------------------------------------------------------

void urt1RM06(void){
	uint8_t temp;
	uint8_t t[16];
	uint8_t r[512];
	uint16_t size;
	switch(urt1.cmd[4]){
		case 0x00: //read			
			for(temp=0;temp<16;temp++){				
				SX1278Read(urt1.cmd[5]+temp, &t[temp]);			
			}
			urt1SendStr(t,16);			
			break;
		case 0x01: //write
			SX1278Write(urt1.cmd[5],urt1.cmd[6]);			
			urt1SendChar(0xff);	
			break;
		case 0x03: //buffer read			
		  SX1278ReadBuffer( 00, t, 3);
			urt1SendStr(t,8);		
			break;
		case 0x04: //buffer write
			SX1278WriteBuffer(0,t,16);	
			urt1SendChar(0xff);		
			break;
		case 0x05: //buffer write					
			urt1SendChar(SX1278Init());	

			break;
		case 0x06: //获得当前状态
			urt1SendChar(SX1278LoRaGetRFState());
			break;
		case 0x07: //获得当前模式
			urt1SendChar(SX1278LoRaGetOpMode());
			break;
		case 0x08: //RXSTART
			SX1278LoRaStartRx();
			break;
		case 0x09://测试数据发送 
//		  send_auto_pack(0x6745,0xC1,0x01);
			break;		
		case 0x10:
			SX1278LoRaGetRxPacket((void *)r, &size);
			if(size>0) urt1SendStr(r,size);
			break;
		
		
		/////////////////////////////多灯强制连接（EERPOM设置）
		case 0xf1:  //强制连接指令
			send_auto_pack(0x6745,0xC0,0xff);
			send_info.send_s_addr = 0x6745;
			send_info.send_type = 0xC0;
			send_info.send_struct_type = 0xff;
			send_info.send_flag = 1;
			send_info.send_chance = 3;
			send_info.send_time_s = 5;
			start_time = HAL_GetTick();
			break;
		case 0x11:
			send_auto_pack(0x0100,0xC0,0xff);
			send_info.send_s_addr = 0x0100;
			send_info.send_type = 0xC0;
			send_info.send_struct_type = 0xff;
			send_info.send_flag = 1;
			send_info.send_chance = 3;
			send_info.send_time_s = 5;
			start_time = HAL_GetTick();
			break;
		case 0x21:
			send_auto_pack(0x0200,0xC0,0xff);
			send_info.send_s_addr = 0x0200;
			send_info.send_type = 0xC0;
			send_info.send_struct_type = 0xff;
			send_info.send_flag = 1;
			send_info.send_chance = 3;
			send_info.send_time_s = 5;
			start_time = HAL_GetTick();
			break;
		case 0x31:
			send_auto_pack(0x0300,0xC0,0xff);
			send_info.send_s_addr = 0x0300;
			send_info.send_type = 0xC0;
			send_info.send_struct_type = 0xff;
			send_info.send_flag = 1;
			send_info.send_chance = 3;
			send_info.send_time_s = 5;
			start_time = HAL_GetTick();
			break;
		case 0x41:
			send_auto_pack(0x0400,0xC0,0xff);
			send_info.send_s_addr = 0x0400;
			send_info.send_type = 0xC0;
			send_info.send_struct_type = 0xff;
			send_info.send_flag = 1;
			send_info.send_chance = 3;
			send_info.send_time_s = 5;
			start_time = HAL_GetTick();
			break;
		case 0x51:
			send_auto_pack(0x0500,0xC0,0xff);
			send_info.send_s_addr = 0x0500;
			send_info.send_type = 0xC0;
			send_info.send_struct_type = 0xff;
			send_info.send_flag = 1;
			send_info.send_chance = 3;
			send_info.send_time_s = 5;
			start_time = HAL_GetTick();
			break;
		case 0x61:
			send_auto_pack(0x0600,0xC0,0xff);
			send_info.send_s_addr = 0x0600;
			send_info.send_type = 0xC0;
			send_info.send_struct_type = 0xff;
			send_info.send_flag = 1;
			send_info.send_chance = 3;
			send_info.send_time_s = 5;
			start_time = HAL_GetTick();
			break;
		////////////////////////////////////////////////////////////////
		case 0x12:
			//send_auto_pack(0xffff,0xC1,0x11);//广播设置亮�
			broad_val.broad_s_addr = 0xffff;
			broad_val.broad_type = 0xC1;
			broad_val.broad_struct_type = 0x11;
			broad_val.broad_chance = 3;
			broad_val.broad_flag = 1;
			break;
		case 0x13:
			send_auto_pack(0x6745,0xC1,0x01);//非广播设置亮度
			send_info.send_s_addr = 0x6745;
			send_info.send_type = 0xC1;
			send_info.send_struct_type = 0x01;
			send_info.send_flag = 1;
			send_info.send_chance = 3;
			send_info.send_time_s = 5;
			start_time = HAL_GetTick();
			break;
		case 0x14:
			send_auto_pack(0x6745,0xC1,0x03);//读取单个LF145C1参�
			//test_flag = 1;
			//nb.new_event=1;
			break;
		case 0x15:
			send_auto_pack(0x6745,0xC1,0x04);//读取当天最值
			break;
		case 0x16:
			//send_auto_pack(0x6745,0xC1,0x05);//读取前一天最�
			send_auto_pack(0x6745,0xC2,0xff);
			break;
		case 0x17:
			//send_auto_pack(0x6745,0xC1,0x0D);//读取之前某一天的历史数据
			//send_auto_pack(0xffff,0xC4,0xff);
			broad_val.broad_s_addr = 0xffff;
			broad_val.broad_type = 0xC4;
			broad_val.broad_struct_type = 0xff;
			broad_val.broad_chance = 3;
			broad_val.broad_flag = 1;
			break;
		
		case 0x18:
			send_auto_pack(0x6745,0xC1,0x07);//读取当日状态信息
			break;
		case 0x19:
			send_auto_pack(0x6745,0xC1,0x0C);//亮灯策略读取
			break;
		case 0x1a:
			send_auto_pack(0x6745,0xC1,0x0B);//亮灯策略设置
			break;
		case 0x1b:
			//send_auto_pack(0x6745,0xC1,0x09);//获取地址命令（10.1）
			SX1278LoRaSetRFFrequency_reset(22);
			urt1SendChar(0x22);
			break;
		case 0x1c:
			send_auto_pack(0x6745,0xC1,0x0E);//设置光控电压
			break;
		case 0x1d:
			send_auto_pack(0x6745,0xC1,0x1E);//查询光控电压
			break;
		case 0x1e:
			send_auto_pack(0x6745,0xC1,0x06);//读取客户信息
			break;
		case 0x1f:
			send_auto_pack(0x6745,0xC1,0x08);//写客户信息
			break;
	
	   }
}

void urt1RM07(void){//验证FM25L16-G存取的正确性，命令格式如说明书
	uint8_t ii;
	uint8_t buf;
	switch(urt1.cmd[4]){
		case 0x01:	//向存储器中写入数据（写入1到64）
			for(ii=0;ii<64;ii++) arry[ii]=0x01+ii;
			SPI_FM_Write(arry, 0, 64);
			break;		
		case 0x02:
			for(ii=0;ii<64;ii++) arry[ii]=0xbb;
			SPI_FM_Write(arry, 0, 64);
		break;
		case 0x03:
			for(ii=0;ii<64;ii++) arry[ii]=0xff;
			SPI_FM_Read(arry, 0, 64);
			urt1SendStr(arry, 64);
			break;
		case 0x04:
			buf=sizeof(kangjian[1]);
			urt1SendChar(buf);
			break;
	}
}

void urt1RM08(void){//验证BC28的通讯是否成功
	//uint8_t ii;
	switch(urt1.cmd[4]){
		case 0x01:	//向存储器中写入数据		
	 nb_initial();
			break;		
		case 0x02:
nb_Get_IMEI();
		break;
		case 0x03:
nb_Get_CSQ();
			break;
				case 0x04:
nb_Get_CIMI();
			break;
				case 0x05:
							
nb_Get_Operator();
				
			break;
				case 0x06:
nb_Test_Report();
			break;
			case 0x07:

			break;
				case 0x08:
			break;
	}
}
