
/*  Nestor Napoles 2012
 *
 *  [Contribución de Emilio Erándu Ceja Cárdenas (ver línea 370)]*/

#include<stdio.h>
#include<allegro5/allegro.h>
#include<allegro5/allegro_image.h>
#include<allegro5/allegro_font.h>
#include<allegro5/allegro_ttf.h>
#include<allegro5/allegro_primitives.h>

#define FPS 60.0

#define GRASS 0
#define OBSTACLE 1
#define HOUSE 2
#define MAXWEIGHT 250
#define BEES 5
#define CHAOTIC 3
#define LEFT -1
#define RIGHT 1
#define STAY 0
#define DOWN 1
#define UP -1

#define PROPAGATE 0
#define RETURN 1
#define REALLOCATE 2
#define WAIT 3


#define ceil(x) ((float)(x)>(int)(x)?(int)(x)+1:(int)(x))
#define max(x,y) ((x)>(y)?(x):(y))
//#define abs(x) ((x)>0?(x):-(x))
 
struct square{
  float x;
  float y;
  int weight;
  int type;
};

struct bee{
  int i;
  int j;
  int weight;
};

enum keys{
  MOUSE_LEFT_BUTTON,MOUSE_RIGHT_BUTTON,ALT_KEY,CTRL_KEY,SHIFT_KEY,PGUP_KEY,PGDN_KEY
};

void cooling_cell(int g_rows,int g_cols,struct square grid[][g_cols],int board_buffer[BEES][3]){
  int i,j,b[5]={0,0,0,0,0},delta=MAXWEIGHT/max(g_cols,g_rows);
  for(i=0;i<5;i++){
    for(j=0;j<5;j++){
      if(board_buffer[i][0]==board_buffer[j][0]&&
	 board_buffer[i][1]==board_buffer[j][1]){
	b[i]++;
      }
    }
  }
  for(i=0;i<5;i++){
    printf("%d %d\n",i,b[i]);
    if(b[i]>1){
      if(grid[board_buffer[i][0]][board_buffer[i][1]].weight>delta){
	grid[board_buffer[i][0]][board_buffer[i][1]].weight-=delta;
      }
    }
    if(b[i]>2){
      if(grid[board_buffer[i][0]][board_buffer[i][1]].weight>delta){
	grid[board_buffer[i][0]][board_buffer[i][1]].weight-=delta;
      }
    }    
  }
}



int bee_board(int root_i,int root_j,struct bee bees[BEES]){
  int winner=0,i;
  
  for(i=1;i<5;i++){
    if(bees[winner].weight>bees[i].weight){
      winner=winner;
    }else if(bees[winner].weight<bees[i].weight){
      winner=i;
    }else{
      winner=rand()%2==0?winner:i;
    }
  }
  return winner;
}
    
  
  /*
 /  if(w0>=w1){
     winner=kp;
  }
  */


void bee_propagation(int g_rows,int g_cols,int environment,int root_i,int root_j,struct square grid[][g_cols],struct bee bees[BEES]){
  int next_bee;
  int all_bees_ready=0;
  int r=rand()%4;
  int tries=0;
  int mask[8]={1,4,16,64,2,8,32,128};
  //int mask[8]={1,2,4,8,16,32,64,128};
  if(r>0){
    mask[4]=r==1?8:r==2?32:128;
    mask[5]=r==1?32:r==2?128:2;
    mask[6]=r==1?128:r==2?2:8;
    mask[7]=r==1?2:r==2?8:32;
  }
  
  for(next_bee=0;next_bee<5;next_bee++){
    bees[next_bee].i=root_i;
    bees[next_bee].j=root_j;
    bees[next_bee].weight=0;
  }

  next_bee=0;
  while(tries<8&&!all_bees_ready){
    if(environment&mask[tries]){
      //      printf("%d&%d:%d\n",environment,mask[tries],environment&mask[tries]);
      switch(mask[tries]){
      case 128:
	bees[next_bee].i=root_i;
	bees[next_bee].j=root_j+LEFT;
	break;
      case 64:
	bees[next_bee].i=root_i+DOWN;
	bees[next_bee].j=root_j+LEFT;
	break;
      case 32:
	bees[next_bee].i=root_i+DOWN;
	bees[next_bee].j=root_j;
	break;
      case 16:
	bees[next_bee].i=root_i+DOWN;
	bees[next_bee].j=root_j+RIGHT;
	break;
      case 8:
	bees[next_bee].i=root_i;
	bees[next_bee].j=root_j+RIGHT;
	break;
      case 4:
	bees[next_bee].i=root_i+UP;
	bees[next_bee].j=root_j+RIGHT;
	break;
      case 2:
	bees[next_bee].i=root_i+UP;
	bees[next_bee].j=root_j;
	break;
      case 1:
	bees[next_bee].i=root_i+UP;
	bees[next_bee].j=root_j+LEFT;
	break;
      }
      bees[next_bee].weight=grid[bees[next_bee].i][bees[next_bee].j].weight;
      next_bee++;
    }
    tries++;
    if(next_bee==5){
      all_bees_ready=1;
    }
  }
}

