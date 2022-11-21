// gpio.c
// Desenvolvido para a placa EK-TM4C1294XL
// Inicializa as portas L, M e K


#include <stdint.h>

#include "tm4c1294ncpdt.h"

void SysTick_Wait1ms(uint32_t delay);
void LCD_Comando(uint32_t comando);
void LCD_Init();
void LCD_Dado(uint32_t dado);
void Girar_motor(uint32_t sentido,uint32_t modo);
void LCD_String(char *String);
void sequencia_leds(uint32_t sentido);
void mostrar_dados(uint32_t sentido,uint32_t velocidade,uint32_t voltas);
void mensagem_final();
void GPIOPortJ_Handler();
#define GPIO_PORTL  (0x0400) //bit 10
#define GPIO_PORTM  (0x0800) //bit 11
#define GPIO_PORTK	(0x0200) //bit 9
#define GPIO_PORTH  (0x0080) //bit 7
#define GPIO_PORTA  (0x0001) //bit 1
#define GPIO_PORTQ  (0x4000) //bit 14
#define GPIO_PORTP  (0x2000) //bit 13
#define GPIO_PORTJ  (0x0100) //bit 8

extern uint32_t estado, controle;


char TECLADO[] = {'1','4','7','*','2','5','8','0','3','6','9','#','A','B','C','D'};
// -------------------------------------------------------------------------------
// Função GPIO_Init
// Inicializa os ports L, M e K
// Parâmetro de entrada: Não tem
// Parâmetro de saída: Não tem
void GPIO_Init(void)
{
	//1a. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO
	SYSCTL_RCGCGPIO_R = (GPIO_PORTL | GPIO_PORTM | GPIO_PORTK | GPIO_PORTH | GPIO_PORTA | GPIO_PORTQ | GPIO_PORTP | GPIO_PORTJ);
	//1b.   após isso verificar no PRGPIO se a porta está pronta para uso.
  while((SYSCTL_PRGPIO_R & (GPIO_PORTL | GPIO_PORTM | GPIO_PORTK | GPIO_PORTH | GPIO_PORTA | GPIO_PORTQ| GPIO_PORTP | GPIO_PORTJ) ) != (GPIO_PORTL | GPIO_PORTM | GPIO_PORTK | GPIO_PORTH| GPIO_PORTA | GPIO_PORTQ | GPIO_PORTP | GPIO_PORTJ) ){};
	
	// 2. Limpar o AMSEL para desabilitar a analógica
	GPIO_PORTL_AMSEL_R = 0x00;
	GPIO_PORTM_AMSEL_R = 0x00;
	GPIO_PORTK_AMSEL_R = 0x00;
	GPIO_PORTH_AHB_AMSEL_R = 0x00;
	GPIO_PORTA_AHB_AMSEL_R = 0x00;
	GPIO_PORTQ_AMSEL_R = 0x00;
	GPIO_PORTP_AMSEL_R = 0x00;
	GPIO_PORTJ_AHB_AMSEL_R = 0x00;
	// 3. Limpar PCTL para selecionar o GPIO
	GPIO_PORTL_PCTL_R = 0x00;
	GPIO_PORTM_PCTL_R = 0x00;
	GPIO_PORTK_PCTL_R = 0x00;
	GPIO_PORTH_AHB_PCTL_R = 0x00;
	GPIO_PORTA_AHB_PCTL_R = 0x00;
	GPIO_PORTQ_PCTL_R = 0x00;
	GPIO_PORTP_PCTL_R = 0x00;
	GPIO_PORTJ_AHB_PCTL_R = 0x00;
	// 4. DIR para 0 se for entrada, 1 se for saída
	GPIO_PORTL_DIR_R = 0x00;
	GPIO_PORTM_DIR_R = 0xFF; 
	GPIO_PORTK_DIR_R = 0xFF;
	GPIO_PORTH_AHB_DIR_R = 0xFF;
	GPIO_PORTA_AHB_DIR_R = 0xFF;
	GPIO_PORTQ_DIR_R = 0xFF;
	GPIO_PORTP_DIR_R = 0xFF;
	GPIO_PORTJ_AHB_DIR_R = 0x00;
	// 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem função alternativa	
	GPIO_PORTL_AFSEL_R = 0x00;
	GPIO_PORTM_AFSEL_R = 0x00; 
	GPIO_PORTK_AFSEL_R = 0x00; 
	GPIO_PORTH_AHB_AFSEL_R = 0x00; 
	GPIO_PORTA_AHB_AFSEL_R = 0x00; 
	GPIO_PORTQ_AFSEL_R = 0x00; 
	GPIO_PORTP_AFSEL_R = 0x00; 
	GPIO_PORTJ_AHB_AFSEL_R = 0x00; 
	// 6. Setar os bits de DEN para habilitar I/O digital	
	GPIO_PORTL_DEN_R = 0x0F;   
	GPIO_PORTM_DEN_R = 0xFF; 	 
	GPIO_PORTK_DEN_R = 0xFF; 	
	GPIO_PORTH_AHB_DEN_R = 0xFF; 	
	GPIO_PORTA_AHB_DEN_R = 0xFF; 	
	GPIO_PORTQ_DEN_R = 0xFF; 	
	GPIO_PORTP_DEN_R = 0xFF; 	
	GPIO_PORTJ_AHB_DEN_R = 0xFF; 	
	// 7. Habilitar resistor de pull-down interno, setar PDR para 1
	GPIO_PORTL_PDR_R = 0x0F;   //Bit0 , bit1, bit 2 e bit 3
	GPIO_PORTA_AHB_PDR_R = 0xFF; 	
	GPIO_PORTQ_PDR_R = 0xFF; 
	GPIO_PORTP_PDR_R = 0xFF; 
	// 7. Habilitar resistor de pull-up interno, setar PUR para 1
	GPIO_PORTA_AHB_PUR_R = 0xFF; 	
	// ----------- Configuração de interrupções ----------------
	// 1. Desabilitando interrupcoes em J0
	GPIO_PORTJ_AHB_IM_R = 0x00;
	// 2. Escolhendo interrupcao por borda em J0 
	GPIO_PORTJ_AHB_IS_R = 0x00;
	// 3a. Escolhendo interrupcao por borda unica J0
	GPIO_PORTJ_AHB_IBE_R = 0x00;
	// 3b. Escolhendo interrupcao por borda de descida em J0
	GPIO_PORTJ_AHB_IEV_R = 0x00;
	// 4. Limpando RIS e MIS (ACK) por meio do ICR
	GPIO_PORTJ_AHB_ICR_R = (GPIO_PORTJ_AHB_ICR_R | 0x01);
	// 5. Habilitando interrupcoes em J0
	GPIO_PORTJ_AHB_IM_R = (GPIO_PORTJ_AHB_IM_R | 0x01);
	// 6. Habilitando a interrupcao do PORT J no NVIC 
	NVIC_EN1_R = 0x00080000;
	// 7. Escolhendo a prioridade da interrupcao do PORT J no NVIC prioridade 5
	NVIC_PRI12_R = 0x05<<29;
}	

