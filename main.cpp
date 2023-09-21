#include <simplecpp>
#include "shooter.h"
#include "bubble.h"

/* Simulation Vars */
const double STEP_TIME = 0.02;

/* Game Vars */
const int PLAY_Y_HEIGHT = 450;
const int LEFT_MARGIN = 70;
const int TOP_MARGIN = 20;
const int BOTTOM_MARGIN = (PLAY_Y_HEIGHT+TOP_MARGIN);


void move_bullets(vector<Bullet> &bullets){
    // move all bullets
    for(unsigned int i=0; i<bullets.size(); i++){
        if(!bullets[i].nextStep(STEP_TIME)){
            bullets.erase(bullets.begin()+i);
        }
    }
}

void move_bubbles(vector<Bubble> &bubbles){
    // move all bubbles
    for (unsigned int i=0; i < bubbles.size(); i++)
    {
        bubbles[i].nextStep(STEP_TIME);
    }
}

vector<Bubble> create_bubbles()
{
    // create initial bubbles in the game
    vector<Bubble> bubbles;
    bubbles.push_back(Bubble(WINDOW_X/2.0, BUBBLE_START_Y, BUBBLE_DEFAULT_RADIUS, -BUBBLE_DEFAULT_VX, 0, COLOR(255,105,180)));
    bubbles.push_back(Bubble(WINDOW_X/4.0, BUBBLE_START_Y, BUBBLE_DEFAULT_RADIUS, BUBBLE_DEFAULT_VX, 0, COLOR(255,105,180)));
    return bubbles;
}

//check collision with shooter
bool is_in_vicinity(vector<Bubble> bubbles,Shooter shooter,int j){
double E[2]; //2 epsilon for 4 cases
E[0] = shooter.get_head_radius() + bubbles[j].get_radius();
E[1] = shooter.get_body_width()/2.0 + bubbles[j].get_radius();
return ((abs(shooter.get_head_center_y() - bubbles[j].get_center_y()) < E[0]
        &&                                                                    //collision with head
        abs(shooter.get_head_center_x() - bubbles[j].get_center_x()) < E[0])

        ||

        (bubbles[j].get_center_y() > shooter.get_head_center_y() + shooter.get_head_radius()
        &&                                                                  //with shooter body
        abs(shooter.get_body_center_x() - bubbles[j].get_center_x()) < E[1])) ;
}

