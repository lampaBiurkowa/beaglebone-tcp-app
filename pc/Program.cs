using System.Net.Sockets;
using System.Text;

class Client
{
    static void Main(string[] args)
    {
        const string BEAGLEBONE_IP = "192.168.7.2";
        const int PORT = 12345;

        try
        {
            var client = new TcpClient(BEAGLEBONE_IP, PORT);
            var stream = client.GetStream();

            while (true)
            {
                Console.Write("Please enter the message: ");
                var message = Console.ReadLine() ?? throw new();
                var data = Encoding.ASCII.GetBytes(message);

                stream.Write(data, 0, data.Length);

                data = new byte[256];
                var bytes = stream.Read(data, 0, data.Length);
                var responseData = Encoding.ASCII.GetString(data, 0, bytes);
                Console.WriteLine($"Received: {responseData}");

                if (responseData == "x") break;
            }

            stream.Close();
            client.Close();
        }
        catch (Exception e)
        {
            Console.WriteLine("Exception: {0}", e.Message);
        }
    }
}
