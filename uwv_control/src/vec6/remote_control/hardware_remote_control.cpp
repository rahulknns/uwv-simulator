#include "hardware_remote_control.h"

void readInput(void){
  char input;

  while (g_do_not_quit)
  {
    input = getch();

    switch (input)
    {
      case 'X':
        g_vec6.state_.is_traversing_ = false;
        g_vec6.allThrustersStop();
        g_do_not_quit = false;
        break;

      // Start/stop traversing
      case 't':
        if (g_vec6.state_.is_traversing_)
        {
          g_vec6.state_.is_traversing_ = false;
          g_vec6.allThrustersStop();
          ROS_INFO_STREAM("Exiting traversing mode.");
        }
        else
        {
          g_vec6.state_.is_traversing_ = true;
          ROS_INFO_STREAM("Entering traversing mode...");
        }
        break;

      // Stop surge/sway motion
      case 'S':
        g_vec6.allThrustersStop();
        g_vec6.state_.set_loc_.x = g_vec6.state_.set_loc_.y = 0;
        ROS_INFO_STREAM("Stopping thrusters");
        break;

      // Heave
      case 'W':
        g_vec6.state_.set_loc_.z += HEAVE_PLUS;
        ROS_INFO_STREAM("Set depth: " << g_vec6.state_.set_loc_.z);
        break;
      case 'Q':
        g_vec6.state_.set_loc_.z -= HEAVE_MINUS;
        if (g_vec6.state_.set_loc_.z < 0)
          g_vec6.state_.set_loc_.z = 0;         // cannot give set point that is above water level
        ROS_INFO_STREAM("Set depth: " << g_vec6.state_.set_loc_.z);
        break;

      // Yaw
      case 'd':
        g_vec6.state_.set_orient_.yaw += YAW_PLUS;
        if(g_vec6.state_.set_orient_.yaw >= 180.01){
					g_vec6.state_.set_orient_.yaw = -(360 - g_vec6.state_.set_orient_.yaw);
				}
        ROS_INFO_STREAM("Set heading: " << g_vec6.state_.set_orient_.yaw);
        break;
      case 'a':
        g_vec6.state_.set_orient_.yaw -= YAW_MINUS;
        if(g_vec6.state_.set_orient_.yaw <= -179.9){
					g_vec6.state_.set_orient_.yaw = 180;
				}
        ROS_INFO_STREAM("Set heading: " << g_vec6.state_.set_orient_.yaw);
        break;

      // Refresh PID configuration
      case 'P':
        g_vec6.state_.is_traversing_ = false;
        ROS_INFO_STREAM("Exiting traversing mode.");
        g_vec6.resetControllers(true, true);
        ROS_INFO_STREAM("PID configuration reloaded. Entering traversing mode.");
        g_vec6.state_.is_traversing_ = true;
        break;
      // Display all PID parameters
      case 'p': g_vec6.showPidsGains();
                break;
      
      // Surge
      case 'w':
        g_vec6.state_.set_loc_.x += SURGE_PLUS;
        ROS_INFO_STREAM("Surging effort: " << g_vec6.state_.set_loc_.x);
        break;
      case 's':
        g_vec6.state_.set_loc_.x -= SURGE_MINUS;
        ROS_INFO_STREAM("Surging effort: " << g_vec6.state_.set_loc_.x);
        break;

      // Sway
      case 'e':
        g_vec6.state_.set_loc_.y += SWAY_PLUS;
        ROS_INFO_STREAM("Sway effort: " << g_vec6.state_.set_loc_.y);
        break;
      case 'q':
        g_vec6.state_.set_loc_.y -= SWAY_MINUS;
        ROS_INFO_STREAM("Sway effort: " << g_vec6.state_.set_loc_.y);
        break;
    }
  }
}

void displayHelp(void)
{
  printw   (  "\nUse the following commands: \n" 
             "t: Start/Stop traversing \n" 
             "--- Motion Commands --- \n" 
             "W: Heave down by  %d  m \n" 
             "Q: Heave up by %d m \n" 
             "d: Positive yaw by %d deg \n" 
             "a: Negative yaw by %d deg \n" 
             "w: Surge+ by %d  \n" 
             "s: Surge- by %d  \n" 
             "e: Sway+ by %d  \n" 
             "q: Sway- by %d \n" 
             "S: Stop thrusters (stops only surge and sway in traversing mode) \n" 
             "--- Parameter settings --- \n" 
             "p: Show all PID parameters \n" 
             "P: Refresh PID configuration \n" 
             "----- \n" 
             "H: Display help \n" 
             "X: Quit\n",
             HEAVE_MINUS,HEAVE_PLUS,YAW_PLUS,YAW_MINUS,SURGE_PLUS,SURGE_MINUS,SWAY_PLUS,SWAY_MINUS );
                                                                                                    
}

int main(int argc, char** argv)
{
  // initializing ROS node
  ros::init(argc, argv, NODE_NAME);
  ros::NodeHandle nh;

  // register a signal handler to ensure that node is exited properly
  signal(SIGINT, signalHandler);

  // initialize the underwater vehicle controller
  g_vec6.initController(nh, 40, 0.4);

  initscr();
  raw();
  noecho();

  displayHelp();

  // begin separate thread to read from the keyboard
  std::thread reading_input_thread(readInput);
  reading_input_thread.detach();

  // initial values
  g_vec6.state_.set_loc_.x = 0;
  g_vec6.state_.set_loc_.y = 0;
  g_vec6.state_.set_loc_.z = 0.7;
  g_vec6.state_.set_orient_.yaw = 0.0;
  ROS_INFO_STREAM("Initial depth value: " << g_vec6.state_.set_loc_.z << " | Initial heading: " << g_vec6.state_.set_orient_.yaw);

  // main loop
  while (g_do_not_quit && ros::ok())
  {
    g_vec6.checkPidThread();
    ros::spinOnce();
  }

  // call this before ending the node
  g_vec6.checkPidThread();

  ROS_INFO_STREAM("Sayonara.");
}

// signal handler
void signalHandler(int signum)
{
  ROS_INFO_STREAM("[!] Press X to quit\n");
}