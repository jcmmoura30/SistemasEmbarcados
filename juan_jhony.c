// === CONFIGURAÇÃO DOS PINOS DO LCD ===
sbit LCD_RS at RE2_bit;
sbit LCD_EN at RE1_bit;
sbit LCD_D7 at RD7_bit;
sbit LCD_D6 at RD6_bit;
sbit LCD_D5 at RD5_bit;
sbit LCD_D4 at RD4_bit;

sbit LCD_RS_Direction at TRISE2_bit;
sbit LCD_EN_Direction at TRISE1_bit;
sbit LCD_D7_Direction at TRISD7_bit;
sbit LCD_D6_Direction at TRISD6_bit;
sbit LCD_D5_Direction at TRISD5_bit;
sbit LCD_D4_Direction at TRISD4_bit;

char textoADC[8];
char textoDuty[8];

void main() {
    unsigned int valorAD = 0;
    unsigned short duty = 0;
    float tensao = 0.0;

    // === CONFIGURAÇÕES INICIAIS ===
    TRISA = 0xFF;      // RA0 como entrada
    TRISB = 0x00;      // PORTB como saída para LEDs
    PORTB = 0x00;

    ADCON1 = 0x0E;     // AN0 como analógico, restante digital
    CMCON  = 0x07;     // Desabilita comparadores

    PWM1_Init(5000);   // Inicializa PWM com 5kHz
    PWM1_Start();      // Inicia PWM

    Lcd_Init();        // Inicializa LCD
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);

    // === LOOP PRINCIPAL ===
    while(1) {
        valorAD = ADC_Get_Sample(0);              // Lê trimpot
        tensao  = (valorAD * 5.0) / 1023.0;        // Converte em volts
        duty    = valorAD / 4;                     // Duty de 0–255

        PWM1_Set_Duty(duty);                       // Aplica PWM

        // === ATUALIZA LCD ===
        FloatToStr(tensao, textoADC);
        IntToStr(duty, textoDuty);

        Lcd_Out(1,1,"V: ");
        Lcd_Out_Cp(textoADC);
        Lcd_Out(2,1,"PWM: ");
        Lcd_Out_Cp(textoDuty);

        // === CONTROLE DOS 8 LEDS GRADATIVO ===
        PORTB = 0x00;

        if (duty >= 32)   PORTB.F0 = 1;
        if (duty >= 64)   PORTB.F1 = 1;
        if (duty >= 96)   PORTB.F2 = 1;
        if (duty >= 128)  PORTB.F3 = 1;
        if (duty >= 160)  PORTB.F4 = 1;
        if (duty >= 192)  PORTB.F5 = 1;
        if (duty >= 224)  PORTB.F6 = 1;
        if (duty == 255)  PORTB.F7 = 1;

        Delay_ms(200);
    }
}
