
with Ada.Text_IO;         use Ada.Text_IO;
with GNAT.Altivec.Low_Level_Vectors;
with GNAT.Sockets;        use GNAT.Sockets;
with Ada.Streams;         use Ada.Streams;
with Ada.Exceptions; use Ada.Exceptions;
with GNATCOLL.JSON; use GNATCOLL.JSON;
with Helpers;


package body UDP_Receiver is


function Setup_UDP_Receiver (Port_Number : Port_Type ) return Socket_Type is
    Sock : Socket_Type;
    Addr : Sock_Addr_Type;
begin
    Create_Socket(Sock, Family_Inet, Socket_Datagram);
    
    Addr := (Family => Family_Inet,
             Addr => Inet_Addr("127.0.0.1"),
             Port => Port_Number);
             
    -- Enable reuse address to avoid "address in use" issues on restart
    --Set_Socket_Option(Sock, Socket_Option_Level_Socket, Socket_Option_Reuse_Addr, True);
    
    Bind_Socket(Sock, Addr);
    
    Put_Line("[Ada] UDP Receiver initialized on port " & Port_Number'Image);
    return Sock;

exception
    when E : others =>
        Put_Line("[Ada] Error during socket setup: " & Exception_Message(E));
        return Sock; -- Return a null socket to indicate failure
end Setup_UDP_Receiver;

-- This function receives data on an existing socket and parses it.
function Receiver_Sensor_Data ( Sock : Socket_Type;
    DC : out Helpers.Sensor_Fusion) return Boolean is
    Buffer        : Stream_Element_Array (1 .. 1024);
    Last          : Stream_Element_Offset;
    Addr          : Sock_Addr_Type; -- This is filled with the sender's address
    Received_JSON : JSON_Value := Create_Object;
   -- DC            : Helpers.Sensor_Fusion;
    
    function To_String (B : Stream_Element_Array; L : Stream_Element_Offset) return String is
        S : String (1 .. Integer (L));
    begin
        for I in 1 .. Integer (L) loop
            S (I) := Character'Val (B (Stream_Element_Offset (I)));
        end loop;
        return S;
    end To_String;
begin
    -- Receive a single UDP packet on the pre-existing socket
    Receive_Socket(Sock, Buffer, Last, Addr);
    
    declare
        Msg : constant String := To_String(Buffer, Last);
    begin
       Put_Line("[Ada] Received JSON: " & Msg);
        Received_JSON := Read(Msg, "json.errors");
        
        -- Parse fused_position
        if Received_JSON.Has_Field("fused_position") then
            declare
                Pos_Array : JSON_Array := Received_JSON.Get("fused_position");
            begin
                DC.Position.X := Get(Pos_Array, 1).Get;
                DC.Position.Y := Get(Pos_Array, 2).Get;
                DC.Position.Z := Get(Pos_Array, 3).Get;
            end;
        end if;
        
        -- Parse fused_velocity
        if Received_JSON.Has_Field("fused_velocity") then
            declare
                Vel_Array : JSON_Array := Received_JSON.Get("fused_velocity");
            begin
                DC.Velocity.X := Get(Vel_Array, 1).Get;
                DC.Velocity.Y := Get(Vel_Array, 2).Get;
                DC.Velocity.Z := Get(Vel_Array, 3).Get;
            end;
        end if;
        
        -- Parse fused_orientation
        if Received_JSON.Has_Field("fused_orientation") then
            declare
                Q_Array : JSON_Array := Received_JSON.Get("fused_orientation");
            begin
                DC.Orientation.X:= Get(Q_Array, 1).Get;
                DC.Orientation.Y:= Get(Q_Array, 2).Get;
                DC.Orientation.Z := Get(Q_Array, 3).Get;
                DC.Orientation.W := Get(Q_Array, 4).Get;
            end;
        end if;
        
        -- You could parse acceleration similarly
        if Received_JSON.Has_Field("fused_acceleration") then
            declare
                Acc_Array : JSON_Array := Received_JSON.Get("fused_acceleration");
            begin
                DC.Acceleration.X := Get(Acc_Array, 1).Get;
                DC.Acceleration.Y := Get(Acc_Array, 2).Get;
                DC.Acceleration.Z := Get(Acc_Array, 3).Get;
            end;
        end if;
    end ;
    
    return True;

exception
    when E : others =>
        Put_Line("[Ada] Error receiving or parsing data: " & Exception_Message(E));
        return False;
end Receiver_Sensor_Data;

end UDP_Receiver; 



--  procedure UDP_Receiver is
--     Sock       : Socket_Type;
--     Addr       : Sock_Addr_Type;
--     Buffer     : Stream_Element_Array (1 .. 1024);
--     Last       : Stream_Element_Offset;
--     --UDP_Port   : Port_Type := 12348;
--     UDP_Port   : Port_Type := 12346;
--     Received_JSON : JSON_Value := Create_Object;

--     use Ada.Text_IO;

--     function To_String (B : Stream_Element_Array; L : Stream_Element_Offset) return String is
--        S : String (1 .. Integer (L));
--     begin
--        for I in 1 .. Integer (L) loop
--           S (I) := Character'Val (B (Stream_Element_Offset (I)));
--        end loop;
--        return S;
--     end To_String;

--  begin
--    -- Initialize;
--     Create_Socket (Sock, Family_Inet, Socket_Datagram);

--     Addr := (Family => Family_Inet,
--           Addr   => Inet_Addr("127.0.0.1"),
--           Port   => UDP_Port);
--     Bind_Socket (Sock, Addr);

--     --Put_Line ("[Ada] Listening for JSON on UDP port " & Port_Type'Image(UDP_Port));

   
--        Receive_Socket (Sock, Buffer, Last, Addr);
--        declare
--           Msg : constant String := To_String (Buffer, Last);
--        begin
--           Put_Line ("[Ada] Received JSON:");
--           Received_JSON := Read (Msg, "json.errors");
--           if Received_JSON.Has_Field("fused_position") then
--              Put_Line("Type: " & Received_JSON.Get("fused_position").Write);
--           end if;
--           if Received_JSON.Has_Field("fusedOrientation") then
--              Put_Line("Type: " & Received_JSON.Get("fusedOrientation").Write);
--           end if;
        
--           Put_Line (Msg);
--     end;
   

--  exception
--     when E : others =>
--        Put_Line ("[Ada] Error: " & Exception_Message (E));
--       -- Finalize;
--  end UDP_Receiver;


-- This function initializes the socket and returns it.