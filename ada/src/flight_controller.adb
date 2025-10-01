-- flight_controller.adb
with Ada.Text_IO; use Ada.Text_IO;
with Drone_Controller;

with System.Fat_Gen;
with UDP_Receiver; use UDP_Receiver;-- Import the UDP receiver procedure
with UDP_Sender; -- Import the UDP receiver procedure
with Mhm;
with Mhmm;
with PID_Pkg;
with Helpers;
with GNAT.Sockets;        use GNAT.Sockets;

procedure Flight_Controller is

 A : Integer := 6;
   B : Integer := 3;
   Result : Mhmm.Sum_Product;
   rec : PID_Pkg.PID;
   DC_Data : Helpers.Sensor_Fusion;
   Sock : Socket_Type;
   Is_Data_Valid : Boolean;
   eu: Helpers.Euler;
   q : Helpers.Quaternion;
   

begin

   Put_Line ("Starting Flight Controller...");
   Sock := UDP_Receiver.Setup_UDP_Receiver(12346);
   delay 1.0;
   Put_Line ("UDP Receiver setup complete on port 12346.");
    q := (0.0, 0.0, 0.0, 1.0);
   Put_Line ("Quaternion after OpenGL rotation: x=" & Float'Image (q.X)
      & ", y=" & Float'Image (q.Y) & ", z=" & Float'Image (q.Z) & ", w=" & Float'Image (q.W));

   q := Helpers.Rotate_For_OpenGL (q);
   Put_Line ("Quaternion after OpenGL rotation: x=" & Float'Image (q.X)
      & ", y=" & Float'Image (q.Y) & ", z=" & Float'Image (q.Z) & ", w=" & Float'Image (q.W));

   --  Put_Line ("Euler angles from identity quaternion: Roll=" & Float'Image (eu.Roll)
   --     & ", Pitch=" & Float'Image (eu.Pitch) & ", Yaw=" & Float'Image (eu.Yaw));


   --dc.Position := (3.0, 4.0, 5.0);
   --  dc.Velocity := (0.0, 0.0, 4.0);
   --dc.Orientation := (0.0, 0.0, 0.0, 1.0);
 delay 2.0;
  -- PID_Pkg.Init_PID(P => rec, Kp => 1.0, Ki => 1.0, Kd => 1.0, Tau => 1.0, Out_Min => 1.0, Out_Max => 1.0, I_Min => 1.0, I_Max => 1.0);
   loop

      Is_Data_Valid := UDP_Receiver.Receiver_Sensor_Data(Sock,DC_Data);  -- This will start the UDP listener
      --Put_Line ("Positionggggg: " & dc.Position.X'Image & ", " & dc.Position.Y'Image & ", " & dc.Position.Z'Image);
      if Is_Data_Valid then
            Drone_Controller.Update(DC_Data);
      else
         Put_Line ("No valid data received.");
      end if;
    -- UDP_Sender;

      --delay 0.5;
   end loop;
   -- Call the UDP receiver procedure
   --UDP_Sender;  -- This will start the UDP listener

   Mhm;
 

   Result := Mhmm.Get_Sum_and_Product(A, B);
   Put_Line("Sum: " & Result.Sum'Image);  -- Output: Sum: 8
   Put_Line("Porduct: " & Result.Product'Image);  -- Output: Sum: 8



end Flight_Controller;