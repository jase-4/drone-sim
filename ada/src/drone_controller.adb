with Ada.Text_IO;              use Ada.Text_IO;
with Ada.Numerics;             use Ada.Numerics;
with Ada.Numerics.Elementary_Functions;
with PID_Pkg; use PID_Pkg;
with Helpers;
with Drone_Controller;
with Ada.Real_Time; use Ada.Real_Time;
with Udp_Sender;

package body Drone_Controller is


   Desired_Pos     : Vector3 := (15.0, 15.0, -15.0);
   Target_Pos      : Vector3 := (0.0, 0.5, 0.0);
   Target_Yaw      : Vector3 := (0.0, 0.0, 0.0);
   Ramp_Factor     : constant Float := 0.02;
   Hover_Thrust    : constant Float := 4.1;
   Max_Angle_Rad   : constant Float := 10.0 * Float (Pi) / 180.0;

   Initialized     : Boolean := False;
   Flag            : Boolean := True;
   Last_Time       : Ada.Real_Time.Time := Ada.Real_Time.Clock;

  
   Alt_PID    : PID := PID_Create (Kp => 0.6, Ki => 0.002, Kd => 0.51,
                                       Out_Min => -3.0, Out_Max => 3.0,
                                       I_Min => -2.0, I_Max => 2.0);

   Pos_PID_X  : PID := PID_Create (0.5, 0.012, 0.81);
  
   
   Pos_PID_Z  : PID := PID_Create (0.5, 0.012, 0.81);
   --  Pos_PID_Z  : PID := PID_Create (0.0, 0.00, 0.0);
   --  Pos_PID_X  : PID := PID_Create (0.0, 0.00, 0.0);
   
   Roll_PID   : PID := PID_Create (0.3315, 0.001, 0.3);
   Pitch_PID  : PID := PID_Create (0.3315, 0.001, 0.3);
   Yaw_PID    : PID := PID_Create (0.0, 0.0, 0.0);

   procedure Update (Msg : Sensor_Fusion) is
      use Ada.Numerics.Elementary_Functions;

      Now   : constant Ada.Real_Time.Time := Ada.Real_Time.Clock;
      Dt    :  Float := Float (Ada.Real_Time.To_Duration (Now - Last_Time));
      Roll, Pitch, Yaw : Float;
      GL_Eurer : Euler;
      Q_Opengl : Quaternion;
      Thrust, Thrust_Corr : Float;
      Desired_Roll, Desired_Pitch : Float;
      Roll_Cmd, Pitch_Cmd, Yaw_Cmd : Float;
   begin
      Last_Time := Now;
      if Dt <= 0.0 then
         return;
      end if;

  
      Put_Line (Msg.Orientation.X'Image & ", " & Msg.Orientation.Y'Image & ", " & Msg.Orientation.Z'Image & ", " & Msg.Orientation.W'Image);
      Q_Opengl := Helpers.Rotate_For_OpenGL (Msg.Orientation);
      
      GL_Eurer := Helpers.To_Euler (Q_Opengl);
      Roll  := GL_Eurer.Roll;
      Pitch := GL_Eurer.Pitch; 
      Yaw   := GL_Eurer.Yaw;  
      
    

      Put_Line ("Orientation (rad): Roll=" & Float'Image (Roll)
                           & ", Pitch=" & Float'Image (Pitch)
                           & ", Yaw=" & Float'Image (Yaw));

    
      if not Initialized then
         Alt_PID.Last_Meas   := Msg.Position.Y;
         Pos_PID_X.Last_Meas := Msg.Position.X;
         Pos_PID_Z.Last_Meas := Msg.Position.Z;
         Initialized := True;
      end if;

   
      Target_Pos.X := Target_Pos.X + (Desired_Pos.X - Target_Pos.X) * Ramp_Factor;
      Target_Pos.Y := Target_Pos.Y + (Desired_Pos.Y - Target_Pos.Y) * Ramp_Factor;
      Target_Pos.Z := Target_Pos.Z + (Desired_Pos.Z - Target_Pos.Z) * Ramp_Factor;

      Thrust_Corr := PID_Update (Alt_PID,
                                 Setpoint => Target_Pos.Y,
                                 Meas     => Msg.Position.Y,
                                 Dt       => Dt,
                                 Deadband => 0.03);
      Thrust := Float'Max (0.0, Float'Min (10.5, Hover_Thrust + Thrust_Corr));

      Desired_Roll := PID_Update (Pos_PID_X, Target_Pos.X, Msg.Position.X, Dt,0.0);
      Desired_Pitch := PID_Update (Pos_PID_Z, Target_Pos.Z, Msg.Position.Z, Dt,0.0);

      Desired_Roll := Float'Max (-Max_Angle_Rad, Float'Min (Max_Angle_Rad, Desired_Roll));
      Desired_Pitch := Float'Max (-Max_Angle_Rad, Float'Min (Max_Angle_Rad, Desired_Pitch));

      Roll_Cmd := PID_Update (Roll_PID, Desired_Roll, Roll, Dt,0.0);
      Pitch_Cmd := PID_Update (Pitch_PID, -Desired_Pitch, Yaw, Dt,0.0);
   

      Yaw_Cmd := PID_Update (Yaw_PID, 0.0, 0.0 - Pitch, Dt,0.0);

      
      

      UDP_Sender (Thrust => Thrust, Roll_Cmd => Roll_Cmd, Pitch_Cmd =>Pitch_Cmd, Yaw_Cmd => Yaw_Cmd);

   end Update;

end Drone_Controller;