// -------------------------------------------------------------------------------
// Função LCD_Init Inicializa o display 16x2
// Parâmetro de entrada: Não tem
// Parâmetro de saída: Não tem
void LCD_Init(){
	//Inicializar no modo 2 linhas / caractere matriz 5x7
	LCD_Comando(0x38);
	//Cursor com autoincremento para direita 
	LCD_Comando(0x06);
	//Configurar o cursor (habilitar o display + cursor + não-pisca)
	LCD_Comando(0x0E);
	//Limpar o display e levar o cursor para o home 
	LCD_Comando(0x01);
}

// -------------------------------------------------------------------------------
// Função LCD_Comando envia comandos para o display 16x2
// Parâmetro de entrada: comando
// Parâmetro de saída: Não tem
void LCD_Comando(uint32_t comando){
	//Colocar o LCD no modo de envio de comandos
	//PM2 PM1 PM0
	//E   RW   RS
  //1    0   0 
	GPIO_PORTK_DATA_R = comando;
	GPIO_PORTM_DATA_R = ((GPIO_PORTM_DATA_R & 0xF0)| 0x04);
	SysTick_Wait1ms(5);
	GPIO_PORTM_DATA_R = (GPIO_PORTM_DATA_R & 0xF0);
	SysTick_Wait1ms(5);
	GPIO_PORTM_DATA_R = ((GPIO_PORTM_DATA_R & 0xF0)| 0x04);
	SysTick_Wait1ms(5);
}	
// -------------------------------------------------------------------------------
// Função LCD_Comando envia dados para o display 16x2
// Parâmetro de entrada: dado
// Parâmetro de saída: Não tem
void LCD_Dado(uint32_t dado){
	//Colocar o LCD no modo de envio de dados
	//PM2 PM1 PM0
	//E   RW   RS
  //1    0   1
	GPIO_PORTK_DATA_R = dado;
	GPIO_PORTM_DATA_R = ((GPIO_PORTM_DATA_R & 0xF0)| 0x05);
	SysTick_Wait1ms(5);
	GPIO_PORTM_DATA_R = (GPIO_PORTM_DATA_R & 0xF0);
	SysTick_Wait1ms(5);
	GPIO_PORTM_DATA_R = ((GPIO_PORTM_DATA_R & 0xF0)| 0x01);
	SysTick_Wait1ms(5);
}