int environment_calculate(int g_rows,int g_cols,int root_i,int root_j,struct square grid[][g_cols]){
  int sum=0;
  int tmp_i=root_i+UP;
  int tmp_j=root_j+LEFT;
  if(tmp_i>=0&&tmp_i<=g_rows-1&&
     tmp_j>=0&&tmp_j<=g_cols-1&&
     grid[tmp_i][tmp_j].type!=OBSTACLE){     
    sum+=1;
  }
  tmp_i=root_i+UP;
  tmp_j=root_j;
  if(tmp_i>=0&&tmp_i<=g_rows-1&&
     tmp_j>=0&&tmp_j<=g_cols-1&&
     grid[tmp_i][tmp_j].type!=OBSTACLE){
    sum+=2;
  }
  tmp_i=root_i+UP;
  tmp_j=root_j+RIGHT;
  if(tmp_i>=0&&tmp_i<=g_rows-1&&
     tmp_j>=0&&tmp_j<=g_cols-1&&
     grid[tmp_i][tmp_j].type!=OBSTACLE){
    sum+=4;
  }
  tmp_i=root_i;
  tmp_j=root_j+RIGHT;
  if(tmp_i>=0&&tmp_i<=g_rows-1&&
     tmp_j>=0&&tmp_j<=g_cols-1&&
     grid[tmp_i][tmp_j].type!=OBSTACLE){
    sum+=8;
  }
  tmp_i=root_i+DOWN;
  tmp_j=root_j+RIGHT;
  if(tmp_i>=0&&tmp_i<=g_rows-1&&
     tmp_j>=0&&tmp_j<=g_cols-1&&
     grid[tmp_i][tmp_j].type!=OBSTACLE){
    sum+=16;
  }
  tmp_i=root_i+DOWN;
  tmp_j=root_j;
  if(tmp_i>=0&&tmp_i<=g_rows-1&&
     tmp_j>=0&&tmp_j<=g_cols-1&&
     grid[tmp_i][tmp_j].type!=OBSTACLE){
    sum+=32;
  }
  tmp_i=root_i+DOWN;
  tmp_j=root_j+LEFT;
  if(tmp_i>=0&&tmp_i<=g_rows-1&&
     tmp_j>=0&&tmp_j<=g_cols-1&&
     grid[tmp_i][tmp_j].type!=OBSTACLE){
    sum+=64;
  }
  tmp_i=root_i;
  tmp_j=root_j+LEFT;
  if(tmp_i>=0&&tmp_i<=g_rows-1&&
     tmp_j>=0&&tmp_j<=g_cols-1&&
     grid[tmp_i][tmp_j].type!=OBSTACLE){
    sum+=128;
  }
  return sum;
}
  




void set_grid_weight(int g_rows,int g_cols,int house_x,int house_y,struct square grid[][g_cols]){
  int i,j,ring;
  int delta=MAXWEIGHT/max(g_rows,g_cols);
  for(i=0;i<g_rows;i++){
    for(j=0;j<g_cols;j++){
      ring=max(abs(house_x-i),abs(house_y-j));
      grid[i][j].weight=MAXWEIGHT-delta*ring;
      //      printf("[%d,%d].weight=%d\n",i,j,grid[i][j].weight);
    }
  }
}

init_weights(int g_rows,int g_cols,struct square grid[][g_cols]){
  int i,j;
  for(i=0;i<g_rows;i++){
    for(j=0;j<g_cols;j++){
      grid[i][j].weight=100;
    }
  }
}

void randomize_obstacles(int g_rows,int g_cols,float res_x,float res_y,struct square grid[][g_cols],int percent){
  int i,j,r;
  for(i=0;i<g_rows;i++){
    for(j=0;j<g_cols;j++){
      r=rand()%100;
      if(r>0&&r<=percent){
	grid[i][j].type=OBSTACLE;
      }else{
	grid[i][j].type=GRASS;
      }
    }
  }
}

void initgrid(int g_rows, int g_cols,float res_x,float res_y,struct square grid[][g_cols]){
  int i,j;
  float square_side_x=res_x/g_cols;
  float square_side_y=res_y/g_rows;
  printf("g_rows:%f\ng_cols:%f\nres_x:%f\nres_y:%f\n",g_rows,g_cols,res_x,res_y);
  for(i=0;i<g_rows;i++){
    for(j=0;j<g_cols;j++){
      grid[i][j].x=square_side_x*j;
      grid[i][j].y=square_side_y*i;
      printf("[%d,%d]\tx=%.1f,y=%.1f\n",i,j,grid[i][j].x,grid[i][j].y);
    }
  }
}

void get_grid_pos(int g_rows,int g_cols,float res_x,float res_y,struct square grid[][g_cols],int mouse_x,int mouse_y,int *click_row,int *click_col){
  int i,j;
  for(i=0;i<g_rows;i++){
    for(j=0;j<g_cols;j++){
      if((mouse_x>=grid[i][j].x&&mouse_x<=(grid[i][j].x+res_x/g_cols))&&
	 (mouse_y>=grid[i][j].y&&mouse_y<=(grid[i][j].y+res_y/g_rows))){
	*click_row=i;
	*click_col=j;
      }
    }
  }
}


void line(int x0,int y0,int x1,int y1,int *route_line_i,int *route_line_j){
  int dx=abs(x1-x0),sx=x0<x1?1:-1;
  int dy=abs(y1-y0),sy=y0<y1?1:-1; 
  int err=(dx>dy?dx:-dy)/2,e2;

  if(x0==x1&&y0==y1){
    *route_line_i=-1;
    *route_line_j=-1;
    return;
  }
  e2=err;
  if(e2>-dx){ 
    err-=dy; 
    x0+=sx; 
  }
  if(e2<dy){
    err += dx; y0 += sy; 
  }
  *route_line_i=x0;
  *route_line_j=y0;
}

