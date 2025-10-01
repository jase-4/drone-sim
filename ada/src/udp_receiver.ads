with GNAT.Sockets;        use GNAT.Sockets;
with Helpers; use Helpers;

package UDP_Receiver is

   -- This function initializes the socket and returns it.
   --function Setup_UDP_Receiver return Socket_Type;
   -- Overload with a port parameter
   function Setup_UDP_Receiver (Port_Number : Port_Type) return Socket_Type;

   -- This function receives data on an existing socket and parses it.
   function Receiver_Sensor_Data ( Sock : Socket_Type;
    DC : out Helpers.Sensor_Fusion) return Boolean;

end UDP_Receiver;