// -------------------------------------------------------------------------------
// Função LCD_String envia uma string para o display 16x2
// Parâmetro de entrada: string
// Parâmetro de saída: Não tem
void LCD_String(char *String){
	
	uint8_t index = 0;
	while(String[index]!=0){
			LCD_Dado(String[index]);
			index++;
	}
	
}



// -------------------------------------------------------------------------------
// Função Le_teclado matricial 4x4
// Detecta 1 tecla pressionada no teclado
// Parâmetro de entrada: Não tem
// Parâmetro de saída: a tecla pressionada em ascii, se 0 ,nada foi pressionado
// OBS.:Utiliza um vetor com o mapa do correspondete ascii do teclado
uint32_t Le_teclado(){
	
	uint32_t tecla = 0;
	uint8_t linha = 0;
	uint8_t coluna = 0;
	uint8_t leitura_linha = 0;
	uint8_t temp = 0;
	
	while(coluna<4){
		//Ativando a coluna e desativando as demais
		//Preservamos os primeiros 4 bits do port M com  & e a mascara 0x0F
		//Setamos o bit correspondente utilizando a mascara 2_0001 0000 desloado coluna vezes com  |
		GPIO_PORTM_DATA_R = ((GPIO_PORTM_DATA_R & 0x0F)|(0x10 << coluna));
		//Leitura da linha
		//Caso a linha tiver valor maior que zero, sai do loop, ou caso o loop exceder o numeros de linhas
		leitura_linha = 0;
		linha = 0;
		while(linha<4 && leitura_linha==0){
			// Lendo os primeiros 4 bits que sao as 4 linhas
			leitura_linha = (GPIO_PORTL_DATA_R & (0x01 << linha));
			linha++;
		}
		linha--;
		if(leitura_linha>0){
			//Tecla detectada pressionada
			//Rotina de debouncing
			SysTick_Wait1ms(50);
			temp = (GPIO_PORTL_DATA_R & (0x01 << linha));
			if(temp == leitura_linha){
				//Leitura estavel
				//Espera o botao ser solto
				while(leitura_linha!=0){
					leitura_linha = (GPIO_PORTL_DATA_R & (0x01 << linha));
				}
				tecla = TECLADO[coluna*4+linha];
				return tecla;
			}else{
				//
			}
				
		}
		coluna++;
		
	}
		
	return tecla;
}