void recalculate_next_step(int kibus_i,int kibus_j,int *kibus_next_i,int *kibus_next_j,int emergent_reaction){
  int i=(*kibus_next_i)-kibus_i;
  int j=(*kibus_next_j)-kibus_j;
  if(emergent_reaction==LEFT){
    if(i==UP&&j==STAY){
      *kibus_next_i=kibus_i+UP;
      *kibus_next_j=kibus_j+LEFT;
    }else if(i==UP&&j==LEFT){
      *kibus_next_i=kibus_i+STAY;
      *kibus_next_j=kibus_j+LEFT;
    }else if(i==STAY&&j==LEFT){
      *kibus_next_i=kibus_i+DOWN;
      *kibus_next_j=kibus_j+LEFT;
    }else if(i==DOWN&&j==LEFT){
      *kibus_next_i=kibus_i+DOWN;
      *kibus_next_j=kibus_j+STAY;
    }else if(i==DOWN&&j==STAY){
      *kibus_next_i=kibus_i+DOWN;
      *kibus_next_j=kibus_j+RIGHT;
    }else if(i==DOWN&&j==RIGHT){
      *kibus_next_i=kibus_i+STAY;
      *kibus_next_j=kibus_j+RIGHT;
    }else if(i==STAY&&j==RIGHT){
      *kibus_next_i=kibus_i+UP;
      *kibus_next_j=kibus_j+RIGHT;
    }else if(i==UP&&j==RIGHT){
      *kibus_next_i=kibus_i+UP;
      *kibus_next_j=kibus_j+STAY;
    }
  }else if(emergent_reaction==RIGHT){
    if(i==UP&&j==STAY){
      *kibus_next_i=kibus_i+UP;
      *kibus_next_j=kibus_j+RIGHT;
    }else if(i==UP&&j==RIGHT){
      *kibus_next_i=kibus_i+STAY;
      *kibus_next_j=kibus_j+RIGHT;
    }else if(i==STAY&&j==RIGHT){
      *kibus_next_i=kibus_i+DOWN;
      *kibus_next_j=kibus_j+RIGHT;
    }else if(i==DOWN&&j==RIGHT){
      *kibus_next_i=kibus_i+DOWN;
      *kibus_next_j=kibus_j+STAY;
    }else if(i==DOWN&&j==STAY){
      *kibus_next_i=kibus_i+DOWN;
      *kibus_next_j=kibus_j+LEFT;
    }else if(i==DOWN&&j==LEFT){
      *kibus_next_i=kibus_i+STAY;
      *kibus_next_j=kibus_j+LEFT;
    }else if(i==STAY&&j==LEFT){
      *kibus_next_i=kibus_i+UP;
      *kibus_next_j=kibus_j+LEFT;
    }else if(i==UP&&j==LEFT){
      *kibus_next_i=kibus_i+UP;
      *kibus_next_j=kibus_j+STAY;
    }
  }else if(emergent_reaction==CHAOTIC){
    if(i==UP&&j==STAY){
      *kibus_next_i=kibus_i+UP;
      *kibus_next_j=kibus_j+RIGHT;
    }else if(i==UP&&j==RIGHT){
      *kibus_next_i=kibus_i+STAY;
      *kibus_next_j=kibus_j+RIGHT;
    }else if(i==STAY&&j==RIGHT){
      *kibus_next_i=kibus_i+DOWN;
      *kibus_next_j=kibus_j+RIGHT;
    }else if(i==DOWN&&j==RIGHT){
      *kibus_next_i=kibus_i+DOWN;
      *kibus_next_j=kibus_j+STAY;
    }else if(i==DOWN&&j==STAY){
      *kibus_next_i=kibus_i+DOWN;
      *kibus_next_j=kibus_j+LEFT;
    }else if(i==DOWN&&j==LEFT){
      *kibus_next_i=kibus_i+STAY;
      *kibus_next_j=kibus_j+LEFT;
    }else if(i==STAY&&j==LEFT){
      *kibus_next_i=kibus_i+UP;
      *kibus_next_j=kibus_j+LEFT;
    }else if(i==UP&&j==LEFT){
      *kibus_next_i=kibus_i+UP;
      *kibus_next_j=kibus_j+STAY;
    }
  }    
}
  



void kibus_next_step(int g_rows,int g_cols,int *kibus_i,int *kibus_j,struct square grid[][g_cols],int *last_step_i,int *last_step_j,int route_line_i,int route_line_j,int emergent_reaction){
  
  int starting_direction_i=route_line_i-(*kibus_i);
  int starting_direction_j=route_line_j-(*kibus_j);
  int kibus_next_i=(*kibus_i)+starting_direction_i;
  int kibus_next_j=(*kibus_j)+starting_direction_j;
  int recalculations=0;

  while(recalculations<8){
    if(grid[kibus_next_i][kibus_next_j].type==OBSTACLE||
       ((*last_step_i)==kibus_next_i&&(*last_step_j)==kibus_next_j)||
       kibus_next_i>g_rows-1||kibus_next_i<0||kibus_next_j>g_cols-1||kibus_next_j<0){     
      recalculate_next_step(*kibus_i,*kibus_j,&kibus_next_i,&kibus_next_j,emergent_reaction);
      recalculations++;
    }else{
      *last_step_i=(*kibus_i);
      *last_step_j=(*kibus_j);
      *kibus_i=kibus_next_i;
      *kibus_j=kibus_next_j;
      break;
    }
  }
  if(recalculations==8){
    *last_step_i=(*kibus_i);
    *last_step_j=(*kibus_j);
    *kibus_i=(*last_step_i);
    *kibus_j=(*last_step_j);
  }
}
  


