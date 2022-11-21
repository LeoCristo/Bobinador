// main.c
// Desenvolvido para a placa EK-TM4C1294XL


#include <stdint.h>

void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void GPIO_Init(void);
uint32_t PortJ_Input(void);
void PortN_Output(uint32_t leds);
void Girar_motor(uint32_t sentido,uint32_t modo);
uint32_t Le_teclado();
void LCD_Comando(uint32_t comando);
void LCD_Init();
void LCD_Dado(uint32_t dado);
void LCD_String(char *String);
uint32_t get_dados(uint32_t *voltas,uint32_t *sentido,uint32_t *velocidade);
void mostrar_dados(uint32_t sentido,uint32_t velocidade,uint32_t voltas);
void mensagem_final();
void sequencia_leds(uint32_t sentido);
void GPIOPortJ_Handler();
uint32_t estado = 0;//estado inicial

int main(void)
{
	
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	LCD_Init();
	
	uint32_t voltas=0,sentido=0,velocidade=0, passos = 0;
	//get_dados(&voltas,&sentido,&velocidade);
	while (1)
	{
		switch(estado){
			case 0:
				//Resetar
				voltas=0,sentido=0,velocidade=0,passos = 0;
				estado = 1;
				break;
			
			case 1:
				//Mostrar menu perguntando quantidade de voltas, sentido de rotacao e velocidade
				estado = get_dados(&voltas,&sentido,&velocidade);
			  break;
			
			case 2:
				//Mostrar no display o sentido e a velocidade e número de voltas sendo decrementado
				mostrar_dados(sentido, velocidade, voltas);
				if(voltas>0){
					estado = 4;
				}else{
					estado = 5;
				}
				break;
				
			case 3:
				sequencia_leds(sentido);
				estado = 4;
				break;
			
			case 4:
				//girar o motor com os parametros especificados, tambem cria efeito nos leds a cada 45 graus 
			  Girar_motor(sentido,velocidade);
			
				if(velocidade == 0){
					//FULL-STEP 4 PASSOS/revolucao 250 passos para 45 graus
					//2048 PASSOS para 1 revolucao
					passos+=4;;
					if(passos%2048==0){
						voltas--;
						estado = 2;
					}else if(passos%250 == 0){
					//45 graus atingido, efeito nos leds
						estado = 3;
					}
				}else if(velocidade == 1){
					//HALF_STEP 8 PASSOS/revolucao 500 passos para 45 graus
					//4096 PASSOS para 1 revolucao
					passos+=8;
					if(passos%4096 == 0){
						voltas--;
						estado = 2;
					}else if(passos%500 == 0){
					//45 graus atingido, efeito nos leds
						estado = 3;
					}
				}
				
				break;
			
			case 5:
				//Processo finalizado, mostrar mensagem final
				mensagem_final();
				estado = 0;
				break;
			
		}
	}
}


//Deve-se solicitar o número de voltas de 1 a 10, o sentido de rotação (Horário ou
//Anti-horário), velocidade (Passo-completo ou meio-passo).
uint32_t get_dados(uint32_t *voltas,uint32_t *sentido,uint32_t *velocidade){

	uint32_t tecla = 0;
  *voltas=0;
	*sentido=0;
	*velocidade=0;
	LCD_Comando(0x01);
	LCD_String("-----Menu-----");
	LCD_Comando(0xC0);
	LCD_String("Voltas(1-10)>");
	LCD_Dado('0');
	LCD_Dado('0');
	LCD_Comando(0x10);//Decrementa o cursor
	
	while(tecla !='A'){
		tecla = Le_teclado();
		if(tecla == '*'){
			return 0;//Retorna para o estado inicial, o reset
		}
			
		if(tecla != 0){
			if(tecla=='1'||tecla=='2')//1 = (+1) 2 = (-1)
			{
				if(tecla=='1'&&(*voltas<10))
				{
					*voltas+=1;
				
				}

				if(tecla=='2'&&(*voltas>1)){
					if(*voltas==10){
						LCD_Comando(0x10);//Decrementa o cursor
						LCD_Dado('0');
					}
					*voltas-=1;
					
				}
				if(*voltas<10){
					LCD_Dado(*voltas+48);
					LCD_Comando(0x10);//Decrementa o cursor
				}else{
					LCD_Comando(0x10);//Decrementa o cursor
					LCD_Dado('1');
					LCD_Dado('0');
					LCD_Comando(0x10);//Decrementa o cursor

				}
				
		
			}
		}
	}
	tecla = 0;
	LCD_Comando(0x01);//Reseta o display
	LCD_String("anti-h =(1)");
	LCD_Comando(0xC0);
	LCD_String("horario=(0) >");
	LCD_Dado(48);
	LCD_Comando(0x10);//Decrementa o cursor
	while(tecla !='A'){
		tecla = Le_teclado(); 
		if(tecla == '*'){
			return 0;//Retorna para o estado inicial, o reset
		}
		if(tecla != 0){
			if(tecla=='1')//
			{
				if(*sentido==0){
					*sentido=1; 
				}else{
					*sentido=0; 
				}
				LCD_Dado(*sentido+48);
				LCD_Comando(0x10);//Decrementa o cursor
			}
		}
	}
//	Passo-completo ou meio-passo
	tecla = 0;
	LCD_Comando(0x01);//Reseta o display
	LCD_String("Full-Step=(0)");
	LCD_Comando(0xC0);
	LCD_String("Half-Step=(1)>");
	LCD_Dado(48);
	LCD_Comando(0x10);//Decrementa o cursor
	
	while(tecla !='A'){
		tecla = Le_teclado();
		if(tecla == '*'){
			return 0;//Retorna para o estado inicial, o reset
		}		
		if(tecla != 0){
			if(tecla=='1')//
			{
				if(*velocidade==0){
					*velocidade=1; 
				}else{
					*velocidade=0; 
				}
				LCD_Dado(*velocidade+48);
				LCD_Comando(0x10);//Decrementa o cursor
			}
		}
	}
	return 2;//Caso o usuario nao digite  *, o proximo estado é o 2
}