#include <watch.h>



Watch::Watch(GFXMatrix* display):p_display(display) {
    

     }




// calculates endpoint for line from origin
Watch::point Watch::calc_target_point(float timesegment, int multiplier, float fac) {

        point target;
        float vec = (timesegment * multiplier) -90;
        if (vec < 0) vec+=360;

        //char   tmpstr[50];
        //sprintf(tmpstr,"second= %.1f, vec= %.1f",timesegment,vec);
        //Serial.println(tmpstr);
        

        float rad = vec * PI / 180;
        target.x = vec_len * fac * cos(rad);
        target.y = vec_len * fac * sin(rad);
        
        return(target);

     }





void Watch::showtime(int std, int min , int sec ) {

    point origin = {31,31};
    point targetpt;
    point targetpt1;

    // draw circle around watch
    p_display->drawCircle(origin.x , origin.y , 31 ,GRAY);

    // draw marker 3 6 9 12 hour
    p_display->drawRect(origin.x-1  , 0            , 3, 3, SEC_COLOR);
    p_display->drawRect(origin.x-1  , 60           , 3, 3, SEC_COLOR);
    p_display->drawRect(0           , origin.y-1   , 3, 3, SEC_COLOR);
    p_display->drawRect(60          , origin.y-1   , 3, 3, SEC_COLOR);


    // draw marker every five minutes
    for (int i=0; i < 60; i++ ) {
      point p1, p2;
      if (i % 5 == 0) {
         float angle = (i * 6) -90;
         if (angle < 0) angle+=360;
         float rad = angle * PI / 180;
         p1.x = vec_len * 0.90 * cos(rad);
         p1.y = vec_len * 0.90*  sin(rad);

         p2.x = vec_len * cos(rad);
         p2.y = vec_len * sin(rad);

         p_display->drawLine(origin.x + p1.x, origin.y + p1.y, origin.x +p2.x, origin.y + p2.y, SEC_COLOR);
      }

      }
    
    // generate second pointer 
    targetpt = Watch::calc_target_point(sec, 6, 0.82);
    p_display->drawLine(origin.x , origin.y , origin.x + targetpt.x , origin.y + targetpt.y , SEC_COLOR);
    // generate minute pointer 
    targetpt = Watch::calc_target_point(min, 6, 0.74);
    p_display->drawLine(origin.x , origin.y , origin.x + targetpt.x , origin.y + targetpt.y , MIN_COLOR);
    p_display->drawLine(origin.x , origin.y+1 , origin.x + targetpt.x , origin.y + targetpt.y+1 , MIN_COLOR);
    p_display->drawLine(origin.x+1 , origin.y , origin.x+1 + targetpt.x , origin.y + targetpt.y , MIN_COLOR);
    // generate hour pointer 
    float mystd;
    if (std >=12) mystd= std - 12; else mystd= std;
    mystd = mystd + (min / 60.0f);
    targetpt = Watch::calc_target_point(mystd, 30, 0.5);
    p_display->drawLine(origin.x , origin.y , origin.x + targetpt.x , origin.y + targetpt.y , MIN_COLOR);
    p_display->drawLine(origin.x , origin.y+1 , origin.x + targetpt.x , origin.y + targetpt.y+1 , MIN_COLOR);
    p_display->drawLine(origin.x+1 , origin.y , origin.x+1 + targetpt.x , origin.y + targetpt.y , MIN_COLOR);
    

     }   

    


     
