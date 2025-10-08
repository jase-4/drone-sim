with GNAT.Sockets;        use GNAT.Sockets;
with Helpers; use Helpers;

package UDP_Receiver is

   function Setup_UDP_Receiver (Port_Number : Port_Type) return Socket_Type;

  
   function Receiver_Sensor_Data ( Sock : Socket_Type;
    DC : out Helpers.Sensor_Fusion) return Boolean;

end UDP_Receiver;