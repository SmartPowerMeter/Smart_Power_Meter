namespace MQTT.Broker.Services.SqlDb.Models
{
    public class UserData
    {
        public string CustomerId { get; set; }

        public string BucketAccessToken { get; set; }

        public string GetBucketName() => $"{CustomerId}_bucket";
    }
}
