package PID_Pkg is

   type PID is record
      Kp, Ki, Kd       : Float;
      Integral         : Float := 0.0;
      Last_Meas        : Float := 0.0;
      Last_Meas_Valid  : Boolean := False;
      D_Filtered       : Float := 0.0;
      Tau              : Float := 0.02;
      Out_Min, Out_Max : Float := 0.0;
      I_Min, I_Max     : Float := 0.0;
      Use_Out_Clamp    : Boolean := False;
      Use_I_Clamp      : Boolean := False;
   end record;

   procedure Init_PID(P : out PID; Kp, Ki, Kd, Tau : Float;
                      Out_Min, Out_Max : Float;
                      I_Min, I_Max : Float);

   function PID_Create (Kp, Ki, Kd : Float;
                    Out_Min    : Float := -Float'Last;
                    Out_Max    : Float := Float'Last;
                    I_Min      : Float := -Float'Last;
                    I_Max      : Float := Float'Last;
                    Tau        : Float := 0.02) return PID;

   function PID_Update(P : in out PID; Setpoint, Meas, Dt, Deadband : Float) return Float;

end PID_Pkg;
