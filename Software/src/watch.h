#ifndef ene_watch_h
#define ene_watch_h

#include <GFXMatrix.h>


class Watch {
    
    public:
     
        Watch(GFXMatrix* display);
    
        void showtime(int std = 0 , int min = 0 , int sec =0 );   



    private:


        #define GRAY        0x7BEE
        #define SEC_COLOR   0xF814
        #define MIN_COLOR   0xBDF7
        #define HOUR_COLOR  0xBDF7


        GFXMatrix*   p_display;      

        uint16_t minColor;
        uint16_t hourColor;


        struct point {
            int x, y;
        }; 



        

        int second_vec = 0;
        int minute_vec = 0;
        int hour_vec = 0;

        uint16_t vec_len = 30; 

  
        point calc_target_point(float timesegment, int multiplier, float fac);
   
    

};


#endif //ene_watch_h