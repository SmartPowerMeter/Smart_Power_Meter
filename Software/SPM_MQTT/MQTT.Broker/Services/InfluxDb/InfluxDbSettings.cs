namespace MQTT.Broker.Services.InfluxDb
{
    public class InfluxDbSettings
    {
        public string ClientUrl { get; set; }

        public string Token { get; set; }

        public string Bucket { get; set; }

        public string Organization { get; set; }
    }
}