// -------------------------------------------------------------------------------
// Função Girar_motor gira o motor com o sentido horario ou anti horario, e tambem com modo full-step ou half-step
// Detecta 1 tecla pressionada no teclado
// Parâmetro de entrada: sentido = 0 horario, sentido = 1 anti-horario, modo = 0 full-step, modo = 1 half_step 
// Parâmetro de saída: Não tem
void Girar_motor(uint32_t sentido,uint32_t modo){
		//                             sentido = 0 horario modo = 0 full step
		//        AZUL ROXO AMARELO LARANJA                           FULL STEP 2 FASES LIGADAS POR VEZ
	  //PASSO   H3    H2   H1       H0       - PORTH
	  //	 			1     2    3        4   - BOBINAS
		//1 		  1     1    0        0
		//2  			0     1    1        0
	  //3  			0     0    1        1
	  //4  			1     0    0        1
		//HALF STEP 1 ou 2 FASES LIGADAS POR VEZ				  
	  // PASSO	1     2    3        4   - BOBINAS
		// 1			1   	0  	 0  			0    	0x08
		// 2			1   	1  	 0  			0 	 	0x0C	
	  // 3			0   	1  	 0  			0	 		0x04
	  // 4			0  	  1 	 1  			0			0x06	
		// 5			0  	  0  	 1  			0   	0x02
		// 6			0   	0 	 1  			1   	0x03
	  // 7			1  	  0 	 0  			1   	0x09
	  // 8			1   	0 	 0  			0   	0x08
		//voltas = 2048 passos para o modo full-step dar uma volta 32x64
		//voltas = 4096 passos para o modo half-step dar uma volta 64x64
	
	
		uint32_t step_delay = 10;
	  if(modo == 0){
			//passos = 2048*temp;
			
			if(sentido == 0){
					GPIO_PORTH_AHB_DATA_R = 0x0C;
					SysTick_Wait1ms(step_delay);
					GPIO_PORTH_AHB_DATA_R = 0x06;
					SysTick_Wait1ms(step_delay);
					GPIO_PORTH_AHB_DATA_R = 0x03;
					SysTick_Wait1ms(step_delay);
					GPIO_PORTH_AHB_DATA_R = 0x09;
					SysTick_Wait1ms(step_delay);
				
			}
			if(sentido == 1){
			
					GPIO_PORTH_AHB_DATA_R = 0x09;
					SysTick_Wait1ms(step_delay);
					GPIO_PORTH_AHB_DATA_R = 0x03;
					SysTick_Wait1ms(step_delay);
					GPIO_PORTH_AHB_DATA_R = 0x06;
					SysTick_Wait1ms(step_delay);
					GPIO_PORTH_AHB_DATA_R = 0x0C;
					SysTick_Wait1ms(step_delay);
				
			}
		}
		if(modo == 1){
			
			if(sentido == 0){
			
					GPIO_PORTH_AHB_DATA_R = 0x08;
					SysTick_Wait1ms(step_delay);
					GPIO_PORTH_AHB_DATA_R = 0x0C;
					SysTick_Wait1ms(step_delay);
					GPIO_PORTH_AHB_DATA_R = 0x04;
					SysTick_Wait1ms(step_delay);
					GPIO_PORTH_AHB_DATA_R = 0x06;
					SysTick_Wait1ms(step_delay);
					GPIO_PORTH_AHB_DATA_R = 0x02;
					SysTick_Wait1ms(step_delay);
					GPIO_PORTH_AHB_DATA_R = 0x03;
					SysTick_Wait1ms(step_delay);
					GPIO_PORTH_AHB_DATA_R = 0x09;
					SysTick_Wait1ms(step_delay);
					GPIO_PORTH_AHB_DATA_R = 0x08;
					SysTick_Wait1ms(step_delay);
				
				
			}
			if(sentido == 1){
			
					GPIO_PORTH_AHB_DATA_R = 0x08;
					SysTick_Wait1ms(step_delay);
					GPIO_PORTH_AHB_DATA_R = 0x09;
					SysTick_Wait1ms(step_delay);
					GPIO_PORTH_AHB_DATA_R = 0x03;
					SysTick_Wait1ms(step_delay);
					GPIO_PORTH_AHB_DATA_R = 0x02;
					SysTick_Wait1ms(step_delay);
					GPIO_PORTH_AHB_DATA_R = 0x06;
					SysTick_Wait1ms(step_delay);
					GPIO_PORTH_AHB_DATA_R = 0x04;
					SysTick_Wait1ms(step_delay);
					GPIO_PORTH_AHB_DATA_R = 0x0C;
					SysTick_Wait1ms(step_delay);
					GPIO_PORTH_AHB_DATA_R = 0x08;
					SysTick_Wait1ms(step_delay);
					
				
			}
		}
		

}

