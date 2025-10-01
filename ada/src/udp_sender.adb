with Ada.Text_IO;         use Ada.Text_IO;
with GNAT.Sockets;        use GNAT.Sockets;
with Ada.Streams;         use type Ada.Streams.Stream_Element_Offset;
with Ada.Exceptions;      use Ada.Exceptions;
with GNATCOLL.JSON; use GNATCOLL.JSON;
with Helpers; use Helpers;


procedure UDP_Sender (
    Thrust    : in Float;
    Roll_Cmd  : in Float;
    Pitch_Cmd : in Float;
    Yaw_Cmd   : in Float
) is
    Sock         : Socket_Type;
    Addr         : Sock_Addr_Type;
    Len          : Ada.Streams.Stream_Element_Offset;
    Message_JSON : JSON_Value := Create_Object;
    Motor_Speeds : Speeds_Record;
begin
    -- 1. Call the mixer function to get the motor speeds
    Motor_Speeds := Mixer(Thrust, Pitch_Cmd, Roll_Cmd, Yaw_Cmd);

    -- 2. Populate the JSON object with the calculated speeds
    Message_JSON.Set_Field("type", "set_speeds");
    Message_JSON.Set_Field("front_left",  Motor_Speeds.Front_Left);
    Message_JSON.Set_Field("back_left",   Motor_Speeds.Back_Left);
    Message_JSON.Set_Field("back_right",  Motor_Speeds.Back_Right);
    Message_JSON.Set_Field("front_right", Motor_Speeds.Front_Right);

    -- 3. Convert JSON to string and stream elements
    declare
        Message_Str : constant String := Message_JSON.Write;
        Msg_Len     : constant Ada.Streams.Stream_Element_Offset := Message_Str'Length;
        Buffer      : Ada.Streams.Stream_Element_Array(1 .. Msg_Len);
    begin
        for I in Buffer'Range loop
            Buffer(I) := Ada.Streams.Stream_Element'Val(Character'Pos(Message_Str(Integer(I))));
        end loop;
        Len := Msg_Len;

        -- 4. Create, send, and close the socket
        Addr := (Family => Family_Inet, Addr => Inet_Addr("127.0.0.1"), Port => 12345);
        Create_Socket(Sock, Family_Inet, Socket_Datagram);
        Send_Socket(Sock, Buffer, Len, Addr);
       -- Put_Line("[Ada] Sent message!");
        Close_Socket(Sock);
    end;

exception
    when E : others =>
        Put_Line("[Ada] Error: " & Exception_Information(E));
end UDP_Sender;


--  procedure UDP_Sender is
--     Sock   : Socket_Type;
--     Addr   : Sock_Addr_Type;
  
--     Len    : Ada.Streams.Stream_Element_Offset;
--     Message_JSON : JSON_Value := Create_Object;
--  begin
--     -- Populate JSON object
  
  
--     Message_JSON.Set_Field("front_left", 4.4);
--     Message_JSON.Set_Field("back_left",   4.4);
--     Message_JSON.Set_Field("back_right",  4.4);
--      Message_JSON.Set_Field("front_right", 4.4);
--      Message_JSON.Set_Field("type", "set_speeds");


--     -- Convert to string
--     declare
--        Message_Str : constant String := Message_JSON.Write;
--          Msg_Len     : constant Ada.Streams.Stream_Element_Offset := Message_Str'Length;
--        Start_Idx : Ada.Streams.Stream_Element_Offset := 1;
--           Last_Idx    : Ada.Streams.Stream_Element_Offset := Ada.Streams.Stream_Element_Offset(Message_Str'Length);
--            Buffer : Ada.Streams.Stream_Element_Array(1 .. Msg_Len);
--     begin
      
--          for I in Start_Idx .. Msg_Len loop
--        -- Need to convert I to Integer to index the string
--           Buffer(I) := Ada.Streams.Stream_Element'Val(Character'Pos(Message_Str(Integer(I))));
--        end loop;
--        Len := Msg_len;  -- Length of the message




--     --:= Ada.Streams.Stream_Element_Array (1 .. Len);
--     Put_Line (Message_JSON.Write);
--      Addr := (Family => Family_Inet,
--              Addr   => Inet_Addr ("127.0.0.1"),
--              Port   => 12345);  -- Match the receiver port

--     Create_Socket (Sock, Family_Inet, Socket_Datagram);
 
--     Put_Line (Len'Image);
--     Send_Socket (Sock,Buffer,Len, Addr);


--     Put_Line ("[Ada] Sent message!");

--     Close_Socket (Sock);
--     end;

--  exception
--     when E : others =>
--        Put_Line ("[Ada] Error: " & Exception_Information (E));  -- Using Ada.Exceptions.Exception_Information
--  end UDP_Sender;
