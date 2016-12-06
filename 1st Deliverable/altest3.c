#include <stdio.h>
#include "allegro5/allegro.h"
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_native_dialog.h"
 
int main(){
 
  ALLEGRO_DISPLAY *display = NULL;
  ALLEGRO_BITMAP *lolo=NULL;
  ALLEGRO_TIMER *timer=NULL;
  ALLEGRO_EVENT_QUEUE *event_queue=NULL;
  int lolo_width=96;
  int lolo_height=96;
 
  if(!al_init()) {
     return 0;
  }
 
  if(!al_init_image_addon()) {
     return 0;
  }

 
  display = al_create_display(800,600);
  if(!display) {
    return 0;
  }


  event_queue=al_create_event_queue();
  if(!event_queue)
    return 0;

  timer=al_create_timer(1.0/60.0);
  if(!timer)
    return 0;

 
  lolo=al_load_bitmap("LotusStanceCutReflectionCut.png");
  al_convert_mask_to_alpha(lolo,al_map_rgb(174,10,68));
  
  if(!lolo)
    return 0;

  al_clear_to_color(al_map_rgb(0,0,0));

  al_register_event_source(event_queue,al_get_timer_event_source(timer));

  al_start_timer(timer);


  int i=0;
  int  location_x=200;
  while(1){
    ALLEGRO_EVENT ev;
    
    al_wait_for_event(event_queue,&ev);

    if(ev.type==ALLEGRO_EVENT_TIMER){

      if(i<5){
	al_draw_scaled_bitmap(lolo,lolo_width*i,0,lolo_width,lolo_height,200,200,80,80,0);
	i++;
      }else if(i<10){
	al_draw_scaled_bitmap(lolo,lolo_width*(i%5),lolo_height*1,lolo_width,lolo_height,200,200,80,80,0);
	i++;
      }else{
	al_draw_scaled_bitmap(lolo,lolo_width*(i%5),lolo_height*2,lolo_width,lolo_height,200,200,80,80,0);
	i++;
	if(i==12){
	  i=0;
	}
      }

    }
    //  al_set_target_bitmap(image2);
    //  al_clear_to_color(al_map_rgb(255,255,255));
    
    //  al_set_target_backbuffer(display);
    //  al_draw_bitmap(image2,0,0,0);


    
    
    
    al_flip_display();
    al_clear_to_color(al_map_rgb(0,0,0));
    if(al_get_timer_count(timer)>=1000){
      break;
    }
  }
		 
  al_destroy_display(display);
  al_destroy_bitmap(lolo);
 
  return 0;
}
