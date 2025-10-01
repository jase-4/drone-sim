
package Helpers is
   type Vector3 is record
      X, Y, Z : Float;
   end record;

   type Quaternion is record
       X, Y, Z, W : Float;
   end record;

   type Sensor_Fusion is record
      Position     : Vector3;
      Velocity     : Vector3;
      Acceleration : Vector3;
      Orientation  : Quaternion;
   end record;


   type Euler is record
      Roll  : Float;
      Pitch : Float;
      Yaw   : Float;
   end record;

   type Speeds_Record is record
      Front_Right : Float;
      Front_Left  : Float;
      Back_Left   : Float;
      Back_Right  : Float;
   end record;


   function To_Euler (Q : Quaternion) return Euler;
   function Multiply (Q1, Q2 : Quaternion) return Quaternion;
   function Rotate_For_OpenGL (Q : Quaternion) return Quaternion;
   function Mixer( Thrust : in Float; Pitch  : in Float; Roll   
   : in Float; Yaw    : in Float ) return Speeds_Record;
end Helpers;
