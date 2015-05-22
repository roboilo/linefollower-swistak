/*
 * gruszka2.c
 *
 * Created: 2015-03-07 10:36:39
 *  Author: roboilo
 */ 

#include <util/delay.h>
#include <avr/io.h>
#define czekaj(czas) for(int i=0;i<(czas);i++) _delay_ms(1); 
int flaga=0;
int main(void)
{
	DDRC = (1<<2); //ustawiamy piny na wyjście
	DDRB = (1<<0) | (1<<1) | (1<<2);//silnik prawy
	DDRD = (1<<5) | (1<<6) | (1<<7);//silnik lewy
	
	//silnik lewy
	PORTD = (1<<6);
	TCCR0A = (1<<COM0B1) | (1<<WGM02) | (1<<WGM00); //tryb
	TCCR0B = (1<<CS00); //czestotliwosc - prescaler
	
	OCR0B = 0; //pwm
	
		//silnik prawy
	PORTB = (1<<0);
	TCCR1A = (1<<COM1B1) | (1<<WGM10) ; //tryb
	TCCR1B = (1<<CS10); //czestotliwosc - prescaler
	
	OCR1B = 0; //pwm
	
	
	int l_p=0, blad = 0, pop_blad = 0, Kp=9, Kd =3, MOC_ZAD=400 ;
	//9 16 325
	//8 28 280
	//9 3 400
	void hamowanie_lewy ()
	{
		PORTD = (1<<6) | (1<<7);
	}
	
	void hamowanie_prawy ()
	{
		PORTB = (1<<0) | (1<<1);
	}
	
	void silnik_lewy (int moc_lewy)
	{

		if (moc_lewy<0)
		{PORTD = (1<<6) | (0<<7); OCR0B = -moc_lewy;}
		
		else if (moc_lewy>0)
		{PORTD = (0<<6) | (1<<7); OCR0B = moc_lewy;}
		
		else
		{PORTD = (0<<6) | (0<<7); OCR0B = 0;}
		
	}
	
	void silnik_prawy (int moc_prawy)
	{
		if (moc_prawy<0)
		{PORTB = (0<<0) | (1<<1); OCR1B = -moc_prawy;}
		
		else if (moc_prawy>0)
		{PORTB = (1<<0) | (0<<1); OCR1B = moc_prawy;}
		
		else
		{PORTB = (0<<0) | (0<<1); OCR1B = 0;}
		
	}
	
	void miganie (int opoznienie)
	{
		
		PORTC &= ~(1<<2);
		czekaj(opoznienie);
		PORTC |=(1<<2);
		czekaj(opoznienie);
	}
	
	void miganko_sprawdzanko()
	{
		if (PINC & (1<<3))
		miganie (45);
		if (PINC & (1<<4))
		miganie (40);
		if (PINC & (1<<1))
		miganie (35);
		if (PINC & (1<<0))
		miganie (30);
		if (PIND & (1<<1))
		miganie (25);
		if (PIND & (1<<0))
		miganie (20);
		if (PIND & (1<<2))
		miganie (15);
		if (PIND & (1<<3))
		miganie (10);
	}
	
	void ostry_lewo()
	{
		//hamowanie_prawy();
		//hamowanie_lewy();
		//czekaj (10);
		
		silnik_prawy(25);
		silnik_lewy(25);
		czekaj(16);
		
		//hamowanie_prawy();
		//hamowanie_lewy();
		//czekaj (10);
		
		while(!(PIND & (1<<1)))
		{silnik_lewy(-40);
			silnik_prawy(40);
		}
		
		hamowanie_lewy();
		hamowanie_prawy();
		czekaj (7);
	//tu skrecamy w lewo
	}
	
	void ostry_prawo()
	{
		
		//hamowanie_prawy();
		//hamowanie_lewy();
		//czekaj (10);
		
		silnik_prawy(25);
		silnik_lewy(25);
		czekaj(16);
		
		//hamowanie_prawy();
		//hamowanie_lewy();
		//czekaj (10);
		
		while(!(PINC & (1<<0)))
		{silnik_lewy(40);
			silnik_prawy(-40);
		}
		
		
		hamowanie_lewy();
		hamowanie_prawy();
		czekaj (7);//u skrecamy w prawo
	}
	
	int licz_blad()
	{
		int n=0;
		int odl=0;
		int p=0;
		
		int waga = 9;
		
		//if (flaga!=0 )
		//waga =5;
		
		if (PINC & (1<<3))
		{odl +=-4*waga; n++;}
		if (PINC & (1<<4))
		{odl +=-3*waga; n++;}
		if (PINC & (1<<1))
		{odl +=-2*waga; n++;}
		if (PINC & (1<<0))
		{odl +=-1*waga; n++;}
		if (PIND & (1<<1))
		{odl +=1*waga; n++;}
		if (PIND & (1<<0))
		{odl +=2*waga; n++;}
		if (PIND & (1<<2))
		{odl +=3*waga; n++;}
		if (PIND & (1<<3))
		{odl +=4*waga; n++;}
			else{}
			
			if (n != 0)
			{
				p = odl/n;
				l_p = p;
			}
			else{
				if (l_p <-35)
				{
					p=-40;
					flaga =1;//przestrzelenie w prawo
				}
				else if(l_p>35)
				{
					p=40;
					flaga =2;//przestrzelenie w lewo
				}
				else
				p=0;
				
			}
			
		if (flaga==1 && p>=0)
		{
			flaga==0;
		} 
		else if(flaga==2 && p<=0)
		{
			flaga==0;
		}
		return p;
	}
	
	int PD ()
	{
		int der = blad - pop_blad;
		pop_blad = blad;
		
		return Kp*blad + Kd*der;
		
	}
	

	//tu zaczyna się program!!!!!!!!!!!!!!!!!!!!!!!!!!!
	while(1){
		while (PINC &(1<<5)){
			miganie(30);
			} czekaj(100);
			
		while (PINC &(1<<5))
			{
				
			/*while(PINC & (1<<5))
			 {silnik_prawy(100);
				 silnik_lewy(100);
			 }*/
								
				if ( ( (PINC & (1<<3)) && (PINC & (1<<1)) )/* || ( (PINC & (1<<0)) && (PINC & (1<<4)) ) /*|| ( (PINC & (1<<0) ) && (PINC & (1<<4)) )*/ )
				{ostry_lewo(); l_p=0; flaga=0; blad = 0; pop_blad = 0; }
				if ( ( (PIND & (1<<3)) && (PIND & (1<<0)) ) /*|| ( (PIND & (1<<2)) && (PIND & (1<<1)) ) || ( (PIND & (1<<1) ) && (PIND & (1<<3 )) )*/ )
				{ostry_prawo(); l_p=0; flaga=0; blad = 0; pop_blad = 0;} 
				
				blad = licz_blad();
				int regulacja = PD();
				
				silnik_lewy((MOC_ZAD+regulacja)/4);
				silnik_prawy((MOC_ZAD-regulacja)/4);
				
			}
		silnik_prawy(0);
		silnik_lewy(0);
		czekaj(100);
		
    }
	}
	
