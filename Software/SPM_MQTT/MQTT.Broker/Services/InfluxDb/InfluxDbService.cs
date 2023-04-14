using InfluxDB.Client;
using InfluxDB.Client.Writes;
using InfluxDB.Client.Api.Domain;

namespace MQTT.Broker.Services.InfluxDb
{
    public class InfluxDbService
    {
        private readonly InfluxDbSettings _settings;

        public InfluxDbService(IConfiguration configuration)
        {
            _settings = configuration.GetSection("InfluxDb").Get<InfluxDbSettings>();
        }

        public void WritePoint(string measurementName, string customerId, double value)
        {
            var pointData = PointData.Measurement(measurementName)
                    .Tag("CustomerId", customerId)
                    .Field("Value", value)
                    .Timestamp(DateTime.UtcNow, WritePrecision.Ns);

            Write(write =>
            {
                write.WritePoint(pointData, _settings.Bucket, _settings.Organization);
            });
        }

        private void Write(Action<WriteApi> action)
        {
            using var client = new InfluxDBClient(_settings.ClientUrl, _settings.Token);
            using var write = client.GetWriteApi();
            action(write);
        }
    }
}
