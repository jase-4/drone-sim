with Ada.Text_IO;              use Ada.Text_IO;

package body PID_Pkg is
   
   procedure Init_PID(P : out PID; Kp, Ki, Kd, Tau : Float;
                      Out_Min, Out_Max : Float;
                      I_Min, I_Max : Float) is
   begin
      P.Kp := Kp; P.Ki := Ki; P.Kd := Kd;
      P.Tau := Tau;
      P.Out_Min := Out_Min; P.Out_Max := Out_Max; P.Use_Out_Clamp := True;
      P.I_Min := I_Min; P.I_Max := I_Max; P.Use_I_Clamp := True;
   end Init_PID;

   function PID_Create (Kp, Ki, Kd : Float;
                    Out_Min    : Float := -Float'Last;
                    Out_Max    : Float := Float'Last;
                    I_Min      : Float := -Float'Last;
                    I_Max      : Float := Float'Last;
                    Tau        : Float := 0.02) return PID is
      P : PID;
   begin
      P.Kp := Kp; P.Ki := Ki; P.Kd := Kd;
      P.Tau := Tau;
      P.Out_Min := Out_Min; P.Out_Max := Out_Max; P.Use_Out_Clamp := True;
      P.I_Min := I_Min; P.I_Max := I_Max; P.Use_I_Clamp := True;
      return P;
   end PID_Create;
   
   -- PID_Update stays as you already wrote it...
   function PID_Update(P : in out PID; Setpoint, Meas, Dt, Deadband : Float) return Float is
      Error  : Float;
      D_Meas : Float;
      Alpha  : Float;
      Output : Float;
   begin
      if Dt <= 0.0 then
         return 0.0;
      end if;

       Error := Setpoint - Meas;
       --Put_Line("Setpoint: " & Float'Image(Setpoint) & ", Meas: " & Float'Image(Meas) & ", Error: " & Float'Image(Error));
      --if Deadband > 0.0 then
         if abs(Error) < Deadband then
            Error := 0.0;
         end if;
     -- end if;

      --Integral with anti-windup
      P.Integral := P.Integral + Error * Dt;
      if P.Use_I_Clamp then
         if P.I_Min > P.Integral then
            P.Integral := P.I_Min;
         elsif P.I_Max < P.Integral then
            P.Integral := P.I_Max;
         end if;
      end if;

      -- Derivative on measurement
      if P.Last_Meas_Valid then
         D_Meas := (Meas - P.Last_Meas) / Dt;
      else
         D_Meas := 0.0;
         P.Last_Meas_Valid := True;
      end if;

      -- Low-pass filter
      Alpha := Dt / (P.Tau + Dt);
      P.D_Filtered := (1.0 - Alpha) * P.D_Filtered + Alpha * D_Meas;

      P.Last_Meas := Meas;

      -- PID output
      Output := P.Kp * Error + P.Ki * P.Integral - P.Kd * P.D_Filtered;

      -- Output clamp
      if P.Use_Out_Clamp then
         if Output < P.Out_Min then
            Output := P.Out_Min;
         elsif Output > P.Out_Max then
            Output := P.Out_Max;
         end if;
      end if;

      return Output;
   end PID_Update;

end PID_Pkg;
