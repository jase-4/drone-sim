with Ada.Numerics; use Ada.Numerics;


with Ada.Numerics.Elementary_Functions;
use Ada.Numerics.Elementary_Functions;

package body Helpers is


 
   function Arctan2(Y, X : Float) return Float is
      Pi : constant Float := 3.14159_26535_89793; 
      Result : Float;
   begin
      if X > 0.0 then
         Result := Arctan(Y / X);
      elsif X < 0.0 then
         if Y >= 0.0 then
            Result := Arctan(Y / X) + Pi;
         else
            Result := Arctan(Y / X) - Pi;
         end if;
      elsif Y > 0.0 then -- X = 0
         Result := Pi / 2.0;
      elsif Y < 0.0 then -- X = 0
         Result := -Pi / 2.0;
      else -- X = 0 and Y = 0
         Result := 0.0;
      end if;

      return Result;
   end Arctan2;

   function To_Euler (Q : Quaternion) return Euler is
      Sinr_Cosp : Float := 2.0 * (Q.W * Q.X + Q.Y * Q.Z);
      Cosr_Cosp : Float := 1.0 - 2.0 * (Q.X * Q.X + Q.Y * Q.Y);
      Roll      : Float := Arctan2(Sinr_Cosp, Cosr_Cosp);

      Sinp      : Float := 2.0 * (Q.W * Q.Y - Q.Z * Q.X);
      Pitch : Float :=
      (if abs (Sinp) < 1.0 then Arcsin (Sinp)
         else (if Sinp > 0.0 then Float (Ada.Numerics.Pi) / 2.0
               else -Float (Ada.Numerics.Pi) / 2.0));
      Siny_Cosp : Float := 2.0 * (Q.W * Q.Z + Q.X * Q.Y);
      Cosy_Cosp : Float := 1.0 - 2.0 * (Q.Y * Q.Y + Q.Z * Q.Z);
      Yaw       : Float := Arctan2(Siny_Cosp, Cosy_Cosp);

   begin
      return (Roll => Roll, Pitch => Pitch, Yaw => Yaw);
   end To_Euler;


   function Multiply (Q1, Q2 : Quaternion) return Quaternion is
      X : Float := Q1.W * Q2.X + Q1.X * Q2.W + Q1.Y * Q2.Z - Q1.Z * Q2.Y;
      Y : Float := Q1.W * Q2.Y - Q1.X * Q2.Z + Q1.Y * Q2.W + Q1.Z * Q2.X;
      Z : Float := Q1.W * Q2.Z + Q1.X * Q2.Y - Q1.Y * Q2.X + Q1.Z * Q2.W;
      W : Float := Q1.W * Q2.W - Q1.X * Q2.X - Q1.Y * Q2.Y - Q1.Z * Q2.Z;
   begin
      return (X => X, Y => Y, Z => Z, W => W);
   end Multiply;


   function Rotate_For_OpenGL (Q : Quaternion) return Quaternion is
      Angle_Rad : constant Float := - Float (Ada.Numerics.Pi); 
      Sin_Half  : Float := Sin (Angle_Rad / 2.0);
      Cos_Half  : Float := Cos (Angle_Rad / 2.0);
      Rotation_Q : Quaternion := (X => Sin_Half, Y => 0.0, Z => 0.0, W => Cos_Half);
   begin
      return Multiply (Rotation_Q, Q);
   end Rotate_For_OpenGL;

function Mixer(
   Thrust : in Float;
   Pitch  : in Float;
   Roll   : in Float;
   Yaw    : in Float
) return Speeds_Record is
   MAX_MOTOR : constant Float := 26.5;
   MIN_MOTOR : constant Float := 0.0;
   Result    : Speeds_Record;

  
   function Clamp(Value : Float) return Float is
   begin
      return Float'Max(MIN_MOTOR, Float'Min(MAX_MOTOR, Value));
   end Clamp;
begin
   Result.Front_Right := Thrust + Pitch - Roll - Yaw;
   Result.Front_Left  := Thrust + Pitch + Roll + Yaw;
   Result.Back_Left   := Thrust - Pitch + Roll - Yaw;
   Result.Back_Right  := Thrust - Pitch - Roll + Yaw;


   Result.Front_Right := Clamp(Result.Front_Right);
   Result.Front_Left  := Clamp(Result.Front_Left);
   Result.Back_Left   := Clamp(Result.Back_Left);
   Result.Back_Right  := Clamp(Result.Back_Right);

   return Result;
end Mixer;

end Helpers;
