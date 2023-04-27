using InfluxDB.Client;
using InfluxDB.Client.Writes;
using InfluxDB.Client.Api.Domain;
using MQTT.Broker.Services.SqlDb.Models;

namespace MQTT.Broker.Services.InfluxDb
{
    public class InfluxDbService
    {
        private readonly InfluxDbSettings _settings;

        public InfluxDbService(IConfiguration configuration)
        {
            _settings = configuration.GetSection("InfluxDb").Get<InfluxDbSettings>();
        }

        public void WritePoint(string measurementName, double value, DateTime timeStamp, UserData user)
        {
            var pointData = PointData.Measurement(measurementName)
                    .Tag("CustomerId", user.CustomerId)
                    .Field("Value", value)
                    .Timestamp(DateTime.SpecifyKind(timeStamp.AddHours(-4), DateTimeKind.Utc), WritePrecision.Ns);

            Write(write =>
            {
                write.WritePoint(pointData, user.GetBucketName(), _settings.Organization);
            }, user);
        }

        private void Write(Action<WriteApi> action, UserData user)
        {
            using var client = new InfluxDBClient(_settings.ClientUrl, user.BucketAccessToken);
            using var write = client.GetWriteApi();
            action(write);
        }
    }
}
