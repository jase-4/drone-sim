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
   
    Motor_Speeds := Mixer(Thrust, Pitch_Cmd, Roll_Cmd, Yaw_Cmd);

    Message_JSON.Set_Field("type", "set_speeds");
    Message_JSON.Set_Field("front_left",  Motor_Speeds.Front_Left);
    Message_JSON.Set_Field("back_left",   Motor_Speeds.Back_Left);
    Message_JSON.Set_Field("back_right",  Motor_Speeds.Back_Right);
    Message_JSON.Set_Field("front_right", Motor_Speeds.Front_Right);

    declare
        Message_Str : constant String := Message_JSON.Write;
        Msg_Len     : constant Ada.Streams.Stream_Element_Offset := Message_Str'Length;
        Buffer      : Ada.Streams.Stream_Element_Array(1 .. Msg_Len);
    begin
        for I in Buffer'Range loop
            Buffer(I) := Ada.Streams.Stream_Element'Val(Character'Pos(Message_Str(Integer(I))));
        end loop;
        Len := Msg_Len;

        Addr := (Family => Family_Inet, Addr => Inet_Addr("127.0.0.1"), Port => 12345);
        Create_Socket(Sock, Family_Inet, Socket_Datagram);
        Send_Socket(Sock, Buffer, Len, Addr);
        Close_Socket(Sock);
    end;

exception
    when E : others =>
        Put_Line("[Ada] Error: " & Exception_Information(E));
end UDP_Sender;

