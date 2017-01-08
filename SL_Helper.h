
//Port-Macros
#define BIT(p,b)                (b) 
#define PORT(p,b)               (PORT ## p) 
#define PIN(p,b)                (PIN ## p) 
#define DDR(p,b)                (DDR ## p) 

#define Set_Bit_High(p,b)       ((p)  |=  (1 << b)) 
#define Set_Bit_Low(p,b)        ((p)  &= ~(1 << b)) 
#define Toggle_Bit(p,b)         ((p)  ^=  (1 << b)) 
#define Get_Bit(p,b)            (((p) &   (1 << b)) != 0) 

#define set_high(io)            Set_Bit_High(PORT(io),BIT(io)) 
#define set_low(io)             Set_Bit_Low(PORT(io),BIT(io)) 
#define toggle(io)              Toggle_Bit(PORT(io),BIT(io)) 

#define get_output(io)          Get_Bit(PORT(io),BIT(io)) 
#define get_input(io)           Get_Bit(PIN(io),BIT(io)) 

#define set_input(io)           Set_Bit_Low(DDR(io),BIT(io)) 
#define set_output(io)          Set_Bit_High(DDR(io),BIT(io)) 