void mostrar_dados(uint32_t sentido,uint32_t velocidade,uint32_t voltas){
	LCD_Comando(0x01); // Limpa o display
	LCD_String("Sentido>");
	if(sentido==0){
		LCD_String("Horario");
	}
	if(sentido==1){
		LCD_String("Anti-H");
	}
	LCD_Comando(0xC0);
	LCD_String("Vel>");
	if(velocidade==0){
		LCD_String("FULL");
	}
	if(velocidade==1){
		LCD_String("HALF");
	}
	LCD_String("V>");
	if(voltas<10){
			LCD_Dado('0');
		  LCD_Dado(voltas+48);
			LCD_Comando(0x10);//Decrementa o cursor
			LCD_Comando(0x10);//Decrementa o cursor
	}else{
				LCD_Dado('1');
				LCD_Dado('0');
				LCD_Comando(0x10);//Decrementa o cursor
				LCD_Comando(0x10);//Decrementa o cursor
	}
	
}

void mensagem_final(){
	LCD_Comando(0x01); // Limpa o display
	LCD_String("-----FIM-----");
	uint32_t tecla;
	while(tecla != '*'){
			tecla = Le_teclado();
			//Retorna para o estado inicial, o reset
	}
}
void sequencia_leds(uint32_t sentido){
	uint32_t  aux = 0;
	GPIO_PORTA_AHB_DATA_R = 0;
	GPIO_PORTQ_DATA_R = 0;
	GPIO_PORTP_DATA_R = 0x20;
	if(sentido == 0){
	//Horario Ligar de A7...A4 ate Q3...Q0
		while(aux<5){
			GPIO_PORTA_AHB_DATA_R = (0x10>>aux);
			SysTick_Wait1ms(50);
			aux++;
		}
		aux = 0;
		while(aux<5){
			GPIO_PORTQ_DATA_R = (0x01>>aux);
			SysTick_Wait1ms(50);
			aux++;
		}
	}
	if(sentido == 1){
		//Anti-horario Ligar de Q0...Q3 ate A4...A7
		while(aux<5){
			GPIO_PORTQ_DATA_R = (0x01<<aux);
			SysTick_Wait1ms(50);
			aux++;
		}
		aux = 0;
		while(aux<5){
			GPIO_PORTA_AHB_DATA_R = (0x10<<aux);
			SysTick_Wait1ms(50);
			aux++;
		}
	}

}

void GPIOPortJ_Handler(){
	//Testando se J0 causou a interrupcao
	// J0 força o retorno para o estado 0
	if((GPIO_PORTJ_AHB_RIS_R & 0x01) == 0x01){
	//J0 causou a interrupcao
		controle = 0;
		estado = 0;
	}
	GPIO_PORTJ_AHB_ICR_R = 0x01;
}