int main()
{
    initCanvas("Bubble Trouble", WINDOW_X, WINDOW_Y);

    Line b1(0, PLAY_Y_HEIGHT, WINDOW_X, PLAY_Y_HEIGHT);
    b1.setColor(COLOR(0, 0, 255));

    string msg_cmd("Cmd: _");
    Text charPressed(LEFT_MARGIN, BOTTOM_MARGIN, msg_cmd);

    // Intialize the shooter
    Shooter shooter(SHOOTER_START_X, SHOOTER_START_Y, SHOOTER_VX);

    // Initialize the bubbles
    vector<Bubble> bubbles = create_bubbles();

    // Initialize the bullets (empty)
    vector<Bullet> bullets;

    XEvent event;

    //time bar initializing
    int i=0,second=0; //i updates after every 0.02 second ans second after every second ignoring execution lag
    string time("TIME:"),out_of("/ 50");
    Text t(LEFT_MARGIN,TOP_MARGIN,time);

    //score and health bar initializing
    string score("SCORE:"),health("HEALTH:"),full_life("/ 3");
    Text sc(WINDOW_X-80,BOTTOM_MARGIN,score); Text h(WINDOW_X-90,TOP_MARGIN,health); //printed score and health
    int sco=0,hea=3;

    int first_collision = -51; //stores time for first collision in terms of 0.02 intialized to suffieciently -ve no.(<-50) so that first condition T only once

    // Main game loop
    while (second<51) //timer of 50s
    {
        bool pendingEvent = checkEvent(event);
        if (pendingEvent)
        {
            // Get the key pressed
            char c = charFromEvent(event);
            msg_cmd[msg_cmd.length() - 1] = c;
            charPressed.setMessage(msg_cmd);

            // Update the shooter
            if(c == 'a')
                shooter.move(STEP_TIME, true);
            else if(c == 'd')
                shooter.move(STEP_TIME, false);
            else if(c == 'w')
                bullets.push_back(shooter.shoot());
            else if(c == 'q')
                return 0;
        }

        // Update the bubbles
        move_bubbles(bubbles);

        // Update the bullets
        move_bullets(bullets);

        //setting score and health
        stringstream river,lake;
        river<<sco; lake<<hea;//inserted score in river and health in lake
        string p,q;
        river>>p; lake>>q; //read into p and q
        //print
        Text p1(WINDOW_X-80 + textWidth(score),BOTTOM_MARGIN,p); p1.imprint(); //score set to 0
        Text q1(WINDOW_X-90 + textWidth(health),TOP_MARGIN,q); q1.imprint();
        Text q2(WINDOW_X-90 + textWidth(health+q+"  "),TOP_MARGIN,full_life); q2.imprint();//health set as 3 by 3

        //collision
        for(unsigned int i=0; i<bullets.size(); i++){
          for(unsigned int j=0; j<bubbles.size(); j++){ //each bullet compared with each bubble
            double Ey = bubbles[j].get_radius() +  bullets[0].get_height()/2.0;
            double Ex = bubbles[j].get_radius() +  bullets[0].get_width()/2.0; //setting epsilon
            double Ey_min = 10.00 +  bullets[0].get_height()/2.0;
            double Ex_min = 10.00 +  bullets[0].get_width()/2.0;

            if(Ey>Ey_min && Ex>Ex_min){ //i.e big bubble
              if(abs(bullets[i].get_center_y() - bubbles[j].get_center_y()) < Ey //check y coordinate (not direct equating as lag)
                 &&
                 abs(bullets[i].get_center_x() - bubbles[j].get_center_x()) < Ex //check x coordinate
                 ){
                 sco++;
                 wait(0.1);
                 int X1 = bubbles[j].get_center_x() - bubbles[j].get_radius()/2.0,X2 =bubbles[j].get_center_x() - bubbles[j].get_radius()/2.0;//jth bubble would be erased so store coordinates
                 int Y1 = bubbles[j].get_center_y();
                 //dissappearing
                 bullets.erase(bullets.begin()+i);
                 bubbles.erase(bubbles.begin()+j);
                 //split
                 bubbles.push_back(Bubble(X1,Y1,BUBBLE_DEFAULT_RADIUS/2.0, -BUBBLE_DEFAULT_VX, 0, COLOR(255,105,180))); //create
                 bubbles.push_back(Bubble(X2,Y1,BUBBLE_DEFAULT_RADIUS/2.0, +BUBBLE_DEFAULT_VX, 0, COLOR(255,105,180)));

                 }
                }

            else{ //small bubble
              if(abs(bullets[i].get_center_y() - bubbles[j].get_center_y()) < Ey //check y coordinate (not direct equating as lag)
                 &&
                 abs(bullets[i].get_center_x() - bubbles[j].get_center_x()) < Ex //check x coordinate
                 ){
                  sco++;
                  //dissappearing
                  bullets.erase(bullets.begin()+i);
                  bubbles.erase(bubbles.begin()+j);
                  }
                }
          }
        }


        //collision with shooter
        for(unsigned int j=0; j<bubbles.size(); j++){
          if(is_in_vicinity(bubbles,shooter,j)) //checks condition of collision
           {
            //color change
            shooter.set_color(1); //color changes to blue

            //1 sec immunity to avoid many collision(enough time) just count 1
            if(i>first_collision+50){ //intialized to -51 so true only once
              hea--; //50i=1sec
              first_collision = i;
              }                    //now actuallly stores firstcollision time so condition will be false for 1sec hence immunity
            if(hea<=0) hea=0;
             }

          //color regained only when not in contact
          if(!is_in_vicinity(bubbles,shooter,j)){
          shooter.set_color(0);
          }
        }

        //victory
        if(bubbles.size()==0){ //no bubbles
          stringstream river;
          river<<sco;
          string p;
          river>>p;
          Text p1(WINDOW_X-80 + textWidth(score),BOTTOM_MARGIN,p); p1.imprint(); //score set to 0 as looped breaked before could print 0
          break;}

        //lose
        if(hea==0){
          stringstream lake;
          lake<<hea;
          string q;
          lake>>q;
          Text q1(WINDOW_X-90 + textWidth(health),TOP_MARGIN,q); q1.imprint();//health printed 0 as loop breaks before
          break;}

        //Timer
        stringstream stream;
        stream<<second; //insert int second in stream
        string s;
        if(i%50==0){ // true for i=0(sets 0) and then for each 50th iterartion 1 second
          stream>>s; //extract it as string
          second++;  //step time 0.02 so when i=50 1 second passed
          //print
          Text t1(LEFT_MARGIN + textWidth(time),TOP_MARGIN,s); t1.imprint();
          Text t2(LEFT_MARGIN + textWidth(time+s+"  "),TOP_MARGIN,out_of); t2.imprint();
          }
        i++; //update i keeps track of time increases by 1 after 0.02
        wait(STEP_TIME);
    }
    if(second ==51){//breaked after time over
      Text timeup(WINDOW_X/2.0,WINDOW_Y/2.0 ,"TIME'S UP!");
      timeup.setColor(COLOR(0,0,255));
      timeup.imprint();
    }

    else if(bubbles.size()==0){    //victory
      Text congo(WINDOW_X/2.0,WINDOW_Y/2.0 ,"CONGRATULATIONS!!");
      congo.setColor(COLOR(0,255,0));
      congo.imprint();
    }

    else if(hea ==0){ //lose
      Text gameover(WINDOW_X/2.0,WINDOW_Y/2.0 ,"GAME OVER!");
      gameover.setColor(COLOR(255,0,0));
      gameover.imprint();
     }

    wait(5);
}
