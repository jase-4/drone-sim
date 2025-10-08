with Ada.Text_IO; use Ada.Text_IO;
with Drone_Controller;

with System.Fat_Gen;
with UDP_Receiver; use UDP_Receiver;
with UDP_Sender; 
with PID_Pkg;
with Helpers;
with GNAT.Sockets;        use GNAT.Sockets;

procedure Flight_Controller is

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

 delay 2.0;
   loop
      Is_Data_Valid := UDP_Receiver.Receiver_Sensor_Data(Sock,DC_Data); 
      if Is_Data_Valid then
            Drone_Controller.Update(DC_Data);
      else
         Put_Line ("No valid data received.");
      end if;
   end loop;
  
end Flight_Controller;