int main(int argc,char *argv[]){
  srand(time(NULL));
  struct bee bees[BEES];
  float res_x=640.0,res_y=480.0;
  int g_rows=15,g_cols=15;
  char text_message[100];
  int board_buffer[BEES][3];
  int winner_bee=-1;
  int bees_behavior=-1;
  int route_line_i=-1;
  int route_line_j=-1;
  int house_i=-1;
  int house_j=-1;
  int aux_house_i=-1;
  int aux_house_j=-1;
  int mouse_x=0;
  int mouse_y=0;
  int kibus_i=-1;
  int kibus_j=-1;
  int bees_root_i=-1;
  int bees_root_j=-1;
  int kibus_last_i=-1;
  int kibus_last_j=-1;
  int kibus_side=96;
  int bee_side=140;
  int bee_step=0;
  int bee_wait_delay=0;
  int kibus_return_delay=0;
  int random_map_delay=0;
  int text_delay=0;
  int obstacle_percent=0;
  int emergent_reaction=LEFT;
  int maximum_steps;
  int current_steps=0;
  int emergent_ccounter=0;
  int environment=0;
  


  if(argc==1||argc==2||argc==3){//parse gridsize & resolution from arguments
    if(argc>=2){ 
      sscanf(argv[1],"%dx%d\n",&g_rows,&g_cols);
      if(g_rows<1||g_rows<1){
	return 0;
      }
    }
    if(argc==3){
      sscanf(argv[2],"%fx%f\n",&res_x,&res_y);
    }
  }else{
    printf("Usages:\t\tprogram\n\t\tprogram <GridRows>x<GridCols> [<ResX>x<ResY>]\nExample:\tprogram 20x20 800x600\n\n\nDefault values for program:15x15 640x480\nNestor Napoles 2012\n");
    return 0;
  }
  
  //  printf("Ladox:%f,Ladoy=%f\n",square_side_x,square_side_y);
 

  //Initializing ALLEGRO5 and ALLEGRO components
  ALLEGRO_DISPLAY *display=NULL;
  ALLEGRO_EVENT_QUEUE *event_queue=NULL;
  ALLEGRO_TIMER *timer=NULL;
  ALLEGRO_BITMAP *bitmaps[3];
  ALLEGRO_BITMAP *kibus,*bee;
  ALLEGRO_FONT *font;
  bool redraw=false;
  bool set_text=true;
  bool set_house=false;
  bool set_obstacle=false;
  bool set_random_map=false;
  bool set_kibus=false;
  bool random_map_text=false;
  bool emergent_reaction_text=false;
  bool draw_kibus=false;
  bool draw_bees=false;
  bool delete_obstacle=false;
  bool kibus_return=false;
  bool keys[7]={false,false,false,false,false,false,false};



  if(!al_init()){
  fprintf(stderr,"Failed to initialize allegro.\n");
    return -1;
  }

  if(!al_init_image_addon()){
    fprintf(stderr,"Failed to init allegro image.\n");
    return -1;
  }

  if(!al_install_mouse()){
    fprintf(stderr,"Failed to init allegro mouse.\n");
    return -1;
  }
  if(!al_install_keyboard()){
    fprintf(stderr,"Failed to init allegro keyboard.\n");
    return -1;
  }
  
  al_init_font_addon();
  if(!al_init_ttf_addon()){
    fprintf(stderr,"Failed to init allegro ttf.\n");
    return -1;
  }
  
  if(!al_init_primitives_addon()){
    fprintf(stderr,"Failed to init primitives.\n");
    return -1;
  }

  display=al_create_display(res_x,res_y);
  if(!display){
    fprintf(stderr,"Failed to create allegro display.\n");
    return -1;
  }

  event_queue=al_create_event_queue();
  if(!event_queue){
    fprintf(stderr,"Failed to create allegro event_queue.\n");
    al_destroy_display(display);
    return -1;
  }

  timer=al_create_timer(1.0/FPS);
  if(!timer){
    fprintf(stderr,"Failed to create allegro timer.\n");
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    return -1;
  }

  bitmaps[GRASS]=al_load_bitmap("Grass.png");
  bitmaps[OBSTACLE]=al_load_bitmap("Obstacle.png");
  bitmaps[HOUSE]=al_load_bitmap("House.png");
  kibus=al_load_bitmap("Orbs.png");
  bee=al_load_bitmap("Spirits.png");
  if(!bitmaps[GRASS]||!bitmaps[OBSTACLE]||!bitmaps[HOUSE]||!kibus||!bees){
    fprintf(stderr,"Failed to create allegro bitmaps.\n");
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);
    al_destroy_bitmap(bitmaps[GRASS]);
    al_destroy_bitmap(bitmaps[OBSTACLE]);
    al_destroy_bitmap(bitmaps[HOUSE]);
    al_destroy_bitmap(kibus);
    al_destroy_bitmap(bee);
    return -1;
  }

  font=al_load_ttf_font("Font3.ttf",res_y/20,0);

  al_register_event_source(event_queue,al_get_display_event_source(display));
  al_register_event_source(event_queue,al_get_mouse_event_source());
  al_register_event_source(event_queue,al_get_timer_event_source(timer));
  al_register_event_source(event_queue,al_get_keyboard_event_source());

  struct square grid[g_rows][g_cols];
  initgrid(g_rows,g_cols,res_x,res_y,grid);
  init_weights(g_rows,g_cols,grid);
  randomize_obstacles(g_rows,g_cols,res_x,res_y,grid,obstacle_percent);
  
  al_clear_to_color(al_map_rgb(0,0,0));  
 
  al_convert_mask_to_alpha(kibus,al_map_rgb(0,0,0));
  al_convert_mask_to_alpha(bee,al_map_rgb(0,0,0));

  al_start_timer(timer);

  
  /*
  for(i=0;i<g_rows;i++){
    for(j=0;j<g_cols;j++){
      al_draw_scaled_bitmap(bitmaps[GRASS],0,0,80,80,grid[i][j].x,grid[i][j].y,res_x/g_cols,res_y/g_rows,0);
    }
  }

  for(i=0;i<g_rows;i++){
    for(j=0;j<g_cols;j++){
      al_draw_scaled_bitmap(bitmaps[OBSTACLE],0,0,80,80,grid[i][j].x,grid[i][j].y,res_x/g_cols,res_y/g_rows,0);
    }
  }
  al_draw_scaled_bitmap(bitmaps[HOUSE],0,0,80,80,grid[g_rows-2][g_cols-2].x,grid[g_rows-2][g_cols-2].y,res_x/g_cols,res_y/g_rows,0);
  */

  /*  for(i=0;i<g_rows;i++)
    al_draw_line(grid[i][0].x,grid[i][0].y,grid[i][g_cols-1].x+(res_x/g_cols),grid[i][g_cols-1].y,al_map_rgb(255,255,255),1.0);
  
  for(i=0;i<g_cols;i++)
  al_draw_line(grid[0][i].x,grid[0][i].y,grid[g_rows-1][i].x,grid[g_rows-1][i].y+(res_y/g_rows),al_map_rgb(255,255,255),1.0);
  */
  int line_i=0;
  maximum_steps=ceil((float)g_rows/2.0)*(g_cols-1);  //Contribución de Emilio Erándu Ceja Cárdenas
  printf("%d\n",maximum_steps);



  while(1){
    ALLEGRO_EVENT ev;
    al_wait_for_event(event_queue,&ev);


    if(ev.type==ALLEGRO_EVENT_TIMER){
      redraw=true;      
      if(keys[PGUP_KEY]){
	if(obstacle_percent>0&&!kibus_return){
	  if(random_map_delay==20){
	    obstacle_percent--;
	    set_house=false;
	    set_obstacle=false;
	    set_random_map=true; 
	  }else{
	    random_map_delay++;	   
	  }
	}
      }
      else if(keys[PGDN_KEY]){
	if(obstacle_percent<80&&!kibus_return){
	  if(random_map_delay==20){
	    obstacle_percent++;
	    set_house=false;
	    set_obstacle=false;
	    set_random_map=true;
	  }else{
	    random_map_delay++;
	  }
	}
      }
    }
    else if(ev.type==ALLEGRO_EVENT_DISPLAY_CLOSE){
      break;
    }
    else if(ev.type==ALLEGRO_EVENT_MOUSE_AXES||ev.type==ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY){
      int i,j;
      mouse_x=ev.mouse.x;
      mouse_y=ev.mouse.y;
      if(keys[MOUSE_LEFT_BUTTON]){
	if(set_obstacle){
	  get_grid_pos(g_rows,g_cols,res_x,res_y,grid,ev.mouse.x,ev.mouse.y,&i,&j);
	  if(grid[i][j].type==GRASS){
	    grid[i][j].type=OBSTACLE;
	  }	 
	}
      }
      //      al_draw_scaled_bitmap(bitmaps[OBSTACLE],0,0,80,80,ev.mouse.x-(res_x/g_cols)/2,ev.mouse.y-(res_y/g_rows)/2,res_x/g_cols,res_y/g_rows,0);
    }
    else if(ev.type==ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
      keys[MOUSE_LEFT_BUTTON]=true;
    }
    else if(ev.type==ALLEGRO_EVENT_MOUSE_BUTTON_UP){
      int i,j;
      if(keys[MOUSE_LEFT_BUTTON]){
	keys[MOUSE_LEFT_BUTTON]=false;
      }
      if(set_house){
	get_grid_pos(g_rows,g_cols,res_x,res_y,grid,ev.mouse.x,ev.mouse.y,&i,&j);
	if(grid[i][j].type!=OBSTACLE){
	  grid[i][j].type=HOUSE;
	  house_i=i;
	  house_j=j;
	  set_house=false;
	  set_grid_weight(g_rows,g_cols,house_i,house_j,grid);
	}
      }
      if(set_kibus){
	get_grid_pos(g_rows,g_cols,res_x,res_y,grid,ev.mouse.x,ev.mouse.y,&i,&j);
	if(grid[i][j].type!=OBSTACLE&&grid[i][j].type!=HOUSE){
	  kibus_i=i;
	  kibus_j=j;
	  kibus_last_i=i;
	  kibus_last_j=j;
	  bees[0].i=bees[1].i=bees[2].i=bees[3].i=bees[4].i=kibus_i;
	  bees[0].j=bees[1].j=bees[2].j=bees[3].j=bees[4].j=kibus_j;
	  set_kibus=false;
	  draw_kibus=true;
	}
      }
      
      if(set_obstacle){
	get_grid_pos(g_rows,g_cols,res_x,res_y,grid,ev.mouse.x,ev.mouse.y,&i,&j);
	if(grid[i][j].type==GRASS){
	  grid[i][j].type=OBSTACLE;
	}
      }
      if(delete_obstacle){	
	get_grid_pos(g_rows,g_cols,res_x,res_y,grid,ev.mouse.x,ev.mouse.y,&i,&j);
	if(grid[i][j].type==OBSTACLE){
	  grid[i][j].type=GRASS;
	}	
      }
	    

      //      printf("Click at:%d,%d\n",i,j);
      //      al_draw_scaled_bitmap(bitmaps[OBSTACLE],0,0,80,80,grid[i][j].x,grid[i][j].y,res_x/g_cols,res_y/g_rows,0);
    }
    else if(ev.type==ALLEGRO_EVENT_KEY_DOWN){
      switch(ev.keyboard.keycode){
      case ALLEGRO_KEY_PGUP:
	keys[PGUP_KEY]=true;
	break;
      case ALLEGRO_KEY_PGDN:
	keys[PGDN_KEY]=true;
	break;
      }
    }
     
    else if(ev.type==ALLEGRO_EVENT_KEY_UP){
      switch(ev.keyboard.keycode){
      case ALLEGRO_KEY_H:
	if(kibus_return)
	  break;
	set_obstacle=false;
	delete_obstacle=false;
	set_kibus=false;
	draw_kibus=false;
	if(house_i!=-1&&house_j!=-1){
	  grid[house_i][house_j].type=GRASS;
	  house_i=-1;
	  house_j=-1;
	  kibus_i=-1;
	  kibus_j=-1;
	}
	set_house=true;      	  
	break;
	
      case ALLEGRO_KEY_K:
	if(kibus_return||(house_i==-1&&house_j==-1))
	  break;
	draw_kibus=false;
	set_obstacle=false;
	delete_obstacle=false;
	set_house=false;
	if(kibus_i!=-1&&kibus_j!=-1){
	  kibus_i=-1;
	  kibus_j=-1;
	}
	set_kibus=true;
	break;


      case ALLEGRO_KEY_O:
	//	if(kibus_return)
	//break;
	set_house=false;
	delete_obstacle=false;
	set_kibus=false;
	set_obstacle=true;
	break;
	
      case ALLEGRO_KEY_PGUP:
	keys[PGUP_KEY]=false;
	if(obstacle_percent>=0&&!kibus_return){
	  if(obstacle_percent>0)
	    obstacle_percent--;
	  random_map_delay=0;
	  set_house=false;
	  set_kibus=false;
	  set_obstacle=false;
	  delete_obstacle=false;
	  set_random_map=true;	 
	}
	break;
	
      case ALLEGRO_KEY_PGDN:
	keys[PGDN_KEY]=false;
	if(obstacle_percent<80&&!kibus_return){
	  obstacle_percent++;
	  random_map_delay=0;
	  set_house=false;
	  set_kibus=false;
	  set_obstacle=false;
	  delete_obstacle=false;
	  set_random_map=true;       
	}
	break;

      case ALLEGRO_KEY_SPACE:
	if(!kibus_return){
	  set_house=false;
	  set_kibus=false;
	  set_obstacle=false;
	  delete_obstacle=false;
	  bees_root_i=kibus_i;
	  bees_root_j=kibus_j;	 
	  bees[0].i=bees[1].i=bees[2].i=bees[3].i=bees[4].i=bees_root_i;
	  bees[0].j=bees[1].j=bees[2].j=bees[3].j=bees[4].j=bees_root_j;
	  bees_behavior=PROPAGATE;
	  bee_wait_delay=0;
	  bee_step=0;
	  draw_bees=true;
	  kibus_return=true;
	}	
	break;       	
	
      case ALLEGRO_KEY_X:
	//	if(kibus_return)
	//break;
	set_house=false;
	set_kibus=false;
	set_obstacle=false;
	delete_obstacle=true;
	break;

      case ALLEGRO_KEY_Q:
	if(set_kibus&&!kibus_return){
	  if(kibus_i>=1&&kibus_j>=1){
	    if(grid[kibus_i-1][kibus_j-1].type!=OBSTACLE){
	      kibus_i--;
	      kibus_j--;
	    }
	  }
	}
	break;
      case ALLEGRO_KEY_W:
	if(set_kibus&&!kibus_return){
	  if(kibus_i>=1){
	    if(grid[kibus_i-1][kibus_j].type!=OBSTACLE){
	      kibus_i--;
	    }
	  }
	}
	break;
      case ALLEGRO_KEY_E:
	if(set_kibus&&!kibus_return){
	  if(kibus_i>=1&&kibus_j<=(g_cols-2)){
	    if(grid[kibus_i-1][kibus_j+1].type!=OBSTACLE){
	      kibus_i--;
	      kibus_j++;
	    }
	  }
	}
	break;
      case ALLEGRO_KEY_A:
	if(set_kibus&&!kibus_return){
	  if(kibus_j>=1){
	    if(grid[kibus_i][kibus_j-1].type!=OBSTACLE){
	      kibus_j--;

	    }
	  }
	}
	break;
      case ALLEGRO_KEY_S:
	if(set_kibus&&!kibus_return){
	  if(kibus_i<=(g_rows-2)){
	    if(grid[kibus_i+1][kibus_j].type!=OBSTACLE){
	      kibus_i++;
	    }
	  }
	}
	break;
      case ALLEGRO_KEY_D:
	if(set_kibus&&!kibus_return){
	  if(kibus_j<=(g_rows-2)){
	    if(grid[kibus_i][kibus_j+1].type!=OBSTACLE){
	      kibus_j++;
	    }
	  }
	}
	break;
      case ALLEGRO_KEY_Z:
	if(set_kibus&&!kibus_return){
	  if(kibus_i<=(g_rows-2)&&kibus_j>=1){
	    if(grid[kibus_i+1][kibus_j-1].type!=OBSTACLE){
	      kibus_i++;
	      kibus_j--;
	    }
	  }
	}
	break;
      case ALLEGRO_KEY_C:
	if(set_kibus&&!kibus_return){
	  if(kibus_i<=(g_rows-2)&&kibus_j<=(g_cols-2)){
	    if(grid[kibus_i+1][kibus_j+1].type!=OBSTACLE){
	      kibus_i++;
	      kibus_j++;
	    }
	  }
	}		
	break;
      }
    }
    
    if(redraw&&al_is_event_queue_empty(event_queue)){
      int i,j;
      redraw=false;      
      al_clear_to_color(al_map_rgb(0,0,0));
      if(set_random_map){      
	randomize_obstacles(g_rows,g_cols,res_x,res_y,grid,obstacle_percent);
	if(kibus_i!=-1&&kibus_j!=-1){
	  printf("obstacles:%d%%\n",obstacle_percent);
	  grid[kibus_i][kibus_j].type=GRASS;
	}
	if(house_i!=-1&&house_j!=-1){
	  printf("obstacles:%d%%\n",obstacle_percent);	  
	  grid[house_i][house_j].type=HOUSE;
	}
	set_random_map=false;
	set_text=true;
	random_map_text=true;
	text_delay=0;
      }
      for(i=0;i<g_rows;i++){
	for(j=0;j<g_cols;j++){
	  al_draw_tinted_scaled_bitmap(bitmaps[GRASS],al_map_rgb(0,5+grid[i][j].weight,0),0,0,80,80,grid[i][j].x,grid[i][j].y,res_x/g_cols,res_y/g_rows,0);
	  if(grid[i][j].type!=GRASS){
	    al_draw_scaled_bitmap(bitmaps[grid[i][j].type],0,0,80,80,grid[i][j].x,grid[i][j].y,res_x/g_cols,res_y/g_rows,0);
	  }
	}
      }
      if(set_house){
	get_grid_pos(g_rows,g_cols,res_x,res_y,grid,mouse_x,mouse_y,&i,&j);
	set_grid_weight(g_rows,g_cols,i,j,grid);
	al_draw_scaled_bitmap(bitmaps[HOUSE],0,0,80,80,mouse_x-(res_x/g_cols)/2,mouse_y-(res_y/g_rows)/2,res_x/g_cols,res_y/g_rows,0);
      }
      if(set_obstacle){
	al_draw_scaled_bitmap(bitmaps[OBSTACLE],0,0,80,80,mouse_x-(res_x/g_cols)/2,mouse_y-(res_y/g_rows)/2,res_x/g_cols,res_y/g_rows,0);
      }
      if(delete_obstacle){
	al_draw_tinted_scaled_bitmap(bitmaps[OBSTACLE],al_map_rgb(0,0,0),0,0,80,80,mouse_x-(res_x/g_cols)/2,mouse_y-(res_y/g_rows)/2,res_x/g_cols,res_y/g_rows,0);
      }
      if(set_kibus){
	al_draw_scaled_bitmap(kibus,kibus_side*(al_get_timer_count(timer)%12),0,kibus_side,kibus_side,mouse_x-(res_x/g_cols)/2,mouse_y-(res_y/g_rows)/2,res_x/g_cols,res_y/g_rows,0);
      }
      if(draw_bees){
	al_draw_scaled_bitmap(bee,bee_side*(al_get_timer_count(timer)%16),0,bee_side,bee_side,grid[bees[0].i][bees[0].j].x,grid[bees[0].i][bees[0].j].y,res_x/g_cols,res_y/g_rows,0);
	al_draw_scaled_bitmap(bee,bee_side*(al_get_timer_count(timer)%16),0,bee_side,bee_side,grid[bees[1].i][bees[1].j].x,grid[bees[1].i][bees[1].j].y,res_x/g_cols,res_y/g_rows,0);
	al_draw_scaled_bitmap(bee,bee_side*(al_get_timer_count(timer)%16),0,bee_side,bee_side,grid[bees[2].i][bees[2].j].x,grid[bees[2].i][bees[2].j].y,res_x/g_cols,res_y/g_rows,0);
	al_draw_scaled_bitmap(bee,bee_side*(al_get_timer_count(timer)%16),0,bee_side,bee_side,grid[bees[3].i][bees[3].j].x,grid[bees[3].i][bees[3].j].y,res_x/g_cols,res_y/g_rows,0);
	al_draw_scaled_bitmap(bee,bee_side*(al_get_timer_count(timer)%16),0,bee_side,bee_side,grid[bees[4].i][bees[4].j].x,grid[bees[4].i][bees[4].j].y,res_x/g_cols,res_y/g_rows,0);
	
      }
      if(draw_kibus){
	al_draw_scaled_bitmap(kibus,kibus_side*(al_get_timer_count(timer)%12),0,kibus_side,kibus_side,grid[kibus_i][kibus_j].x,grid[kibus_i][kibus_j].y,res_x/g_cols,res_y/g_rows,0);
      }
      if(kibus_return){
	if(bees_behavior==PROPAGATE){
	  printf("PROPAGATE\n");
	  environment=environment_calculate(g_rows,g_cols,bees_root_i,bees_root_j,grid);
	  // printf("propagation:%d root_i:%d root_j:%d\n",environment,bees_root_i,bees_root_j);
	  bee_propagation(g_rows,g_cols,environment,bees_root_i,bees_root_j,grid,bees);
	  //	  printf("Bees:\n\t0:[%d,%d] %d\n\t1:[%d,%d] %d\n\t2:[%d,%d] %d\n\t3:[%d,%d] %d\n\t4:[%d,%d] %d\n",bees[0].i,bees[0].j,bees[0].weight,bees[1].i,bees[1].j,bees[1].weight,bees[2].i,bees[2].j,bees[2].weight,bees[3].i,bees[3].j,bees[3].weight,bees[4].i,bees[4].j,bees[4].weight);
	  bees_behavior=RETURN;
	}else if(bees_behavior==RETURN){
	  printf("RETURN\n");
	  winner_bee=bee_board(bees_root_i,bees_root_j,bees);
	  board_buffer[bee_step][0]=bees[winner_bee].i;
	  board_buffer[bee_step][1]=bees[winner_bee].j;
	  board_buffer[bee_step][2]=bees[winner_bee].weight;
	  bees[0].i=bees[1].i=bees[2].i=bees[3].i=bees[4].i=bees_root_i;
	  bees[0].j=bees[1].j=bees[2].j=bees[3].j=bees[4].j=bees_root_j;	 
	  bees_behavior=REALLOCATE;
	}else if(bees_behavior==REALLOCATE){	  
	  printf("REALLOCATE\n");
	  bees_root_i=board_buffer[bee_step][0];
	  bees_root_j=board_buffer[bee_step][1];
	  bees[0].i=bees[1].i=bees[2].i=bees[3].i=bees[4].i=bees_root_i;
	  bees[0].j=bees[1].j=bees[2].j=bees[3].j=bees[4].j=bees_root_j;
	  if(board_buffer[bee_step][2]==MAXWEIGHT){
	    bees_behavior=WAIT;
	    printf("Bees:Found House!\n");
	  }else{
	    bee_step++;
	    bees_behavior=bee_step==5?WAIT:PROPAGATE;	 
	    //	    printf("\tWinnerBee:%d %d\n\n",winner_bee,bees[winner_bee].weight);
	  }
	}else if(bees_behavior==WAIT){
	  kibus_i=board_buffer[bee_wait_delay][0];
	  kibus_j=board_buffer[bee_wait_delay][1];
	  if(board_buffer[bee_wait_delay][2]==MAXWEIGHT){
	    bees_behavior=-1;
	    bee_step=0;
	    bee_wait_delay=0;
	    kibus_return=false;
	  }
	  bee_wait_delay++;
	  if(bee_wait_delay==5&&kibus_return){
	    bee_step=0;
	    bee_wait_delay=0;
	    srand(al_get_timer_count(timer));
	    cooling_cell(g_rows,g_cols,grid,board_buffer);
	    bees_behavior=PROPAGATE;	   
	  }
	}    
      }  
      
	
      if(set_text&&text_delay<=60){
	if(random_map_text){
	  al_draw_textf(font,al_map_rgb(200,200,200),0,0,ALLEGRO_ALIGN_LEFT,"Obstacles:%d%%",obstacle_percent);
	}
	if(emergent_reaction_text){	
	  al_draw_textf(font,al_map_rgb(200,200,200),0,0,ALLEGRO_ALIGN_LEFT,"Emergent algorithm:%s",emergent_reaction==-1?"LEFT":emergent_reaction==CHAOTIC?"CHAOTIC":"RIGHT");
	}
	text_delay++;
      }else{
	random_map_text=false;
	emergent_reaction_text=false;
	set_text=false;
	text_delay=0;
      }
      al_flip_display();    
    }
  }
  al_destroy_display(display);
  al_destroy_event_queue(event_queue);
  al_destroy_timer(timer);
  al_destroy_bitmap(bitmaps[GRASS]);
  al_destroy_bitmap(bitmaps[OBSTACLE]);
  al_destroy_bitmap(bitmaps[HOUSE]);
  al_destroy_font(font);
  return 0;
}  

